---
title: TLS/DTLS 开发指南
---

# 前言<a name="ZH-CN_TOPIC_0000001809364380"></a>

**概述<a name="section4537382116410"></a>**

本文档主要介绍TLS/DTLS组件的开发实现示例。

TLS/DTLS以及其他加密套基于开源组件mbedtls 3.1.0实现，详细说明请参见官方说明：[https://tls.mbed.org/api/index.html](https://tls.mbed.org/api/index.html)

如果官方说明版本与SDK版本不一致，请参考官方release说明：[https://github.com/ARMmbed/mbedtls/releases](https://github.com/ARMmbed/mbedtls/releases)

# API接口说明<a name="ZH-CN_TOPIC_0000001856243049"></a>




## 结构体说明<a name="ZH-CN_TOPIC_0000001809364372"></a>

mbedtls详细的结构体说明请参考官方说明文档：[https://tls.mbed.org/api/annotated.html](https://tls.mbed.org/api/annotated.html)

## API列表<a name="ZH-CN_TOPIC_0000001809524248"></a>

mbedtls详细的API说明请参考官方说明文档：[https://tls.mbed.org/api/globals\_func.html](https://tls.mbed.org/api/globals_func.html)

## 配置说明<a name="ZH-CN_TOPIC_0000001856243041"></a>

mbedtls详细的配置项说明请参考官方说明文档：[https://tls.mbed.org/api/config\_8h.html\#ab3bca0048342cf2789e7d170548ff3a5](https://tls.mbed.org/api/config_8h.html#ab3bca0048342cf2789e7d170548ff3a5)

# 开发指南<a name="ZH-CN_TOPIC_0000001809524244"></a>

mbedtls详细的开发DEMO请参考官方说明文档：[https://tls.mbed.org/api/modules.html](https://tls.mbed.org/api/modules.html)

# 硬件适配<a name="ZH-CN_TOPIC_0000001856243045"></a>



## 配置说明<a name="ZH-CN_TOPIC_0000001809364376"></a>

在工程的build\\config\\target\_config\\ws63\\config.py中的对应编译目标中添加MBEDTLS\_HARDEN\_OPEN宏，开启硬件加速回调接口注册功能。

在mbedtls\_v3.1.0\\harden\\platform\\connect\\mbedtls\_platform\_hardware\_config.h中开启对应的算法宏，会直接调用硬件驱动接口。

目前支持的硬件算法有AES,RSA,HASH,大数模幂，随机数，ECP算法。

## 适配说明<a name="ZH-CN_TOPIC_0000001809524252"></a>







### AES适配<a name="ZH-CN_TOPIC_0000001856163045"></a>

-   使能MBEDTLS\_AES\_ALT后，AES算法在使用硬件加速器时，会锁定硬件加速器资源，即AES操作是阻塞的，直至驱动获取资源或超时返回失败。

### 大数模幂适配<a name="ZH-CN_TOPIC_0000001809364360"></a>

-   使能MBEDTLS\_BIGNUM\_EXP\_MOD\_USE\_HARDWARE后，会调用硬件驱动接口完成大数模幂运算。

### 随机数适配<a name="ZH-CN_TOPIC_0000001856243033"></a>

使能MBEDTLS\_ENTROPY\_HARDWARE\_ALT后，系统会选用默认增加硬件随机数作为一个强随机数源。如果此宏被关闭，而用户也没有注册其他强随机数源，会导致mbedTLS无法提供安全随机数，影响系统的安全性。

### RSA数字签名适配<a name="ZH-CN_TOPIC_0000001809364364"></a>

使能MBEDTLS\_RSA\_ALT编译宏之后，mbedTLS会对RSA数字签名操作和验签操作进行硬件加速。

### HASH算法适配<a name="ZH-CN_TOPIC_0000001863121673"></a>

目前WS63规格支持的硬件加速HASH算法有SHA1,SHA224,SHA256,SHA384,SHA512,分别开启MBEDTLS\_SHA1\_USE\_HARDWARE，MBEDTLS\_SHA224\_USE\_HARDWARE，MBEDTLS\_SHA256\_USE\_HARDWARE，MBEDTLS\_SHA384\_USE\_HARDWARE，MBEDTLS\_SHA512\_USE\_HARDWARE来使能。

### ECP适配<a name="ZH-CN_TOPIC_0000001809364368"></a>

目前WS63规格支持的硬件加速ECP算法有SECP192R1, SECP224R1,SECP256R1,SECP384R1,SECP521R1,BP256R1,BP384R1,BP512R1,CURVE25519,CURVE448,分别开启MBEDTLS\_SECP192R1\_USE\_HARDWARE，MBEDTLS\_SECP224R1\_USE\_HARDWARE，MBEDTLS\_SECP256R1\_USE\_HARDWARE，MBEDTLS\_SECP384R1\_USE\_HARDWARE，MBEDTLS\_SECP521R1\_USE\_HARDWARE，MBEDTLS\_BP256R1\_USE\_HARDWARE, MBEDTLS\_BP384R1\_USE\_HARDWARE, MBEDTLS\_BP512R1\_USE\_HARDWARE, MBEDTLS\_CURVE25519\_USE\_HARDWARE,MBEDTLS\_CURVE448\_USE\_HARDWARE来开启。

# 注意事项<a name="ZH-CN_TOPIC_0000001809524240"></a>





## 关于配置SSL接收缓存的注意事项<a name="ZH-CN_TOPIC_0000001856163033"></a>

-   SSL接收缓存由编译项MBEDTLS\_SSL\_IN\_CONTENT\_LEN控制，默认为16KB。如果实际应用中，用户可以确保SSL上层数据包的最大长度不超过2KB或4KB，则可以通过mbedtls\_ssl\_conf\_max\_frag\_len接口设置SSL接收缓存的长度，达到节省内存的目的。

    **注意：mbedtls\_ssl\_conf\_max\_frag\_len接口的调用必须先于mbedtls\_ssl\_setup接口。**

-   考虑到多级数字证书可能导致TLS握手包的长度大于1KB，因此调用mbedtls\_ssl\_conf\_max\_frag\_len接口时，只有mfl\_code为MBEDTLS\_SSL\_MAX\_FRAG\_LEN\_2048或MBEDTLS\_SSL\_MAX\_FRAG\_LEN\_4096时，mbedtls才会修改接收缓存；如果mfl\_code为MBEDTLS\_SSL\_MAX\_FRAG\_LEN\_512或MBEDTLS\_SSL\_MAX\_FRAG\_LEN\_1024，则接收缓存仍然为16KB。
-   如果修改SSL接收缓存为2KB或4KB后，Client端接收到Server端的一个大于2KB或4KB的数据包，此时mbedtls\_ssl\_read接口会返回失败，错误码为MBEDTLS\_ERR\_SSL\_MSG\_TOO\_LONG（此为新增的一个特定错误码），当用户获得此错误码时，必须关闭SSL连接，不允许继续从SSL链路接收数据。
-   通过mbedtls\_ssl\_conf\_max\_frag\_len接口设置SSL接收缓存，目前只对SSL Client有效。

## 关于数字证书有效期验证的注意事项<a name="ZH-CN_TOPIC_0000001856243037"></a>

由于WS63平台无Real Time Controller，因此系统启动后，无法获取UTC时间，这种情况下数字证书的有效期验证会失败，导致TLS建链失败。针对此种情况，mbedtls默认关闭MBEDTLS\_HAVE\_TIME\_DATE，此时TLS的证书校验会关闭。如果用户可以确保TLS证书校验之前，可以通过其他方式获取UTC时间（例如：SNTP），则可以打开MBEDTLS\_HAVE\_TIME\_DATE编译宏。

## 关于部分默认配置变更的说明<a name="ZH-CN_TOPIC_0000001856163041"></a>

MbedTLS安全库的默认配置见include/mbedtls/mbedtls\_config.h文件。开源版本默认开启大部分功能，LiteOS对MbedTLS的默认配置进行了适度修改，主要目的是增强MbedTLS的安全性，降低代码体积。修改后的MbedTLS满足IoT绝大部分场景，改动的主要原则如下：

-   默认配置必须保证安全性要求。
-   关闭不安全算法或功能。
-   关闭不适合IoT场景的功能，例如TLS Server模式、X509证书签名请求CSR。
-   关闭不适合IoT场景的算法，例如SECP521R1 ECC曲线，IoT场景下推荐使用128比特安全强度的ECC曲线。
-   关闭不常用算法或功能，例如IoT场景不常用的PKCS\#12证书。如果打开PKCS\#12，可能存在一定的应用风险。

## 关于FCC认证默认配置变更说明<a name="ZH-CN_TOPIC_0000002070167405"></a>

FCC认证为EMC强制性认证, 主要针对 9K-3000GHZ的电子电器产品，内容涉及无线电、通信等各方面。FCC认证过程依赖KEY\_EXCHANGE\_ECDHE\_RSA算法，此算法默认未开启。如果设备有通过FCC认证的需求，可以开启此算法。开启方法：修改open\_source/mbedtls/mbedtls\_v3.1.0/harden/platform/connect/mbedtls\_platform\_hardware\_config.h文件，将\#undef MBEDTLS\_KEY\_EXCHANGE\_ECDHE\_RSA\_ENABLED注释掉。

