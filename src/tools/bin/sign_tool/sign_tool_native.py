#!/usr/bin/env python3
# coding=utf-8
"""Native WS63 firmware image signing for the SDK's BrainpoolP256r1 format."""

import argparse
import ctypes
import lzma
from hashlib import sha256
import os
from pathlib import Path
import struct
import subprocess
import sys
import tempfile

# Matches secure_verify_boot.h and fota_format_st.py.
STRUCT_VERSION = 0x10000
ECC_ALGORITHM = 0x2A13C812
ROOT_AREA_SIZE = 0x80
KEY_AREA_SIZE = 0x100
CODE_INFO_SIZE = 0x200
COORDINATE_SIZE = 32
ECC_SIZE = 2 * COORDINATE_SIZE
FOTA_IMAGE_ID = 0xCB8D154E
FLASH_PLAINTEXT = 0x3C7896E1
FLASH_ENCRYPTED = 0x1E6987C3
KEY_AREA_SIGN_OFFSET = KEY_AREA_SIZE - ECC_SIZE
CODE_INFO_SIGN_OFFSET = CODE_INFO_SIZE - 2 * ECC_SIZE
# DER encoding of the RFC 5639 BrainpoolP256r1 OID.
BRAINPOOL_P256R1_OID = bytes.fromhex("06092b2403030208010107")


def load_config(path):
    config = {}
    for line in path.read_text(encoding="utf-8-sig").splitlines():
        name, sep, value = line.partition("=")
        name = name.strip()
        if sep and name and not name.startswith(("#", ";")):
            config[name] = value.strip().strip('"')
    return config


def openssl(*args, data=None):
    command = os.environ.get("WS63_OPENSSL", "openssl")
    proc = subprocess.run([command, *map(str, args)], input=data, capture_output=True)
    if proc.returncode != 0:
        # Some OpenSSL invocations carry sensitive arguments. Keep them out
        # of any exception or build log.
        raise ValueError("OpenSSL operation failed. Check the key format and OpenSSL installation.")
    return proc.stdout


def public_key(path):
    data = openssl("pkey", "-in", path, "-pubout", "-outform", "DER")
    if (BRAINPOOL_P256R1_OID not in data[:-ECC_SIZE]
            or data[-ECC_SIZE - 4:-ECC_SIZE] != b"\x03\x42\x00\x04"):
        raise ValueError("Expected an uncompressed BrainpoolP256r1 public key.")
    return data[-ECC_SIZE:]


def ecc_signature(key_path, data):
    der = openssl("dgst", "-sha256", "-sign", key_path, data=data)
    if not der or der[0] != 0x30 or len(der) < 8 or der[1] != len(der) - 2:
        raise ValueError("Invalid OpenSSL ECDSA signature.")
    values = []
    position = 2
    for _ in range(2):
        if position + 2 > len(der) or der[position] != 2:
            raise ValueError("Invalid ECDSA signature component.")
        length = der[position + 1]
        start = position + 2
        position = start + length
        if not 1 <= length <= COORDINATE_SIZE + 1 or position > len(der) or der[start] & 0x80:
            raise ValueError("Invalid ECDSA signature component length or sign.")
        values.append(int.from_bytes(der[start:position], "big").to_bytes(COORDINATE_SIZE, "big"))
    if position != len(der):
        raise ValueError("Trailing data in OpenSSL ECDSA signature.")
    return b"".join(values)


def sign_area(area, offset, config, key_name):
    if int(config["SignSuite"], 0) == 4:
        signature = sha256(area[:offset]).digest().ljust(ECC_SIZE, b"\0")
    else:
        signature = ecc_signature(config[key_name], bytes(area[:offset]))
    area[offset:offset + ECC_SIZE] = signature


def pack_root_key(config, source):
    image_id = int(config["ImageId"], 0)
    area = bytearray(ROOT_AREA_SIZE)
    struct.pack_into("<8I", area, 0, image_id, STRUCT_VERSION, ROOT_AREA_SIZE,
                     int(config["KeyOwnerId"], 0), int(config["KeyId"], 0),
                     ECC_ALGORITHM, ECC_ALGORITHM, ECC_SIZE)
    area[-ECC_SIZE:] = public_key(source)
    return bytes(area)


def pack_firmware(config, body):
    def number(key):
        return int(config[key], 0)

    suite = number("SignSuite")
    if suite not in (0, 1, 4):
        raise ValueError("This firmware signing mode is not supported by the native tool.")
    if number("KeyAlg") != ECC_ALGORITHM or number("CompressFlag") != 0:
        raise ValueError("Expected an uncompressed WS63 BrainpoolP256r1 firmware image.")

    key = bytearray(KEY_AREA_SIZE)
    struct.pack_into("<13I", key, 0, number("ImageId"), STRUCT_VERSION, KEY_AREA_SIZE,
                     ECC_SIZE, number("KeyOwnerId"), number("KeyId"), ECC_ALGORITHM,
                     ECC_ALGORITHM, ECC_SIZE, number("KeyVersion"), number("KeyVersionMask"),
                     number("Msid"), number("MsidMask"))
    if suite != 4:
        public_key(config["RootKeyFile"])
        key[KEY_AREA_SIGN_OFFSET - ECC_SIZE:KEY_AREA_SIGN_OFFSET] = public_key(config["SubKeyFile"])
    sign_area(key, KEY_AREA_SIGN_OFFSET, config, "RootKeyFile")

    info = bytearray(CODE_INFO_SIZE)
    struct.pack_into("<10I", info, 0, number("CodeInfoImageId"), STRUCT_VERSION,
                     CODE_INFO_SIZE, ECC_SIZE, number("Version"), number("VersionMask"),
                     number("Msid"), number("MsidMask"), 0, len(body))
    info[0x28:0x48] = sha256(body).digest()
    struct.pack_into("<I", info, 0x48, FLASH_ENCRYPTED if suite == 1 else FLASH_PLAINTEXT)
    struct.pack_into("<II", info, 0x80, len(body), number("TextSegmentSize"))
    if suite == 1:
        salt_l1 = bytes.fromhex(config["ProtectionKeyL1"])
        salt_l2 = bytes.fromhex(config["ProtectionKeyL2"])
        iv = bytes.fromhex(config["Iv"])
        if len(salt_l1) != 16 or len(salt_l2) not in (12, 16) or len(body) % 256:
            raise ValueError("Flash encryption requires a 28- or 32-byte salt and a 256-byte-aligned image.")
        info[0x4C:0x6C] = salt_l1 + salt_l2.ljust(16, b"\0")
        if len(iv) != 16:
            raise ValueError("Flash encryption requires a 16-byte IV.")
        info[0x6C:0x7C] = iv
        body = encrypt_aes_ctr(body, bytes.fromhex(config["PlainKey"]), iv)
    sign_area(info, CODE_INFO_SIGN_OFFSET, config, "SubKeyFile")
    return key + info + body


def pack_fota(config, data):
    suite = int(config["SignSuite"], 0)
    if suite not in (0, 1, 4):
        raise ValueError("Unsupported FOTA signing mode.")
    key = bytearray(data[:KEY_AREA_SIZE])
    info = bytearray(data[KEY_AREA_SIZE:2 * KEY_AREA_SIZE])
    if (len(info) != KEY_AREA_SIZE
            or struct.unpack_from("<I", key)[0] != FOTA_IMAGE_ID
            or struct.unpack_from("<I", info)[0] != FOTA_IMAGE_ID
            or struct.unpack_from("<I", key, 8)[0] != KEY_AREA_SIZE
            or struct.unpack_from("<I", info, 8)[0] != KEY_AREA_SIZE):
        raise ValueError("Invalid WS63 FOTA image header.")
    if suite != 4:
        public_key(config["RootKeyFile"])
        key[KEY_AREA_SIGN_OFFSET - ECC_SIZE:KEY_AREA_SIGN_OFFSET] = public_key(config["SubKeyFile"])
    sign_area(key, KEY_AREA_SIGN_OFFSET, config, "RootKeyFile")
    sign_area(info, KEY_AREA_SIGN_OFFSET, config, "SubKeyFile")
    return key + info + data[2 * KEY_AREA_SIZE:]


def encrypt_aes_ctr(data, key, iv):
    """Use the macOS CommonCrypto AES implementation without putting keys in argv."""
    if len(key) != 16 or len(iv) != 16:
        raise ValueError("AES-128-CTR requires a 16-byte key and IV.")
    crypto = ctypes.CDLL("/usr/lib/libSystem.B.dylib")
    pointer = ctypes.c_void_p
    size = ctypes.c_size_t
    status = ctypes.c_int
    crypto.CCCryptorCreateWithMode.argtypes = (
        status, status, status, status, pointer, pointer, size, pointer, size,
        status, ctypes.c_uint32, ctypes.POINTER(pointer),
    )
    crypto.CCCryptorCreateWithMode.restype = status
    crypto.CCCryptorUpdate.argtypes = (
        pointer, pointer, size, pointer, size, ctypes.POINTER(size),
    )
    crypto.CCCryptorUpdate.restype = status
    crypto.CCCryptorRelease.argtypes = (pointer,)
    crypto.CCCryptorRelease.restype = status
    context = pointer()
    # kCCEncrypt, kCCModeCTR, kCCAlgorithmAES, ccNoPadding, kCCModeOptionCTR_BE.
    result = crypto.CCCryptorCreateWithMode(
        0, 4, 0, 0, iv, key, len(key), None, 0, 0, 2, ctypes.byref(context)
    )
    if result != 0:
        raise ValueError(f"Cannot initialize AES-CTR ({result}).")
    try:
        output = ctypes.create_string_buffer(len(data))
        written = size()
        result = crypto.CCCryptorUpdate(
            context, data, len(data), output, len(output), ctypes.byref(written)
        )
        if result != 0 or written.value != len(data):
            raise ValueError(f"AES-CTR encryption failed ({result}).")
        return output.raw
    finally:
        crypto.CCCryptorRelease(context)


def make_diff(old, new):
    # Target control blocks are defined in upg_patch_info.h. For growth, the
    # updater writes from the end and expects literals in reverse order.
    if len(new) > len(old):
        control = struct.pack("<4sIIi", b"KNVD", 0, len(new), 0)
        delta = new[::-1]
    else:
        control = struct.pack("<4sIIi", b"KNVD", len(new), 0, -1)
        delta = bytes((b - a) & 0xFF for a, b in zip(old, new))
    raw = control + delta
    if not new or len(raw) >= 1 << 24:
        raise ValueError("Diff is outside the WS63 24-bit LZMA stream size limit.")
    filters = [{"id": lzma.FILTER_LZMA1, "dict_size": 8192, "lc": 3, "lp": 0, "pb": 2}]
    compressed = lzma.compress(raw, format=lzma.FORMAT_ALONE, filters=filters)
    return compressed[:5] + len(raw).to_bytes(3, "little") + compressed[13:]


def write_output(path, data):
    with tempfile.NamedTemporaryFile(dir=path.parent, prefix=f".{path.name}.", delete=False) as stream:
        temp_path = Path(stream.name)
        try:
            stream.write(data)
            stream.close()
            temp_path.replace(path)
        finally:
            if temp_path.exists():
                temp_path.unlink()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("mode", choices=("0", "1", "3", "4", "5"))
    parser.add_argument("config", type=Path)
    args = parser.parse_args()

    try:
        config = load_config(args.config)
        if config.get("RootKeyFileExt") or config.get("SubKeyFileExt"):
            raise ValueError("Extended dual-key WS63 configurations require a separate signing profile.")
        if args.mode == "4":
            old = Path(config["OldVersionFile"]).read_bytes()
            new = Path(config["NewVersionFile"]).read_bytes()
            write_output(Path(config["DiffFile"]), make_diff(old, new))
            return
        source = Path(config["SrcFile"])
        destination = Path(config["DstFile"])
        if args.mode == "1":
            image = pack_root_key(config, source)
            write_output(destination, image)
            write_output(Path(str(destination) + ".hash"), sha256(image).hexdigest().upper().encode())
        else:
            data = source.read_bytes()
            if args.mode == "0":
                image = pack_firmware(config, data)
            elif args.mode == "3":
                image = pack_fota(config, data)
            else:
                image = encrypt_aes_ctr(data, bytes.fromhex(config["PlainKey"]),
                                        bytes.fromhex(config["Iv"]))
            write_output(destination, image)
    except (KeyError, OSError, ValueError, OverflowError, struct.error) as exc:
        parser.exit(1, f"Native WS63 signer: {exc}\n")


if __name__ == "__main__":
    main()
