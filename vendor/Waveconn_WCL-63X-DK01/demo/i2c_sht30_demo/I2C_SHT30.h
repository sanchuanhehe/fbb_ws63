#ifndef __I2C_SHT30__
#define __I2C_SHT30__

/* SHT30传感器数据类型定义 ------------------------------------------------------------*/
typedef struct {
    float    Humidity;          // 湿度
    float    Temperature;       // 温度
}SHT30_Data;

extern SHT30_Data read_data;

/* 寄存器宏定义 --------------------------------------------------------------------*/
#define I2C_OWN_ADDRESS                            0x0A

#define SHT30_Addr 0x44


void Init_SHT30(void);
void SHT30_Read_Data(SHT30_Data *ReadData);

#endif





