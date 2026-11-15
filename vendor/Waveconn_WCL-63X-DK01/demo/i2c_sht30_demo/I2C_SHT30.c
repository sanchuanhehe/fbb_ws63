#include "I2C_SHT30.h"
#include "i2c.h"
#include "osal_debug.h"

const int16_t POLYNOMIAL = 0x131;
#define CONFIG_I2C_MASTER_BUS_ID 1


/***************************************************************
 * 函数名称: Init_SHT30
 * 说    明: 初始化SHT30，设置测量周期
 * 参    数: 无
 * 返 回 值: 无
 ***************************************************************/
void Init_SHT30(void)
{
    i2c_data_t data = {0};
    uint8_t tx_buffer[2] = {0x22, 0x36};
    data.send_buf = tx_buffer;
    data.send_len = 2;
    uint32_t retval = uapi_i2c_master_write(CONFIG_I2C_MASTER_BUS_ID, SHT30_Addr, &data);
    if (retval != 0)
    {
        osal_printk("Init_SHT30() failed, %0X!\n", retval);
    }
    osal_printk("init finish\r\n");
}

/***************************************************************
* 函数名称: SHT3x_CheckCrc
* 说    明: 检查数据正确性
* 参    数: data：读取到的数据
                        nbrOfBytes：需要校验的数量
                        checksum：读取到的校对比验值
* 返 回 值: 校验结果，0-成功        1-失败
***************************************************************/
uint8_t SHT3x_CheckCrc(char data[], char nbrOfBytes, char checksum)
{
    char crc = 0xFF;
    char bit = 0;
    int byteCtr;

    // calculates 8-Bit checksum with given polynomial
    for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
    {
        crc ^= (data[byteCtr]);
        for (bit = 8; bit > 0; --bit)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ POLYNOMIAL;
            else
                crc = (crc << 1);
        }
    }

    if (crc != checksum)
        return 1;
    else
        return 0;
}

/***************************************************************
 * 函数名称: SHT3x_CalcTemperatureC
 * 说    明: 温度计算
 * 参    数: u16sT：读取到的温度原始数据
 * 返 回 值: 计算后的温度数据
 ***************************************************************/
float SHT3x_CalcTemperatureC(unsigned short u16sT)
{
    float temperatureC = 0; // variable for result

    u16sT &= ~0x0003; // clear bits [1..0] (status bits)
    //-- calculate temperature [℃] --
    temperatureC = (175 * (float)u16sT / 65535 - 45); // T = -45 + 175 * rawValue / (2^16-1)

    return temperatureC;
}

/***************************************************************
 * 函数名称: SHT3x_CalcRH
 * 说    明: 湿度计算
 * 参    数: u16sRH：读取到的湿度原始数据
 * 返 回 值: 计算后的湿度数据
 ***************************************************************/
float SHT3x_CalcRH(unsigned short u16sRH)
{
    float humidityRH = 0; // variable for result

    u16sRH &= ~0x0003; // clear bits [1..0] (status bits)
    //-- calculate relative humidity [%RH] --
    humidityRH = (100 * (float)u16sRH / 65535); // RH = rawValue / (2^16-1) * 10

    return humidityRH;
}

/***************************************************************
 * 函数名称: SHT30_Read_Data
 * 说    明: 测量温度、湿度
 * 参    数: 无
 * 返 回 值: 无
 ***************************************************************/
void SHT30_Read_Data(SHT30_Data *ReadData)
{
    char data[3]; // data array for checksum verification
    unsigned short tmp = 0;
    uint16_t dat;
    uint8_t SHT3X_Data_Buffer[6];                       
    i2c_data_t data1 = {0};
    uint8_t send_data[2] = {0xE0, 0x00};
    data1.send_buf = send_data;
    data1.send_len = 2;
    data1.receive_buf = SHT3X_Data_Buffer;
    data1.receive_len = 6;
    uapi_i2c_master_write(CONFIG_I2C_MASTER_BUS_ID, SHT30_Addr, &data1);
    uapi_i2c_master_read(CONFIG_I2C_MASTER_BUS_ID, SHT30_Addr, &data1);
    //    /* check tem */
    data[0] = SHT3X_Data_Buffer[0];
    data[1] = SHT3X_Data_Buffer[1];
    data[2] = SHT3X_Data_Buffer[2];
    tmp = SHT3x_CheckCrc(data, 2, data[2]);
    if (!tmp) /* value is ture */
    {
        dat = ((uint16_t)data[0] << 8) | data[1];
        ReadData->Temperature = SHT3x_CalcTemperatureC(dat);
    }
    //    /* check humidity */
    data[0] = SHT3X_Data_Buffer[3];
    data[1] = SHT3X_Data_Buffer[4];
    data[2] = SHT3X_Data_Buffer[5];

    tmp = SHT3x_CheckCrc(data, 2, data[2]);
    if (!tmp) /* value is ture */
    {
        dat = ((uint16_t)data[0] << 8) | data[1];
        ReadData->Humidity = SHT3x_CalcRH(dat);
    }
}