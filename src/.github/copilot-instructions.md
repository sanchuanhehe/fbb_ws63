# GitHub Copilot 工作区指南

## 项目概览

这是一个基于海思 WS63V100 芯片的嵌入式开发工作区，提供了完整的 WiFi、蓝牙（BLE）、星闪（SLE）和雷达功能的开发支持。

### 核心信息
- **项目名称**: fbb_ws63
- **版本**: CFBB 0.9.0.5
- **主芯片**: WS63V100 
- **主要功能**: WiFi、BLE、SLE、雷达感知
- **构建系统**: CMake + Python
- **操作系统**: LiteOS
- **开发语言**: C/C++

## 工作区结构

```
src/
├── application/           # 应用层代码
│   ├── samples/          # 示例代码
│   │   ├── wifi/        # WiFi 示例
│   │   ├── bt/          # 蓝牙示例  
│   │   ├── peripheral/   # 外设示例
│   │   └── radar/       # 雷达示例
│   └── ws63/            # WS63平台应用
├── bootloader/           # 引导加载程序
├── drivers/              # 驱动程序
├── kernel/               # 内核层 (LiteOS)
├── middleware/           # 中间件
│   ├── services/        # 服务组件
│   └── utils/           # 工具库
├── protocol/             # 协议栈
│   ├── wifi/            # WiFi协议
│   ├── bt/              # 蓝牙协议
│   └── radar/           # 雷达协议
├── open_source/          # 第三方开源组件
│   ├── mbedtls/         # TLS安全库
│   ├── lwip/            # TCP/IP协议栈
│   ├── cjson/           # JSON解析
│   └── wpa_supplicant/  # WiFi认证
├── include/              # 头文件
├── build/                # 构建系统
├── tools/                # 开发工具
├── output/               # 编译输出
└── docs/                 # 文档
```

## 开发环境

### 构建工具
- **构建脚本**: `build.py`
- **配置脚本**: `activate_env.sh` / `config.sh`
- **构建工具**: CMake + Ninja/Make
- **编译器**: RISC-V GCC 7.3.0

### 常用构建命令
```bash
# 激活开发环境
source activate_env.sh

# 配置项目
./build.py -ninja menuconfig

# 构建项目
./build.py -ninja

# 清理后构建
./build.py -ninja -c

# 指定组件构建
./build.py -ninja -component=wifi,bt

# 调试版本
./build.py -ninja -debug
```

## 核心技术栈

### WiFi 开发
- **标准**: 802.11 b/g/n
- **模式**: STA、SoftAP、STA+AP 并存
- **功能**: 扫描、连接、热点、共存
- **示例位置**: `application/samples/wifi/`

### 蓝牙开发 
- **支持协议**: BLE 5.0+、SLE (星闪)
- **功能**: GATT服务、配对连接、数据传输
- **示例位置**: `application/samples/bt/`

### 雷达开发
- **功能**: 目标检测、距离测量、速度检测
- **示例位置**: `application/samples/radar/`

### 网络协议栈
- **TCP/IP**: lwIP
- **安全**: mbedTLS/GmSSL
- **应用协议**: HTTP、MQTT、CoAP

## 开发规范

### 代码风格
- 使用 `.clang-format` 配置的代码格式
- 遵循海思编码规范
- 文件编码: UTF-8
- 行尾: LF

### 目录命名
- 驱动: `drivers/chips/ws63/`
- 应用: `application/samples/[功能]/`
- 中间件: `middleware/services/[服务]/`
- 协议: `protocol/[协议]/`

### 头文件包含
```c
#include "common_def.h"     // 公共定义
#include "errcode.h"        // 错误码定义
#include "osal_addr.h"      // 系统抽象层
```

## 项目构建

### 建立源码目录流程

当需要创建新的应用程序时，可以在 `application/ws63/` 目录下建立新的项目目录。以下以建立 `my_demo` 项目为例：

#### 1. 创建项目目录结构
```bash
# 创建项目目录
mkdir application/ws63/my_demo
```

#### 2. 复制并配置 CMakeLists.txt
```bash
# 复制模板文件
cp application/ws63/ws63_liteos_application/CMakeLists.txt application/ws63/my_demo/CMakeLists.txt
```

#### 3. 修改 CMakeLists.txt 配置

CMakeLists.txt 中的关键变量配置：

| 变量名称 | 变量含义 |
|---------|----------|
| `COMPONENT_NAME` | 当前组件名称，如 `my_demo` |
| `SOURCES` | 当前组件的C文件列表，`CMAKE_CURRENT_SOURCE_DIR` 变量标识当前路径 |
| `PUBLIC_HEADER` | 当前组件需要对外提供的头文件的路径 |
| `PRIVATE_HEADER` | 当前组件内部的头文件搜索路径 |
| `PRIVATE_DEFINES` | 当前组件内部生效的宏定义 |
| `PUBLIC_DEFINES` | 当前组件需要对外提供的宏定义 |
| `COMPONENT_PUBLIC_CCFLAGS` | 当前组件需要对外提供的编译选项 |
| `COMPONENT_CCFLAGS` | 当前组件内部生效的编译选项 |

#### 4. 典型的 CMakeLists.txt 示例
```cmake
set(COMPONENT_NAME "my_demo")

set(SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/my_demo_main.c
    # 添加其他源文件...
)

set(PUBLIC_HEADER
    ${ROOT_DIR}/include
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

build_component()
```

#### 5. 注册到构建系统
修改以下两个文件以将新组件加入构建：

**修改 application/ws63/CMakeLists.txt：**
```cmake
# 添加子目录
add_subdirectory(my_demo)
```

**修改 build/config/target_config/ws63/config.py：**
在 `ram_component` 字段中添加 `'my_demo'`：
```python
ram_component = [
    # ... 其他组件
    'my_demo',
]
```

#### 6. 创建主要源文件
在 `application/ws63/my_demo/` 目录下创建 `my_demo_main.c`：
```c
#include "pinctrl.h"
#include "gpio.h"
#include "soc_osal.h" 
#include "app_init.h"

static int my_demo_task(const char *arg)
{
    unused(arg);
    
    // 你的应用逻辑
    while (1) {
        osal_msleep(1000);
        osal_printk("My demo is running.\r\n");
    }
    
    return 0;
}

static void my_demo_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    
    task_handle = osal_kthread_create((osal_kthread_handler)my_demo_task, 
                                      0, "MyDemoTask", 0x1000);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, 24);
        osal_kfree(task_handle);
    }
    
    osal_kthread_unlock();
}

app_run(my_demo_entry);
```

#### 7. 编译验证
```bash
# 激活环境
source activate_env.sh

# 编译项目
./build.py -ninja
```

### 目录规范建议
- 将功能相关的源文件放在同一目录下
- 头文件和源文件分离，头文件放在 `include/` 子目录
- 配置文件和资源文件放在 `config/` 子目录
- 测试代码放在 `test/` 子目录

## 常见开发场景

### 最简单的任务开发流程 (Blinky示例)

基于 `application/samples/peripheral/blinky/` 的经典闪烁LED示例，展示WS63最基本的任务开发模式：

#### 1. 基本结构
```c
#include "pinctrl.h"
#include "gpio.h"
#include "soc_osal.h"
#include "app_init.h"

#define TASK_DURATION_MS     500
#define TASK_PRIO           24
#define TASK_STACK_SIZE     0x1000
```

#### 2. 任务函数实现
```c
static int my_task(const char *arg)
{
    unused(arg);
    
    // 配置引脚为GPIO模式
    uapi_pin_set_mode(CONFIG_PIN_NUM, HAL_PIO_FUNC_GPIO);
    
    // 设置GPIO方向和初始值
    uapi_gpio_set_dir(CONFIG_PIN_NUM, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(CONFIG_PIN_NUM, GPIO_LEVEL_LOW);
    
    // 主循环
    while (1) {
        osal_msleep(TASK_DURATION_MS);
        uapi_gpio_toggle(CONFIG_PIN_NUM);
        osal_printk("Task working.\r\n");
    }
    
    return 0;
}
```

#### 3. 任务初始化和启动
```c
static void my_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    
    task_handle = osal_kthread_create((osal_kthread_handler)my_task, 
                                      0, "MyTask", TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, TASK_PRIO);
        osal_kfree(task_handle);
    }
    
    osal_kthread_unlock();
}

// 注册应用入口
app_run(my_entry);
```

#### 4. 开发步骤
1. **创建文件**: 在 `application/samples/` 下创建功能目录
2. **包含头文件**: 根据需要包含相应的驱动头文件
3. **定义常量**: 设置任务优先级、栈大小等参数
4. **实现任务函数**: 编写具体的业务逻辑
5. **创建任务**: 使用OSAL接口创建和配置任务
6. **注册入口**: 使用 `app_run()` 注册应用入口

#### 5. 关键API说明
- `uapi_pin_set_mode()`: 设置引脚功能模式
- `uapi_gpio_set_dir()`: 设置GPIO方向
- `uapi_gpio_set_val()`: 设置GPIO电平
- `uapi_gpio_toggle()`: 翻转GPIO电平
- `osal_kthread_create()`: 创建内核线程
- `osal_msleep()`: 毫秒级延时
- `osal_printk()`: 内核打印函数

### WiFi STA 连接
```c
#include "wifi_device.h"

// 初始化WiFi
wifi_init();

// 扫描网络
wifi_sta_scan();

// 连接网络
wifi_sta_connect(&config);
```

### BLE 服务开发
```c
#include "ohos_bt_gatt_server.h"

// 初始化BLE
BleGattsRegister();

// 创建服务
BleGattsAddService();

// 启动广播
BleStartAdvEx();
```

### 雷达检测
```c
#include "radar_service.h"

// 初始化雷达
radar_init();

// 开始检测
radar_start_detection();
```

## 调试和测试

### 日志系统
- 使用 `PRINT()` 进行日志输出
- 日志级别: ERROR、WARN、INFO、DEBUG
- 日志位置: `middleware/utils/log/`

### 测试框架
- 单元测试: `build/script/`
- 集成测试: `application/samples/`
- AT命令测试: 支持WiFi、BT、SLE、雷达指令

### 调试工具
- GDB调试支持
- 串口调试
- 镜像分析工具

## 配置系统

### Kconfig 配置
- 位置: `build/menuconfig/`
- 使用: `python3 build.py ws63-liteos-app menuconfig`
- 配置文件: `.config`

### 编译宏定义
- 平台宏: `CONFIG_WS63`
- 功能宏: `CONFIG_WIFI_SUPPORT`、`CONFIG_BT_SUPPORT`
- 调试宏: `CONFIG_DEBUG`

## 第三方组件

### 网络组件
- **lwIP**: 轻量级TCP/IP协议栈
- **mbedTLS**: 安全传输层
- **wpa_supplicant**: WiFi安全认证

### 工具库
- **cJSON**: JSON数据处理
- **libcoap**: CoAP协议支持
- **littlefs**: 嵌入式文件系统

## 烧录和部署

### 烧录工具
- BurnTool: 官方烧录工具
- 支持格式: `.bin`、`.elf`

### 分区布局
- Bootloader: 引导程序
- Application: 应用程序
- NV存储: 参数存储
- 文件系统: 数据存储

## 性能优化

### 内存管理
- 堆内存: 动态分配
- 栈内存: 任务栈配置
- 静态内存: 编译时分配

### 功耗优化
- 睡眠模式: 深度睡眠支持
- 时钟管理: 动态调频
- 外设控制: 按需启用

## 常见问题

### 编译问题
1. 确保已运行 `source activate_env.sh`
2. 检查工具链路径配置
3. 清理后重新编译

### 连接问题
1. 检查天线连接
2. 确认频段配置
3. 验证功率设置

### 调试问题  
1. 检查串口波特率
2. 确认日志级别
3. 使用GDB断点调试

## 版本管理

### 版本号格式
- 主版本.次版本.修订版.补丁版
- 当前版本: 0.9.0.5

### 发布流程
1. 代码审查
2. 功能测试
3. 性能验证
4. 文档更新

---

**注意**: 本工作区包含海思专有技术，请遵守相关许可协议和保密要求。开发过程中如遇问题，请参考 `docs/` 目录下的详细文档。
