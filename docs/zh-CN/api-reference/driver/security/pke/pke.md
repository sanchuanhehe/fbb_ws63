# PKE

PKE (Public Key Engine) 提供公钥密码运算接口，支持 ECC (Elliptic Curve Cryptography)/ECDSA (Elliptic Curve Digital Signature Algorithm)/EdDSA/ECDH (Elliptic Curve Diffie-Hellman)/SM2/RSA (Rivest-Shamir-Adleman)/DH 等非对称算法的密钥生成、签名、验签、加密、解密与密钥协商，以及模加/模减等大数运算。

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

## 接口清单

| 接口名称 | 功能简述 |
| -------- | -------- |
| [uapi_drv_cipher_pke_ecc_gen_key](#uapi_drv_cipher_pke_ecc_gen_key) | 生成ECC密钥对 |
| [uapi_drv_cipher_pke_ecdsa_sign](#uapi_drv_cipher_pke_ecdsa_sign) | ECDSA椭圆曲线数字签名 |
| [uapi_drv_cipher_pke_ecdsa_verify](#uapi_drv_cipher_pke_ecdsa_verify) | ECDSA椭圆曲线数字验签 |
| [uapi_drv_cipher_pke_eddsa_sign](#uapi_drv_cipher_pke_eddsa_sign) | EdDSA爱德华兹曲线数字签名 |
| [uapi_drv_cipher_pke_eddsa_verify](#uapi_drv_cipher_pke_eddsa_verify) | EdDSA爱德华兹曲线数字验签 |
| [uapi_drv_cipher_pke_ecc_gen_ecdh_key](#uapi_drv_cipher_pke_ecc_gen_ecdh_key) | ECDH椭圆曲线密钥协商 |
| [uapi_drv_cipher_pke_check_dot_on_curve](#uapi_drv_cipher_pke_check_dot_on_curve) | 检查点是否在椭圆曲线上 |
| [uapi_drv_cipher_pke_sm2_dsa_hash](#uapi_drv_cipher_pke_sm2_dsa_hash) | SM2杂凑的SM3 (SM3 Cryptographic Hash Algorithm) 摘要计算 |
| [uapi_drv_cipher_pke_sm2_public_encrypt](#uapi_drv_cipher_pke_sm2_public_encrypt) | SM2公钥加密 |
| [uapi_drv_cipher_pke_sm2_private_decrypt](#uapi_drv_cipher_pke_sm2_private_decrypt) | SM2私钥解密 |
| [uapi_drv_cipher_pke_rsa_sign](#uapi_drv_cipher_pke_rsa_sign) | RSA (Rivest-Shamir-Adleman) 签名 |
| [uapi_drv_cipher_pke_rsa_verify](#uapi_drv_cipher_pke_rsa_verify) | RSA验签 |
| [uapi_drv_cipher_pke_rsa_public_encrypt](#uapi_drv_cipher_pke_rsa_public_encrypt) | RSA公钥加密 |
| [uapi_drv_cipher_pke_rsa_private_decrypt](#uapi_drv_cipher_pke_rsa_private_decrypt) | RSA私钥解密 |
| [uapi_drv_cipher_pke_dh_gen_key](#uapi_drv_cipher_pke_dh_gen_key) | DH公私钥对生成或由私钥生成公钥 |
| [uapi_drv_cipher_pke_dh_compute_key](#uapi_drv_cipher_pke_dh_compute_key) | DH密钥协商算法 |
| [uapi_drv_cipher_pke_add_mod](#uapi_drv_cipher_pke_add_mod) | 模加运算 c = (a + b) mod p |
| [uapi_drv_cipher_pke_sub_mod](#uapi_drv_cipher_pke_sub_mod) | 模减运算 c = (a - b) mod p |
| [uapi_drv_cipher_pke_mul_mod](#uapi_drv_cipher_pke_mul_mod) | 模乘运算 c = (a * b) mod p |
| [uapi_drv_cipher_pke_inv_mod](#uapi_drv_cipher_pke_inv_mod) | 模逆运算 c = (a^-1) mod p |
| [uapi_drv_cipher_pke_mod](#uapi_drv_cipher_pke_mod) | 取模运算 c = a mod p |
| [uapi_drv_cipher_pke_mul](#uapi_drv_cipher_pke_mul) | 大数乘法 c = a * b |
| [uapi_drv_cipher_pke_exp_mod](#uapi_drv_cipher_pke_exp_mod) | 模幂运算 out = (in ^ k) mod n |

## Functions

### uapi_drv_cipher_pke_ecc_gen_key <a id="uapi_drv_cipher_pke_ecc_gen_key"></a>

```c
errcode_t uapi_drv_cipher_pke_ecc_gen_key(uapi_drv_cipher_pke_ecc_curve_type_t curve_type, const uapi_drv_cipher_pke_data_t *input_priv_key, const uapi_drv_cipher_pke_data_t *output_priv_key, const uapi_drv_cipher_pke_ecc_point_t *output_pub_key)
```

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

**功能说明**

- 生成ECC密钥对，根据指定的ECC曲线类型生成私钥和公钥
- 输入私钥可以为空指针，为空时由硬件随机生成私钥
- 调用期间会添加睡眠否决标记，防止系统进入深度睡眠，调用结束后移除

**前置条件**

- PKE模块已通过驱动初始化完成，CONFIG_PKE_SUPPORT_ECC_GEN_KEY宏已开启
- 调用方需确保output_priv_key和output_pub_key指向的缓冲区已申请成功，且长度满足对应曲线的密钥长度要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| curve_type | [uapi_drv_cipher_pke_ecc_curve_type_t](#enum_uapi_drv_cipher_pke_ecc_curve_type_t) | ECC曲线类型 | UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P256(0) ~ UAPI_DRV_CIPHER_PKE_ECC_TYPE_SM2(12) |
| input_priv_key | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输入私钥，可以为空指针 | NULL或有效的data指针，length对应曲线密钥长度 |
| output_priv_key | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输出私钥 | 非NULL，data指向已申请缓冲区，length对应曲线密钥长度 |
| output_pub_key | const [uapi_drv_cipher_pke_ecc_point_t](#struct_uapi_drv_cipher_pke_ecc_point_t) * | 输出公钥 | 非NULL，x、y指向已申请缓冲区，length对应曲线密钥长度 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| output_priv_key | [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输出的私钥数据 |
| output_pub_key | [uapi_drv_cipher_pke_ecc_point_t](#struct_uapi_drv_cipher_pke_ecc_point_t) * | 输出的公钥数据，包含X坐标和Y坐标 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | ECC密钥对生成成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PKE_SUPPORT_ECC_GEN_KEY | 功能宏 | 支持ECC密钥对生成功能 | y |

### uapi_drv_cipher_pke_ecdsa_sign <a id="uapi_drv_cipher_pke_ecdsa_sign"></a>

```c
errcode_t uapi_drv_cipher_pke_ecdsa_sign(uapi_drv_cipher_pke_ecc_curve_type_t curve_type, const uapi_drv_cipher_pke_data_t *priv_key, const uapi_drv_cipher_pke_data_t *hash, const uapi_drv_cipher_pke_ecc_sig_t *sig)
```

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

**功能说明**

- 使用ECDSA算法对摘要数据进行椭圆曲线数字签名
- 输入私钥和摘要数据，输出签名值R和S
- 调用期间会添加睡眠否决标记，防止系统进入深度睡眠，调用结束后移除

**前置条件**

- PKE模块已通过驱动初始化完成
- 调用方需确保priv_key、hash、sig指向的缓冲区已申请成功，且长度满足对应曲线要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| curve_type | [uapi_drv_cipher_pke_ecc_curve_type_t](#enum_uapi_drv_cipher_pke_ecc_curve_type_t) | ECC曲线类型 | UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P256(0) ~ UAPI_DRV_CIPHER_PKE_ECC_TYPE_SM2(12) |
| priv_key | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输入私钥 | 非NULL，length对应曲线密钥长度 |
| hash | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输入摘要 | 非NULL，length对应摘要算法输出长度 |
| sig | const [uapi_drv_cipher_pke_ecc_sig_t](#struct_uapi_drv_cipher_pke_ecc_sig_t) * | 输出签名 | 非NULL，r、s指向已申请缓冲区，length对应曲线签名长度 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| sig | [uapi_drv_cipher_pke_ecc_sig_t](#struct_uapi_drv_cipher_pke_ecc_sig_t) * | 输出的签名值，包含R和S分量 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | ECDSA签名成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PKE_SUPPORT_ECC_SIGN | 功能宏 | 支持ECC签名功能 | y |

### uapi_drv_cipher_pke_ecdsa_verify <a id="uapi_drv_cipher_pke_ecdsa_verify"></a>

```c
errcode_t uapi_drv_cipher_pke_ecdsa_verify(uapi_drv_cipher_pke_ecc_curve_type_t curve_type, const uapi_drv_cipher_pke_ecc_point_t *pub_key, const uapi_drv_cipher_pke_data_t *hash, const uapi_drv_cipher_pke_ecc_sig_t *sig)
```

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

**功能说明**

- 使用ECDSA算法对签名数据进行椭圆曲线数字验签
- 输入公钥、摘要和签名值，验证签名是否合法
- 调用期间会添加睡眠否决标记，防止系统进入深度睡眠，调用结束后移除

**前置条件**

- PKE模块已通过驱动初始化完成
- 调用方需确保pub_key、hash、sig指向的缓冲区已申请成功，且长度满足对应曲线要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| curve_type | [uapi_drv_cipher_pke_ecc_curve_type_t](#enum_uapi_drv_cipher_pke_ecc_curve_type_t) | ECC曲线类型 | UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P256(0) ~ UAPI_DRV_CIPHER_PKE_ECC_TYPE_SM2(12) |
| pub_key | const [uapi_drv_cipher_pke_ecc_point_t](#struct_uapi_drv_cipher_pke_ecc_point_t) * | 输入公钥 | 非NULL，x、y指向有效数据，length对应曲线密钥长度 |
| hash | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输入摘要 | 非NULL，length对应摘要算法输出长度 |
| sig | const [uapi_drv_cipher_pke_ecc_sig_t](#struct_uapi_drv_cipher_pke_ecc_sig_t) * | 输入签名 | 非NULL，r、s指向有效数据，length对应曲线签名长度 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | ECDSA验签通过 |
| Other | 其他错误码，参考errcode_t | 验签失败或执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PKE_SUPPORT_ECC_VERIFY | 功能宏 | 支持ECC验签功能 | y |

### uapi_drv_cipher_pke_eddsa_sign <a id="uapi_drv_cipher_pke_eddsa_sign"></a>

```c
errcode_t uapi_drv_cipher_pke_eddsa_sign(uapi_drv_cipher_pke_ecc_curve_type_t curve_type, const uapi_drv_cipher_pke_data_t *priv_key, const uapi_drv_cipher_pke_msg_t *msg, const uapi_drv_cipher_pke_ecc_sig_t *sig)
```

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

**功能说明**

- 使用EdDSA算法对消息数据进行爱德华兹曲线数字签名
- 输入私钥和原始消息（非摘要），输出签名值R和S
- 调用期间会添加睡眠否决标记，防止系统进入深度睡眠，调用结束后移除

**前置条件**

- PKE模块已通过驱动初始化完成，CONFIG_PKE_SUPPORT_EDWARD宏已开启
- 调用方需确保priv_key、msg、sig指向的缓冲区已申请成功，且长度满足对应曲线要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| curve_type | [uapi_drv_cipher_pke_ecc_curve_type_t](#enum_uapi_drv_cipher_pke_ecc_curve_type_t) | ECC曲线类型 | UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC8032(10) |
| priv_key | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输入私钥 | 非NULL，length对应曲线密钥长度 |
| msg | const [uapi_drv_cipher_pke_msg_t](#struct_uapi_drv_cipher_pke_msg_t) * | 输入消息 | 非NULL，data指向有效消息数据，length对应消息长度 |
| sig | const [uapi_drv_cipher_pke_ecc_sig_t](#struct_uapi_drv_cipher_pke_ecc_sig_t) * | 输出签名 | 非NULL，r、s指向已申请缓冲区，length对应曲线签名长度 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| sig | [uapi_drv_cipher_pke_ecc_sig_t](#struct_uapi_drv_cipher_pke_ecc_sig_t) * | 输出的签名值，包含R和S分量 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | EdDSA签名成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PKE_SUPPORT_EDWARD | 功能宏 | 支持EdDSA爱德华兹曲线签名验签功能 | y |

### uapi_drv_cipher_pke_eddsa_verify <a id="uapi_drv_cipher_pke_eddsa_verify"></a>

```c
errcode_t uapi_drv_cipher_pke_eddsa_verify(uapi_drv_cipher_pke_ecc_curve_type_t curve_type, const uapi_drv_cipher_pke_ecc_point_t *pub_key, const uapi_drv_cipher_pke_msg_t *msg, const uapi_drv_cipher_pke_ecc_sig_t *sig)
```

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

**功能说明**

- 使用EdDSA算法对签名数据进行爱德华兹曲线数字验签
- 输入公钥、原始消息和签名值，验证签名是否合法
- 调用期间会添加睡眠否决标记，防止系统进入深度睡眠，调用结束后移除

**前置条件**

- PKE模块已通过驱动初始化完成，CONFIG_PKE_SUPPORT_EDWARD宏已开启
- 调用方需确保pub_key、msg、sig指向的缓冲区已申请成功，且长度满足对应曲线要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| curve_type | [uapi_drv_cipher_pke_ecc_curve_type_t](#enum_uapi_drv_cipher_pke_ecc_curve_type_t) | ECC曲线类型 | UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC8032(10) |
| pub_key | const [uapi_drv_cipher_pke_ecc_point_t](#struct_uapi_drv_cipher_pke_ecc_point_t) * | 输入公钥 | 非NULL，x、y指向有效数据，length对应曲线密钥长度 |
| msg | const [uapi_drv_cipher_pke_msg_t](#struct_uapi_drv_cipher_pke_msg_t) * | 输入消息 | 非NULL，data指向有效消息数据，length对应消息长度 |
| sig | const [uapi_drv_cipher_pke_ecc_sig_t](#struct_uapi_drv_cipher_pke_ecc_sig_t) * | 输入签名 | 非NULL，r、s指向有效数据，length对应曲线签名长度 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | EdDSA验签通过 |
| Other | 其他错误码，参考errcode_t | 验签失败或执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PKE_SUPPORT_EDWARD | 功能宏 | 支持EdDSA爱德华兹曲线签名验签功能 | y |
**出参**

无

### uapi_drv_cipher_pke_ecc_gen_ecdh_key <a id="uapi_drv_cipher_pke_ecc_gen_ecdh_key"></a>

```c
errcode_t uapi_drv_cipher_pke_ecc_gen_ecdh_key(uapi_drv_cipher_pke_ecc_curve_type_t curve_type, const uapi_drv_cipher_pke_ecc_point_t *input_pub_key, const uapi_drv_cipher_pke_data_t *input_priv_key, const uapi_drv_cipher_pke_data_t *output_shared_key)
```

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

**功能说明**

- 使用ECDH算法进行椭圆曲线密钥协商，生成共享密钥
- 输入对端公钥和本地私钥，输出协商后的共享密钥
- 调用期间会添加睡眠否决标记，防止系统进入深度睡眠，调用结束后移除

**前置条件**

- PKE模块已通过驱动初始化完成，CONFIG_PKE_SUPPORT_ECC_ECDH宏已开启
- 调用方需确保input_pub_key、input_priv_key、output_shared_key指向的缓冲区已申请成功，且长度满足对应曲线要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| curve_type | [uapi_drv_cipher_pke_ecc_curve_type_t](#enum_uapi_drv_cipher_pke_ecc_curve_type_t) | ECC曲线类型 | UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P256(0) ~ UAPI_DRV_CIPHER_PKE_ECC_TYPE_SM2(12) |
| input_pub_key | const [uapi_drv_cipher_pke_ecc_point_t](#struct_uapi_drv_cipher_pke_ecc_point_t) * | 输入公钥 | 非NULL，x、y指向有效数据，length对应曲线密钥长度 |
| input_priv_key | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输入私钥 | 非NULL，length对应曲线密钥长度 |
| output_shared_key | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输出协商密钥 | 非NULL，data指向已申请缓冲区，length对应曲线密钥长度 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| output_shared_key | [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 协商生成的共享密钥 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | ECDH密钥协商成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PKE_SUPPORT_ECC_ECDH | 功能宏 | 支持ECDH密钥协商功能 | y |

### uapi_drv_cipher_pke_check_dot_on_curve <a id="uapi_drv_cipher_pke_check_dot_on_curve"></a>

```c
errcode_t uapi_drv_cipher_pke_check_dot_on_curve(uapi_drv_cipher_pke_ecc_curve_type_t curve_type, const uapi_drv_cipher_pke_ecc_point_t *pub_key, bool *is_on_curve)
```

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

**功能说明**

- 检查给定的点是否在指定的椭圆曲线上
- 输入公钥点坐标，输出布尔值表示该点是否在曲线上
- 调用期间会添加睡眠否决标记，防止系统进入深度睡眠，调用结束后移除

**前置条件**

- PKE模块已通过驱动初始化完成，CONFIG_PKE_SUPPORT_ECC_CAL宏已开启
- 调用方需确保pub_key指向的缓冲区已申请成功，is_on_curve指针非NULL

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| curve_type | [uapi_drv_cipher_pke_ecc_curve_type_t](#enum_uapi_drv_cipher_pke_ecc_curve_type_t) | ECC曲线类型 | UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P256(0) ~ UAPI_DRV_CIPHER_PKE_ECC_TYPE_SM2(12) |
| pub_key | const [uapi_drv_cipher_pke_ecc_point_t](#struct_uapi_drv_cipher_pke_ecc_point_t) * | 输入公钥点 | 非NULL，x、y指向有效数据，length对应曲线密钥长度 |
| is_on_curve | bool * | 输出结果 | 非NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| is_on_curve | bool * | 点是否在椭圆曲线上，true表示在曲线上，false表示不在曲线上 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 检查操作完成 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PKE_SUPPORT_ECC_CAL | 功能宏 | 支持ECC点校验功能 | y |

### uapi_drv_cipher_pke_sm2_dsa_hash <a id="uapi_drv_cipher_pke_sm2_dsa_hash"></a>

```c
errcode_t uapi_drv_cipher_pke_sm2_dsa_hash(const uapi_drv_cipher_pke_data_t *sm2_id, const uapi_drv_cipher_pke_ecc_point_t *pub_key, const uapi_drv_cipher_pke_msg_t *msg, uapi_drv_cipher_pke_data_t *hash)
```

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

**功能说明**

- 计算SM2杂凑的SM3摘要，用于SM2签名验签前的摘要预处理
- 输入SM2 ID、公钥和消息，输出SM3杂凑摘要
- 调用期间会添加睡眠否决标记，防止系统进入深度睡眠，调用结束后移除

**前置条件**

- PKE模块已通过驱动初始化完成，CONFIG_PKE_SUPPORT_SM2_SIGN或CONFIG_PKE_SUPPORT_SM2_VERIFY宏至少开启一个
- 调用方需确保sm2_id、pub_key、msg、hash指向的缓冲区已申请成功，且长度满足SM2要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| sm2_id | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | SM2 ID | 非NULL，data指向有效ID数据 |
| pub_key | const [uapi_drv_cipher_pke_ecc_point_t](#struct_uapi_drv_cipher_pke_ecc_point_t) * | 输入公钥 | 非NULL，x、y指向有效数据，length对应SM2密钥长度 |
| msg | const [uapi_drv_cipher_pke_msg_t](#struct_uapi_drv_cipher_pke_msg_t) * | 输入消息 | 非NULL，data指向有效消息数据 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| hash | [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输出的SM3杂凑摘要 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | SM3摘要计算成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PKE_SUPPORT_SM2_SIGN | 功能宏 | 支持SM2签名功能 | y |
| CONFIG_PKE_SUPPORT_SM2_VERIFY | 功能宏 | 支持SM2验签功能 | y |

### uapi_drv_cipher_pke_sm2_public_encrypt <a id="uapi_drv_cipher_pke_sm2_public_encrypt"></a>

```c
errcode_t uapi_drv_cipher_pke_sm2_public_encrypt(const uapi_drv_cipher_pke_ecc_point_t *pub_key, const uapi_drv_cipher_pke_data_t *plain_text, const uapi_drv_cipher_pke_data_t *cipher_text)
```

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

**功能说明**

- 使用SM2公钥加密算法对明文数据进行加密
- 输入公钥和明文，输出密文
- 调用期间会添加睡眠否决标记，防止系统进入深度睡眠，调用结束后移除

**前置条件**

- PKE模块已通过驱动初始化完成，CONFIG_PKE_SUPPORT_SM2_CRYPTO宏已开启
- 调用方需确保pub_key、plain_text、cipher_text指向的缓冲区已申请成功，cipher_text缓冲区长度不小于SM2密文长度要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pub_key | const [uapi_drv_cipher_pke_ecc_point_t](#struct_uapi_drv_cipher_pke_ecc_point_t) * | 输入公钥 | 非NULL，x、y指向有效数据，length对应SM2密钥长度 |
| plain_text | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输入明文 | 非NULL，data指向有效明文数据 |
| cipher_text | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输出密文 | 非NULL，data指向已申请缓冲区，长度不小于SM2密文长度 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| cipher_text | [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输出的SM2密文数据 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | SM2加密成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PKE_SUPPORT_SM2_CRYPTO | 功能宏 | 支持SM2加解密功能 | y |

### uapi_drv_cipher_pke_sm2_private_decrypt <a id="uapi_drv_cipher_pke_sm2_private_decrypt"></a>

```c
errcode_t uapi_drv_cipher_pke_sm2_private_decrypt(const uapi_drv_cipher_pke_data_t *priv_key, const uapi_drv_cipher_pke_data_t *cipher_text, const uapi_drv_cipher_pke_data_t *plain_text)
```

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

**功能说明**

- 使用SM2私钥解密算法对密文数据进行解密
- 输入私钥和密文，输出明文
- 调用期间会添加睡眠否决标记，防止系统进入深度睡眠，调用结束后移除

**前置条件**

- PKE模块已通过驱动初始化完成，CONFIG_PKE_SUPPORT_SM2_CRYPTO宏已开启
- 调用方需确保priv_key、cipher_text、plain_text指向的缓冲区已申请成功，plain_text缓冲区长度不小于SM2明文长度要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| priv_key | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输入私钥 | 非NULL，length对应SM2密钥长度 |
| cipher_text | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输入密文 | 非NULL，data指向有效密文数据 |
| plain_text | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输出明文 | 非NULL，data指向已申请缓冲区，长度不小于SM2明文长度 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| plain_text | [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输出的SM2明文数据 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | SM2解密成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PKE_SUPPORT_SM2_CRYPTO | 功能宏 | 支持SM2加解密功能 | y |

### uapi_drv_cipher_pke_rsa_sign <a id="uapi_drv_cipher_pke_rsa_sign"></a>

```c
errcode_t uapi_drv_cipher_pke_rsa_sign(const uapi_drv_cipher_pke_rsa_priv_key_t *priv_key, uapi_drv_cipher_pke_rsa_scheme_t scheme, uapi_drv_cipher_pke_hash_type_t hash_type, const uapi_drv_cipher_pke_data_t *input_hash, uapi_drv_cipher_pke_data_t *sign)
```

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

**功能说明**

- 使用RSA私钥对摘要数据进行签名
- 输入私钥、填充方式、摘要算法和摘要数据，输出签名结果
- 调用期间会添加睡眠否决标记，防止系统进入深度睡眠，调用结束后移除

**前置条件**

- PKE模块已通过驱动初始化完成，CONFIG_PKE_SUPPORT_RSA宏已开启
- 调用方需确保priv_key、input_hash、sign指向的缓冲区已申请成功，且长度满足RSA密钥要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| priv_key | const [uapi_drv_cipher_pke_rsa_priv_key_t](#struct_uapi_drv_cipher_pke_rsa_priv_key_t) * | 输入私钥 | 非NULL，各参数指针非NULL，长度满足RSA密钥规格 |
| scheme | [uapi_drv_cipher_pke_rsa_scheme_t](#enum_uapi_drv_cipher_pke_rsa_scheme_t) | RSA填充方式 | UAPI_DRV_CIPHER_PKE_RSA_SCHEME_PKCS1_V15(0x00)或UAPI_DRV_CIPHER_PKE_RSA_SCHEME_PKCS1_V21(0x01) |
| hash_type | [uapi_drv_cipher_pke_hash_type_t](#enum_uapi_drv_cipher_pke_hash_type_t) | RSA填充使用的摘要算法 | UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA1(0x00) ~ UAPI_DRV_CIPHER_PKE_HASH_TYPE_SM3(0x05) |
| input_hash | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输入摘要 | 非NULL，length对应摘要算法输出长度 |
| sign | [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输出签名 | 非NULL，data指向已申请缓冲区，length对应RSA密钥长度 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| sign | [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输出的RSA签名结果 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | RSA签名成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PKE_SUPPORT_RSA | 功能宏 | 支持RSA功能 | y |

### uapi_drv_cipher_pke_rsa_verify <a id="uapi_drv_cipher_pke_rsa_verify"></a>

```c
errcode_t uapi_drv_cipher_pke_rsa_verify(const uapi_drv_cipher_pke_rsa_pub_key_t *pub_key, uapi_drv_cipher_pke_rsa_scheme_t scheme, uapi_drv_cipher_pke_hash_type_t hash_type, uapi_drv_cipher_pke_data_t *input_hash, const uapi_drv_cipher_pke_data_t *sig)
```

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

**功能说明**

- 使用RSA公钥对签名数据进行验签
- 输入公钥、填充方式、摘要算法、摘要和签名值，验证签名是否合法
- 调用期间会添加睡眠否决标记，防止系统进入深度睡眠，调用结束后移除

**前置条件**

- PKE模块已通过驱动初始化完成，CONFIG_PKE_SUPPORT_RSA宏已开启
- 调用方需确保pub_key、input_hash、sig指向的缓冲区已申请成功，且长度满足RSA密钥要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| pub_key | const [uapi_drv_cipher_pke_rsa_pub_key_t](#struct_uapi_drv_cipher_pke_rsa_pub_key_t) * | 输入公钥 | 非NULL，n、e指向有效数据，len对应RSA密钥长度 |
| scheme | [uapi_drv_cipher_pke_rsa_scheme_t](#enum_uapi_drv_cipher_pke_rsa_scheme_t) | RSA填充方式 | UAPI_DRV_CIPHER_PKE_RSA_SCHEME_PKCS1_V15(0x00)或UAPI_DRV_CIPHER_PKE_RSA_SCHEME_PKCS1_V21(0x01) |
| hash_type | [uapi_drv_cipher_pke_hash_type_t](#enum_uapi_drv_cipher_pke_hash_type_t) | RSA填充使用的摘要算法 | UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA1(0x00) ~ UAPI_DRV_CIPHER_PKE_HASH_TYPE_SM3(0x05) |
| input_hash | [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输入摘要 | 非NULL，length对应摘要算法输出长度 |
| sig | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输入签名 | 非NULL，data指向有效签名数据，length对应RSA密钥长度 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | RSA验签通过 |
| Other | 其他错误码，参考errcode_t | 验签失败或执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PKE_SUPPORT_RSA | 功能宏 | 支持RSA功能 | y |
**出参**

无

### uapi_drv_cipher_pke_rsa_public_encrypt <a id="uapi_drv_cipher_pke_rsa_public_encrypt"></a>

```c
errcode_t uapi_drv_cipher_pke_rsa_public_encrypt(uapi_drv_cipher_pke_rsa_scheme_t scheme, uapi_drv_cipher_pke_hash_type_t hash_type, const uapi_drv_cipher_pke_rsa_pub_key_t *pub_key, const uapi_drv_cipher_pke_data_t *input, const uapi_drv_cipher_pke_data_t *label, uapi_drv_cipher_pke_data_t *output)
```

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

**功能说明**

- 使用RSA公钥对明文数据进行加密
- 输入填充方式、摘要算法、公钥、明文和标签，输出密文
- hash_type和label仅OAEP (Optimal Asymmetric Encryption Padding) 填充模式时使用
- 调用期间会添加睡眠否决标记，防止系统进入深度睡眠，调用结束后移除

**前置条件**

- PKE模块已通过驱动初始化完成，CONFIG_PKE_SUPPORT_RSA宏已开启
- 调用方需确保pub_key、input、output指向的缓冲区已申请成功，output长度不小于RSA密钥长度

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| scheme | [uapi_drv_cipher_pke_rsa_scheme_t](#enum_uapi_drv_cipher_pke_rsa_scheme_t) | RSA填充方式 | UAPI_DRV_CIPHER_PKE_RSA_SCHEME_PKCS1_V15(0x00)或UAPI_DRV_CIPHER_PKE_RSA_SCHEME_PKCS1_V21(0x01) |
| hash_type | [uapi_drv_cipher_pke_hash_type_t](#enum_uapi_drv_cipher_pke_hash_type_t) | RSA填充使用的摘要算法，仅OAEP填充模式时使用 | UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA1(0x00) ~ UAPI_DRV_CIPHER_PKE_HASH_TYPE_SM3(0x05) |
| pub_key | const [uapi_drv_cipher_pke_rsa_pub_key_t](#struct_uapi_drv_cipher_pke_rsa_pub_key_t) * | 输入公钥 | 非NULL，n、e指向有效数据，len对应RSA密钥长度 |
| input | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输入明文 | 非NULL，data指向有效明文数据 |
| label | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | RSA标签，仅OAEP填充模式时使用 | 可为NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| output | [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输出的RSA密文数据 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | RSA加密成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PKE_SUPPORT_RSA | 功能宏 | 支持RSA功能 | y |

### uapi_drv_cipher_pke_rsa_private_decrypt <a id="uapi_drv_cipher_pke_rsa_private_decrypt"></a>

```c
errcode_t uapi_drv_cipher_pke_rsa_private_decrypt(uapi_drv_cipher_pke_rsa_scheme_t scheme, uapi_drv_cipher_pke_hash_type_t hash_type, const uapi_drv_cipher_pke_rsa_priv_key_t *priv_key, const uapi_drv_cipher_pke_data_t *input, const uapi_drv_cipher_pke_data_t *label, const uapi_drv_cipher_pke_data_t *output)
```

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

**功能说明**

- 使用RSA私钥对密文数据进行解密
- 输入填充方式、摘要算法、私钥、密文和标签，输出明文
- hash_type和label仅OAEP填充模式时使用
- 调用期间会添加睡眠否决标记，防止系统进入深度睡眠，调用结束后移除

**前置条件**

- PKE模块已通过驱动初始化完成，CONFIG_PKE_SUPPORT_RSA宏已开启
- 调用方需确保priv_key、input、output指向的缓冲区已申请成功，output长度不小于RSA明文长度

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| scheme | [uapi_drv_cipher_pke_rsa_scheme_t](#enum_uapi_drv_cipher_pke_rsa_scheme_t) | RSA填充方式 | UAPI_DRV_CIPHER_PKE_RSA_SCHEME_PKCS1_V15(0x00)或UAPI_DRV_CIPHER_PKE_RSA_SCHEME_PKCS1_V21(0x01) |
| hash_type | [uapi_drv_cipher_pke_hash_type_t](#enum_uapi_drv_cipher_pke_hash_type_t) | RSA填充使用的摘要算法，仅OAEP填充模式时使用 | UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA1(0x00) ~ UAPI_DRV_CIPHER_PKE_HASH_TYPE_SM3(0x05) |
| priv_key | const [uapi_drv_cipher_pke_rsa_priv_key_t](#struct_uapi_drv_cipher_pke_rsa_priv_key_t) * | 输入私钥 | 非NULL，各参数指针非NULL，长度满足RSA密钥规格 |
| input | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输入密文 | 非NULL，data指向有效密文数据 |
| label | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | RSA标签，仅OAEP填充模式时使用 | 可为NULL |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| output | [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输出的RSA明文数据 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | RSA解密成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PKE_SUPPORT_RSA | 功能宏 | 支持RSA功能 | y |

### uapi_drv_cipher_pke_dh_gen_key <a id="uapi_drv_cipher_pke_dh_gen_key"></a>

```c
errcode_t uapi_drv_cipher_pke_dh_gen_key(const uapi_drv_cipher_pke_data_t *g_data, const uapi_drv_cipher_pke_data_t *mod_n, const uapi_drv_cipher_pke_data_t *input_priv_key, const uapi_drv_cipher_pke_data_t *output_priv_key, const uapi_drv_cipher_pke_data_t *output_pub_key)
```

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

**功能说明**

- DH公私钥对生成或由私钥生成公钥
- 输入公开底数和模数，生成私钥和公钥；若输入私钥非空则由私钥生成公钥
- 模数的有效字节数与mod_n->length支持的规格之差不能大于2字节
- 调用期间会添加睡眠否决标记，防止系统进入深度睡眠，调用结束后移除

**前置条件**

- PKE模块已通过驱动初始化完成，CONFIG_PKE_SUPPORT_RSA宏已开启
- 调用方需确保g_data、mod_n、output_priv_key、output_pub_key指向的缓冲区已申请成功，且长度满足要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| g_data | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 公开的底数，该数为质数 | 非NULL，g_data->length不能大于mod_n->length |
| mod_n | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 公开的模数，该数为质数 | 非NULL，仅支持192/224/256/384/512/521/1024/2048/3072/4096bits |
| input_priv_key | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输入私钥，可以为空 | NULL或有效的data指针，length与mod_n->length相同 |
| output_priv_key | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输出私钥 | 非NULL，length与mod_n->length相同 |
| output_pub_key | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输出公钥 | 非NULL，length与mod_n->length相同 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| output_priv_key | [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 生成的私钥数据 |
| output_pub_key | [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 生成的公钥数据 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | DH密钥生成成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PKE_SUPPORT_RSA | 功能宏 | 支持RSA/DH功能 | y |

### uapi_drv_cipher_pke_dh_compute_key <a id="uapi_drv_cipher_pke_dh_compute_key"></a>

```c
errcode_t uapi_drv_cipher_pke_dh_compute_key(const uapi_drv_cipher_pke_data_t *mod_n, const uapi_drv_cipher_pke_data_t *input_priv_key, const uapi_drv_cipher_pke_data_t *input_pub_key, const uapi_drv_cipher_pke_data_t *output_shared_key)
```

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

**功能说明**

- DH密钥协商算法，根据本地私钥和对端公钥计算共享密钥
- 输入模数、本地私钥和对端公钥，输出共享密钥
- 调用期间会添加睡眠否决标记，防止系统进入深度睡眠，调用结束后移除

**前置条件**

- PKE模块已通过驱动初始化完成，CONFIG_PKE_SUPPORT_RSA宏已开启
- 调用方需确保mod_n、input_priv_key、input_pub_key、output_shared_key指向的缓冲区已申请成功，且长度满足要求

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| mod_n | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 公开的模数，该数为质数 | 非NULL |
| input_priv_key | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输入私钥 | 非NULL |
| input_pub_key | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输入公钥 | 非NULL |
| output_shared_key | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输出共享密钥 | 非NULL，data指向已申请缓冲区 |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| output_shared_key | [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 协商生成的共享密钥 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | DH密钥协商成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PKE_SUPPORT_RSA | 功能宏 | 支持RSA/DH功能 | y |

### uapi_drv_cipher_pke_add_mod <a id="uapi_drv_cipher_pke_add_mod"></a>

```c
errcode_t uapi_drv_cipher_pke_add_mod(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *b, const uapi_drv_cipher_pke_data_t *p, const uapi_drv_cipher_pke_data_t *c)
```

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

**功能说明**

- 模加运算 c = (a + b) mod p
- 输入两个大数和模数，输出模加结果
- 模数的有效字节数与p->length支持的规格之差不能大于2字节
- 调用期间会添加睡眠否决标记，防止系统进入深度睡眠，调用结束后移除

**前置条件**

- PKE模块已通过驱动初始化完成，CONFIG_PKE_SUPPORT_BIG_NUMBER宏已开启
- 调用方需确保a、b、p、c指向的缓冲区已申请成功，且c->length等于p->length

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| a | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 入参a | 非NULL，a->length不能大于p->length |
| b | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 入参b | 非NULL，b->length不能大于p->length |
| p | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 模数 | 非NULL，仅支持192/224/256/384/512/521/1024/1536/2048/3072/4096bits |
| c | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 计算结果 | 非NULL，c->length等于p->length |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| c | [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 模加运算结果 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 模加运算成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PKE_SUPPORT_BIG_NUMBER | 功能宏 | 支持大数运算功能 | y |

### uapi_drv_cipher_pke_sub_mod <a id="uapi_drv_cipher_pke_sub_mod"></a>

```c
errcode_t uapi_drv_cipher_pke_sub_mod(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *b, const uapi_drv_cipher_pke_data_t *p, const uapi_drv_cipher_pke_data_t *c)
```

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

**功能说明**

- 模减运算 c = (a - b) mod p
- 输入两个大数和模数，输出模减结果
- 模数的有效字节数与p->length支持的规格之差不能大于2字节
- 调用期间会添加睡眠否决标记，防止系统进入深度睡眠，调用结束后移除

**前置条件**

- PKE模块已通过驱动初始化完成，CONFIG_PKE_SUPPORT_BIG_NUMBER宏已开启
- 调用方需确保a、b、p、c指向的缓冲区已申请成功，且c->length等于p->length

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| a | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 入参a | 非NULL，a->length不能大于p->length |
| b | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 入参b | 非NULL，b->length不能大于p->length |
| p | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 模数 | 非NULL，仅支持192/224/256/384/512/521/1024/1536/2048/3072/4096bits |
| c | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 计算结果 | 非NULL，c->length等于p->length |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| c | [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 模减运算结果 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 模减运算成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PKE_SUPPORT_BIG_NUMBER | 功能宏 | 支持大数运算功能 | y |

### uapi_drv_cipher_pke_mul_mod <a id="uapi_drv_cipher_pke_mul_mod"></a>

```c
errcode_t uapi_drv_cipher_pke_mul_mod(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *b, const uapi_drv_cipher_pke_data_t *p, const uapi_drv_cipher_pke_data_t *c)
```

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

**功能说明**

- 模乘运算 c = (a * b) mod p
- 输入两个大数和模数，输出模乘结果
- 模数的有效字节数与p->length支持的规格之差不能大于2字节，且模数不能为偶数
- 调用期间会添加睡眠否决标记，防止系统进入深度睡眠，调用结束后移除

**前置条件**

- PKE模块已通过驱动初始化完成，CONFIG_PKE_SUPPORT_BIG_NUMBER宏已开启
- 调用方需确保a、b、p、c指向的缓冲区已申请成功，且c->length等于p->length

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| a | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 入参a | 非NULL，a->length不能大于p->length |
| b | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 入参b | 非NULL，b->length不能大于p->length |
| p | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 模数 | 非NULL，仅支持192/256/384/512/1024/1536/2048/3072/4096bits，且模数不能为偶数 |
| c | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 计算结果 | 非NULL，c->length等于p->length |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| c | [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 模乘运算结果 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 模乘运算成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PKE_SUPPORT_BIG_NUMBER | 功能宏 | 支持大数运算功能 | y |

### uapi_drv_cipher_pke_inv_mod <a id="uapi_drv_cipher_pke_inv_mod"></a>

```c
errcode_t uapi_drv_cipher_pke_inv_mod(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *p, const uapi_drv_cipher_pke_data_t *c)
```

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

**功能说明**

- 模逆运算 c = (a^-1) mod p
- 输入大数和模数，输出模逆结果
- 模数的有效字节数与p->length支持的规格之差不能大于2字节，且模数不能为偶数
- 调用期间会添加睡眠否决标记，防止系统进入深度睡眠，调用结束后移除

**前置条件**

- PKE模块已通过驱动初始化完成，CONFIG_PKE_SUPPORT_BIG_NUMBER宏已开启
- 调用方需确保a、p、c指向的缓冲区已申请成功，且a->length等于p->length，c->length等于p->length

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| a | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 入参 | 非NULL，a->length等于p->length |
| p | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 模数 | 非NULL，仅支持192/256/384/512/1024/1536/2048/3072/4096bits，且模数不能为偶数 |
| c | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 计算结果 | 非NULL，c->length等于p->length |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| c | [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 模逆运算结果 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 模逆运算成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PKE_SUPPORT_BIG_NUMBER | 功能宏 | 支持大数运算功能 | y |

### uapi_drv_cipher_pke_mod <a id="uapi_drv_cipher_pke_mod"></a>

```c
errcode_t uapi_drv_cipher_pke_mod(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *p, const uapi_drv_cipher_pke_data_t *c)
```

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

**功能说明**

- 取模运算 c = a mod p
- 输入大数和模数，输出取模结果
- 模数的有效字节数与p->length支持的规格之差不能大于2字节，且模数不能为偶数
- 调用期间会添加睡眠否决标记，防止系统进入深度睡眠，调用结束后移除

**前置条件**

- PKE模块已通过驱动初始化完成，CONFIG_PKE_SUPPORT_BIG_NUMBER宏已开启
- 调用方需确保a、p、c指向的缓冲区已申请成功，a有效数据长度不能大于2倍模数有效数据长度，c->length等于p->length

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| a | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 入参 | 非NULL，有效数据长度不能大于2倍模数的有效数据长度 |
| p | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 模数 | 非NULL，仅支持192/256/384/512/1024/1536/2048/3072/4096bits，且模数不能为偶数 |
| c | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 计算结果 | 非NULL，c->length等于p->length |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| c | [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 取模运算结果 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 取模运算成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PKE_SUPPORT_BIG_NUMBER | 功能宏 | 支持大数运算功能 | y |

### uapi_drv_cipher_pke_mul <a id="uapi_drv_cipher_pke_mul"></a>

```c
errcode_t uapi_drv_cipher_pke_mul(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *b, const uapi_drv_cipher_pke_data_t *c)
```

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

**功能说明**

- 大数乘法 c = a * b
- 输入两个大数，输出乘法结果
- 调用期间会添加睡眠否决标记，防止系统进入深度睡眠，调用结束后移除

**前置条件**

- PKE模块已通过驱动初始化完成，CONFIG_PKE_SUPPORT_BIG_NUMBER宏已开启
- 调用方需确保a、b、c指向的缓冲区已申请成功，c->length不能小于a->length + b->length

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| a | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 入参a | 非NULL，a->length不能大于2048bits |
| b | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 入参b | 非NULL，b->length不能大于2048bits |
| c | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 计算结果 | 非NULL，c->length不能小于a->length + b->length |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| c | [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 大数乘法结果 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 大数乘法成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PKE_SUPPORT_BIG_NUMBER | 功能宏 | 支持大数运算功能 | y |

### uapi_drv_cipher_pke_exp_mod <a id="uapi_drv_cipher_pke_exp_mod"></a>

```c
errcode_t uapi_drv_cipher_pke_exp_mod(const uapi_drv_cipher_pke_data_t *n, const uapi_drv_cipher_pke_data_t *k, const uapi_drv_cipher_pke_data_t *in, const uapi_drv_cipher_pke_data_t *out)
```

**头文件清单**

```c
#include "include/driver/security_unified/unified_cipher_pke.h"
```

**功能说明**

- 模幂运算 out = (in ^ k) mod n
- 输入模数、指数和底数，输出模幂结果
- 模数的有效字节数与n->length支持的规格之差不能大于2字节，且模数不能为偶数
- 调用期间会添加睡眠否决标记，防止系统进入深度睡眠，调用结束后移除

**前置条件**

- PKE模块已通过驱动初始化完成，CONFIG_PKE_SUPPORT_BIG_NUMBER宏已开启
- 调用方需确保n、k、in、out指向的缓冲区已申请成功，out->length等于n->length

**入参**

| 名称 | 参数类型 | 详细说明 | 约束取值范围 |
| ---- | ---- | ---- | ---- |
| n | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 模数 | 非NULL，仅支持192/256/384/512/1024/1536/2048/3072/4096bits，且模数不能为偶数 |
| k | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 指数 | 非NULL，数据长度不能大于4096bits |
| in | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输入数据 | 非NULL，数据长度不能大于4096bits |
| out | const [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 输出结果 | 非NULL，out->length等于n->length |

**出参**

| 名称 | 数据类型 | 输出说明 |
| ---- | ---- | ---- |
| out | [uapi_drv_cipher_pke_data_t](#struct_uapi_drv_cipher_pke_data_t) * | 模幂运算结果 |

**返回值**

| 返回值 | 文字含义 | 触发场景 |
| -------- | -------- | -------- |
| ERRCODE_SUCC(0x0) | 执行成功 | 模幂运算成功 |
| Other | 其他错误码，参考errcode_t | 执行失败 |

**Kconfig配置**

| 配置项 | 宏类型 | 说明 | 默认值 |
| -------- | -------- | -------- | -------- |
| CONFIG_PKE_SUPPORT_BIG_NUMBER | 功能宏 | 支持大数运算功能 | y |

## Enumerations

### uapi_drv_cipher_pke_ecc_curve_type_t <a id="enum_uapi_drv_cipher_pke_ecc_curve_type_t"></a>

```c
typedef enum {
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P256 = 0,      /* RFC 5639 - Brainpool P256/384/512 */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P384,          /* RFC 5639 - Brainpool P256/384/512 */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P512,          /* RFC 5639 - Brainpool P256/384/512 */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P256K,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P192R,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P224R,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P256R,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P384R,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P521R,            /* NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC7748,               /* RFC 7748 - Curve25519 */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC7748_448,           /* RFC 7748 - Curve448 */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC8032,               /* RFC 8032 - ED25519 */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_SM2,                   /* GMT 0003.2-2012 */
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_MAX,
    UAPI_DRV_CIPHER_PKE_ECC_TYPE_INVALID = 0xffffffff,
} uapi_drv_cipher_pke_ecc_curve_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P256 | 0 | RFC 5639 Brainpool P256曲线 |
| UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P384 | 1 | RFC 5639 Brainpool P384曲线 |
| UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P512 | 2 | RFC 5639 Brainpool P512曲线 |
| UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P256K | 3 | NIST (National Institute of Standards and Technology) FIPS 186-4 P256K曲线，不建议使用 |
| UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P192R | 4 | NIST FIPS 186-4 P192R曲线，不建议使用 |
| UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P224R | 5 | NIST FIPS 186-4 P224R曲线，不建议使用 |
| UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P256R | 6 | NIST FIPS 186-4 P256R曲线，不建议使用 |
| UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P384R | 7 | NIST FIPS 186-4 P384R曲线，不建议使用 |
| UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P521R | 8 | NIST FIPS 186-4 P521R曲线，不建议使用 |
| UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC7748 | 9 | RFC 7748 Curve25519曲线 |
| UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC7748_448 | 10 | RFC 7748 Curve448曲线 |
| UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC8032 | 11 | RFC 8032 ED25519曲线 |
| UAPI_DRV_CIPHER_PKE_ECC_TYPE_SM2 | 12 | GMT 0003.2-2012 SM2曲线 |
| UAPI_DRV_CIPHER_PKE_ECC_TYPE_MAX | 13 | 最大值 |
| UAPI_DRV_CIPHER_PKE_ECC_TYPE_INVALID | 0xffffffff | 无效值 |

### uapi_drv_cipher_pke_rsa_scheme_t <a id="enum_uapi_drv_cipher_pke_rsa_scheme_t"></a>

```c
typedef enum {
    UAPI_DRV_CIPHER_PKE_RSA_SCHEME_PKCS1_V15 = 0x00,    /* not security, suggest not to use */
    UAPI_DRV_CIPHER_PKE_RSA_SCHEME_PKCS1_V21,
    UAPI_DRV_CIPHER_PKE_RSA_SCHEME_MAX,
    UAPI_DRV_CIPHER_PKE_RSA_SCHEME_INVALID = 0xffffffff,
} uapi_drv_cipher_pke_rsa_scheme_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| UAPI_DRV_CIPHER_PKE_RSA_SCHEME_PKCS1_V15 | 0x00 | PKCS1 V15填充方式，不建议使用 |
| UAPI_DRV_CIPHER_PKE_RSA_SCHEME_PKCS1_V21 | 0x01 | PKCS1 V21填充方式 |
| UAPI_DRV_CIPHER_PKE_RSA_SCHEME_MAX | 0x02 | 最大值 |
| UAPI_DRV_CIPHER_PKE_RSA_SCHEME_INVALID | 0xffffffff | 无效值 |

### uapi_drv_cipher_pke_hash_type_t <a id="enum_uapi_drv_cipher_pke_hash_type_t"></a>

```c
typedef enum {
    UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA1 = 0x00,  /* not security, suggest not to use */
    UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA224,
    UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA256,
    UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA384,
    UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA512,
    UAPI_DRV_CIPHER_PKE_HASH_TYPE_SM3,
    UAPI_DRV_CIPHER_PKE_HASH_TYPE_MAX,
    UAPI_DRV_CIPHER_PKE_HASH_TYPE_INVALID = 0xffffffff,
} uapi_drv_cipher_pke_hash_type_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA1 | 0x00 | SHA1 (Secure Hash Algorithm 1) 摘要算法，不建议使用 |
| UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA224 | 0x01 | SHA224 (Secure Hash Algorithm 224-bit) 摘要算法，不建议使用 |
| UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA256 | 0x02 | SHA256 (Secure Hash Algorithm 256-bit) 摘要算法 |
| UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA384 | 0x03 | SHA384 (Secure Hash Algorithm 384-bit) 摘要算法 |
| UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA512 | 0x04 | SHA512 (Secure Hash Algorithm 512-bit) 摘要算法 |
| UAPI_DRV_CIPHER_PKE_HASH_TYPE_SM3 | 0x05 | SM3摘要算法 |
| UAPI_DRV_CIPHER_PKE_HASH_TYPE_MAX | 0x06 | 最大值 |
| UAPI_DRV_CIPHER_PKE_HASH_TYPE_INVALID | 0xffffffff | 无效值 |

### uapi_drv_cipher_pke_buffer_secure_t <a id="enum_uapi_drv_cipher_pke_buffer_secure_t"></a>

```c
typedef enum {
    UAPI_DRV_CIPHER_PKE_BUF_NONSECURE = 0x00,
    UAPI_DRV_CIPHER_PKE_BUF_SECURE,
    UAPI_DRV_CIPHER_PKE_BUF_INVALID = 0xffffffff,
} uapi_drv_cipher_pke_buffer_secure_t;
```

| 枚举成员 | 取值 | 描述 |
| ------- | ---- | ---- |
| UAPI_DRV_CIPHER_PKE_BUF_NONSECURE | 0x00 | 非安全缓冲区 |
| UAPI_DRV_CIPHER_PKE_BUF_SECURE | 0x01 | 安全缓冲区 |
| UAPI_DRV_CIPHER_PKE_BUF_INVALID | 0xffffffff | 无效值 |

## Structures

### uapi_drv_cipher_pke_data_t <a id="struct_uapi_drv_cipher_pke_data_t"></a>

```c
typedef struct {
    uint32_t  length;   /*!< PKE通用数据缓冲区长度。 */
    uint8_t  *data;     /*!< PKE通用数据缓冲区。 */
} uapi_drv_cipher_pke_data_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| length | uint32_t | PKE通用数据缓冲区长度 |
| data | uint8_t * | PKE通用数据缓冲区 |

### uapi_drv_cipher_pke_ecc_point_t <a id="struct_uapi_drv_cipher_pke_ecc_point_t"></a>

```c
typedef struct {
    uint8_t *x;    /*!< 公钥的X坐标，调用方确保如果此密钥的有效大小小于ecc密钥大小，则用前导零填充。 */
    uint8_t *y;    /*!< 公钥的Y坐标，调用方确保如果此密钥的有效大小小于ecc密钥大小，则用前导零填充。 */
    uint32_t length;    /*!< ECC公钥长度。 */
} uapi_drv_cipher_pke_ecc_point_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| x | uint8_t * | 公钥的X坐标，有效大小小于ecc密钥大小时用前导零填充 |
| y | uint8_t * | 公钥的Y坐标，有效大小小于ecc密钥大小时用前导零填充 |
| length | uint32_t | ECC公钥长度 |

### uapi_drv_cipher_pke_ecc_sig_t <a id="struct_uapi_drv_cipher_pke_ecc_sig_t"></a>

```c
typedef struct {
    uint8_t *r;    /*!< ECC签名值R。 */
    uint8_t *s;    /*!< ECC签名值S。 */
    uint32_t length;    /*!< ECC签名数据长度。 */
} uapi_drv_cipher_pke_ecc_sig_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| r | uint8_t * | ECC签名值R |
| s | uint8_t * | ECC签名值S |
| length | uint32_t | ECC签名数据长度 |

### uapi_drv_cipher_pke_msg_t <a id="struct_uapi_drv_cipher_pke_msg_t"></a>

```c
typedef struct {
    uint32_t  length;    /*!< ECC输入消息缓冲区长度。 */
    uint8_t  *data;      /*!< ECC输入消息缓冲区。 */
    uapi_drv_cipher_pke_buffer_secure_t buf_sec;
} uapi_drv_cipher_pke_msg_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| length | uint32_t | ECC输入消息缓冲区长度 |
| data | uint8_t * | ECC输入消息缓冲区 |
| buf_sec | [uapi_drv_cipher_pke_buffer_secure_t](#enum_uapi_drv_cipher_pke_buffer_secure_t) | 缓冲区安全属性 |

### uapi_drv_cipher_pke_rsa_priv_key_t <a id="struct_uapi_drv_cipher_pke_rsa_priv_key_t"></a>

```c
typedef struct {
    uint8_t *n;          /*!< RSA秘钥参数n。 */
    uint8_t *e;          /*!< RSA公钥参数e。 */
    uint8_t *d;          /*!< RSA私钥参数d。 */
    uint8_t *p;          /*!< RSA第一素数因子。 */
    uint8_t *q;          /*!< RSA第二素数因子。 */
    uint8_t *dp;         /*!< D % (P - 1)的结果。 */
    uint8_t *dq;         /*!< D % (Q - 1)的结果。 */
    uint8_t *qp;         /*!< 1 / (Q % P)的结果。 */
    uint16_t n_len;      /*!< RSA秘钥参数n的长度。 */
    uint16_t e_len;      /*!< RSA公钥参数e的长度。 */
    uint16_t d_len;      /*!< RSA私钥参数d的长度。 */
    uint16_t p_len;      /*!< RSA第一素因子的长度，应该是n_len的一半。 */
    uint16_t q_len;      /*!< RSA第二素因子的长度，应该是n_len的一半。 */
    uint16_t dp_len;     /*!< D % (P - 1)结果的长度，应该是n_len的一半。 */
    uint16_t dq_len;     /*!< D % (Q - 1)结果的长度，应该是n_len的一半。 */
    uint16_t qp_len;     /*!< 1 / (Q % P)结果的长度，应该是n_len的一半。 */
} uapi_drv_cipher_pke_rsa_priv_key_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| n | uint8_t * | RSA秘钥参数n |
| e | uint8_t * | RSA公钥参数e |
| d | uint8_t * | RSA私钥参数d |
| p | uint8_t * | RSA第一素数因子 |
| q | uint8_t * | RSA第二素数因子 |
| dp | uint8_t * | D % (P - 1)的结果 |
| dq | uint8_t * | D % (Q - 1)的结果 |
| qp | uint8_t * | 1 / (Q % P)的结果 |
| n_len | uint16_t | RSA秘钥参数n的长度 |
| e_len | uint16_t | RSA公钥参数e的长度 |
| d_len | uint16_t | RSA私钥参数d的长度 |
| p_len | uint16_t | RSA第一素因子的长度，应该是n_len的一半 |
| q_len | uint16_t | RSA第二素因子的长度，应该是n_len的一半 |
| dp_len | uint16_t | D % (P - 1)结果的长度，应该是n_len的一半 |
| dq_len | uint16_t | D % (Q - 1)结果的长度，应该是n_len的一半 |
| qp_len | uint16_t | 1 / (Q % P)结果的长度，应该是n_len的一半 |

### uapi_drv_cipher_pke_rsa_pub_key_t <a id="struct_uapi_drv_cipher_pke_rsa_pub_key_t"></a>

```c
typedef struct {
    uint8_t  *n;            /*!< RSA私钥参数d。 */
    uint8_t  *e;            /*!< RSA公钥参数e。 */
    uint16_t len;           /*!< RSA公钥长度。 */
} uapi_drv_cipher_pke_rsa_pub_key_t;
```

**成员说明**

| 成员名称 | 数据类型 | 描述 |
| ------- | ------- | ---- |
| n | uint8_t * | RSA公钥参数n |
| e | uint8_t * | RSA公钥参数e |
| len | uint16_t | RSA公钥长度 |

