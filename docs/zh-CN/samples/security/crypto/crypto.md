# 硬件加密引擎

> 使用技术：Security Unified API

## 学习目标

- 理解 WS63 硬件加密引擎的架构——对称加密（AES/SM4）+ 哈希（SHA256 (Secure Hash Algorithm 256-bit)）+ 随机数（TRNG (True Random Number Generator)）+ 密钥管理（KM）
- 掌握 AES-128/256-CBC/GCM 硬件加解密
- 掌握 SM4-CBC（国密对称加密）硬件加解密
- 掌握 SHA256 硬件哈希和 TRNG 真随机数生成
- 理解对称加密的软硬件性能差异——硬件加速远快于纯 CPU 软件实现

## 基本概念

### 硬件加密 vs 软件加密（对称算法）

| 对比项 | 硬件加密 | 软件加密 |
|--------|:---:|:---:|
| 速度 | 快 | 慢 |
| CPU 占用 | 低（独立硬件计算） | 高（CPU 逐字节算） |
| 功耗 | 低 | 高 |
| 适用场景 | 大数据量：OTA (Over-The-Air) 校验、日志加密 | 小数据：几十字节的配置或凭证加密 |

### 国密 vs 国际算法

| 国密 | 等效国际 | 使用场景 |
|------|:---:|------|
| SM4 (SM4 Block Cipher) | AES (Advanced Encryption Standard) | 对称加密 |
| SM2 | ECC (Elliptic Curve Cryptography) | 非对称加密/签名 |
| SM3 (SM3 Cryptographic Hash Algorithm) | SHA256 | 哈希摘要 |

> 国内合规项目（政务、军工、金融）必须使用国密算法。

### 密钥管理（KM）

硬件密钥管理模块——密钥存储在安全区域（不可读，只能用来加密/签名），比存储在 NV (Non-Volatile) 中更安全。即使攻击者获取固件 dump 也无法读取密钥。

## 涉及 API

| API | 用途 |
|-----|------|
| `uapi_drv_cipher_symc_encrypt()` | AES/SM4 对称加密（需先 init + set_config） |
| `uapi_drv_cipher_sha256(buf, len, out, out_len)` | SHA256 哈希（单次调用） |
| `uapi_drv_cipher_trng_get_random_bytes(buf, size)` | 真随机数生成（用于密钥/IV） |
| `uapi_drv_keyslot_create()` | 创建密钥槽（KM 安全区域） |
| `uapi_drv_klad_set_clear_key()` | 向密钥槽写入明文密钥 |

> 具体 API 以 `cipher.h`、`km.h`、`trng.h` 头文件为准。

## 案例说明

### 案例简介

TRNG 生成随机密钥 → 存储到 KM 安全区域 → AES-CBC 加密传感器数据 → 通过 MQTT (Message Queuing Telemetry Transport) 发送密文 → 云端解密。

## 关键配置

| 参数 | 推荐值 | 说明 |
|------|:---:|------|
| 加密算法 | AES（国际）/ SM4（国密） | 国内合规项目选 SM4 |
| 加密模式 | CBC (Cipher Block Chaining) / GCM (Galois/Counter Mode) | GCM 带认证防篡改 |
| 密钥长度 | AES-128: 16B / AES-256: 32B | 安全要求高选 256 |

## 代码详解

概念性代码：

```c
/* TRNG 生成随机 IV */
uint8_t iv[16];
uapi_drv_cipher_trng_get_random_bytes(iv, sizeof(iv));

/* KM 密钥槽示例 */
uint32_t keyslot_handle;
uapi_drv_km_init();
uapi_drv_keyslot_create(&keyslot_handle, UAPI_DRV_KEYSLOT_TYPE_USER);
uapi_drv_klad_set_clear_key(keyslot_handle, key, sizeof(key));

/* TODO: 以实际 symc handle 驱动加密和哈希操作 */
```

> TRNG生成的真随机数比 `rand()` 伪随机数安全得多——用于密钥和 IV (Initialization Vector) 生成。

---

