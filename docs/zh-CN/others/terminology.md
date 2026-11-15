# 术语汇总表

| 缩略语 | 英文全称 | 解释 |
|--------|----------|------|
| A2DP | Advanced Audio Distribution Profile | 高级音频分发配置，蓝牙音频传输协议，用于在蓝牙设备之间传输高质量音频流 |
| AAC | Advanced Audio Coding | 高级音频编码，A2DP 支持的音频编码格式 |
| ACB | Announce Control Block | 广播控制块，SLE 连接管理结构 |
| ACK | Acknowledgment | 确认应答，接收方对收到的数据的确认 |
| ACL | Asynchronous Connection-Less | 异步无连接链路，蓝牙协议栈中的一种数据传输链路类型，用于传输非实时数据 |
| AD | Advertising Data | 广播数据，BLE 设备向外广播的数据包，含设备名、服务 UUID 等信息 |
| ADC | Analog-to-Digital Converter | 模数转换器，将模拟电压转换为数字值，温度传感器、光敏电阻、电位器等模拟信号都需要 ADC 才能被 CPU 理解 |
| AES | Advanced Encryption Standard | 高级加密标准，广泛使用的对称加密算法，文档中提及"AES/DES/RSA/ECC 加密算法" |
| AG | Audio Gateway | 音频网关，HFP 协议中发起通话的设备角色（如手机） |
| AGC | Automatic Gain Control | 自动增益控制，射频接收链路自动调整信号增益 |
| AI | Artificial Intelligence | 人工智能，使机器模拟人类智能（学习、推理、感知等）的技术 |
| ALG | Application Layer Gateway | 应用层网关，Wi-Fi 中间件中的 NAT 穿越特性，用于对 FTP、SIP 等应用层协议做动态端口映射，使内网设备能正常进行此类通信 |
| AP | Access Point | Wi-Fi 接入点，无线网络中心，管理 STA 并提供数据中继 |
| ARM | Advanced RISC Machines | ARM 处理器架构（调试接口如 SWD/JTAG 基于 ARM） |
| ASCII | American Standard Code for Information Interchange | 美国信息交换标准代码 |
| AVRCP | Audio/Video Remote Control Profile | 音视频远程控制协议，用于通过蓝牙远程控制音视频设备的播放、暂停、上下曲等操作 |
| BCLK | Bit Clock | 位时钟，I2S 数字音频接口的位时钟信号 |
| BLE | Bluetooth Low Energy | 低功耗蓝牙，蓝牙低功耗通信技术，广泛兼容 |
| BR | Basic Rate | 基础速率，经典蓝牙的基础速率模式（常与 EDR 并列使用，如 BR/EDR） |
| BSP | Board Support Package | 板级支持包，为特定硬件板提供的底层驱动和初始化代码 |
| BSSID | Basic Service Set Identifier | 基本服务集标识符，无线局域网中接入点（AP）的 MAC 地址 |
| BT | Bluetooth | 蓝牙，短距离无线通信技术标准 |
| BTS | Bluetooth Subsystem | 蓝牙子系统（文档中表述为"BTS（蓝牙）"） |
| CBC | Cipher Block Chaining | 密码分组链接模式，分组密码工作模式 |
| CCA | Clear Channel Assessment | 空闲信道评估，Wi-Fi/蓝牙中用于判断信道是否空闲的能量检测/载波侦听机制 |
| CCC | Common Configuration Characteristic | 通用配置特征，BLE GATT 中配置 MTU 的特征 |
| CCCD | Client Characteristic Configuration Descriptor | 客户端特征配置描述符，SLE 规范的使能开关，Client 通过它告知 Server 自己期望接收哪种推送 |
| CCM | Counter with CBC-MAC | 计数器模式加 CBC 消息认证码，一种带认证的加密模式，用于 AES 和 SM4 加密算法（如 AES-128-CCM、SM4-128-CCM） |
| CHBA | Converged Host Bus Adapter | 汇聚主机总线适配器，把 SLE 链路虚拟成网卡，上层 TCP/IP 协议栈（lwIP）可通过 SLE 链路收发 IP 数据包 |
| CLI | Command Line Interface | 命令行界面 |
| CMAC | Cipher-based Message Authentication Code | 基于密码的消息认证码 |
| CoAP | Constrained Application Protocol | 受限应用协议，面向资源受限物联网设备的应用层协议 |
| CRC | Cyclic Redundancy Check | 循环冗余校验，一种数据完整性校验机制 |
| CS | Carrier Synchronization | 载波同步 |
| CSI | Channel State Information | 信道状态信息 |
| CT | Audio/Video Remote Control Profile Controller | 音视频远程控制协议控制端 |
| CTR | Counter Mode | 计数器模式，分组密码工作模式 |
| CTS | Clear To Send | 清除发送，UART 硬件流控的接收就绪信号 |
| CVSD | Continuous Variable Slope Delta | 连续可变斜率增量调制，蓝牙窄带语音编码 |
| DAC | Digital-to-Analog Converter | 数模转换器，将数字值转换为模拟电压 |
| DCACHE | Data Cache | 数据缓存，CPU 内部用于暂存数据的快速存储器 |
| DFU | Device Firmware Update | 设备固件升级，通过 USB 等接口对设备固件进行升级 |
| DFX | Diagnostic & Feedback | 诊断与反馈，WS63 的日志分级、诊断命令、死机现场保存体系 |
| DHCP | Dynamic Host Configuration Protocol | 动态主机配置协议，自动分配 IP 地址等网络参数 |
| DIAG | Diagnostic | 诊断，用于检测、定位设备或系统故障与状态的功能或模块 |
| DLE | Data Length Extension | 数据长度扩展，BLE 4.2 引入的特性，允许更长的数据包 |
| DMA | Direct Memory Access | 直接内存访问，数据搬运不需要 CPU 逐字节参与 |
| DNS | Domain Name System | 域名系统，将域名解析为 IP 地址 |
| DTCM | Data Tightly Coupled Memory | 数据紧耦合内存，CPU 直接高速访问的片上 RAM |
| DTIM | Delivery Traffic Indication Message | 传输指示消息，Wi-Fi 节能广播周期信标 |
| DTLS | Datagram Transport Layer Security | 数据报传输层安全，基于 UDP 的 TLS 协议，文档中提及"mbedTLS（TLS / DTLS）" |
| DTMF | Dual-Tone Multi-Frequency | 双音多频，电话拨号信令（HFP 场景使用） |
| EA1 | Encryption Algorithm 1 | 加密算法 1，指 AES-128-CCM 加密算法，国际通用标准 |
| EA2 | Encryption Algorithm 2 | 加密算法 2，指 SM4-128-CCM 加密算法，符合 GB/T 32907 国密标准 |
| ECC | Elliptic Curve Cryptography | 椭圆曲线密码学 |
| ECDH | Elliptic Curve Diffie-Hellman | 椭圆曲线 Diffie-Hellman 密钥协商协议 |
| ECDSA | Elliptic Curve Digital Signature Algorithm | 椭圆曲线数字签名算法，基于 ECC 的签名/验签 |
| EDR | Enhanced Data Rate | 增强数据速率，经典蓝牙的高速率模式（常与 BR 并列使用，如 BR/EDR） |
| EEPROM | Electrically Erasable Programmable Read-Only Memory | 电可擦除可编程只读存储器 |
| EFUSE | Electronic Fuse | 电子熔丝，一次性可编程存储器，每个 bit 初始状态为 0，通过施加特定编程电压可将选中的 bit 熔断为 1，不可逆 |
| ELF | Executable and Linkable Format | 可执行与可链接格式，含调试符号，用于 GDB 调试 |
| FAQ | Frequently Asked Questions | 常见问题解答 |
| FBB | Firmware Boot Block | 固件引导块 |
| FIFO | First-In First-Out | 先进先出，一种数据存储结构，先写入的数据先被读取 |
| FOTA | Firmware Over-The-Air | 固件空中升级，通过无线方式对设备固件进行升级 |
| GAP | Generic Access Profile | 通用访问配置，BLE 协议栈的基础层 API，管理广播、扫描、配对、连接等功能 |
| GATT | Generic Attribute Profile | 通用属性配置，BLE 协议栈中定义服务和特征的数据结构 |
| GC | Group Client | 组客户端，星闪/蓝牙组网中作为 Group 成员一方的客户端设备 |
| GCC | GNU Compiler Collection | GNU 编译器套件 |
| GCM | Galois/Counter Mode | 伽罗瓦/计数器模式，带认证的加密模式 |
| GDB | GNU Debugger | GNU 调试器，用于程序调试和故障定位 |
| GFSK | Gaussian Frequency Shift Keying | 高斯频移键控，一种调制方式 |
| GLP | Generic Layer Protocol | 通用层协议（SLE 通用层协议） |
| GO | Group Owner | 组所有者，星闪/蓝牙组网中作为 Group 管理方的设备（类似 SoftAP） |
| GPIO | General Purpose Input/Output | 通用输入输出，MCU 控制外设的最基本方式 |
| GUI | Graphical User Interface | 图形用户界面 |
| HADM | High Accuracy Distance Measurement | 高精度距离测量，基于 Channel Sounding 技术，通过收发双方交换 IQ 数据计算无线信号飞行时间来估算设备距离，精度范围 0.1m~1m |
| HAL | Hardware Abstraction Layer | 硬件抽象层，屏蔽硬件差异的软件层 |
| HCI | Host Controller Interface | 主机控制器接口，蓝牙协议栈主机与控制器之间的标准接口 |
| HF | Hands-Free | 免提，HFP 协议的免提端角色（如车载/耳机） |
| HFP | Hands-Free Profile | 免提配置，蓝牙免提协议，用于车载、耳机等免提通话场景 |
| HID | Human Interface Device | 人机交互设备，如键盘、鼠标等 |
| HiSpark | HiSpark | 海思面向开发者的开发板/工具品牌，如 HiSpark Studio |
| HKDF | HMAC-based Key Derivation Function | 基于 HMAC 的密钥派生函数，从主密钥派生子密钥 |
| HMAC | Hash-based Message Authentication Code | 基于哈希的消息认证码，文档中提及"HMAC-SHA256 算法"、"HMAC 密钥槽" |
| HOGP | HID over GATT Profile | 基于 GATT 的 HID 配置，BLE 设备作为 HID 设备的协议 |
| HTTP | HyperText Transfer Protocol | 超文本传输协议 |
| I2C | Inter-Integrated Circuit | 集成电路间两线制串行总线，SCL（时钟）+ SDA（数据），支持一主多从 |
| I2S | Inter-IC Sound | 集成电路间数字音频接口，三线时序：BCLK 位时钟、LRCK 声道选择、SDATA 音频数据 |
| ICMP | Internet Control Message Protocol | 互联网控制消息协议，用于网络诊断和错误报告（如 ping） |
| IDE | Integrated Development Environment | 集成开发环境，如 HiSpark Studio for VS Code |
| IE | Information Element | 信息元素（管理帧中的信息单元） |
| IP | Internet Protocol | 互联网协议，网络层协议 |
| IPC | Inter-Process Communication | 进程间通信 |
| IPEX | IPEX Connector | IPEX 射频连接器，小型同轴天线接口 |
| IPV6 | Internet Protocol Version 6 | 互联网协议第 6 版，128 位地址 |
| IRQ | Interrupt Request | 中断请求，外设向 CPU 发出的中断信号 |
| ISB | Instruction Synchronization Barrier | 指令同步屏障，刷新流水线保证后续指令重新取指 |
| ISR | Interrupt Service Routine | 中断服务例程，中断发生时 CPU 暂停当前任务跳转执行 |
| ITCM | Instruction Tightly Coupled Memory | 指令紧耦合内存，CPU 直接高速取指的片上 RAM |
| IV | Initialization Vector | 初始化向量，加密算法中用于增强密文随机性的初始输入值 |
| JTAG | Joint Test Action Group | 联合测试行动组，一种调试接口标准 |
| KM | Key Management | 密钥管理 |
| LCD | Liquid Crystal Display | 液晶显示屏 |
| LDO | Low Dropout Regulator | 低压差线性稳压器，用于稳定输出电压 |
| LED | Light Emitting Diode | 发光二极管，常用的指示灯与显示用半导体器件 |
| LiteOS | Huawei LiteOS | 华为开源的轻量级物联网实时操作系统内核 |
| LLI | Linked List Item | 链表项，DMA 链式传输中描述单次传输配置的节点 |
| LMA | Load Memory Address | 装载地址，表示程序装载的内存地址 |
| LNA | Low Noise Amplifier | 低噪声放大器，射频接收链路前端放大微弱信号 |
| LPM | Low Power Management | 低功耗管理，负责低功耗模式及挂起/恢复等功能 |
| LRCK | Left/Right Clock | 声道选择时钟，I2S 接口的左右声道选择信号 |
| LTK | Long Term Key | 长期密钥，BLE 配对后生成的用于加密后续连接的长期密钥 |
| LVGL | Light and Versatile Graphics Library | 轻量级通用图形库，用于嵌入式图形界面开发 |
| lwIP | Lightweight IP | 轻量级 TCP/IP 协议栈，专为嵌入式系统设计 |
| MAC | Media Access Control | 媒体访问控制，MAC 地址是网络设备的唯一标识 |
| mbedTLS | mbed Transport Layer Security | ARM 开源的 TLS/SSL 加密库（原名 PolarSSL） |
| MCS | Modulation and Coding Scheme | 调制与编码方案，决定每个符号携带多少有效比特，MCS 0~12，数字越大速率越高但对信号质量要求越苛刻 |
| MCU | Microcontroller Unit | 微控制器，集成了 CPU、内存、外设等的单片计算机 |
| MESH | Mesh Network | Mesh 网状网络，多节点自组织的网络拓扑 |
| MISO | Master In Slave Out | 主机输入从机输出，SPI 从机到主机的数据线 |
| MITM | Man-In-The-Middle | 中间人攻击，一种网络安全攻击方式 |
| MMU | Memory Management Unit | 内存管理单元，负责虚拟地址到物理地址映射 |
| MOSI | Master Out Slave In | 主机输出从机输入，SPI 主机到从机的数据线 |
| MPS | Maximum Packet Size | 最大包大小，SLE SSAP 单包最大数据长度 |
| MPU | Memory Protection Unit | 内存保护单元，硬件级内存区域权限保护 |
| MQTT | Message Queuing Telemetry Transport | 消息队列遥测传输，物联网消息协议 |
| MSBC | Wide Band Speech Codec | 宽带语音编码，蓝牙 HFP 宽带语音编码方案 |
| MSID | Module/Manufacturer Specific ID | 模块/厂商特定标识，EFUSE 中烧录的标识 |
| MTU | Maximum Transmission Unit | 最大传输单元，单次能传输的最大数据量 |
| NAPOT | Naturally Aligned Power-Of-Two | 自然对齐的 2 的幂区域，PMP 地址匹配模式 |
| NEON | ARM NEON | ARM 的 SIMD 指令扩展，用于多媒体/信号处理加速 |
| NFC | Near Field Communication | 近场通信，短距离无线通信技术 |
| NIST | National Institute of Standards and Technology | 美国国家标准与技术研究院，定义 FIPS/P256 等密码标准 |
| NMI | Non-Maskable Interrupt | 不可屏蔽中断，无法被软件屏蔽的高优先级中断 |
| NOR | NOR Flash | NOR 型闪存，支持随机读取，常存代码 |
| NV | Non-Volatile | 非易失性存储，持久化存储（绑定信息、配置等） |
| NVIC | Nested Vectored Interrupt Controller | 嵌套向量中断控制器，ARM Cortex-M 系列的中断控制器 |
| OAEP | Optimal Asymmetric Encryption Padding | 最优非对称加密填充，RSA 加密的填充方案 |
| OLED | Organic Light-Emitting Diode | 有机发光二极管显示屏 |
| OOB | Out of Band | 带外，通过其他通道交换配对信息 |
| OOM | Out Of Memory | 内存耗尽（分配失败） |
| OS | Operating System | 操作系统，管理硬件与软件资源的系统软件 |
| OSAL | Operating System Abstraction Layer | 操作系统抽象层，让上层代码不依赖具体内核实现 |
| OTA | Over-The-Air | 空中升级，通过无线方式对设备进行固件升级 |
| OTP | One-Time Programmable | 一次性可编程存储器，EFUSE 的工作原理 |
| P2P | Peer-to-Peer | Wi-Fi Direct 点对点直连，又称 Wi-Fi P2P |
| PAN | Personal Area Network | 个人区域网，短距离无线通信网络 |
| PBAP | Phone Book Access Profile | 电话簿访问协议，蓝牙协议中用于访问手机电话簿 |
| PBKDF2 | Password-Based Key Derivation Function 2 | 基于口令的密钥派生函数 2，从密码派生密钥 |
| PCB | Printed Circuit Board | 印制电路板 |
| PDU | Protocol Data Unit | 协议数据单元，协议层间传输的数据包 |
| PER | Packet Error Rate | 误包率，射频接收质量指标 |
| PHY | Physical Layer | 物理层，决定比特怎么在空中传输 |
| PID | Process ID / Parameter ID | 进程标识符 / 参数标识符 |
| PINCTRL | Pin Control | 引脚控制，配置芯片 GPIO 引脚复用方式与电气属性的模块 |
| PINMUX | Pin Multiplexing | 引脚复用，一个物理引脚可配置为多种功能 |
| PKE | Public Key Engine | 公钥密码引擎，硬件加速模块，用于 RSA、ECC 签名/验签、ECDH 密钥协商及大数运算等公钥密码运算 |
| PMF | Protected Management Frames | 受保护的管理帧，Wi-Fi 管理帧加密保护 |
| PMP | Physical Memory Protection | 物理内存保护，RISC-V 指令集定义的硬件内存保护机制，为物理内存区域设置读/写/执行权限 |
| PMU | Power Management Unit | 电源管理单元，芯片电源管理核心，负责 LDO 稳压、模块上下电、休眠唤醒 |
| POSIX | Portable Operating System Interface | 可移植操作系统接口，一套操作系统接口标准 |
| PRBS | Pseudo-Random Binary Sequence | 伪随机二进制序列，射频测试用调制序列 |
| PSD | Power Spectral Density | 功率谱密度 |
| PSE | Phone Book Server Equipment | 电话簿服务端设备，PBAP 协议的服务端角色 |
| PSK | Pre-Shared Key | 预共享密钥 |
| PWM | Pulse Width Modulation | 脉冲宽度调制，通过调整高低电平的时间比例来控制平均电压 |
| QPSK | Quadrature Phase Shift Keying | 正交相移键控，一种调制方式 |
| QSPI | Quad Serial Peripheral Interface | 四线串行外设接口，SPI 的 4 倍数据宽度变体 |
| RAM | Random Access Memory | 随机存取存储器，易失性内存 |
| REE | Rich Execution Environment | 富执行环境，非安全侧的常规运行环境（与 TEE 相对） |
| RESTful | Representational State Transfer | 表现层状态转移（RESTful） |
| RF | Radio Frequency | 射频，无线通信中使用的频率范围 |
| RFCOMM | Radio Frequency Communication | 射频通信，RS-232 仿真协议，为上层提供串口仿真 |
| RISC | RISC-V | RISC-V 开源精简指令集架构（WS63 采用 RISC-V CPU） |
| ROM | Read-Only Memory | 只读存储器，出厂固化的程序存储 |
| RS485 | Recommended Standard 485 | RS-485 差分串行通信标准，支持长距离多节点 |
| RSA | Rivest-Shamir-Adleman | RSA 非对称加密算法，用于公钥加密/签名 |
| RSSI | Received Signal Strength Indicator | 接收信号强度指示，单位 dBm，可用于距离估算，但受环境多径、遮挡等因素影响，精度有限 |
| RTC | Real-Time Clock | 实时时钟，独立于系统主时钟的硬件定时器，由独立电源域供电，即使系统深度休眠仍继续运行 |
| RTOS | Real-Time Operating System | 实时操作系统，提供任务调度、内存管理等功能 |
| RTS | Request To Send | 请求发送，UART 硬件流控的发送请求信号 |
| RX | Receive | 接收，指数据或信号的接收端或接收方向 |
| SAE | Simultaneous Authentication of Equals | WPA3 对等同时认证机制 |
| SAR | Successive Approximation Register | 逐次逼近型，ADC 的一种常见架构，WS63 片内 ADC 为 SAR 型 |
| SBC | Sub-Band Coding | 子带编码，A2DP 强制要求的蓝牙音频编码格式 |
| SCK | Serial Clock | 串行时钟，SPI 总线的时钟信号 |
| SCL | Serial Clock Line | 串行时钟线，I2C 总线的时钟信号 |
| SCO | Synchronous Connection-Oriented | 面向连接的同步链路，蓝牙语音通话使用的同步数据链路 |
| SDA | Serial Data Line | 串行数据线，I2C 总线的数据信号 |
| SDATA | Serial Data | 串行数据，I2S 数字音频接口的数据信号 |
| SDIO | Secure Digital Input/Output | 安全数字输入输出，标准化的芯片间高速通信接口 |
| SDK | Software Development Kit | 软件开发工具包 |
| SDP | Service Discovery Protocol | 服务发现协议 |
| SFC | Serial Flash Controller | 串行闪存控制器 |
| SHA | Secure Hash Algorithm | 安全哈希算法，文档中提及"安全哈希（SHA）" |
| SHA1 | Secure Hash Algorithm 1 | 安全哈希算法 1，输出 160 位摘要（已不推荐） |
| SHA224 | Secure Hash Algorithm 224-bit | 安全哈希算法，输出 224 位摘要 |
| SHA256 | Secure Hash Algorithm 256-bit | 安全哈希算法，输出 256 位摘要 |
| SHA384 | Secure Hash Algorithm 384-bit | 安全哈希算法，输出 384 位摘要 |
| SHA512 | Secure Hash Algorithm 512-bit | 安全哈希算法，输出 512 位摘要 |
| SIGIO | Signal I/O | I/O 异步信号，内核向用户态发送的异步通知信号 |
| SIM | Subscriber Identity Module | 用户身份模块，存储运营商签约身份信息的智能卡 |
| SIO | Serial I/O | 串行输入/输出，以串行方式收发数据的硬件接口 |
| SIX | Portable Operating System Interface | 可移植操作系统接口（POSIX），Unix 类系统的标准化接口规范 |
| SLB | SparkLink Basic | 星闪基础高速率模式，高带宽、高吞吐，适用于音视频传输、文件分享、无线投屏 |
| SLC | Service Level Connection | 服务级连接（HFP 中的 SL 连接） |
| SLE | SparkLink Low Energy | 星闪低功耗低延迟通信协议，中国自主研发的新一代无线短距离通信技术标准，适用于鼠标键盘、游戏手柄、传感器、可穿戴设备 |
| SLP | SparkLink Positioning | 星闪定位/低功耗定位特性，基于 SLE 提供高精度测距与定位能力（厘米级精度、超低功耗），是 SparkLink 的感知定位子系统，项目中也作为雷达子模块的命名 |
| SM3 | SM3 Cryptographic Hash Algorithm | 国密 SM3 密码杂凑算法，输出 256 位摘要，中国商用密码标准 |
| SM4 | SM4 Block Cipher | 国密 SM4 分组密码算法，中国商用密码标准，文档中提及"SM4-128-CCM (EA2)" |
| SMC | Secure Monitor Call | 安全监控调用，触发安全世界切换的指令 |
| SMP | Security Manager Protocol | 安全管理协议，BLE 协议中负责配对和密钥管理 |
| SPI | Serial Peripheral Interface | 串行外设接口，Motorola 发明的四线制同步串行总线 |
| SPP | Serial Port Profile | 串口协议，BR/EDR 蓝牙串口通信配置 |
| SRAM | Static Random Access Memory | 静态随机存取存储器，WS63 片内高速内存 |
| SSAP | SLE Service Access Protocol | SLE 服务访问协议，定义了设备之间如何组织和访问数据，采用 Service → Property → Descriptor 三级结构 |
| SSB | Secure Secondary Boot | 安全二级引导，芯片上电后经过签名校验的第二阶段启动流程 |
| SSI | Synchronous Serial Interface | 同步串行接口，简化版 SPI，专为寄存器级外设通信设计，适合 RF CMOS 前端、PMIC 等只需寄存器读写的芯片 |
| SSID | Service Set Identifier | 服务集标识符，即无线网络名称 |
| STA | Station | Wi-Fi 站点（终端设备），连接 AP 接入无线网络 |
| SWD | Serial Wire Debug | 串行线调试，ARM 的一种调试接口 |
| TCP | Transmission Control Protocol | 传输控制协议，面向连接的可靠传输协议 |
| TCXO | Temperature Compensated Crystal Oscillator | 温度补偿晶振，补偿温度对晶振频率的影响，保证射频通信的时钟精度 |
| TEE | Trusted Execution Environment | 可信执行环境，安全侧的隔离运行环境 |
| TFT | Thin Film Transistor | 薄膜场效应晶体管，TFT-LCD 主动矩阵液晶显示 |
| TG | Audio/Video Remote Control Profile Target | 音视频远程控制协议目标端 |
| TKIP | Temporal Key Integrity Protocol | 临时密钥完整性协议，WPA 使用的加密协议 |
| TLS | Transport Layer Security | 传输层安全协议，文档中提及"mbedTLS（安全传输层）" |
| TLV | Tag-Length-Value | 标签-长度-值，通用数据封装格式 |
| ToF | Time of Flight | 飞行时间，通过测量无线信号飞行时间来估算距离 |
| TPC | Transmit Power Control | 发射功率控制 |
| TRNG | True Random Number Generator | 真随机数生成器，比 rand() 伪随机数安全得多，用于密钥和 IV 生成 |
| TTL | Time To Live | 生存时间，IP 数据包在网络中的最大传输跳数 |
| TX | Transmit | 发送，指数据或信号的发送端或发送方向 |
| UAPI | Unified API | 统一应用编程接口层，WS63 面向应用的统一 API 前缀（如 uapi_adc_init） |
| UART | Universal Asynchronous Receiver/Transmitter | 通用异步收发器，嵌入式最常用的异步串行通信接口，仅需 TX/RX/GND 三根线 |
| UDP | User Datagram Protocol | 用户数据报协议，无连接的不可靠传输协议 |
| UPG | Upgrade | 固件升级框架，负责固件包校验写入和升级流程管理 |
| URI | Uniform Resource Identifier | 统一资源标识符 |
| USB | Universal Serial Bus | 通用串行总线，外设连接标准 |
| UTF | Unicode Transformation Format | Unicode 转换格式，如 UTF-8 |
| UUID | Universally Unique Identifier | 通用唯一识别码，BLE/SLE 中标识服务与特征 |
| VFS | Virtual File System | 虚拟文件系统，对不同文件系统的抽象 |
| VGA | Variable Gain Amplifier | 可变增益放大器，射频接收链路增益调节 |
| VMA | Virtual Memory Address | 虚拟内存地址，表示程序运行时的内存地址 |
| WAPI | WLAN Authentication and Privacy Infrastructure | 无线局域网鉴别与保密基础结构，中国 WLAN 安全标准 |
| WEP | Wired Equivalent Privacy | 有线等效保密，一种无线局域网安全协议（已过时） |
| WNM | Wireless Network Management | 无线网络管理，Wi-Fi 网络管理帧 |
| WoW | Wake on Wireless | 无线唤醒 |
| WPA | Wi-Fi Protected Access | Wi-Fi 保护访问，无线局域网安全协议 |
| WPA2 | Wi-Fi Protected Access 2 | Wi-Fi 保护访问第二代安全协议 |
| WPA3 | Wi-Fi Protected Access 3 | Wi-Fi 保护访问第三代安全协议（更强安全性） |
| WPS | Wi-Fi Protected Setup | Wi-Fi 保护设置，简化配网的安全标准 |
| XTAL | Crystal Oscillator | 晶体振荡器，提供系统时钟源 |
