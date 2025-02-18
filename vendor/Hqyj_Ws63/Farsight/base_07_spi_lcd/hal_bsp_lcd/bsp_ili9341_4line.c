/*
 * Copyright (c) 2024 Beijing HuaQingYuanJian Education Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 ******************************************************************************
 * @file   bsp_ili9341_4line.c
 * @brief  2.8寸屏ili9341驱动文件，采用4线SPI
 *
 ******************************************************************************
 */
#include "bsp_ili9341_4line.h"

// SPI
#define SPI_SLAVE_NUM 1
#define SPI_BUS_CLK 24000000
#define SPI_FREQUENCY 10
#define SPI_FRAME_FORMAT_STANDARD 0
#define SPI_WAIT_CYCLES 0x10
#define SPI_RST_MASTER_PIN 6
#define SPI_DO_MASTER_PIN 9
#define SPI_CLK_MASTER_PIN 7
#define SPI_DC_MASTER_PIN 11
#define SPI_MASTER_PIN_MODE 3
#define SPI_MASTER_BUS_ID 0

#define SPI_TIMEOUT_VALUE 0xff

#define HIGH_BYTE_SHIFT 8
#define LOW_BYTE_SHIFT 0xff

#define ILI9341_WIDTH 320
#define ILI9341_HEIGHT 240
// LCD的画笔颜色和背景色
uint16_t POINT_COLOR = BLACK; // 画笔颜色
uint16_t BACK_COLOR = WHITE;  // 背景色
static uint8_t DFT_SCAN_DIR;  // 扫描方向
// 管理ili9341重要参数
static ili_dev g_ili9341dev;

void app_spi_init_pin(void)
{
    uapi_pin_set_mode(GPIO_05, PIN_MODE_4);
    uapi_gpio_set_dir(GPIO_05, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(GPIO_05, GPIO_LEVEL_HIGH);

    uapi_pin_set_mode(SPI_RST_MASTER_PIN, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(SPI_RST_MASTER_PIN, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(SPI_RST_MASTER_PIN, GPIO_LEVEL_HIGH);

    uapi_pin_set_mode(SPI_DC_MASTER_PIN, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(SPI_DC_MASTER_PIN, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(SPI_DC_MASTER_PIN, GPIO_LEVEL_HIGH);

    uapi_pin_set_mode(SPI_DO_MASTER_PIN, SPI_MASTER_PIN_MODE);
    uapi_pin_set_mode(SPI_CLK_MASTER_PIN, SPI_MASTER_PIN_MODE);
    uapi_pin_set_ds(SPI_CLK_MASTER_PIN, PIN_DS_7); // 由于时钟线的问题，需要设置驱动能力
}

void app_spi_master_init_config(void)
{
    spi_attr_t config = {0};
    spi_extra_attr_t ext_config = {0};

    config.is_slave = false;
    config.slave_num = SPI_SLAVE_NUM;
    config.bus_clk = SPI_BUS_CLK;
    config.freq_mhz = SPI_FREQUENCY;
    config.clk_polarity = 0;
    config.clk_phase = 0;
    config.frame_format = SPI_CFG_FRAME_FORMAT_MOTOROLA_SPI;
    config.spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    // 80001338代表发送数据的格式不对，需要修改配置参数
    config.frame_size = HAL_SPI_FRAME_SIZE_8;
    config.tmod = HAL_SPI_TRANS_MODE_TX;
    config.sste = 0;

    int ret = uapi_spi_init(SPI_MASTER_BUS_ID, &config, &ext_config);
    if (ret != 0) {
        printf("spi init fail %0x\r\n", ret);
    }
}

uint32_t hal_spi_transmit(spi_bus_t bus, uint8_t *txdata, uint32_t data_len, uint32_t timeout)
{
    uint8_t *buffer = txdata;
    spi_xfer_data_t data = {0};

    data.tx_buff = buffer;
    data.tx_bytes = data_len;
    uapi_spi_master_write(bus, &data, timeout);
    return 0;
}
/*
**********************************************************************
* @fun     :ili9341_WriteReg
* @brief   :写寄存器函数
* @param   :REG:寄存器值
* @return  :None
**********************************************************************
*/
void ili9341_WriteReg(uint8_t REG)
{
    uint8_t txdata[] = {REG};
    uapi_gpio_set_val(SPI_DC_MASTER_PIN, GPIO_LEVEL_LOW);

    hal_spi_transmit(SPI_MASTER_BUS_ID, txdata, 1, SPI_TIMEOUT_VALUE); /* 2: 传输字节数 */

    uapi_gpio_set_val(SPI_DC_MASTER_PIN, GPIO_LEVEL_HIGH);
}
/*
**********************************************************************
* @fun     :ili9341_WriteData
* @brief   :写ili9341数据
* @param   :DATA:要写入的值
* @return  :None
**********************************************************************
*/
void ili9341_WriteData(uint8_t DATA)
{
    uint8_t txdata[] = {DATA};

    hal_spi_transmit(SPI_MASTER_BUS_ID, txdata, 1, SPI_TIMEOUT_VALUE); /* 1: 传输字节数 */
}
/*
**********************************************************************
* @fun     :ili9341_WriteDevReg
* @brief   :ili9341_Reg:寄存器地址，ili9341_RegValue:要写入的数据
* @return  :None
**********************************************************************
*/
void ili9341_WriteDevReg(uint8_t ili9341_Reg, uint8_t ili9341_RegValue)
{
    ili9341_WriteReg(ili9341_Reg);
    ili9341_WriteData(ili9341_RegValue);
}
/*
**********************************************************************
* @fun     :ili9341_write_ram_prepare
* @brief   :开始写GRAM
* @return  :None
**********************************************************************
*/
void ili9341_write_ram_prepare(void)
{
    ili9341_WriteReg(g_ili9341dev.wramcmd);
}
/*
**********************************************************************
* @fun     :ili9341_WriteRam
* @brief   :ili9341写GRAM，SPI数据写入方式不同，功能同ili9341_write_ram_prepare
* @return  :None
**********************************************************************
*/
void ili9341_WriteRam(uint16_t DAT)
{
    uint8_t TempBufferD[2] = {DAT >> 8, DAT};

    hal_spi_transmit(SPI_MASTER_BUS_ID, TempBufferD, 2, SPI_TIMEOUT_VALUE); /* 2: 传输字节数 */
}
/*
**********************************************************************
* @fun     :ili9341_SetCursor
* @brief   :设置光标位置
* @param   :Xpos:横坐标，Ypos:纵坐标
* @return  :None
**********************************************************************
*/
void ili9341_SetCursor(uint16_t xpos, uint16_t ypos)
{
    uint8_t TempBufferX[2] = {xpos >> 8, xpos & 0XFF};
    uint8_t TempBufferY[2] = {ypos >> 8, ypos & 0XFF};

    ili9341_WriteReg(g_ili9341dev.setxcmd);

    hal_spi_transmit(SPI_MASTER_BUS_ID, TempBufferX, 2, SPI_TIMEOUT_VALUE); /* 2: 传输字节数 */

    ili9341_WriteReg(g_ili9341dev.setycmd);

    hal_spi_transmit(SPI_MASTER_BUS_ID, TempBufferY, 2, SPI_TIMEOUT_VALUE); /* 2: 传输字节数 */
}
/*
**********************************************************************
* @fun     :ili9341_SetArea
* @brief   :设置显示区域
* @param   :x0/x1:横坐标，y0/y1:纵坐标
* @return  :None
**********************************************************************
*/
static uint16_t old_x0 = 0xFFFF, old_x1 = 0xFFFF, old_y0 = 0xFFFF, old_y1 = 0xFFFF;

void ili9341_SetArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t arguments[4];
    // Set columns, if changed
    if (x0 != old_x0 || x1 != old_x1) {
        arguments[0] = x0 >> HIGH_BYTE_SHIFT; /* 0: 数组下标 */
        arguments[1] = x0 & LOW_BYTE_SHIFT;   /* 1: 数组下标 */
        arguments[2] = x1 >> HIGH_BYTE_SHIFT; /* 2: 数组下标 */
        arguments[3] = x1 & LOW_BYTE_SHIFT;   /* 3: 数组下标 */

        ili9341_WriteReg(g_ili9341dev.setxcmd);
        hal_spi_transmit(SPI_MASTER_BUS_ID, arguments, 4, SPI_TIMEOUT_VALUE); /* 4: 传输字节数 */

        old_x0 = x0;
        old_x1 = x1;
    }
    // Set rows, if changed
    if (y0 != old_y0 || y1 != old_y1) {
        arguments[0] = y0 >> HIGH_BYTE_SHIFT; /* 0: 数组下标 */
        arguments[1] = y0 & LOW_BYTE_SHIFT;   /* 1: 数组下标 */
        arguments[2] = y1 >> HIGH_BYTE_SHIFT; /* 2: 数组下标 */
        arguments[3] = y1 & LOW_BYTE_SHIFT;   /* 3: 数组下标 */

        ili9341_WriteReg(g_ili9341dev.setycmd);
        hal_spi_transmit(SPI_MASTER_BUS_ID, arguments, 4, SPI_TIMEOUT_VALUE); /* 4: 传输字节数 */

        old_y0 = y0;
        old_y1 = y1;
    }
}
/*
**********************************************************************
* @fun     :ili9341_display_dir
* @brief   :设置ili9341的自动扫描方向
                            Memory Access Control (36h)
                            This command defines read/write scanning direction of the frame memory.

                            These 3 bits control the direction from the MPU to memory write/read.

                            Bit  Symbol  Name  Description
                            D7   MY  Row Address Order
                            D6   MX  Column Address Order
                            D5   MV  Row/Column Exchange
                            D4   ML  Vertical Refresh Order  LCD vertical refresh direction control. 、

                            D3   BGR RGB-BGR Order   Color selector switch control
                                        (0 = RGB color filter panel, 1 = BGR color filter panel )
                            D2   MH  Horizontal Refresh ORDER  LCD horizontal refreshing direction control.
                            D1   X   Reserved  Reserved
                            D0   X   Reserved  Reserved
* @return  :None
**********************************************************************
*/
void ili9341_display_dir(screen_show_dir show_dir)
{
    uint16_t regval = 0x08; // RGB-BGR Order不能改变
    uint8_t dirreg = 0;

    if (show_dir == SCANVERTICAL) { // 竖屏
        g_ili9341dev.width = ILI9341_HEIGHT;
        g_ili9341dev.height = ILI9341_WIDTH;

        g_ili9341dev.wramcmd = 0X2C;
        g_ili9341dev.setxcmd = 0X2A;
        g_ili9341dev.setycmd = 0X2B;
        DFT_SCAN_DIR = L2R_U2D;

        switch (DFT_SCAN_DIR) {
            case L2R_U2D:                                 // 从左到右,从上到下 竖屏
                regval |= (0 << 7) | (0 << 6) | (0 << 5); /* 7: 移位 6: 移位 5: 移位 */
                break;
            case R2L_D2U:                                 // 从右到左,从下到上 竖屏
                regval |= (1 << 7) | (1 << 6) | (0 << 5); /* 7: 移位 6: 移位 5: 移位 */
                break;
            default: // 缺省分支
                break;
        }
    } else { // 横屏
        g_ili9341dev.width = ILI9341_WIDTH;
        g_ili9341dev.height = ILI9341_HEIGHT;

        g_ili9341dev.wramcmd = 0X2C;
        g_ili9341dev.setxcmd = 0X2A;
        g_ili9341dev.setycmd = 0X2B;
        DFT_SCAN_DIR = D2U_L2R;

        switch (DFT_SCAN_DIR) {
            case U2D_R2L:                                 // 从上到下,从右到左 横屏
                regval |= (0 << 7) | (1 << 6) | (1 << 5); /* 7: 移位 6: 移位 5: 移位 */
                break;
            case D2U_L2R:                                 // 从下到上,从左到右 横屏
                regval |= (1 << 7) | (0 << 6) | (1 << 5); /* 7: 移位 6: 移位 5: 移位 */
                break;
            default: // 缺省分支
                break;
        }
    }
    dirreg = 0X36;
    regval |= 0x00;
    ili9341_WriteDevReg(dirreg, regval);
    // 设置光标在原点位置
    ili9341_WriteReg(g_ili9341dev.setxcmd);
    ili9341_WriteData(0);
    ili9341_WriteData(0);
    ili9341_WriteData((g_ili9341dev.width - 1) >> HIGH_BYTE_SHIFT);
    ili9341_WriteData((g_ili9341dev.width - 1) & LOW_BYTE_SHIFT);
    ili9341_WriteReg(g_ili9341dev.setycmd);
    ili9341_WriteData(0);
    ili9341_WriteData(0);
    ili9341_WriteData((g_ili9341dev.height - 1) >> HIGH_BYTE_SHIFT);
    ili9341_WriteData((g_ili9341dev.height - 1) & LOW_BYTE_SHIFT);
}
/*
**********************************************************************
* @fun     :ili9341_init
* @brief   :初始化ili9341
* @return  :None
**********************************************************************
*/
void ili9341_init(void)
{
    // ili9341复位
    uapi_gpio_set_val(SPI_RST_MASTER_PIN, GPIO_LEVEL_LOW);
    osDelay(1); /* 1: 延时10ms */
    uapi_gpio_set_val(SPI_RST_MASTER_PIN, GPIO_LEVEL_HIGH);
    osDelay(10); /* 10: 延时100ms */

    ili9341_WriteReg(0xCF);
    ili9341_WriteData(0x00);
    ili9341_WriteData(0xC1);
    ili9341_WriteData(0X30);

    ili9341_WriteReg(0xED);
    ili9341_WriteData(0x64);
    ili9341_WriteData(0x03);
    ili9341_WriteData(0X12);
    ili9341_WriteData(0X81);

    ili9341_WriteReg(0xE8);
    ili9341_WriteData(0x85);
    ili9341_WriteData(0x10);
    ili9341_WriteData(0x7A);

    ili9341_WriteReg(0xCB);
    ili9341_WriteData(0x39);
    ili9341_WriteData(0x2C);
    ili9341_WriteData(0x00);
    ili9341_WriteData(0x34);
    ili9341_WriteData(0x02);

    ili9341_WriteReg(0xF7);
    ili9341_WriteData(0x20);

    ili9341_WriteReg(0xEA);
    ili9341_WriteData(0x00);
    ili9341_WriteData(0x00);

    ili9341_WriteReg(0xC0);  // Power control
    ili9341_WriteData(0x1B); // VRH[5:0]

    ili9341_WriteReg(0xC1);  // Power control
    ili9341_WriteData(0x01); // SAP[2:0];BT[3:0]

    ili9341_WriteReg(0xC5);  // VCM control
    ili9341_WriteData(0x30); // 3F
    ili9341_WriteData(0x30); // 3C

    ili9341_WriteReg(0xC7); // VCM control2
    ili9341_WriteData(0XB7);

    ili9341_WriteReg(0x36);  // Memory Access Control
    ili9341_WriteData(0x48); // RGB

    ili9341_WriteReg(0x3A);
    ili9341_WriteData(0x55);

    ili9341_WriteReg(0xB1);
    ili9341_WriteData(0x00);
    ili9341_WriteData(0x10); // Frame Rate 119(Hz)

    ili9341_WriteReg(0xB6); // Display Function Control
    ili9341_WriteData(0x0A);
    ili9341_WriteData(0xA2);

    ili9341_WriteReg(0xF2); // 3Gamma Function Disable
    ili9341_WriteData(0x00);

    ili9341_WriteReg(0x26); // Gamma curve selected
    ili9341_WriteData(0x01);

    ili9341_WriteReg(0xE0); // Set Gamma
    ili9341_WriteData(0x0F);
    ili9341_WriteData(0x2A);
    ili9341_WriteData(0x28);
    ili9341_WriteData(0x08);
    ili9341_WriteData(0x0E);
    ili9341_WriteData(0x08);
    ili9341_WriteData(0x54);
    ili9341_WriteData(0XA9);
    ili9341_WriteData(0x43);
    ili9341_WriteData(0x0A);
    ili9341_WriteData(0x0F);
    ili9341_WriteData(0x00);
    ili9341_WriteData(0x00);
    ili9341_WriteData(0x00);
    ili9341_WriteData(0x00);

    ili9341_WriteReg(0XE1); // Set Gamma
    ili9341_WriteData(0x00);
    ili9341_WriteData(0x15);
    ili9341_WriteData(0x17);
    ili9341_WriteData(0x07);
    ili9341_WriteData(0x11);
    ili9341_WriteData(0x06);
    ili9341_WriteData(0x2B);
    ili9341_WriteData(0x56);
    ili9341_WriteData(0x3C);
    ili9341_WriteData(0x05);
    ili9341_WriteData(0x10);
    ili9341_WriteData(0x0F);
    ili9341_WriteData(0x3F);
    ili9341_WriteData(0x3F);
    ili9341_WriteData(0x0F);

    ili9341_WriteReg(0x2B);
    ili9341_WriteData(0x00);
    ili9341_WriteData(0x00);
    ili9341_WriteData(0x01);
    ili9341_WriteData(0x3f);

    ili9341_WriteReg(0x2A);
    ili9341_WriteData(0x00);
    ili9341_WriteData(0x00);
    ili9341_WriteData(0x00);
    ili9341_WriteData(0xef);

    ili9341_WriteReg(0x35); // Tearing Effect Line ON
    ili9341_WriteData(0x00);

    ili9341_WriteReg(0x11); // Sleep Out
    ili9341_WriteReg(0x21); // Display Inversion ON

    ili9341_WriteReg(0x29);            // display on
    ili9341_display_dir(SCANVERTICAL); // 竖屏显示
    ili9341_Clear(WHITE);
}
/*
**********************************************************************
* @fun     :ili9341_Clear
* @brief   :清屏函数，color:要清屏的填充色
* @return  :None
**********************************************************************
*/
void ili9341_Clear(uint16_t color)
{
    uint8_t TempBufferD[2] = {color >> 8, color};
    uint32_t index = 0;
    uint32_t totalpoint = g_ili9341dev.width;
    totalpoint *= g_ili9341dev.height; // 得到总点数

    ili9341_SetCursor(0x00, 0x00); // 设置光标位置
    ili9341_write_ram_prepare();   // 开始写入GRAM

    for (index = 0; index < totalpoint; index++) {
        hal_spi_transmit(SPI_MASTER_BUS_ID, TempBufferD, 2, SPI_TIMEOUT_VALUE); /* 2: 传输字节数 */
    }
}
/*
**********************************************************************
* @fun     :ili9341_DrawPoint
* @brief   :uGUI函数调用，画点函数
* @param   :x,y:坐标，color:此点的颜色
* @return  :None
**********************************************************************
*/
void ili9341_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    uint8_t TempBufferX[2] = {x >> 8, x & 0XFF};
    uint8_t TempBufferY[2] = {y >> 8, y & 0XFF};
    uint8_t TempBufferD[2] = {color >> 8, color};

    ili9341_WriteReg(g_ili9341dev.setxcmd);

    hal_spi_transmit(SPI_MASTER_BUS_ID, TempBufferX, 2, SPI_TIMEOUT_VALUE); /* 2: 传输字节数 */

    ili9341_WriteReg(g_ili9341dev.setycmd);

    hal_spi_transmit(SPI_MASTER_BUS_ID, TempBufferY, 2, SPI_TIMEOUT_VALUE); /* 2: 传输字节数 */

    ili9341_WriteReg(g_ili9341dev.wramcmd);

    hal_spi_transmit(SPI_MASTER_BUS_ID, TempBufferD, 2, SPI_TIMEOUT_VALUE); /* 2: 传输字节数 */
}
/*
**********************************************************************
* @fun     :ili9341_FillFrame
* @brief   :在指定区域内填充单个颜色
* @param   :(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1),color:要填充的颜色
* @return  :None
**********************************************************************
*/
void ili9341_FillFrame(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    uint16_t i = 0;
    uint16_t j = 0;
    uint16_t xlen = 0;
    xlen = ex - sx + 1;

    uint8_t TempBuffer[2] = {color >> 8, color};
    // 数据写入屏幕
    for (i = sy; i <= ey; i++) {
        ili9341_SetCursor(sx, i);    // 设置光标位置
        ili9341_write_ram_prepare(); // 开始写入GRAM
        for (j = 0; j < xlen; j++) {
            hal_spi_transmit(SPI_MASTER_BUS_ID, TempBuffer, 2, SPI_TIMEOUT_VALUE); /* 2: 传输字节数 */
        }
    }
}

/*
**********************************************************************
* @fun     :ili9341_DrawLine
* @brief   :画线
* @param   :_usX1,_usY1:起点坐标,_usX2,_usY2:终点坐标,_usColor:要填充的颜色
* @return  :None
**********************************************************************
*/
void ili9341_DrawLine(uint16_t _usX1, uint16_t _usY1, uint16_t _usX2, uint16_t _usY2, uint16_t _usColor)
{
    int32_t dx;
    int32_t dy;
    int32_t tx;
    int32_t ty;
    int32_t inc1;
    int32_t inc2;
    int32_t d;
    int32_t iTag;
    int32_t x;
    int32_t y;
    /* 采用 Bresenham 算法，在2点间画一条直线 */
    ili9341_DrawPoint(_usX1, _usY1, _usColor);
    /* 如果两点重合，结束后面的动作 */
    if (_usX1 == _usX2 && _usY1 == _usY2) {
        return;
    }

    iTag = 0;
    if (_usX2 >= _usX1) {
        dx = _usX2 - _usX1;
    } else {
        dx = _usX1 - _usX2;
    }

    if (_usY2 >= _usY1) {
        dy = _usY2 - _usY1;
    } else {
        dy = _usY1 - _usY2;
    }
    /* 如果dy为计长方向，则交换纵横坐标。 */
    if (dx < dy) {
        uint16_t temp;
        iTag = 1;
        temp = _usX1;
        _usX1 = _usY1;
        _usY1 = temp;
        temp = _usX2;
        _usX2 = _usY2;
        _usY2 = temp;
        temp = dx;
        dx = dy;
        dy = temp;
    }

    tx = _usX2 > _usX1 ? 1 : -1; /* 确定是增1还是减1 */
    ty = _usY2 > _usY1 ? 1 : -1;
    x = _usX1;
    y = _usY1;
    inc1 = 2 * dy;        /* 2: 计算坐标 */
    inc2 = 2 * (dy - dx); /* 2: 计算坐标 */
    d = inc1 - dx;
    /* 循环画点 */
    while (x != _usX2) {
        if (d < 0) {
            d += inc1;
        } else {
            y += ty;
            d += inc2;
        }

        if (iTag) {
            ili9341_DrawPoint(y, x, _usColor);
        } else {
            ili9341_DrawPoint(x, y, _usColor);
        }

        x += tx;
    }
}
/*
**********************************************************************
* @fun     :LCD_DrawRect
* @brief   :绘制水平放置的矩形
* @param   :_usX,_usY: 矩形左上角的坐标
*			_usHeight : 矩形的高度
*			_usWidth  : 矩形的宽度
* @return  :None
**********************************************************************
*/
void LCD_DrawRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
{
    ili9341_DrawLine(_usX, _usY, _usX + _usWidth - 1, _usY, _usColor);                                 /* 顶 */
    ili9341_DrawLine(_usX, _usY + _usHeight - 1, _usX + _usWidth - 1, _usY + _usHeight - 1, _usColor); /* 底 */

    ili9341_DrawLine(_usX, _usY, _usX, _usY + _usHeight - 1, _usColor);                           /* 左 */
    ili9341_DrawLine(_usX + _usWidth - 1, _usY, _usX + _usWidth - 1, _usY + _usHeight, _usColor); /* 右 */
}
/*
**********************************************************************
* @fun     :LCD_DrawCircle
* @brief   :绘制一个圆，笔宽为1个像素
* @param   :_usX,_usY  : 圆心的坐标
*			_usRadius  : 圆的半径
* @return  :None
**********************************************************************
*/
void LCD_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor)
{
    int32_t D;     /* Decision Variable */
    uint32_t CurX; /* 当前 X 值 */
    uint32_t CurY; /* 当前 Y 值 */

    D = 3 - (_usRadius << 1); /* 3: 计算坐标 */
    CurX = 0;
    CurY = _usRadius;

    while (CurX <= CurY) {
        ili9341_DrawPoint(_usX + CurX, _usY + CurY, _usColor);
        ili9341_DrawPoint(_usX + CurX, _usY - CurY, _usColor);
        ili9341_DrawPoint(_usX - CurX, _usY + CurY, _usColor);
        ili9341_DrawPoint(_usX - CurX, _usY - CurY, _usColor);
        ili9341_DrawPoint(_usX + CurY, _usY + CurX, _usColor);
        ili9341_DrawPoint(_usX + CurY, _usY - CurX, _usColor);
        ili9341_DrawPoint(_usX - CurY, _usY + CurX, _usColor);
        ili9341_DrawPoint(_usX - CurY, _usY - CurX, _usColor);

        if (D < 0) {
            D += (CurX << 2) + 6; /* 2: 计算坐标 */
        } else {
            D += ((CurX - CurY) << 2) + 10; /* 2: 计算坐标 */
            CurY--;
        }
        CurX++;
    }
}
/*
**********************************************************************
* @fun     :LCD_ShowChar
* @brief   :在指定位置显示一个字符
* @param   : x,y:起始坐标
            num:要显示的字符:" "--->"~"
            size:字体大小 12/16/24
            mode:叠加方式(1)还是非叠加方式(0)
* @return  :None
**********************************************************************
*/
uint32_t LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode)
{
    uint8_t temp;
    uint8_t t1;
    uint8_t t;
    uint16_t y0 = y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); // 得到字体一个字符对应点阵集所占的字节数
    num = num - ' '; // 得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
    for (t = 0; t < csize; t++) {
        if (size == 12) { /* 12: 字符大小 */
            temp = asc2_1206[num][t];
        } else if (size == 16) { /* 16: 字符大小 */
            temp = asc2_1608[num][t];
        } else if (size == 24) { /* 24: 字符大小 */
            temp = asc2_2412[num][t];
        } else {
            return 1;
        }

        for (t1 = 0; t1 < 8; t1++) { /* 8: 一个字节比特数 */
            if (temp & 0x80) {
                ili9341_DrawPoint(x, y, POINT_COLOR);
            } else if (mode == 0) {
                ili9341_DrawPoint(x, y, BACK_COLOR);
            }
            temp <<= 1;
            y++;
            if (y >= g_ili9341dev.height) {
                return 1; // 超区域了
            }
            if ((y - y0) == size) {
                y = y0;
                x++;
                if (x >= g_ili9341dev.width) {
                    return 1; // 超区域了
                }
                break;
            }
        }
    }
    return 0; // 超区域了
}
/*
**********************************************************************
* @fun     :LCD_ShowString
* @brief   :显示字符串
* @param   : x,y:起始坐标
            width,height:区域大小
            size:字体大小 12/16/24
            *p:字符串起始地址
* @return  :None
**********************************************************************
*/
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t len, uint8_t size, uint8_t *p)
{
    uint16_t i = 0;
    uint16_t width = 240;
    uint16_t height = 320;
    uint16_t x0;
    x0 = x;
    // 判断是不是非法字符!
    while ((i < len) && (*p <= '~') && (*p >= ' ')) {
        i++;
        if (x >= width) {
            x = x0;
            y += size;
        }
        if (y >= height) {
            break;
        }
        LCD_ShowChar(x, y, *p, size, 1);
        x += size / 2; /* 2: 宽度 */
        p++;
    }
}

void LCD_DrawPointPic(uint16_t x, uint16_t y, uint16_t color)
{
    uint8_t TempBuffer[2] = {color >> 8, color};
    ili9341_SetCursor(x, y);     // 设置光标位置
    ili9341_write_ram_prepare(); // 开始写入GRAM

    hal_spi_transmit(SPI_MASTER_BUS_ID, TempBuffer, 2, SPI_TIMEOUT_VALUE); /* 2: 传输字节数 */
}

/****************************************************************************
* 名    称：void LCD_DrawPicture(uint16_t StartX,uint16_t StartY,uint16_t EndX,uint16_t EndY,uint16_t *pic)
* 功    能：在指定座标范围显示一副图片
* 入口参数： StartX     行起始座标
*           StartY     列起始座标
*           EndX       行结束座标
*           EndY       列结束座标
                        pic        图片头指针
* 出口参数：无
* 说    明：图片取模格式为水平扫描，16位颜色模式
* 调用方法：LCD_DrawPicture(0,0,100,100,(uint16_t*)demo);
****************************************************************************/
void LCD_DrawPicture(uint16_t StartX, uint16_t StartY, uint16_t Xend, uint16_t Yend, uint8_t *pic)
{
    static uint16_t i = 0;
    static uint16_t j = 0;
    uint16_t *bitmap = (uint16_t *)pic;
    for (j = 0; j < Yend - StartY; j++) {
        for (i = 0; i < Xend - StartX; i++)
            LCD_DrawPointPic(StartX + i, StartY + j, *bitmap++);
    }
}