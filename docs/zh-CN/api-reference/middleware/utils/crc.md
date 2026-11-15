# CRC 计算

**头文件清单**
```c
#include "include/middleware/utils/uapi_crc.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_crc16](#uapi_crc16) | 计算16位CRC (Cyclic Redundancy Check) 校验值 |
| [uapi_crc32](#uapi_crc32) | 计算32位CRC校验值 |
| [uapi_crc32_no_comp](#uapi_crc32_no_comp) | 计算32位CRC校验值（无补码） |

## Functions

### uapi_crc16 <a id="uapi_crc16"></a>

```c
uint16_t uapi_crc16(uint16_t crc_start, const uint8_t *buf, uint32_t length)
```

**头文件清单** ``include/middleware/utils/uapi_crc.h``

**功能说明**

- 计算16位CRC校验值，多项式为x16 + x12 + x5 + 1 (0x1021)
- 支持分段计算，前一段计算结果作为后一段的输入
- 初始值为0

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| crc_start | uint16_t | CRC初始值 | 首次计算时为0，分段计算时为前一次结果 |
| buf | const uint8_t * | 指向待计算数据的指针 | 非NULL |
| length | uint32_t | 数据长度（字节） | 大于0 |

**返回值**

- 返回类型：uint16_t
- CRC计算结果

### uapi_crc32 <a id="uapi_crc32"></a>

```c
uint32_t uapi_crc32(uint32_t crc_start, const uint8_t *buf, uint32_t length)
```

**头文件清单** ``include/middleware/utils/uapi_crc.h``

**功能说明**

- 计算32位CRC校验值，多项式符合IEEE 802.3 CRC-32标准（0x04C11DB7）
- 支持分段计算，前一段计算结果作为后一段的输入
- 初始值为0

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| crc_start | uint32_t | CRC初始值 | 首次计算时为0，分段计算时为前一次结果 |
| buf | const uint8_t * | 指向待计算数据的指针 | 非NULL |
| length | uint32_t | 数据长度（字节） | 大于0 |

**返回值**

- 返回类型：uint32_t
- CRC计算结果

### uapi_crc32_no_comp <a id="uapi_crc32_no_comp"></a>

```c
uint32_t uapi_crc32_no_comp(uint32_t crc_start, const uint8_t *buf, uint32_t length)
```

**头文件清单** ``include/middleware/utils/uapi_crc.h``

**功能说明**

- 计算32位CRC校验值（无补码），多项式符合IEEE 802.3 CRC-32标准（0x04C11DB7）
- 支持分段计算，前一段计算结果作为后一段的输入
- 初始值为0，与uapi_crc32区别在于不对结果取补码

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| crc_start | uint32_t | CRC初始值 | 首次计算时为0，分段计算时为前一次结果 |
| buf | const uint8_t * | 指向待计算数据的指针 | 非NULL |
| length | uint32_t | 数据长度（字节） | 大于0 |

**返回值**

- 返回类型：uint32_t
- CRC计算结果

