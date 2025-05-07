#include "moter_control.h"
#include "i2c.h"
#include "osal_timer.h"

void pwm_ReadData(struct pwm_info *PWM_info)
{

    uint8_t buffer[21] = {0}; // 接收21位数据
    i2c_data_t data;
    data.receive_len = sizeof(buffer);
    data.receive_buf = buffer;
    errcode_t ret = uapi_i2c_master_read(I2C_MASTER_BUS_ID, 0x5a, &data); // 0x5a是stm8s i2c地址
    if (ret != 0) {
        printf("pwmRead(len:%d) failed, %0X!\n", data.receive_len, ret);
        return;
    }
    PWM_info->TIM1_PSCR = (buffer[0] << 8) | buffer[1];   // 左移8位，拼接为16位
    PWM_info->TIM1_ARR = (buffer[2] << 8) | buffer[3];    // buff[2]左移8位与buffer[3]，拼接为16位
    PWM_info->TIM1_CCR1 = (buffer[4] << 8) | buffer[5];   // buff[4]左移8位与buffer[5]，拼接为16位
    PWM_info->TIM1_CCR2 = (buffer[6] << 8) | buffer[7];   // buff[6]左移8位与buffer[7]，拼接为16位
    PWM_info->TIM1_CCR3 = (buffer[8] << 8) | buffer[9];   // buff[8]左移8位与buffer[9]，拼接为16位
    PWM_info->TIM1_CCR4 = (buffer[10] << 8) | buffer[11]; // buff[10]左移8位与buffer[11]，拼接为16位
    PWM_info->TIM2_PSCR = buffer[12];                     // buff[12]值
    PWM_info->TIM2_ARR = (buffer[13] << 8) | buffer[14];  // buff[13]左移8位与buffer[14]，拼接为16位
    PWM_info->TIM2_CCR1 = (buffer[15] << 8) | buffer[16]; // buff[15]左移8位与buffer[16]，拼接为16位
    PWM_info->TIM2_CCR2 = (buffer[17] << 8) | buffer[18]; // buff[17]左移8位与buffer[18]，拼接为16位
    PWM_info->TIM2_CCR3 = (buffer[19] << 8) | buffer[20]; // buff[19]左移8位与buffer[20]，拼接为16位
    printf("TIM1->PSCR: %d\r\n", PWM_info->TIM1_PSCR);
    printf("TIM1->ARR: %d\r\n", PWM_info->TIM1_ARR);
    printf("TIM1->CCR1: %d\r\n", PWM_info->TIM1_CCR1);
    printf("TIM1->CCR2: %d\r\n", PWM_info->TIM1_CCR2);
    printf("TIM1->CCR3: %d\r\n", PWM_info->TIM1_CCR3);
    printf("TIM1->CCR4: %d\r\n", PWM_info->TIM1_CCR4);
    printf("TIM2->PSCR: %d\r\n", PWM_info->TIM2_PSCR);
    printf("TIM2->ARR: %d\r\n", PWM_info->TIM2_ARR);
    printf("TIM2->CCR1: %d\r\n", PWM_info->TIM2_CCR1);
    printf("TIM2->CCR2: %d\r\n", PWM_info->TIM2_CCR2);
    printf("TIM2->CCR3: %d\r\n", PWM_info->TIM2_CCR3);
}

void pwm_write(uint8_t reg_data)
{

    uint8_t buffer[] = {reg_data};
    i2c_data_t data = {0};
    data.send_buf = buffer;
    data.send_len = sizeof(buffer);
    errcode_t ret = uapi_i2c_master_write(I2C_MASTER_BUS_ID, 0x5a, &data); // 0x5a是stm8s i2c地址
    if (ret != 0) {
        printf("pwm_write: failed, %0X!\n", ret);
        return;
    }
    printf("pwm_write: success! address:0x5a\r\n");
}

void pwm_writes(uint8_t *reg_data)
{
    i2c_data_t data = {0};
    data.send_buf = reg_data;
    data.send_len = sizeof(reg_data);
    errcode_t ret = uapi_i2c_master_write(I2C_MASTER_BUS_ID, 0x5a, &data); // 0x5a是stm8s i2c地址
    if (ret != 0) {
        printf("pwm_write: failed, %0X!\n", ret);
        return;
    }
    printf("pwm_write: success! address:0x5a\r\n");
}

/*------------------------------------------------------------------------------------------
函数: pwm_set_freq  设置频率预选值
参数：option 取值范围 0 到 13
说明： stm8s的芯片时钟频率 16MHz
     option取值      对应频率Hz        预分频          ARR值(即调用left_wheel_set和right_wheel_set的参数limit)
        0               10              32           49999
        1               25              16           39999
        2               50              16           19999
        3               100             16           9999
        4               200             16           4999
        5               500             16           1999
        6               1k              16           999
        7               2k              16           499
        8               5k              16           199
        9               10k             1            1599
        10              20k             1            799
        11              50k             1            319
        12              100k            1            159
        13              200k            1            79
----------------------------------------------------------------------------------------------
*/
void pwm_set_freq(uint8_t option)
{
    if (option > 13) { // 13代表限定范围
        option = 13;   // 13代表限定范围
    }
    uint8_t freq_select = 0x10 | option; // 0x10 表示要设置预选频率
    pwm_write(freq_select);
}

/*
函数：pwm_set_freq_accu   精确设定pwm频率
参数：psc 预分频系数，范围 0x0000 到 0xffff     arr 自动重装载值，范围 0x0000 到 0xffff
说明：stm8s时钟频率是16MHz，设定频率后的值为 16MHz/( (psc + 1) * (arr + 1) )
        如果用该函数设定pwm频率，那么limit值就是arr值
*/
void pwm_set_freq_accu(uint16_t psc, uint16_t arr)
{
    uint8_t freq[5] = {0};   // 5位数据
    freq[0] = 0x20;          // 选定要设定的是精确频率
    uint8_t psch = psc >> 8; // 取高8位
    uint8_t pscl = psc;
    uint8_t arrh = arr >> 8; // 取高8位
    uint8_t arrl = arr;
    freq[1] = psch; // 预分频高位
    freq[2] = pscl; // 2预分频低位
    freq[3] = arrh; // 3重装载高位
    freq[4] = arrl; // 4重装载低位
    pwm_writes(freq);
}

void right_wheel_set(uint16_t CRR,
                     uint16_t limit,
                     bool dir) // limit 是设定stm8s的定时器ARR值, 可参考pwm_set_freq函数注释
{
    if (CRR > limit)
        CRR = limit;
    uint8_t CRRH, CRRL;
    CRRH = (CRR & 0xff00u) >> 8; // 取高8位
    CRRL = (CRR & 0x00ffu);      // 取低8位
    if (dir)                     // dir为真 方向向前
    {
        uint8_t PWM1CH1[3] = {0x70, 0x00, 0x00}; // stm8s接收的第一个数据时是0x70时,表示要设置timer1的通道1
        uint8_t PWM1CH2[3] = {0x80, CRRH, CRRL}; // stm8s接收的第一个数据时是0x80时,表示要设置timer1的通道2
        pwm_writes(PWM1CH1);
        pwm_writes(PWM1CH2);
    } else // dir不为真 方向向后
    {
        uint8_t PWM1CH1[3] = {0x70, CRRH, CRRL}; // stm8s接收的第一个数据时是0x70时,表示要设置timer1的通道1
        uint8_t PWM1CH2[3] = {0x80, 0x00, 0x00}; // stm8s接收的第一个数据时是0x80时,表示要设置timer1的通道2
        pwm_writes(PWM1CH1);
        pwm_writes(PWM1CH2);
    }
}

void left_wheel_set(uint16_t CRR,
                    uint16_t limit,
                    bool dir) // limit 是设定stm8s的定时器ARR值, 可参考pwm_set_freq函数注释
{
    if (CRR > limit)
        CRR = limit;
    uint8_t CRRH, CRRL;
    CRRH = (CRR & 0xff00u) >> 8; // 取高8位
    CRRL = (CRR & 0x00ffu);      // 取低8位
    if (dir)                     // dir为真 方向向前
    {
        uint8_t PWM1CH3[3] = {0x90, CRRH, CRRL}; // stm8s接收的第一个数据时是0x90时,表示要设置timer1的通道3
        uint8_t PWM1CH4[3] = {0xa0, 0x00, 0x00}; // stm8s接收的第一个数据时是0xa0时,表示要设置timer1的通道4
        pwm_writes(PWM1CH3);
        pwm_writes(PWM1CH4);
    } else // dir不为真 方向向后
    {
        uint8_t PWM1CH3[3] = {0x90, 0x00, 0x00}; // stm8s接收的第一个数据时是0x90时,表示要设置timer1的通道3
        uint8_t PWM1CH4[3] = {0xa0, CRRH, CRRL}; // stm8s接收的第一个数据时是0xa0时,表示要设置timer1的通道4
        pwm_writes(PWM1CH3);
        pwm_writes(PWM1CH4);
    }
}