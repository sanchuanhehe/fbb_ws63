# dfrobot_bme680

BME680 是专为移动应用和可穿戴设备开发的集成环境传感器其中尺寸和低功耗是关键要求。 BME680 扩展了 Bosch Sensortec <br>
现有的环境传感器系列，首次集成了用于气体、压力、湿度和温度的单个高线性度和高精度传感器。<br>

![产品效果图](./resources/images/SEN0248.png) 

## 产品链接（[https://www.dfrobot.com.cn/goods-1621.html](https://www.dfrobot.com.cn/goods-1621.html)）
    SKU: SEN0248

## 目录

  * [概述](#概述)
  * [API](#API)
  * [历史](#历史)

## 概述

提供一个 WS63 库，用于通过 I2C 通过 SPI 读取和解释 Bosch BME680 数据。读取温度、湿度、气体、压力并计算高度。


## API
```C++
  /**
   * @fn begin
   * @brief 启动BME680传感器设备
   * @return 结果
   * @retval  非0值 : 失败
   * @retval  0     : 成功
   */
  int16_t bme680_bme680_begin(void);

  /**
   * @fn update
   * @brief 将所有数据更新到 MCU 内存
   */
  void    bme680_bme680_update(void);

  /**
   * @fn start_convert
   * @brief 开始转换以获得准确的值
   */
  void  start_convert(void);
  /**
   * @fn read_temperature
   * @brief 获取温度值 (单位 摄氏度)
   *
   * @return 温度值, 这个值有两个小数点
   */
  float read_temperature(void);
  /**
   * @fn read_pressure
   * @brief 读取压强值 (单位 帕)
   *
   * @return 压强值, 这个值有两个小数点
   */
  float read_pressure(void);
  /**
   * @fn read_humidity
   * @brief 读取湿度值 (单位 %rh)
   * @return 湿度值, 这个值有两个小数点
   */
  float read_humidity(void);
  /**
   * @fn read_altitude
   * @brief 读取高度（单位米）
   * @return 高度值, 这个值有两个小数点
   */
  float read_altitude(void);
  /**
   * @fn read_calibrated_altitude
   * @brief 读取校准高度（单位米）
   *
  * @param sea_level  正规化大气压
   *
   * @return 标定高度值，该值有两位小数
   */
  float read_calibrated_altitude(float sea_level);
  /**
   * @fn read_gas_resistance
   * @brief 读取气体电阻（单位欧姆）
   * @return 温度值，这个值有两位小数
   */
  float read_gas_resistance(void);
  /**
   * @fn read_sea_level
   * @brief 读取标准化大气压力（单位帕）
   * @param altitude   标准化大气压力
   * @return 标准化大气压力
   */
  float read_sea_level(float altitude);
  /**
   * @fn set_param
   * @brief 设置bme680的参数
   *
  * @param e_param        : 需要设置的参数
   *        dat           : 对象数据，不能超过5
   */  
  void    set_param(e_bme680_param_t e_param, uint8_t dat);
  /**
   * @fn set_gas_heater
   * @brief 设置bme680燃气加热器
   * @param temp        :目标温度
   * @param t           :以毫秒为单位花费的时间
   */
   void    set_gas_heater(uint16_t temp, uint16_t t);
```

## 历史

- 2025/09/29 - WS63版本 V1.0.0 - Written by Martin(Martin@dfrobot.com), 2025.

- 2017/12/04 - Arduino版本 V2.0.0 - Written by Frank(jiehan.guo@dfrobot.com), 2017.
- 2017/09/04 - Arduino版本 V1.0.0 - Written by Frank(jiehan.guo@dfrobot.com), 2017.
