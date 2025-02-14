#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "i2c.h"

#include "ndef.h"
#include "nfcForum.h"
#include "NT3H.h"

#define DELAY_WRITE 300000
#define INDEX_NDEF_START 0
#define INDEX_END_RECORD 1
#define INDEX_NDEF_HEADER 2

uint8_t nfcPageBuffer[NFC_PAGE_SIZE];
NT3HerrNo errNo;

// due to the nature of the NT3H a timeout is required to
// protectd 2 consecutive I2C access

inline const uint8_t *get_last_ncf_page(void)
{
    return nfcPageBuffer;
}

static bool writeTimeout(uint8_t *data, uint8_t dataSend)
{
    uint32_t status = 0;
    i2c_data_t nt3h1101_i2c_data1 = {0};

    nt3h1101_i2c_data1.send_buf = data;
    nt3h1101_i2c_data1.send_len = dataSend;

    status = uapi_i2c_master_write(1, NT3H1X_SLAVE_ADDRESS, &nt3h1101_i2c_data1);
    if (status != 0) {
        printf("===== Error: I2C write status1 = 0x%x! =====\r\n", status);
        return 0;
    }
    usleep(DELAY_WRITE);
    return 1;
}

static bool readTimeout(uint8_t address, uint8_t *block_data)
{
    uint32_t status = 0;
    i2c_data_t nt3h1101_i2c_data = {0};
    uint8_t buffer[1] = {address};
    nt3h1101_i2c_data.send_buf = buffer;
    nt3h1101_i2c_data.send_len = 1;
    nt3h1101_i2c_data.receive_buf = block_data;
    nt3h1101_i2c_data.receive_len = NFC_PAGE_SIZE;

    status = uapi_i2c_master_write(1, NT3H1X_SLAVE_ADDRESS, &nt3h1101_i2c_data);
    if (status != 0) {
        printf("===== Error: I2C write status1 = 0x%x! =====\r\n", status);
        return 0;
    }
    status = uapi_i2c_master_read(1, NT3H1X_SLAVE_ADDRESS, &nt3h1101_i2c_data);
    if (status != 0) {
        printf("===== Error: I2C read status = 0x%x! =====\r\n", status);
        return 0;
    }

    return 1;
}

bool NT3HReadHeaderNfc(uint8_t *endRecordsPtr, uint8_t *ndefHeader)
{
    *endRecordsPtr = 0;
    bool ret = NT3HReadUserData(0);
    // read the first page to see where is the end of the Records.
    if (ret == true) {
        // if the first byte is equals to NDEF_START_BYTE there are some records
        // store theend of that
        if ((nfcPageBuffer[INDEX_NDEF_START] == NDEF_START_BYTE) && (nfcPageBuffer[INDEX_NDEF_HEADER] != NTAG_ERASED)) {
            *endRecordsPtr = nfcPageBuffer[INDEX_END_RECORD];
            *ndefHeader = nfcPageBuffer[INDEX_NDEF_HEADER];
            return true;
        }
        return true;
    } else {
        errNo = NT3HERROR_READ_HEADER;
    }

    return ret;
}

bool NT3HWriteHeaderNfc(uint8_t endRecordsPtr, uint8_t ndefHeader)
{
    // read the first page to see where is the end of the Records.
    bool ret = NT3HReadUserData(0);
    if (ret == true) {
        nfcPageBuffer[INDEX_END_RECORD] = endRecordsPtr;
        nfcPageBuffer[INDEX_NDEF_HEADER] = ndefHeader;
        ret = NT3HWriteUserData(0, nfcPageBuffer);
        if (ret == false) {
            errNo = NT3HERROR_WRITE_HEADER;
        }
    } else {
        errNo = NT3HERROR_READ_HEADER;
    }

    return ret;
}

bool NT3HEraseAllTag(void)
{
    bool ret = true;
    uint8_t erase[NFC_PAGE_SIZE + 1] = {USER_START_REG, 0x03, 0x03, 0xD0, 0x00, 0x00, 0xFE};
    ret = writeTimeout(erase, sizeof(erase));
    if (ret == false) {
        errNo = NT3HERROR_ERASE_USER_MEMORY_PAGE;
    }
    return ret;
}

bool NT3HReaddManufactoringData(uint8_t *manuf)
{
    return readTimeout(MANUFACTORING_DATA_REG, manuf);
}

bool NT3HReadConfiguration(uint8_t *configuration)
{
    return readTimeout(CONFIG_REG, configuration);
}

bool getSessionReg(void)
{
    return readTimeout(SESSION_REG, nfcPageBuffer);
}

bool NT3HReadUserData(uint8_t page)
{
    uint8_t reg = USER_START_REG + page;
    // if the requested page is out of the register exit with error
    if (reg > USER_END_REG) {
        errNo = NT3HERROR_INVALID_USER_MEMORY_PAGE;
        return false;
    }

    bool ret = readTimeout(reg, nfcPageBuffer);
    if (ret == false) {
        errNo = NT3HERROR_READ_USER_MEMORY_PAGE;
    }

    return ret;
}

bool NT3HWriteUserData(uint8_t page, const uint8_t *data)
{
    bool ret = true;
    uint8_t dataSend[NFC_PAGE_SIZE + 1]; // data plus register
    uint8_t reg = USER_START_REG + page;

    // if the requested page is out of the register exit with error
    if (reg > USER_END_REG) {
        errNo = NT3HERROR_INVALID_USER_MEMORY_PAGE;
        ret = false;
        return ret;
    }

    dataSend[0] = reg; // store the register
    memcpy_s(&dataSend[1], NFC_PAGE_SIZE, data, NFC_PAGE_SIZE);
    ret = writeTimeout(dataSend, sizeof(dataSend));
    if (ret == false) {
        errNo = NT3HERROR_WRITE_USER_MEMORY_PAGE;
        return ret;
    }

    return ret;
}

bool NT3HReadSram(void)
{
    bool ret = false;
    int i = 0, j = 0;

    for (i = SRAM_START_REG, j = 0; i <= SRAM_END_REG; i++, j++) {
        ret = readTimeout(i, nfcPageBuffer);
        if (ret == false) {
            return ret;
        }

        printf("[page=%d]: ", i);
        for (j = 0; j < NFC_PAGE_SIZE; j++) {
            // printf("0x%x ", nfcPageBuffer[i]);
        }
        printf("\n");
    }
    return ret;
}

void NT3HGetNxpSerialNumber(char *buffer)
{
    uint8_t manuf[16];
    int i = 0;

    if (NT3HReaddManufactoringData(manuf)) {
        for (i = 0; i < SERIAL_NUM_LEN; i++) {
            buffer[i] = manuf[i];
        }
    }
}
