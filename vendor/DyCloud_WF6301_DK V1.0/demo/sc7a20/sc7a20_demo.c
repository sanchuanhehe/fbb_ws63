/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: I2C Sample Source. \n
 *
 * History: \n
 * 2023-05-25, Create file. \n
 */
#include "pinctrl.h"
#include "i2c.h"
#include "soc_osal.h"
#include "app_init.h"
//#include "sc7a20_demo.h"
#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
#include "dma.h"
#endif

#define I2C_MASTER_ADDR                   0x0
#define I2C_SET_BAUDRATE                  100000
#define I2C_TASK_DURATION_MS              5000
#define I2C_TRANSFER_LEN                  50

#define SC7A20_REG_WHO_AM_I     0x0F
#define SC7A20_REG_CTRL_1		0x20
#define SC7A20_REG_CTRL_2		0x21
#define SC7A20_REG_CTRL_3		0x22
#define SC7A20_REG_CTRL_4		0x23
#define SC7A20_REG_X_L          0x28
#define SC7A20_REG_X_H          0x29
#define SC7A20_REG_Y_L          0x2A
#define SC7A20_REG_Y_H          0x2B
#define SC7A20_REG_Z_L          0x2C
#define SC7A20_REG_Z_H          0x2D
#define SC7A20_REG_STATUS		0x27
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
#define I2C_INT_TRANSFER_DELAY_MS         800
#endif

#define I2C_TASK_PRIO                     24
#define I2C_TASK_STACK_SIZE               0x1000

// 将16位补码转换为有符号整数
errcode_t twos_complement_to_int16(int16_t *value) {
    // 如果最高位为1（负数），则进行补码转换
    if ( *value & 0x8000) {
        *value =  -((~(*value) + 1) & 0xFFFF); // 取反加一，再转为负数
    } 
    return ERRCODE_SUCC; // 写入成功
}
/**
 * @brief 从 SC7A20  读取数据
 * @param addr 要读取的地址 (8位)
 * @param buffer 用于存储读取数据的缓冲区
 * @param len 要读取的数据长度
 * @return 成功返回0，失败返回非0
 */
errcode_t SC7A20_Read(uint8_t addr, uint8_t *buffer, uint16_t len) {
    uint8_t addr_buffer[2];
    i2c_data_t data = { 0 };

    if (addr > 0x37){
        return ERRCODE_I2C_ADDRESS_INVLID;
    }
    
    addr_buffer[0] = (addr & 0xFF) | 0x80; // 地址字节 连续读取模式

    data.send_buf = addr_buffer;
    data.send_len = 1;
    data.receive_buf = buffer;
    data.receive_len = len;
    // 首先发送要读取的地址
    if (uapi_i2c_master_write(CONFIG_I2C_SC7A20_BUS_ID, CONFIG_I2C_SC7A20_SLAVE_ADDR, &data) != ERRCODE_SUCC)
    {
        osal_printk("SC7A20 WRITE ADDR ERROR\n");
        return ERRCODE_FAIL; // 如果写入地址失败，直接返回错误
    }
    return uapi_i2c_master_read(CONFIG_I2C_SC7A20_BUS_ID, CONFIG_I2C_SC7A20_SLAVE_ADDR, &data);
}
/**
 * @brief 写入数据到 SC7A20 
 * @param addr 要写入的地址 (8位)
 * @param buffer 要写入的数据缓冲区
 * @param len 要写入的数据长度
 * @return 成功返回0，失败返回非0
 */
errcode_t SC7A20_Write(uint8_t addr, uint8_t *buffer, uint16_t len) {

    uint8_t write_buffer[len + 1]; // 缓冲区大小为页大小 + 1字节的地址
    i2c_data_t data = { 0 };

    if (addr > 0x37){
        return ERRCODE_I2C_ADDRESS_INVLID;
    }

    write_buffer[0] = (addr & 0xFF)| 0x80; // 地址字节

        // 将数据复制到缓冲区中
    for (uint16_t i = 0; i < len; i++) {
        write_buffer[i + 1] = buffer[i];
    }
        data.send_buf = write_buffer;
        data.send_len = len + 1;
        
        if (uapi_i2c_master_write(CONFIG_I2C_SC7A20_BUS_ID, CONFIG_I2C_SC7A20_SLAVE_ADDR , &data) != ERRCODE_SUCC) {
            return ERRCODE_FAIL; // 写入失败
        }
    return ERRCODE_SUCC; // 写入成功
}
/**
 * @brief 从 SC7A20  读取加速度数据
 * @param x X轴
 * @param y Y轴
 * @param z Z轴
 * @return 成功返回0，失败返回非0
 */
errcode_t SC7A20_ReadAcceleration(int16_t *x,int16_t *y,int16_t *z){

    uint8_t Read_buffer[20] = {0};
    int ret = 0;

    if ((ret = SC7A20_Read(SC7A20_REG_STATUS, Read_buffer, 1)) != ERRCODE_SUCC) {
        osal_printk("SC7A20 READ ERROR: %d\n",ret);
        return ERRCODE_FAIL; // 读取失败
    }
    
    if ( (Read_buffer[0] & 0x08) ){   //检测3轴数据是否全部转换完成
        if (SC7A20_Read(SC7A20_REG_X_L, Read_buffer, 6) != ERRCODE_SUCC) {
            return ERRCODE_FAIL; // 读取失败
        }

    *x = ((Read_buffer[1] << 8) | Read_buffer[0]) ;
    *y = ((Read_buffer[3] << 8) | Read_buffer[2]) ;
    *z = ((Read_buffer[5] << 8) | Read_buffer[4]) ;

    twos_complement_to_int16(x);
    twos_complement_to_int16(y);
    twos_complement_to_int16(z);
    }
    else{
        osal_printk("SC7A20 Data not ready");
        return ERRCODE_FAIL; // 读取失败
    }
 
    return ERRCODE_SUCC; // 获取成功
}
errcode_t Sc7a20Init(void) {
    uint8_t write_buffer[5]; 

    write_buffer[0] = 0x47;//50Hz+正常模式xyz使能
    write_buffer[1] = 0x00;//关闭滤波器，手册上面没有滤波器截止频率设置说明，开启后无法测量静止状态下的重力加速度
    write_buffer[2] = 0x00;//关闭中断
    write_buffer[3] = 0x88;//读取完成再更新，小端模式，、2g+正常模式，高精度模式

    // 使用while循环持续尝试写入，直到成功为止
    while(SC7A20_Write(SC7A20_REG_CTRL_1, write_buffer, 4) != ERRCODE_SUCC) {
        osal_printk("No SC7A20 detected\r\n");
        osal_msleep(1000);
    }

    osal_printk(" SC7A20_init succ\n");
    return ERRCODE_SUCC; // 写入成功
}
static void app_i2c_init_pin(void)
{
    /* I2C pinmux. */
    uapi_pin_set_mode(CONFIG_I2C_SCL_SC7A20_PIN, CONFIG_I2C_SC7A20_PIN_MODE);
    uapi_pin_set_mode(CONFIG_I2C_SDA_SC7A20_PIN, CONFIG_I2C_SC7A20_PIN_MODE);
}

static void *i2c_sc7a20_task(const char *arg)
{
    unused(arg);

    uint32_t baudrate = I2C_SET_BAUDRATE;
    uint8_t hscode = I2C_MASTER_ADDR;
 
    int16_t x = 0;
    int16_t y = 0;
    int16_t z = 0;

    /* I2C master init config. */
    app_i2c_init_pin();
    uapi_i2c_master_init(CONFIG_I2C_SC7A20_BUS_ID, baudrate, hscode);


    Sc7a20Init();//SC7A20初始化

    while (1) {
        
        if (SC7A20_ReadAcceleration(&x,&y,&z) == ERRCODE_SUCC) {
            osal_printk("i2c%d SC7A20 x: %d,y: %d,z: %d \r\n", CONFIG_I2C_SC7A20_BUS_ID,x,y,z);
        } else {
            osal_printk("CHT20 failed to read data\r\n");
            osal_msleep(500);
            continue;
        }
        osal_msleep(100);
    }

    return NULL;
}

static void i2c_sc7a20_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)i2c_sc7a20_task, 0, "I2cSc7a20Task", I2C_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, I2C_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the i2c_master_entry. */
app_run(i2c_sc7a20_entry);