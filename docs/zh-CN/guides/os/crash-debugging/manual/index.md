---
title: LiteOS 死机问题定位指南
---

# 前言<a name="ZH-CN_TOPIC_0000002563266117"></a>

**概述<a name="section4537382116410"></a>**

本文档详细的描述了LiteOS结合日志信息定位死机问题的方法和思路。

# 概述<a name="ZH-CN_TOPIC_0000002524734534"></a>




## 背景<a name="ZH-CN_TOPIC_0000002555814459"></a>

在嵌入式系统中，多任务并发、资源竞争、内存管理异常、硬件驱动缺陷或配置不当等因素，系统死机问题频发，FBB-RTOS采用单进程设计且未启用MMU机制，物理内存全局可见的特性在提升效率的同时，也使得非法内存操作更易导致系统级崩溃。相较于Android/Linux等具备完善DFX工具链和日志系统的操作系统，FBB-RTOS的异常诊断面临更大挑战，死机问题往往难以快速定位，尤其是死机发生并非第一现场时，问题排查效率低。

本文旨在系统化梳理FBB-RTOS现有DFX能力及通用诊断方法，为研发团队提供结构化的问题排查思路。需要特别说明的是：

-   死机问题通常与具体业务场景深度耦合。
-   当前文档仅覆盖现有DFX工具的使用方法。
-   内容将随DFX能力增强持续更新。

## 死机日志解读<a name="ZH-CN_TOPIC_0000002524574584"></a>

在FBB-RTOS运行过程中，系统发生死机时会输出关键日志信息，这些日志包含异常类型、错误地址、寄存器状态等关键调试数据。正确解读这些日志信息是定位死机根因的关键步骤。系统死机时常见日志及含义注解如下：

<a name="table988mcpsimp"></a>
<table><tbody><tr id="row992mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p994mcpsimp"><a name="p994mcpsimp"></a><a name="p994mcpsimp"></a><strong id="b2123156153619"><a name="b2123156153619"></a><a name="b2123156153619"></a>Instruction access fault      //①异常类型：如取指异常、Stack overflow、PMP access fault等</strong></p>
<p id="p995mcpsimp"><a name="p995mcpsimp"></a><a name="p995mcpsimp"></a><strong id="b1112835613615"><a name="b1112835613615"></a><a name="b1112835613615"></a>Memory map region access fault</strong></p>
<p id="p996mcpsimp"><a name="p996mcpsimp"></a><a name="p996mcpsimp"></a>task:app_Task            <strong id="b163071328374"><a name="b163071328374"></a><a name="b163071328374"></a> //②产生异常的任务名</strong></p>
<p id="p997mcpsimp"><a name="p997mcpsimp"></a><a name="p997mcpsimp"></a>thrdPid:0x3              <strong id="b1691825153718"><a name="b1691825153718"></a><a name="b1691825153718"></a> //③产生异常的任务id</strong></p>
<p id="p998mcpsimp"><a name="p998mcpsimp"></a><a name="p998mcpsimp"></a>type:0x1</p>
<p id="p999mcpsimp"><a name="p999mcpsimp"></a><a name="p999mcpsimp"></a>nestCnt:1               <strong id="b3657105374"><a name="b3657105374"></a><a name="b3657105374"></a>  //④异常嵌套层数，&gt;1表示在异常处理中发生了异常重入</strong></p>
<p id="p1000mcpsimp"><a name="p1000mcpsimp"></a><a name="p1000mcpsimp"></a>phase:Task</p>
<p id="p1001mcpsimp"><a name="p1001mcpsimp"></a><a name="p1001mcpsimp"></a>ccause:0x1</p>
<p id="p1002mcpsimp"><a name="p1002mcpsimp"></a><a name="p1002mcpsimp"></a>mcause:0x1</p>
<p id="p1003mcpsimp"><a name="p1003mcpsimp"></a><a name="p1003mcpsimp"></a><strong id="b193118331376"><a name="b193118331376"></a><a name="b193118331376"></a>mtval:0xfffffffffe </strong>         <strong id="b871961433714"><a name="b871961433714"></a><a name="b871961433714"></a>//⑤异常时访问的内存地址</strong></p>
<p id="p1004mcpsimp"><a name="p1004mcpsimp"></a><a name="p1004mcpsimp"></a>gp:0x110004</p>
<p id="p1005mcpsimp"><a name="p1005mcpsimp"></a><a name="p1005mcpsimp"></a>mstatus:0x1880</p>
<p id="p1006mcpsimp"><a name="p1006mcpsimp"></a><a name="p1006mcpsimp"></a><strong id="b914218413375"><a name="b914218413375"></a><a name="b914218413375"></a>mepc:0xfffffffffe </strong>        <strong id="b162811923113719"><a name="b162811923113719"></a><a name="b162811923113719"></a> //⑥异常时正在执行的指令地址（可反查到异常相对应代码）</strong></p>
<p id="p1007mcpsimp"><a name="p1007mcpsimp"></a><a name="p1007mcpsimp"></a><strong id="b1935011453379"><a name="b1935011453379"></a><a name="b1935011453379"></a>ra:0x1161a8 </strong>           <strong id="b1838284993715"><a name="b1838284993715"></a><a name="b1838284993715"></a> //⑦异常时异常指令所属父函数的下一条指令地址（反查到函数）</strong></p>
<p id="p1008mcpsimp"><a name="p1008mcpsimp"></a><a name="p1008mcpsimp"></a>sp:0x103930           <strong id="b1896715220376"><a name="b1896715220376"></a><a name="b1896715220376"></a> //⑧异常时任务或者中断的栈指针</strong></p>
<p id="p1009mcpsimp"><a name="p1009mcpsimp"></a><a name="p1009mcpsimp"></a>tp:0xfc631c8</p>
<p id="p1010mcpsimp"><a name="p1010mcpsimp"></a><a name="p1010mcpsimp"></a>t0:0x103875</p>
<p id="p1011mcpsimp"><a name="p1011mcpsimp"></a><a name="p1011mcpsimp"></a>t1:0xa</p>
<p id="p1012mcpsimp"><a name="p1012mcpsimp"></a><a name="p1012mcpsimp"></a>t2:0x11012000</p>
<p id="p1013mcpsimp"><a name="p1013mcpsimp"></a><a name="p1013mcpsimp"></a>s0:0x3</p>
<p id="p1014mcpsimp"><a name="p1014mcpsimp"></a><a name="p1014mcpsimp"></a>s1:0x119338</p>
</td>
</tr>
</tbody>
</table>

注：根据指令地址反查是指代在elf文件中搜索该地址，会与代码段中某行汇编的地址一样，例：

反查mepc:0x115df4，在elf中搜索115df4，说明异常时正在执行的指令为sw a0,0\(a1\)。

<a name="table1017mcpsimp"></a>
<table><tbody><tr id="row1021mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p1023mcpsimp"><a name="p1023mcpsimp"></a><a name="p1023mcpsimp"></a>00115dd8 &lt;app_init&gt;:</p>
<p id="p1024mcpsimp"><a name="p1024mcpsimp"></a><a name="p1024mcpsimp"></a>115dc: 717d addi sp,sp,-16</p>
<p id="p1025mcpsimp"><a name="p1025mcpsimp"></a><a name="p1025mcpsimp"></a>115de0: c606 sw ra,12(sp)</p>
<p id="p1026mcpsimp"><a name="p1026mcpsimp"></a><a name="p1026mcpsimp"></a>115de4: 00108b2a051f l.li a0,108b2a &lt;g_xRegsMap+0x56c&gt;</p>
<p id="p1027mcpsimp"><a name="p1027mcpsimp"></a><a name="p1027mcpsimp"></a>115de8: 7f0000ef jal ra,101196</p>
<p id="p1028mcpsimp"><a name="p1028mcpsimp"></a><a name="p1028mcpsimp"></a>115dec: 12345678051f l.li a0,12345678 &lt;__heap_end+0x12225678&gt;</p>
<p id="p1029mcpsimp"><a name="p1029mcpsimp"></a><a name="p1029mcpsimp"></a>115df0: 018005b7 lui a1,0x1800</p>
<p id="p1030mcpsimp"><a name="p1030mcpsimp"></a><a name="p1030mcpsimp"></a><strong id="b961497103819"><a name="b961497103819"></a><a name="b961497103819"></a>115df4: c188 sw a0,0(a1)</strong></p>
<p id="p1031mcpsimp"><a name="p1031mcpsimp"></a><a name="p1031mcpsimp"></a>115df8: 40b2 lw ra,12(sp)</p>
</td>
</tr>
</tbody>
</table>

## 整体思路<a name="ZH-CN_TOPIC_0000002555654487"></a>

综合历史问题处理经验，总结出FBB-RTOS死机问题的主要原因分布如下：

-   内存踩踏引起的Load/Store access fault
-   栈溢出（stack overflow）
-   取指异常（Instruction access fault）
-   看门狗超时（Watchdog Timeout）
-   Out Of Memory
-   Panic
-   非法内存访问：包括访问保留内存（Store/AMO access fault）、PMP保护内存（PMP access fault）
-   内存布局非对齐（Unaligned memory layout）
-   死锁（Dead Lock）
-   非对齐访问（Unaligned Access）

后续章节将通过典型问题案例，系统性覆盖上述各类异常，结合真实场景提供定位思路与解决方案，帮助开发者快速识别问题根源，高效修复系统异常。

针对死机问题的故障定位，可采用分层递进的分析方法，具体实施步骤如下：

-   **根据异常类型溯源**

    死机日志中的异常类型信息是FBB-RTOS通过解析mcause、ccause等硬件异常寄存器生成，直接反映了触发死机的硬件级或者系统级异常事件。分析时应：

    优先锁定日志中记录的异常类型（如取指异常、栈溢出等），明确最直接的异常诱因。

    根据异常类型分析可能的原因（如取指异常的原因可能为函数指针异常或者代码段异常）。

    针对可能的原因逐个排查分析。

-   **排查异常上下文代码逻辑**

    分析关键寄存器值ra、mepc、mtval可以分别获得死机时正在执行的具体函数、具体哪条汇编指令及操作的内存地址，可排查分析异常上下文代码逻辑。

-   **使用DFX工具进行动态诊断**

    当常规分析无法定位时，可启用DFX机制，开启内核调试配置（如backtrace、内存访问监控trigger等）重新编译后复现问题，获取更详尽的运行时数据。

# 死机问题定位指南<a name="ZH-CN_TOPIC_0000002524734536"></a>

常见关键异常信息及其对应的直接死机原因如下表所示，后文将逐个展开详细说明。

<a name="table768mcpsimp"></a>
<table><thead align="left"><tr id="row773mcpsimp"><th class="cellrowborder" valign="top" width="45%" id="mcps1.1.3.1.1"><p id="p775mcpsimp"><a name="p775mcpsimp"></a><a name="p775mcpsimp"></a><strong id="b776mcpsimp"><a name="b776mcpsimp"></a><a name="b776mcpsimp"></a>关键异常信息</strong></p>
</th>
<th class="cellrowborder" valign="top" width="55.00000000000001%" id="mcps1.1.3.1.2"><p id="p778mcpsimp"><a name="p778mcpsimp"></a><a name="p778mcpsimp"></a><strong id="b779mcpsimp"><a name="b779mcpsimp"></a><a name="b779mcpsimp"></a>死机原因</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row780mcpsimp"><td class="cellrowborder" valign="top" width="45%" headers="mcps1.1.3.1.1 "><p id="p782mcpsimp"><a name="p782mcpsimp"></a><a name="p782mcpsimp"></a>Stack overflow</p>
</td>
<td class="cellrowborder" valign="top" width="55.00000000000001%" headers="mcps1.1.3.1.2 "><p id="p784mcpsimp"><a name="p784mcpsimp"></a><a name="p784mcpsimp"></a>栈溢出</p>
</td>
</tr>
<tr id="row785mcpsimp"><td class="cellrowborder" valign="top" width="45%" headers="mcps1.1.3.1.1 "><p id="p787mcpsimp"><a name="p787mcpsimp"></a><a name="p787mcpsimp"></a>Instruction access fault</p>
</td>
<td class="cellrowborder" valign="top" width="55.00000000000001%" headers="mcps1.1.3.1.2 "><p id="p789mcpsimp"><a name="p789mcpsimp"></a><a name="p789mcpsimp"></a>取指异常</p>
</td>
</tr>
<tr id="row790mcpsimp"><td class="cellrowborder" valign="top" width="45%" headers="mcps1.1.3.1.1 "><p id="p792mcpsimp"><a name="p792mcpsimp"></a><a name="p792mcpsimp"></a>Oops:NMI</p>
</td>
<td class="cellrowborder" valign="top" width="55.00000000000001%" headers="mcps1.1.3.1.2 "><p id="p794mcpsimp"><a name="p794mcpsimp"></a><a name="p794mcpsimp"></a>狗超时</p>
</td>
</tr>
<tr id="row795mcpsimp"><td class="cellrowborder" valign="top" width="45%" headers="mcps1.1.3.1.1 "><p id="p797mcpsimp"><a name="p797mcpsimp"></a><a name="p797mcpsimp"></a>Panic</p>
</td>
<td class="cellrowborder" valign="top" width="55.00000000000001%" headers="mcps1.1.3.1.2 "><p id="p799mcpsimp"><a name="p799mcpsimp"></a><a name="p799mcpsimp"></a>主动Panic</p>
</td>
</tr>
<tr id="row800mcpsimp"><td class="cellrowborder" valign="top" width="45%" headers="mcps1.1.3.1.1 "><p id="p802mcpsimp"><a name="p802mcpsimp"></a><a name="p802mcpsimp"></a>PMP access fault</p>
</td>
<td class="cellrowborder" valign="top" width="55.00000000000001%" headers="mcps1.1.3.1.2 "><p id="p804mcpsimp"><a name="p804mcpsimp"></a><a name="p804mcpsimp"></a>访问PMP保护的地址</p>
</td>
</tr>
<tr id="row805mcpsimp"><td class="cellrowborder" valign="top" width="45%" headers="mcps1.1.3.1.1 "><p id="p807mcpsimp"><a name="p807mcpsimp"></a><a name="p807mcpsimp"></a>Store/AMO access fault</p>
</td>
<td class="cellrowborder" valign="top" width="55.00000000000001%" headers="mcps1.1.3.1.2 "><p id="p809mcpsimp"><a name="p809mcpsimp"></a><a name="p809mcpsimp"></a>访问保留内存</p>
</td>
</tr>
<tr id="row810mcpsimp"><td class="cellrowborder" valign="top" width="45%" headers="mcps1.1.3.1.1 "><p id="p812mcpsimp"><a name="p812mcpsimp"></a><a name="p812mcpsimp"></a>Dead lock</p>
</td>
<td class="cellrowborder" valign="top" width="55.00000000000001%" headers="mcps1.1.3.1.2 "><p id="p814mcpsimp"><a name="p814mcpsimp"></a><a name="p814mcpsimp"></a>死锁</p>
</td>
</tr>
<tr id="row815mcpsimp"><td class="cellrowborder" valign="top" width="45%" headers="mcps1.1.3.1.1 "><p id="p817mcpsimp"><a name="p817mcpsimp"></a><a name="p817mcpsimp"></a>Load/Store address misaligned</p>
</td>
<td class="cellrowborder" valign="top" width="55.00000000000001%" headers="mcps1.1.3.1.2 "><p id="p819mcpsimp"><a name="p819mcpsimp"></a><a name="p819mcpsimp"></a>非对齐访问</p>
</td>
</tr>
</tbody>
</table>











## 栈溢出<a name="ZH-CN_TOPIC_0000002555814461"></a>




### 问题识别<a name="ZH-CN_TOPIC_0000002524574586"></a>

FBB-RTOS默认开启栈溢出检测，检测方法分为魔术字检测和硬件栈检测。

-   魔术字检测原理：在栈顶填充4字节（32位）的魔术字，任务切换时，软件会检查魔术字是否被改写，若魔术字被修改，则判定为栈溢出。
-   硬件栈检测原理：部分芯片提供栈限制寄存器，任务切换或中断时，系统会将新任务的栈顶指针写入栈限制寄存器，芯片硬件会自动检测栈指针SP是否越界。

    检测到栈溢出时系统会挂死，并在异常类型中输出stack overflow。例：

    <a name="table1339mcpsimp"></a>
    <table><tbody><tr id="row1343mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p1345mcpsimp"><a name="p1345mcpsimp"></a><a name="p1345mcpsimp"></a>[2025-10-30 21:47:07] CURRENT task ID: <strong id="b1546192413818"><a name="b1546192413818"></a><a name="b1546192413818"></a>Task_A:9 stack overflow!</strong></p>
    </td>
    </tr>
    </tbody>
    </table>

### 原因分析<a name="ZH-CN_TOPIC_0000002555654489"></a>

-   **过大的局部变量**：在函数内定义超大数组或复杂结构体。

-   **栈空间分配不足**：**未预留安全余量**，即任务栈大小设置时，未考虑最坏执行路径的栈需求（如异常处理分支）。

### 问题定位<a name="ZH-CN_TOPIC_0000002524734538"></a>

**定位方法<a name="section893713318437"></a>**

1.  **线程识别**：通过系统日志中的任务名称和任务ID（task ID:）可确定发生栈溢出的具体线程。
2.  **使用静态估算工具**：利用栈估算工具估算任务/函数的栈使用峰值，识别栈消耗过大的函数（如含大型局部变量的函数），结合task命令查看任务栈当前配置的大小，确定是函数栈开销过大亦或是配置不合理。工具使用方法参见LiteOS开发指南（不足：对函数指针调用链的分析存在误差）。

**参考案例<a name="section164812434435"></a>**

<a name="table1361mcpsimp"></a>
<table><tbody><tr id="row1365mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p1367mcpsimp"><a name="p1367mcpsimp"></a><a name="p1367mcpsimp"></a>[2025-10-30 21:47:07] CURRENT task ID: <strong id="b811043433810"><a name="b811043433810"></a><a name="b811043433810"></a>Task_A:9 stack overflow!</strong></p>
<p id="p1368mcpsimp"><a name="p1368mcpsimp"></a><a name="p1368mcpsimp"></a>[2025-10-30 21:47:07] B</p>
<p id="p1369mcpsimp"><a name="p1369mcpsimp"></a><a name="p1369mcpsimp"></a>[2025-10-30 21:47:41] breakpoint</p>
<p id="p1370mcpsimp"><a name="p1370mcpsimp"></a><a name="p1370mcpsimp"></a>[2025-10-30 21:47:41] Not available</p>
<p id="p1371mcpsimp"><a name="p1371mcpsimp"></a><a name="p1371mcpsimp"></a>[2025-10-30 21:47:41] APP&gt;exception:42</p>
<p id="p1372mcpsimp"><a name="p1372mcpsimp"></a><a name="p1372mcpsimp"></a>[2025-10-30 21:47:41] task:Task_A</p>
<p id="p1373mcpsimp"><a name="p1373mcpsimp"></a><a name="p1373mcpsimp"></a>[2025-10-30 21:47:41] thrdPid:0x9</p>
<p id="p1374mcpsimp"><a name="p1374mcpsimp"></a><a name="p1374mcpsimp"></a>[2025-10-30 21:47:41] type:0x42</p>
<p id="p1375mcpsimp"><a name="p1375mcpsimp"></a><a name="p1375mcpsimp"></a>[2025-10-30 21:47:41] nestCnt:1</p>
<p id="p1376mcpsimp"><a name="p1376mcpsimp"></a><a name="p1376mcpsimp"></a>[2025-10-30 21:47:41] phase:Task</p>
<p id="p1377mcpsimp"><a name="p1377mcpsimp"></a><a name="p1377mcpsimp"></a>[2025-10-30 21:47:41] ccause:0x73616870</p>
<p id="p1378mcpsimp"><a name="p1378mcpsimp"></a><a name="p1378mcpsimp"></a>[2025-10-30 21:47:41] mcause:0x61543a65</p>
<p id="p1379mcpsimp"><a name="p1379mcpsimp"></a><a name="p1379mcpsimp"></a>[2025-10-30 21:47:41] mtval:0xaab73</p>
<p id="p1380mcpsimp"><a name="p1380mcpsimp"></a><a name="p1380mcpsimp"></a>[2025-10-30 21:47:41] gp:0x0</p>
<p id="p1381mcpsimp"><a name="p1381mcpsimp"></a><a name="p1381mcpsimp"></a>[2025-10-30 21:47:41] mstatus:0x0</p>
<p id="p1382mcpsimp"><a name="p1382mcpsimp"></a><a name="p1382mcpsimp"></a>[2025-10-30 21:47:41] mepc:0x0</p>
<p id="p1383mcpsimp"><a name="p1383mcpsimp"></a><a name="p1383mcpsimp"></a>[2025-10-30 21:47:41] ra:0x37df6</p>
<p id="p1384mcpsimp"><a name="p1384mcpsimp"></a><a name="p1384mcpsimp"></a>[2025-10-30 21:47:41] sp:0x0</p>
</td>
</tr>
</tbody>
</table>

-   根据日志中的“stack overflow”可确认是栈溢出问题。
-   根据日志中的"task ID: Task\_A:9"可确认发生栈溢出任务为Task\_A。
-   根据task命令获知栈大小为默认值，过小，调整栈大小后解决。

## 取指异常<a name="ZH-CN_TOPIC_0000002555814463"></a>




### 问题识别<a name="ZH-CN_TOPIC_0000002524574588"></a>

取指异常（Instruction Fetch Exception）通常发生在CPU尝试从非法或不可执行的内存地址读取指令，异常类型为Instruction access fault。例：

<a name="table579mcpsimp"></a>
<table><tbody><tr id="row583mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p585mcpsimp"><a name="p585mcpsimp"></a><a name="p585mcpsimp"></a><strong id="b24221343183817"><a name="b24221343183817"></a><a name="b24221343183817"></a>Instruction access fault</strong></p>
<p id="p586mcpsimp"><a name="p586mcpsimp"></a><a name="p586mcpsimp"></a><strong id="b134231643143818"><a name="b134231643143818"></a><a name="b134231643143818"></a>Memory map region access fault</strong></p>
</td>
</tr>
</tbody>
</table>

### 原因分析<a name="ZH-CN_TOPIC_0000002555654491"></a>

-   **使用空指针/野指针**：当函数指针为空或者未初始化/未赋值时，函数指针值为随机值。

-   **代码段被踩**：程序和数据放置在相邻内存区域，当写数据操作发生越界时（memcpy/memset）会造成代码区被重写。

-   **代码段未拷贝**：设备启动时通常会从外部存储介质中读取代码，当启动时未将代码段拷贝到对应的ram中会造成代码段不存在。

-   **链接脚本配置问题：**链接脚本支持配置多个代码段，当将A代码段链接到B代码段时，会导致代码段搬移到错误代码段。

### 问题定位<a name="ZH-CN_TOPIC_0000002524734540"></a>

**定位方法<a name="section96841617104412"></a>**

-   **排查函数指针**：根据mtval、ra定位异常代码上下文，排查异常是否由函数指针导致，若是则检查函数指针是否为空指针、野指针。

-   **排查链接脚本：**检查异常函数所在代码段是否有拷贝动作，若有则检查链接脚本，查看拷贝的源地址、目的地址是否符合预期。

-   **排查代码段拷贝**：比较异常函数运行地址的内存值是否与反汇编中函数的第一条指令值相同，若不同则排查代码段被踩或者代码段未拷贝/错拷贝。

-   **代码段监测**：利用PMP将异常函数所在代码段配置为RX权限，监测代码段是否被踩。

**参考案例<a name="section158209424512"></a>**

<a name="table1141mcpsimp"></a>
<table><tbody><tr id="row1145mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p1147mcpsimp"><a name="p1147mcpsimp"></a><a name="p1147mcpsimp"></a><strong id="b1879317113919"><a name="b1879317113919"></a><a name="b1879317113919"></a>Instruction access fault            // 异常信息</strong></p>
<p id="p1148mcpsimp"><a name="p1148mcpsimp"></a><a name="p1148mcpsimp"></a><strong id="b479518115391"><a name="b479518115391"></a><a name="b479518115391"></a>Memory map region access fault    // 异常信息</strong></p>
<p id="p1149mcpsimp"><a name="p1149mcpsimp"></a><a name="p1149mcpsimp"></a>task:app_Task</p>
<p id="p1150mcpsimp"><a name="p1150mcpsimp"></a><a name="p1150mcpsimp"></a>thrdPid:0x3</p>
<p id="p1151mcpsimp"><a name="p1151mcpsimp"></a><a name="p1151mcpsimp"></a>type:0x1</p>
<p id="p1152mcpsimp"><a name="p1152mcpsimp"></a><a name="p1152mcpsimp"></a>nestCnt:1</p>
<p id="p1153mcpsimp"><a name="p1153mcpsimp"></a><a name="p1153mcpsimp"></a>phase:Task</p>
<p id="p1154mcpsimp"><a name="p1154mcpsimp"></a><a name="p1154mcpsimp"></a>ccause:0x1</p>
<p id="p1155mcpsimp"><a name="p1155mcpsimp"></a><a name="p1155mcpsimp"></a>mcause:0x1</p>
<p id="p1156mcpsimp"><a name="p1156mcpsimp"></a><a name="p1156mcpsimp"></a>mtval:0xfffffffffe</p>
<p id="p1157mcpsimp"><a name="p1157mcpsimp"></a><a name="p1157mcpsimp"></a>gp:0x110004</p>
<p id="p1158mcpsimp"><a name="p1158mcpsimp"></a><a name="p1158mcpsimp"></a>mstatus:0x1880</p>
<p id="p1159mcpsimp"><a name="p1159mcpsimp"></a><a name="p1159mcpsimp"></a><strong id="b15431112403913"><a name="b15431112403913"></a><a name="b15431112403913"></a>mepc:0xfffffffffe                // 访问的异常指令</strong></p>
<p id="p1160mcpsimp"><a name="p1160mcpsimp"></a><a name="p1160mcpsimp"></a><strong id="b44311224113917"><a name="b44311224113917"></a><a name="b44311224113917"></a>ra:0x1161a8                   // 父函数</strong></p>
<p id="p1161mcpsimp"><a name="p1161mcpsimp"></a><a name="p1161mcpsimp"></a>sp:0x103930</p>
<p id="p1162mcpsimp"><a name="p1162mcpsimp"></a><a name="p1162mcpsimp"></a>tp:0xfc631c8</p>
<p id="p1163mcpsimp"><a name="p1163mcpsimp"></a><a name="p1163mcpsimp"></a>t0:0x103875</p>
<p id="p1164mcpsimp"><a name="p1164mcpsimp"></a><a name="p1164mcpsimp"></a>t1:0xa</p>
<p id="p1165mcpsimp"><a name="p1165mcpsimp"></a><a name="p1165mcpsimp"></a>t2:0x11012000</p>
<p id="p1166mcpsimp"><a name="p1166mcpsimp"></a><a name="p1166mcpsimp"></a>s0:0x3</p>
<p id="p1167mcpsimp"><a name="p1167mcpsimp"></a><a name="p1167mcpsimp"></a>s1:0x119338</p>
</td>
</tr>
</tbody>
</table>

-   定位异常函数：根据ra的值0x807556，在反汇编文件中定位当前正在执行的函数。

    <a name="table1170mcpsimp"></a>
    <table><tbody><tr id="row1174mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p1176mcpsimp"><a name="p1176mcpsimp"></a><a name="p1176mcpsimp"></a>__attribute__((weak)) void app_init(void)</p>
    <p id="p1177mcpsimp"><a name="p1177mcpsimp"></a><a name="p1177mcpsimp"></a>{</p>
    <p id="p1178mcpsimp"><a name="p1178mcpsimp"></a><a name="p1178mcpsimp"></a>fn_test_t fn = (fn_test_t)(UINT32 *)<strong id="b13407183515393"><a name="b13407183515393"></a><a name="b13407183515393"></a>0xffffffff</strong>;</p>
    <p id="p1179mcpsimp"><a name="p1179mcpsimp"></a><a name="p1179mcpsimp"></a>fn();</p>
    <p id="p1180mcpsimp"><a name="p1180mcpsimp"></a><a name="p1180mcpsimp"></a>}</p>
    </td>
    </tr>
    </tbody>
    </table>

-   确认函数内有函数指针调用，且函数指针异常。

## 看门狗超时<a name="ZH-CN_TOPIC_0000002555814465"></a>




### 问题识别<a name="ZH-CN_TOPIC_0000002524574590"></a>

FBB-RTOS运行时需要定期喂狗（重置定时器），若未按时喂狗，看门狗计时器溢出，会产生NMI异常，并在异常类型中输出Oops:NMI。例：

<a name="table565mcpsimp"></a>
<table><tbody><tr id="row569mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p571mcpsimp"><a name="p571mcpsimp"></a><a name="p571mcpsimp"></a>try to enter wfi</p>
<p id="p572mcpsimp"><a name="p572mcpsimp"></a><a name="p572mcpsimp"></a><strong id="b61751044183917"><a name="b61751044183917"></a><a name="b61751044183917"></a>Oops:NMI</strong></p>
<p id="p573mcpsimp"><a name="p573mcpsimp"></a><a name="p573mcpsimp"></a>task:pm_suspend_task</p>
<p id="p574mcpsimp"><a name="p574mcpsimp"></a><a name="p574mcpsimp"></a>thrdPid:0x6</p>
</td>
</tr>
</tbody>
</table>

### 原因分析<a name="ZH-CN_TOPIC_0000002555654493"></a>

-   **CPU资源被占用**
    -   CPU资源被中断和高优先级任务占用，具体原因包括：
        1.  **中断处理过长**：中断处理时间过长或者中断无法正常退出。
        2.  **中断风暴**：某个外设产生大量中断，导致CPU忙于处理中断。
        3.  **高优先级任务异常**：某个高优先级任务由于bug而一直运行\(比如进入死循环\)，导致低优先级的喂狗任务永远得不到时间片。

    -   **高优先级任务一直在抢占CPU**：系统负载过高，轮不到喂狗任务。

-   **喂狗任务调度异常**
    -   **任务参数错误**：喂狗任务周期大于看门狗超时阈值。
    -   **任务被误删除**：错误的API调用误删除喂狗任务。
    -   **调度器故障**：任务调度异常。

### 问题定位<a name="ZH-CN_TOPIC_0000002524734542"></a>

**定位方法<a name="section1233958164712"></a>**

-   **初步分析**

    根据MEPC查找反汇编，排查分析异常上下文代码逻辑。

-   **问题分析**

    分析看门狗超时问题，需重点考察系统挂死前关键时间段内（即喂狗超时窗口期内）的任务（含中断）调度和执行情况，具体可从以下三个维度进行分析：

    1.  **CPU空闲状态检查：**若存在CPU idle时段，则表明喂狗任务调度出现异常。
    2.  **任务CPU占用分析：**若发现某个任务CPU占用显著异常，则可能是该任务陷入死循环或逻辑错误。
    3.  **系统负载评估：**若出现频繁任务切换且CPU利用率持续满载，则是系统整体负载过高导致喂狗任务无法及时调度。

    上述三个维度的信息可通过系统提供的DFX工具trace或者调度统计获取，trace和调度统计具体使用方法可分别参考《LiteOS 开发指南》的“Trace”章节和“调度统计”章节内容。。

-   **针对CPU占用异常高任务的诊断方法**

    若发现某任务CPU占用率异常高，需获取其完整调用栈以精确定位问题代码，可以打开系统的backtrace功能并在NMI异常中补充输出所有任务的调用栈。

**参考案例<a name="section5928114265516"></a>**

<a name="table848mcpsimp"></a>
<table><tbody><tr id="row852mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p854mcpsimp"><a name="p854mcpsimp"></a><a name="p854mcpsimp"></a>[APP][00026106]:[INFO app_pm_suspend-&gt;15]:app_pm_suspend</p>
<p id="p855mcpsimp"><a name="p855mcpsimp"></a><a name="p855mcpsimp"></a>pm_suspend.</p>
<p id="p856mcpsimp"><a name="p856mcpsimp"></a><a name="p856mcpsimp"></a>try to enter wfi</p>
<p id="p857mcpsimp"><a name="p857mcpsimp"></a><a name="p857mcpsimp"></a><strong id="b1190115313914"><a name="b1190115313914"></a><a name="b1190115313914"></a>Oops:NMI</strong></p>
<p id="p858mcpsimp"><a name="p858mcpsimp"></a><a name="p858mcpsimp"></a>task:pm_suspend_task</p>
<p id="p859mcpsimp"><a name="p859mcpsimp"></a><a name="p859mcpsimp"></a>thrdPid:0x6</p>
<p id="p860mcpsimp"><a name="p860mcpsimp"></a><a name="p860mcpsimp"></a>type:0xc</p>
<p id="p861mcpsimp"><a name="p861mcpsimp"></a><a name="p861mcpsimp"></a>nestCnt:0</p>
<p id="p862mcpsimp"><a name="p862mcpsimp"></a><a name="p862mcpsimp"></a>phase:1</p>
<p id="p863mcpsimp"><a name="p863mcpsimp"></a><a name="p863mcpsimp"></a>ccause:0x2</p>
<p id="p864mcpsimp"><a name="p864mcpsimp"></a><a name="p864mcpsimp"></a>mcause:0x8000000c</p>
<p id="p865mcpsimp"><a name="p865mcpsimp"></a><a name="p865mcpsimp"></a>mtval:0x0</p>
<p id="p866mcpsimp"><a name="p866mcpsimp"></a><a name="p866mcpsimp"></a>gp:0x7720572a</p>
<p id="p867mcpsimp"><a name="p867mcpsimp"></a><a name="p867mcpsimp"></a>mstatus:0x1880</p>
<p id="p868mcpsimp"><a name="p868mcpsimp"></a><a name="p868mcpsimp"></a><strong id="b122301928404"><a name="b122301928404"></a><a name="b122301928404"></a>mepc:0x1182c4</strong></p>
<p id="p869mcpsimp"><a name="p869mcpsimp"></a><a name="p869mcpsimp"></a>ra:0x1182bc</p>
<p id="p870mcpsimp"><a name="p870mcpsimp"></a><a name="p870mcpsimp"></a>sp:0x103920</p>
</td>
</tr>
</tbody>
</table>

根据mepc的值0x1182c4，查看反汇编

<a name="table872mcpsimp"></a>
<table><tbody><tr id="row876mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p878mcpsimp"><a name="p878mcpsimp"></a><a name="p878mcpsimp"></a>001182C0 &lt;wfi_loop&gt;:</p>
<p id="p879mcpsimp"><a name="p879mcpsimp"></a><a name="p879mcpsimp"></a>1182c0: 10500073	   wfi</p>
<p id="p880mcpsimp"><a name="p880mcpsimp"></a><a name="p880mcpsimp"></a><strong id="b778210136409"><a name="b778210136409"></a><a name="b778210136409"></a>1182c4: bff5	            j 1182c0 &lt;wfi_loop&gt;</strong></p>
<p id="p881mcpsimp"><a name="p881mcpsimp"></a><a name="p881mcpsimp"></a>1182c6: bfcd	            j 1182b8 &lt;drv_pm_suspend+0x76&gt;</p>
</td>
</tr>
</tbody>
</table>

找到对应代码：

<a name="table883mcpsimp"></a>
<table><tbody><tr id="row887mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p889mcpsimp"><a name="p889mcpsimp"></a><a name="p889mcpsimp"></a>while (1)  {</p>
<p id="p890mcpsimp"><a name="p890mcpsimp"></a><a name="p890mcpsimp"></a>pm_printk("try to enter wfi\n");</p>
<p id="p891mcpsimp"><a name="p891mcpsimp"></a><a name="p891mcpsimp"></a>asm volatile("fence\n\r"</p>
<p id="p892mcpsimp"><a name="p892mcpsimp"></a><a name="p892mcpsimp"></a>"wfi_loop:\n\r"</p>
<p id="p893mcpsimp"><a name="p893mcpsimp"></a><a name="p893mcpsimp"></a>"wfi\n\r"</p>
<p id="p894mcpsimp"><a name="p894mcpsimp"></a><a name="p894mcpsimp"></a>"j wfi_loop\n\r");</p>
<p id="p895mcpsimp"><a name="p895mcpsimp"></a><a name="p895mcpsimp"></a>}</p>
</td>
</tr>
</tbody>
</table>

说明系统进入低功耗/待机，此时只有中断才能唤醒，分析中断间隔，发现中断频率小于喂狗时间，引起狗超时重启，属于调度异常范畴。

## Panic<a name="ZH-CN_TOPIC_0000002555814467"></a>




### 问题识别<a name="ZH-CN_TOPIC_0000002524574592"></a>

软件主动Panic是系统针对不可恢复或严重违反系统规则时采取的主动终止机制，系统会输出详细的错误诊断信息，主要包括以下关键内容：

-   **错误描述：**panic的原因（如“ASSERT ERROR!  at xxx.c:123”）。
-   **当前运行任务上下文：**当前任务名称、任务ID、寄存器值。

### 原因分析<a name="ZH-CN_TOPIC_0000002555654495"></a>

-   **断言：**LOS\_ASSERT\(expr\)宏在条件不满足时触发，通常指示程序逻辑错误或系统状态异常。

-   **LOS\_Panic：**LOS\_Panic\(\)函数在检测到关键系统异常时调用，如重复内存释放、堆内存头被踩踏。

### 问题定位<a name="ZH-CN_TOPIC_0000002524734544"></a>

1.  **定位方法**

-   **初步定位Panic源头**

    若日志包含文件名和行号，直接查看对应源码，确认问题位置。

    搜索代码中的打印信息或结合mepc（PC寄存器值）定位挂死代码行。

    若为断言失败，检查断言表达式，分析为何不成立（如空指针、越界、非法状态等）。

-   **根据LOS\_PANIC原因深入排查**

    根据LOS\_PANIC原因如重复内存释放、堆内存头被踩踏等参考相对应章节分析方法。

**参考案例<a name="section16978820165611"></a>**

**例1：断言失败**

<a name="table181143365716"></a>
<table><tbody><tr id="row4111133155716"><td class="cellrowborder" valign="top" width="100%"><p id="p91113332578"><a name="p91113332578"></a><a name="p91113332578"></a><strong id="b63758254407"><a name="b63758254407"></a><a name="b63758254407"></a>ASSERT ERROR! los_cpup.c, 378, OsCpupGetCycle</strong></p>
<p id="p711123319571"><a name="p711123319571"></a><a name="p711123319571"></a>task:CpupGuardCreator</p>
<p id="p13111033125718"><a name="p13111033125718"></a><a name="p13111033125718"></a>thrdPid:0x0</p>
<p id="p13118334572"><a name="p13118334572"></a><a name="p13118334572"></a>type:0x2</p>
<p id="p011133165717"><a name="p011133165717"></a><a name="p011133165717"></a>nestCnt:1</p>
<p id="p1911113312577"><a name="p1911113312577"></a><a name="p1911113312577"></a>phase:1</p>
<p id="p311173316571"><a name="p311173316571"></a><a name="p311173316571"></a>ccause:0x0</p>
<p id="p511133316572"><a name="p511133316572"></a><a name="p511133316572"></a>mcause:0x0</p>
<p id="p1611133317576"><a name="p1611133317576"></a><a name="p1611133317576"></a>mtval:0x0</p>
<p id="p8117335579"><a name="p8117335579"></a><a name="p8117335579"></a>gp:0x0</p>
<p id="p131110332576"><a name="p131110332576"></a><a name="p131110332576"></a>mstatus:0x0</p>
<p id="p1511143355710"><a name="p1511143355710"></a><a name="p1511143355710"></a>mepc:0x0</p>
<p id="p61113332571"><a name="p61113332571"></a><a name="p61113332571"></a>ra:0xd</p>
<p id="p171173395715"><a name="p171173395715"></a><a name="p171173395715"></a>sp:0x0</p>
</td>
</tr>
</tbody>
</table>

根据上述异常打印信息可知：

异常位置：los\_cpup.c第378行。

断言条件：LOS\_ASSERT\(cycles \>= g\_startCycles\)。

直接原因：系统检测到cycles值小于g\_startCycles，违反预期逻辑。

**例2：LOS\_PANIC示例**

<a name="table234mcpsimp"></a>
<table><tbody><tr id="row238mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p240mcpsimp"><a name="p240mcpsimp"></a><a name="p240mcpsimp"></a>The node:0x112270 is not used!</p>
<p id="p241mcpsimp"><a name="p241mcpsimp"></a><a name="p241mcpsimp"></a>task:app_Task</p>
<p id="p242mcpsimp"><a name="p242mcpsimp"></a><a name="p242mcpsimp"></a>thrdPid:0x3</p>
<p id="p243mcpsimp"><a name="p243mcpsimp"></a><a name="p243mcpsimp"></a>type:0xb</p>
<p id="p244mcpsimp"><a name="p244mcpsimp"></a><a name="p244mcpsimp"></a>nestCnt:1</p>
<p id="p245mcpsimp"><a name="p245mcpsimp"></a><a name="p245mcpsimp"></a>phase:1</p>
<p id="p246mcpsimp"><a name="p246mcpsimp"></a><a name="p246mcpsimp"></a>ccause:0x0</p>
<p id="p247mcpsimp"><a name="p247mcpsimp"></a><a name="p247mcpsimp"></a>mcause:0xb</p>
<p id="p248mcpsimp"><a name="p248mcpsimp"></a><a name="p248mcpsimp"></a>mtval:0x0</p>
<p id="p249mcpsimp"><a name="p249mcpsimp"></a><a name="p249mcpsimp"></a>gp:0x10ff00</p>
<p id="p250mcpsimp"><a name="p250mcpsimp"></a><a name="p250mcpsimp"></a>mstatus:0x1800</p>
<p id="p251mcpsimp"><a name="p251mcpsimp"></a><a name="p251mcpsimp"></a><strong id="b9978233124011"><a name="b9978233124011"></a><a name="b9978233124011"></a>mepc:0x1008fe</strong></p>
<p id="p252mcpsimp"><a name="p252mcpsimp"></a><a name="p252mcpsimp"></a>ra:0x1008fe</p>
<p id="p253mcpsimp"><a name="p253mcpsimp"></a><a name="p253mcpsimp"></a>sp:0x111690</p>
</td>
</tr>
</tbody>
</table>

异常位置定位：根据mepc的值在反汇编文件反查代码段或者在源码中搜索打印信息找到Panic源头LOS\_PANIC\("The node:%p is not used!\\n", node\)。

## 访问PMP保护内存<a name="ZH-CN_TOPIC_0000002555814469"></a>




### 问题识别<a name="ZH-CN_TOPIC_0000002524574594"></a>

系统配置PMP后，将根据PMP的配置对总线的取指、数据访问、数据存储进行权限校验，若校验不通过则上报异常（正在执行的取指、数据访问、数据存储操作不被执行），异常信息中会输出PMP access fault，同时异常类型还会明确指出异常操作的类型。

-   Instruction access fault // 异常操作为“取指”
-   Load access fault// 异常操作为“Load”
-   Store/AMO access fault // 异常操作为“Store”

例：

<a name="table594mcpsimp"></a>
<table><tbody><tr id="row598mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p600mcpsimp"><a name="p600mcpsimp"></a><a name="p600mcpsimp"></a>Instruction access fault</p>
<p id="p601mcpsimp"><a name="p601mcpsimp"></a><a name="p601mcpsimp"></a><strong id="b042254110402"><a name="b042254110402"></a><a name="b042254110402"></a>PMP access fault</strong></p>
</td>
</tr>
</tbody>
</table>

### 原因分析<a name="ZH-CN_TOPIC_0000002555654497"></a>

-   操作和权限不匹配，若存在以下几种情况将上报PMP异常，其中×表示上报异常：

    <a name="table1423mcpsimp"></a>
    <table><thead align="left"><tr id="row1430mcpsimp"><th class="cellrowborder" valign="top" width="25%" id="mcps1.1.5.1.1"><p id="p1432mcpsimp"><a name="p1432mcpsimp"></a><a name="p1432mcpsimp"></a>操作/权限</p>
    </th>
    <th class="cellrowborder" valign="top" width="25%" id="mcps1.1.5.1.2"><p id="p1434mcpsimp"><a name="p1434mcpsimp"></a><a name="p1434mcpsimp"></a>R</p>
    </th>
    <th class="cellrowborder" valign="top" width="25%" id="mcps1.1.5.1.3"><p id="p1436mcpsimp"><a name="p1436mcpsimp"></a><a name="p1436mcpsimp"></a>W</p>
    </th>
    <th class="cellrowborder" valign="top" width="25%" id="mcps1.1.5.1.4"><p id="p1438mcpsimp"><a name="p1438mcpsimp"></a><a name="p1438mcpsimp"></a>X</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row1439mcpsimp"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.1 "><p id="p1441mcpsimp"><a name="p1441mcpsimp"></a><a name="p1441mcpsimp"></a>取指</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.2 "><p id="p1443mcpsimp"><a name="p1443mcpsimp"></a><a name="p1443mcpsimp"></a>×</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.3 "><p id="p1445mcpsimp"><a name="p1445mcpsimp"></a><a name="p1445mcpsimp"></a>×</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.4 ">&nbsp;&nbsp;</td>
    </tr>
    <tr id="row1447mcpsimp"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.1 "><p id="p1449mcpsimp"><a name="p1449mcpsimp"></a><a name="p1449mcpsimp"></a>Load</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.2 ">&nbsp;&nbsp;</td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.3 "><p id="p1452mcpsimp"><a name="p1452mcpsimp"></a><a name="p1452mcpsimp"></a>×</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.4 "><p id="p1454mcpsimp"><a name="p1454mcpsimp"></a><a name="p1454mcpsimp"></a>×</p>
    </td>
    </tr>
    <tr id="row1455mcpsimp"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.1 "><p id="p1457mcpsimp"><a name="p1457mcpsimp"></a><a name="p1457mcpsimp"></a>Store</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.2 "><p id="p1459mcpsimp"><a name="p1459mcpsimp"></a><a name="p1459mcpsimp"></a>×</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.3 ">&nbsp;&nbsp;</td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.4 "><p id="p1462mcpsimp"><a name="p1462mcpsimp"></a><a name="p1462mcpsimp"></a>×</p>
    </td>
    </tr>
    </tbody>
    </table>

-   如下场景下也会触发PMP异常。

    跨PMP区间的操作，如：读取4bytes数据，前2bytes在A region，后2bytes在B region。

    PMP entry≥1，访存未匹配任何PMP entry。

### 问题定位<a name="ZH-CN_TOPIC_0000002524734546"></a>

**定位方法<a name="section55480256474"></a>**

-   **通过mepc定位异常指令地址**

    根据mepc寄存器值，确认异常发生时CPU试图执行的指令地址。

-   **通过mtval确认异常访问地址**

    根据mtval确认异常发生时访问的内存地址（用于Load/Store异常）或指令地址（用于取指异常）。

-   **若异常地址/指令符合内存布局预期检查PMP配置**

    若异常指令地址或访问地址符合系统内存布局（如位于合法代码段、数据段、堆栈等），则优先检查PMP（物理内存保护）配置是否正确。

-   **若异常指令（取指）不符合预期参考**“[取指异常](取指异常.md)”章节。
-   **若异常指令为访问非PMP配置内存区间参考踩内存章节定位。**

**参考案例<a name="section17222182674819"></a>**

<a name="table691mcpsimp"></a>
<table><tbody><tr id="row695mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p697mcpsimp"><a name="p697mcpsimp"></a><a name="p697mcpsimp"></a>Instruction access fault</p>
<p id="p698mcpsimp"><a name="p698mcpsimp"></a><a name="p698mcpsimp"></a><strong id="b299825324017"><a name="b299825324017"></a><a name="b299825324017"></a>PMP access fault</strong></p>
<p id="p699mcpsimp"><a name="p699mcpsimp"></a><a name="p699mcpsimp"></a>APP exception:1</p>
<p id="p700mcpsimp"><a name="p700mcpsimp"></a><a name="p700mcpsimp"></a>task: at</p>
<p id="p701mcpsimp"><a name="p701mcpsimp"></a><a name="p701mcpsimp"></a>thrdPid:0xb</p>
<p id="p702mcpsimp"><a name="p702mcpsimp"></a><a name="p702mcpsimp"></a>type:0x1</p>
<p id="p703mcpsimp"><a name="p703mcpsimp"></a><a name="p703mcpsimp"></a>nestCnt:1</p>
<p id="p704mcpsimp"><a name="p704mcpsimp"></a><a name="p704mcpsimp"></a>phase:Task</p>
<p id="p705mcpsimp"><a name="p705mcpsimp"></a><a name="p705mcpsimp"></a>ccause:0x7</p>
<p id="p706mcpsimp"><a name="p706mcpsimp"></a><a name="p706mcpsimp"></a>mcause:0x38000001</p>
<p id="p707mcpsimp"><a name="p707mcpsimp"></a><a name="p707mcpsimp"></a><strong id="b18352026419"><a name="b18352026419"></a><a name="b18352026419"></a>mtval:0x22082444</strong></p>
<p id="p708mcpsimp"><a name="p708mcpsimp"></a><a name="p708mcpsimp"></a>gp:0x10017bed</p>
<p id="p709mcpsimp"><a name="p709mcpsimp"></a><a name="p709mcpsimp"></a>mstatus:0x80207880</p>
<p id="p710mcpsimp"><a name="p710mcpsimp"></a><a name="p710mcpsimp"></a><strong id="b1679825124119"><a name="b1679825124119"></a><a name="b1679825124119"></a>mepc:0x22082444</strong></p>
<p id="p711mcpsimp"><a name="p711mcpsimp"></a><a name="p711mcpsimp"></a>ra:0x2b09e</p>
<p id="p712mcpsimp"><a name="p712mcpsimp"></a><a name="p712mcpsimp"></a>sp:0x1003f5e0</p>
</td>
</tr>
</tbody>
</table>

-   异常地址定位：根据mepc可知异常访问的地址为0x22082444，通过mtval确认被保护的地址也为0x22082444，说明该地址并未配置可执行权限，但系统却希望从该地址取指。
-   排查PMP权限配置：排查代码中对该内存的PMP配置，根据业务需求确认PMP权限是否配置合理。

    <a name="table716mcpsimp"></a>
    <table><tbody><tr id="row720mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p722mcpsimp"><a name="p722mcpsimp"></a><a name="p722mcpsimp"></a>pmpRegion.accPermission.readAcc = E_MEM_RD_ACC_RD;</p>
    <p id="p723mcpsimp"><a name="p723mcpsimp"></a><a name="p723mcpsimp"></a>pmpRegion.accPermission.writeAcc = E_MEM_WR_ACC_NON_WR;</p>
    <p id="p724mcpsimp"><a name="p724mcpsimp"></a><a name="p724mcpsimp"></a>pmpRegion.accPermission.executeAcc = E_MEM_EX_ACC_NON_EX;</p>
    <p id="p725mcpsimp"><a name="p725mcpsimp"></a><a name="p725mcpsimp"></a>pmpRegion.memAttr = MEM_ATTR_DEV_NON_BUF;</p>
    <p id="p726mcpsimp"><a name="p726mcpsimp"></a><a name="p726mcpsimp"></a>pmpRegion.blocked = FALSE;</p>
    <p id="p727mcpsimp"><a name="p727mcpsimp"></a><a name="p727mcpsimp"></a>pmpRegion.ucaAddressMatch = PMP_RGN_ADDR_MATCH_NAPOT;</p>
    <p id="p728mcpsimp"><a name="p728mcpsimp"></a><a name="p728mcpsimp"></a>/* Configure nonsecure pagetable */</p>
    <p id="p729mcpsimp"><a name="p729mcpsimp"></a><a name="p729mcpsimp"></a>pmpRegion.ucNumber = PMP_REGION_NS_PAGE_TABLE_START;</p>
    <p id="p730mcpsimp"><a name="p730mcpsimp"></a><a name="p730mcpsimp"></a>pmpRegion.uwBaseAddress = pt-&gt;nonsec.baseAddr;</p>
    <p id="p731mcpsimp"><a name="p731mcpsimp"></a><a name="p731mcpsimp"></a>pmpRegion.uwRegionSize = pt-&gt;nonsec.tableSize;</p>
    <p id="p732mcpsimp"><a name="p732mcpsimp"></a><a name="p732mcpsimp"></a>pmpRegion.sectl = SEC_CONTROl_NOSECURE_NONMMU;</p>
    <p id="p733mcpsimp"><a name="p733mcpsimp"></a><a name="p733mcpsimp"></a>ret = ArchProtectionRegionSet(&amp;pmpRegion);</p>
    <p id="p734mcpsimp"><a name="p734mcpsimp"></a><a name="p734mcpsimp"></a>if (ret != LOS_OK) {</p>
    <p id="p735mcpsimp"><a name="p735mcpsimp"></a><a name="p735mcpsimp"></a>PRINT_ERR("ArchProtectionRegionSet failed!!", _FUNCTION_, __LINE__, ret);</p>
    <p id="p736mcpsimp"><a name="p736mcpsimp"></a><a name="p736mcpsimp"></a>return LOS_NOK;</p>
    <p id="p737mcpsimp"><a name="p737mcpsimp"></a><a name="p737mcpsimp"></a>}</p>
    </td>
    </tr>
    </tbody>
    </table>

## 访问保留内存<a name="ZH-CN_TOPIC_0000002555814471"></a>




### 问题识别<a name="ZH-CN_TOPIC_0000002524574596"></a>

预留的地址空间在被访问时会触发异常，异常信息中的含有Store/AMO access fault且没有PMP access fault。

例：

<a name="table260mcpsimp"></a>
<table><tbody><tr id="row264mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p266mcpsimp"><a name="p266mcpsimp"></a><a name="p266mcpsimp"></a><strong id="b14631327134112"><a name="b14631327134112"></a><a name="b14631327134112"></a>Store/AMO access fault      // 异常信息</strong></p>
<p id="p267mcpsimp"><a name="p267mcpsimp"></a><a name="p267mcpsimp"></a>AXIM error response</p>
</td>
</tr>
</tbody>
</table>

### 原因分析<a name="ZH-CN_TOPIC_0000002555654499"></a>

系统访问了预留地址空间。

### 问题定位<a name="ZH-CN_TOPIC_0000002524734548"></a>

**定位方法<a name="section10405194804811"></a>**

-   **通过mtval定位异常访问地址并验证合法性**。

    通过mtval可锁定异常发生时CPU尝试访问的内存地址。结合系统提供的地址空间分配表确认该地址是否属于保留地址。若非保留地址，排查该地址是否满足内存对齐要求，针对 LINX 核架构。

    lw / sw（32位加载/存储）：地址必须为4字节对齐（即地址末尾为0x00、0x04、0x08等）。

    lh / sh（16 位加载/存储）：地址必须为2字节对齐（即地址末尾为0x00、0x02等）。

-   **通过mepc定位异常指令地址并反查反汇编，定位异常源码**。

**参考案例<a name="section124911642494"></a>**

<a name="table284mcpsimp"></a>
<table><tbody><tr id="row288mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p290mcpsimp"><a name="p290mcpsimp"></a><a name="p290mcpsimp"></a><strong id="b12704113564117"><a name="b12704113564117"></a><a name="b12704113564117"></a>Store/AMO access fault      // 异常信息</strong></p>
<p id="p291mcpsimp"><a name="p291mcpsimp"></a><a name="p291mcpsimp"></a><strong id="b5705193534116"><a name="b5705193534116"></a><a name="b5705193534116"></a>AXIM error response        // 异常信息</strong></p>
<p id="p292mcpsimp"><a name="p292mcpsimp"></a><a name="p292mcpsimp"></a>task:app_Task</p>
<p id="p293mcpsimp"><a name="p293mcpsimp"></a><a name="p293mcpsimp"></a>thrdPid:0x3</p>
<p id="p294mcpsimp"><a name="p294mcpsimp"></a><a name="p294mcpsimp"></a>type:0x7</p>
<p id="p295mcpsimp"><a name="p295mcpsimp"></a><a name="p295mcpsimp"></a>nestCnt:1</p>
<p id="p296mcpsimp"><a name="p296mcpsimp"></a><a name="p296mcpsimp"></a>phase:Task</p>
<p id="p297mcpsimp"><a name="p297mcpsimp"></a><a name="p297mcpsimp"></a>ccause:0x2</p>
<p id="p298mcpsimp"><a name="p298mcpsimp"></a><a name="p298mcpsimp"></a>mcause:0x7</p>
<p id="p299mcpsimp"><a name="p299mcpsimp"></a><a name="p299mcpsimp"></a>mtval:0x18000000          // 异常访问的地址</p>
<p id="p300mcpsimp"><a name="p300mcpsimp"></a><a name="p300mcpsimp"></a>gp:0x110004</p>
<p id="p301mcpsimp"><a name="p301mcpsimp"></a><a name="p301mcpsimp"></a>mstatus:0x1880</p>
<p id="p302mcpsimp"><a name="p302mcpsimp"></a><a name="p302mcpsimp"></a>mepc:0x115df8            // 触发异常的指令地址</p>
<p id="p303mcpsimp"><a name="p303mcpsimp"></a><a name="p303mcpsimp"></a>ra:0x11687c</p>
<p id="p304mcpsimp"><a name="p304mcpsimp"></a><a name="p304mcpsimp"></a>sp:0x103910</p>
</td>
</tr>
</tbody>
</table>

-   根据mtval可以锁定异常访问的内存地址为0x1800000，确认为保留内存地址。
-   根据mepc反查反汇编文件可知问题代码所在行。

    <a name="table308mcpsimp"></a>
    <table><tbody><tr id="row312mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p314mcpsimp"><a name="p314mcpsimp"></a><a name="p314mcpsimp"></a>00115dd8 &lt;app_init&gt;:</p>
    <p id="p315mcpsimp"><a name="p315mcpsimp"></a><a name="p315mcpsimp"></a>115dc: 717d addi sp,sp,-16</p>
    <p id="p316mcpsimp"><a name="p316mcpsimp"></a><a name="p316mcpsimp"></a>115de0: c606 sw ra,12(sp)</p>
    <p id="p317mcpsimp"><a name="p317mcpsimp"></a><a name="p317mcpsimp"></a>115de4: 00108b2a051f l.li a0,108b2a &lt;g_xRegsMap+0x56c&gt;</p>
    <p id="p318mcpsimp"><a name="p318mcpsimp"></a><a name="p318mcpsimp"></a>115de8: 7f0000ef jal ra,101196</p>
    <p id="p319mcpsimp"><a name="p319mcpsimp"></a><a name="p319mcpsimp"></a>115dec: 12345678051f l.li a0,12345678 &lt;__heap_end+0x12225678&gt;</p>
    <p id="p320mcpsimp"><a name="p320mcpsimp"></a><a name="p320mcpsimp"></a>115df0: 018005b7 lui a1,0x1800</p>
    <p id="p321mcpsimp"><a name="p321mcpsimp"></a><a name="p321mcpsimp"></a><strong id="b107671951174114"><a name="b107671951174114"></a><a name="b107671951174114"></a>115df4: c188 sw a0,0(a1)</strong></p>
    <p id="p322mcpsimp"><a name="p322mcpsimp"></a><a name="p322mcpsimp"></a>115df8: 40b2 lw ra,12(sp)</p>
    <p id="p323mcpsimp"><a name="p323mcpsimp"></a><a name="p323mcpsimp"></a>115dfc: 6141 addi sp,sp,16</p>
    <p id="p324mcpsimp"><a name="p324mcpsimp"></a><a name="p324mcpsimp"></a>115e00: bdd9 j 100890 &lt;tc_mem_013_7&gt;</p>
    </td>
    </tr>
    </tbody>
    </table>

<a name="table325mcpsimp"></a>
<table><tbody><tr id="row329mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p331mcpsimp"><a name="p331mcpsimp"></a><a name="p331mcpsimp"></a>__attribute__((weak)) void app_init(void)</p>
<p id="p332mcpsimp"><a name="p332mcpsimp"></a><a name="p332mcpsimp"></a>{</p>
<p id="p333mcpsimp"><a name="p333mcpsimp"></a><a name="p333mcpsimp"></a>UINT32 t = 0x1800000;</p>
<p id="p334mcpsimp"><a name="p334mcpsimp"></a><a name="p334mcpsimp"></a>*(UINT32 *)t = 0x12345678;</p>
<p id="p335mcpsimp"><a name="p335mcpsimp"></a><a name="p335mcpsimp"></a>}</p>
</td>
</tr>
</tbody>
</table>

## 死锁<a name="ZH-CN_TOPIC_0000002555814473"></a>




### 问题识别<a name="ZH-CN_TOPIC_0000002524574598"></a>

FBB-RTOS提供了死锁检测DFX特性，支持自旋锁、互斥锁、二值信号量和线程锁四种类型锁的死锁，以及重复上锁、错误释放和锁的深度溢出的检测。相关内核配置如下：

<a name="table103mcpsimp"></a>
<table><thead align="left"><tr id="row110mcpsimp"><th class="cellrowborder" valign="top" width="35%" id="mcps1.1.5.1.1"><p id="p112mcpsimp"><a name="p112mcpsimp"></a><a name="p112mcpsimp"></a>配置项</p>
</th>
<th class="cellrowborder" valign="top" width="24%" id="mcps1.1.5.1.2"><p id="p114mcpsimp"><a name="p114mcpsimp"></a><a name="p114mcpsimp"></a>含义</p>
</th>
<th class="cellrowborder" valign="top" width="9%" id="mcps1.1.5.1.3"><p id="p116mcpsimp"><a name="p116mcpsimp"></a><a name="p116mcpsimp"></a>默认值</p>
</th>
<th class="cellrowborder" valign="top" width="32%" id="mcps1.1.5.1.4"><p id="p118mcpsimp"><a name="p118mcpsimp"></a><a name="p118mcpsimp"></a>依赖</p>
</th>
</tr>
</thead>
<tbody><tr id="row119mcpsimp"><td class="cellrowborder" valign="top" width="35%" headers="mcps1.1.5.1.1 "><p id="p121mcpsimp"><a name="p121mcpsimp"></a><a name="p121mcpsimp"></a>LOSCFG_KERNEL_LOCKDEP</p>
</td>
<td class="cellrowborder" valign="top" width="24%" headers="mcps1.1.5.1.2 "><p id="p123mcpsimp"><a name="p123mcpsimp"></a><a name="p123mcpsimp"></a>使能死锁检测</p>
</td>
<td class="cellrowborder" valign="top" width="9%" headers="mcps1.1.5.1.3 "><p id="p125mcpsimp"><a name="p125mcpsimp"></a><a name="p125mcpsimp"></a>N</p>
</td>
<td class="cellrowborder" valign="top" width="32%" headers="mcps1.1.5.1.4 "><p id="p127mcpsimp"><a name="p127mcpsimp"></a><a name="p127mcpsimp"></a>LOSCFG_KERNEL_MEM_ALLOC</p>
</td>
</tr>
<tr id="row128mcpsimp"><td class="cellrowborder" valign="top" width="35%" headers="mcps1.1.5.1.1 "><p id="p130mcpsimp"><a name="p130mcpsimp"></a><a name="p130mcpsimp"></a>LOSCFG_KERNEL_SPINDEP</p>
</td>
<td class="cellrowborder" valign="top" width="24%" headers="mcps1.1.5.1.2 "><p id="p132mcpsimp"><a name="p132mcpsimp"></a><a name="p132mcpsimp"></a>使能自旋锁死锁检测</p>
</td>
<td class="cellrowborder" valign="top" width="9%" headers="mcps1.1.5.1.3 "><p id="p134mcpsimp"><a name="p134mcpsimp"></a><a name="p134mcpsimp"></a>N</p>
</td>
<td class="cellrowborder" valign="top" width="32%" headers="mcps1.1.5.1.4 "><p id="p136mcpsimp"><a name="p136mcpsimp"></a><a name="p136mcpsimp"></a>LOSCFG_KERNEL_SMP</p>
</td>
</tr>
<tr id="row137mcpsimp"><td class="cellrowborder" valign="top" width="35%" headers="mcps1.1.5.1.1 "><p id="p139mcpsimp"><a name="p139mcpsimp"></a><a name="p139mcpsimp"></a>LOSCFG_KERNEL_MUXDEP</p>
</td>
<td class="cellrowborder" valign="top" width="24%" headers="mcps1.1.5.1.2 "><p id="p141mcpsimp"><a name="p141mcpsimp"></a><a name="p141mcpsimp"></a>使能互斥锁死锁检测</p>
</td>
<td class="cellrowborder" valign="top" width="9%" headers="mcps1.1.5.1.3 "><p id="p143mcpsimp"><a name="p143mcpsimp"></a><a name="p143mcpsimp"></a>N</p>
</td>
<td class="cellrowborder" valign="top" width="32%" headers="mcps1.1.5.1.4 "><p id="p145mcpsimp"><a name="p145mcpsimp"></a><a name="p145mcpsimp"></a>LOSCFG_BASE_IPC_MUX</p>
</td>
</tr>
<tr id="row146mcpsimp"><td class="cellrowborder" valign="top" width="35%" headers="mcps1.1.5.1.1 "><p id="p148mcpsimp"><a name="p148mcpsimp"></a><a name="p148mcpsimp"></a>LOSCFG_KERNEL_SEMDEP</p>
</td>
<td class="cellrowborder" valign="top" width="24%" headers="mcps1.1.5.1.2 "><p id="p150mcpsimp"><a name="p150mcpsimp"></a><a name="p150mcpsimp"></a>使能二值信号量死锁检测</p>
</td>
<td class="cellrowborder" valign="top" width="9%" headers="mcps1.1.5.1.3 "><p id="p152mcpsimp"><a name="p152mcpsimp"></a><a name="p152mcpsimp"></a>N</p>
</td>
<td class="cellrowborder" valign="top" width="32%" headers="mcps1.1.5.1.4 "><p id="p154mcpsimp"><a name="p154mcpsimp"></a><a name="p154mcpsimp"></a>LOSCFG_BASE_IPC_SEM</p>
</td>
</tr>
<tr id="row155mcpsimp"><td class="cellrowborder" valign="top" width="35%" headers="mcps1.1.5.1.1 "><p id="p157mcpsimp"><a name="p157mcpsimp"></a><a name="p157mcpsimp"></a>LOSCFG_PTHREAD_MUXDEP</p>
</td>
<td class="cellrowborder" valign="top" width="24%" headers="mcps1.1.5.1.2 "><p id="p159mcpsimp"><a name="p159mcpsimp"></a><a name="p159mcpsimp"></a>使能线程锁死锁检测</p>
</td>
<td class="cellrowborder" valign="top" width="9%" headers="mcps1.1.5.1.3 "><p id="p161mcpsimp"><a name="p161mcpsimp"></a><a name="p161mcpsimp"></a>N</p>
</td>
<td class="cellrowborder" valign="top" width="32%" headers="mcps1.1.5.1.4 "><p id="p163mcpsimp"><a name="p163mcpsimp"></a><a name="p163mcpsimp"></a>LOSCFG_COMPAT_POSIX</p>
</td>
</tr>
</tbody>
</table>

若相关内核配置被使能，检测到死锁时会挂死，异常信息中会输出dead lock。例：

<a name="table165mcpsimp"></a>
<table><tbody><tr id="row169mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p171mcpsimp"><a name="p171mcpsimp"></a><a name="p171mcpsimp"></a>[2025-11-17 11:20:49] lockdep check failed</p>
<p id="p172mcpsimp"><a name="p172mcpsimp"></a><a name="p172mcpsimp"></a>[2025-11-17 11:20:49] error type   : <strong id="b532141334212"><a name="b532141334212"></a><a name="b532141334212"></a>dead lock</strong></p>
<p id="p173mcpsimp"><a name="p173mcpsimp"></a><a name="p173mcpsimp"></a>[2025-11-17 11:20:49] request addr : 0x47262</p>
<p id="p174mcpsimp"><a name="p174mcpsimp"></a><a name="p174mcpsimp"></a>[2025-11-17 11:20:49] task name    : Task_B</p>
</td>
</tr>
</tbody>
</table>

### 原因分析<a name="ZH-CN_TOPIC_0000002555654501"></a>

两个任务分别等待对方所占的资源。

### 问题定位<a name="ZH-CN_TOPIC_0000002524734550"></a>

**定位方法<a name="section4521631134916"></a>**

**反查请求地址定位异常锁函数：**将触发异常的request addr值复制至系统镜像的反汇编文件中，通过地址定位至具体代码行，即可识别是哪个锁函数在执行过程中发生异常。

注：request addr反查结果通常指向具体的锁函数入口，若任务中存在多个同类锁，可通过锁ID 进行进一步区分，以精确定位具体锁对象。

**参考案例<a name="section983475124913"></a>**

<a name="table911mcpsimp"></a>
<table><tbody><tr id="row915mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p917mcpsimp"><a name="p917mcpsimp"></a><a name="p917mcpsimp"></a>[2025-11-17 11:20:49] app_init start ok</p>
<p id="p918mcpsimp"><a name="p918mcpsimp"></a><a name="p918mcpsimp"></a>[2025-11-17 11:20:49] Task_A pend muxid 31</p>
<p id="p919mcpsimp"><a name="p919mcpsimp"></a><a name="p919mcpsimp"></a>[2025-11-17 11:20:49] Task_B pend muxid 32</p>
<p id="p920mcpsimp"><a name="p920mcpsimp"></a><a name="p920mcpsimp"></a>[2025-11-17 11:20:49] APP|thread_main start</p>
<p id="p921mcpsimp"><a name="p921mcpsimp"></a><a name="p921mcpsimp"></a>[2025-11-17 11:20:49] lockdep check failed</p>
<p id="p922mcpsimp"><a name="p922mcpsimp"></a><a name="p922mcpsimp"></a>[2025-11-17 11:20:49] <strong id="b12781025144220"><a name="b12781025144220"></a><a name="b12781025144220"></a>error type   : dead lock</strong></p>
<p id="p923mcpsimp"><a name="p923mcpsimp"></a><a name="p923mcpsimp"></a>[2025-11-17 11:20:49] <strong id="b17117129154215"><a name="b17117129154215"></a><a name="b17117129154215"></a>request addr : 0x47262</strong></p>
<p id="p924mcpsimp"><a name="p924mcpsimp"></a><a name="p924mcpsimp"></a>[2025-11-17 11:20:49] <strong id="b1158293224216"><a name="b1158293224216"></a><a name="b1158293224216"></a>task name    : Task_B</strong></p>
<p id="p925mcpsimp"><a name="p925mcpsimp"></a><a name="p925mcpsimp"></a>[2025-11-17 11:20:49] task id      : 8</p>
<p id="p926mcpsimp"><a name="p926mcpsimp"></a><a name="p926mcpsimp"></a>[2025-11-17 11:20:49] cpu num      : 0</p>
<p id="p927mcpsimp"><a name="p927mcpsimp"></a><a name="p927mcpsimp"></a>[2025-11-17 11:20:49] start dumping lockdep information</p>
<p id="p928mcpsimp"><a name="p928mcpsimp"></a><a name="p928mcpsimp"></a>[2025-11-17 11:20:49] [0] <strong id="b1617114013428"><a name="b1617114013428"></a><a name="b1617114013428"></a>Mutex ID: 0032</strong></p>
<p id="p929mcpsimp"><a name="p929mcpsimp"></a><a name="p929mcpsimp"></a>[2025-11-17 11:20:49] [1] <strong id="b1482154415420"><a name="b1482154415420"></a><a name="b1482154415420"></a>Mutex ID: 0031 &lt;-- now</strong></p>
<p id="p930mcpsimp"><a name="p930mcpsimp"></a><a name="p930mcpsimp"></a>[2025-11-17 11:20:49] task name    : Task_A</p>
<p id="p931mcpsimp"><a name="p931mcpsimp"></a><a name="p931mcpsimp"></a>[2025-11-17 11:20:49] task id      : 7</p>
<p id="p932mcpsimp"><a name="p932mcpsimp"></a><a name="p932mcpsimp"></a>[2025-11-17 11:20:49] cpu num      : 0</p>
<p id="p933mcpsimp"><a name="p933mcpsimp"></a><a name="p933mcpsimp"></a>[2025-11-17 11:20:49] start dumping lockdep information</p>
<p id="p934mcpsimp"><a name="p934mcpsimp"></a><a name="p934mcpsimp"></a>[2025-11-17 11:20:49] [0] <strong id="b11352195213428"><a name="b11352195213428"></a><a name="b11352195213428"></a>Mutex ID: 0031</strong></p>
<p id="p935mcpsimp"><a name="p935mcpsimp"></a><a name="p935mcpsimp"></a>[2025-11-17 11:20:49] [1] <strong id="b16295155624216"><a name="b16295155624216"></a><a name="b16295155624216"></a>Mutex ID: 0032 &lt;-- now</strong></p>
<p id="p936mcpsimp"><a name="p936mcpsimp"></a><a name="p936mcpsimp"></a>[2025-11-17 11:20:49] runTask-&gt;taskName = Task_B</p>
<p id="p937mcpsimp"><a name="p937mcpsimp"></a><a name="p937mcpsimp"></a>[2025-11-17 11:20:49] runTask-&gt;taskId = 8</p>
<p id="p938mcpsimp"><a name="p938mcpsimp"></a><a name="p938mcpsimp"></a>[2025-11-17 11:20:49] fp:0x10021010</p>
</td>
</tr>
</tbody>
</table>

-   根据异常信息可知任务Task\_A与Task\_B发生死锁，Task\_B持有0032的锁申请0031的锁，Task\_A持有0031的锁申请0032的锁；
-   根据request addr的值（本例中为0x47262），在系统镜像的反汇编文件中找到该地址，可确定具体的锁函数LOS\_MuxPend，进一步定位到Task A调用互斥锁位置。

    <a name="table942mcpsimp"></a>
    <table><tbody><tr id="row946mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p948mcpsimp"><a name="p948mcpsimp"></a><a name="p948mcpsimp"></a>000253da &lt;Task_A&gt;:</p>
    <p id="p949mcpsimp"><a name="p949mcpsimp"></a><a name="p949mcpsimp"></a>253da: 40 99        	c.push	{ra,s0-s1}, -16</p>
    <p id="p950mcpsimp"><a name="p950mcpsimp"></a><a name="p950mcpsimp"></a>253dc: 00 08        	addi	s0, sp, 16</p>
    <p id="p951mcpsimp"><a name="p951mcpsimp"></a><a name="p951mcpsimp"></a>253de: 37 d5 01 20  	lui	a0, 131101</p>
    <p id="p952mcpsimp"><a name="p952mcpsimp"></a><a name="p952mcpsimp"></a>253e2: 93 04 c5 ed  	addi	s1, a0, -292</p>
    <p id="p953mcpsimp"><a name="p953mcpsimp"></a><a name="p953mcpsimp"></a>253e6: c8 44        	lw	a0, 12(s1)</p>
    <p id="p954mcpsimp"><a name="p954mcpsimp"></a><a name="p954mcpsimp"></a>253e8: fd 55        	addi	a1, zero, -1</p>
    <p id="p955mcpsimp"><a name="p955mcpsimp"></a><a name="p955mcpsimp"></a><strong id="b10190314194317"><a name="b10190314194317"></a><a name="b10190314194317"></a>253ea: ef 10 12 60  	jal	ra, 0x471ea &lt;LOS_MuxPend&gt;</strong></p>
    <p id="p956mcpsimp"><a name="p956mcpsimp"></a><a name="p956mcpsimp"></a>253ee: 11 e9        	bne	a0, zero, 0x25402 &lt;Task_A+0x28&gt;</p>
    <p id="p957mcpsimp"><a name="p957mcpsimp"></a><a name="p957mcpsimp"></a>253f0: d0 44        	lw	a2, 12(s1)</p>
    <p id="p958mcpsimp"><a name="p958mcpsimp"></a><a name="p958mcpsimp"></a>253f2: 55 65        	lui	a0, 21</p>
    <p id="p959mcpsimp"><a name="p959mcpsimp"></a><a name="p959mcpsimp"></a>253f4: 13 05 75 e8  	addi	a0, a0, -377</p>
    <p id="p960mcpsimp"><a name="p960mcpsimp"></a><a name="p960mcpsimp"></a>253f8: d1 65        	lui	a1, 20</p>
    <p id="p961mcpsimp"><a name="p961mcpsimp"></a><a name="p961mcpsimp"></a>253fa: 93 85 45 fe  	addi	a1, a1, -28</p>
    <p id="p962mcpsimp"><a name="p962mcpsimp"></a><a name="p962mcpsimp"></a>253fe: ef 20 c2 1c  	jal	ra, 0x475ca &lt;dprintf&gt;</p>
    <p id="p963mcpsimp"><a name="p963mcpsimp"></a><a name="p963mcpsimp"></a>25402: 09 45        	addi	a0, zero, 2</p>
    <p id="p964mcpsimp"><a name="p964mcpsimp"></a><a name="p964mcpsimp"></a>25404: ef 10 52 2a  	jal	ra, 0x46ea8 &lt;LOS_Msleep&gt;</p>
    <p id="p965mcpsimp"><a name="p965mcpsimp"></a><a name="p965mcpsimp"></a>25408: 88 48        	lw	a0, 16(s1)</p>
    <p id="p966mcpsimp"><a name="p966mcpsimp"></a><a name="p966mcpsimp"></a>2540a: fd 55        	addi	a1, zero, -1</p>
    <p id="p967mcpsimp"><a name="p967mcpsimp"></a><a name="p967mcpsimp"></a><strong id="b15278104215430"><a name="b15278104215430"></a><a name="b15278104215430"></a>2540c: ef 10 f2 5d  	jal	ra, 0x471ea &lt;LOS_MuxPend&gt;</strong></p>
    <p id="p968mcpsimp"><a name="p968mcpsimp"></a><a name="p968mcpsimp"></a>……..</p>
    <p id="p969mcpsimp"><a name="p969mcpsimp"></a><a name="p969mcpsimp"></a>000471ea &lt;LOS_MuxPend&gt;:</p>
    <p id="p970mcpsimp"><a name="p970mcpsimp"></a><a name="p970mcpsimp"></a>471ea: c0 9b        	c.push	{ra,s0-s6}, -32</p>
    <p id="p971mcpsimp"><a name="p971mcpsimp"></a><a name="p971mcpsimp"></a>471ec: 00 10        	addi	s0, sp, 32</p>
    <p id="p972mcpsimp"><a name="p972mcpsimp"></a><a name="p972mcpsimp"></a>471ee: 2a 8b        	add	s6, a0, zero</p>
    <p id="p973mcpsimp"><a name="p973mcpsimp"></a><a name="p973mcpsimp"></a>471f0: 28 91        	c.zext.h	a0, a0</p>
    <p id="p974mcpsimp"><a name="p974mcpsimp"></a><a name="p974mcpsimp"></a>……..</p>
    <p id="p975mcpsimp"><a name="p975mcpsimp"></a><a name="p975mcpsimp"></a>4724e: 63 0d 55 03  	beq	a0, s5, 0x47288 &lt;LOS_MuxPend+0x9e&gt;</p>
    <p id="p976mcpsimp"><a name="p976mcpsimp"></a><a name="p976mcpsimp"></a>47252: 5a 85        	add	a0, s6, zero</p>
    <p id="p977mcpsimp"><a name="p977mcpsimp"></a><a name="p977mcpsimp"></a>47254: ef e0 9f c5  	jal	ra, 0x45eac &lt;OsMuxLockDepGet&gt;</p>
    <p id="p978mcpsimp"><a name="p978mcpsimp"></a><a name="p978mcpsimp"></a>47258: aa 85        	add	a1, a0, zero</p>
    <p id="p979mcpsimp"><a name="p979mcpsimp"></a><a name="p979mcpsimp"></a>4725a: 01 45        	addi	a0, zero, 0</p>
    <p id="p980mcpsimp"><a name="p980mcpsimp"></a><a name="p980mcpsimp"></a>4725c: 5a 86        	add	a2, s6, zero</p>
    <p id="p981mcpsimp"><a name="p981mcpsimp"></a><a name="p981mcpsimp"></a>4725e: ef e0 9f dd  	jal	ra, 0x46036 &lt;OsLockDepCheckIn&gt;</p>
    <p id="p982mcpsimp"><a name="p982mcpsimp"></a><a name="p982mcpsimp"></a><strong id="b1465410551431"><a name="b1465410551431"></a><a name="b1465410551431"></a>47262: 52 85        	add	a0, s4, zero</strong></p>
    <p id="p983mcpsimp"><a name="p983mcpsimp"></a><a name="p983mcpsimp"></a>47264: db 55 c5 00  	lhu.u	a1, 12(a0)</p>
    <p id="p984mcpsimp"><a name="p984mcpsimp"></a><a name="p984mcpsimp"></a>47268: 9d c5        	beq	a1, zero, 0x47296 &lt;LOS_MuxPend+0xac&gt;</p>
    <p id="p985mcpsimp"><a name="p985mcpsimp"></a><a name="p985mcpsimp"></a>4726a: 63 85 09 04  	beq	s3, zero, 0x472b4 &lt;LOS_MuxPend+0xca&gt;</p>
    </td>
    </tr>
    </tbody>
    </table>

## 非对齐访问<a name="ZH-CN_TOPIC_0000002555814475"></a>




### 问题识别<a name="ZH-CN_TOPIC_0000002524574600"></a>

内存对齐访问要求：访问数据时，其物理地址必须是该数据类型字节宽度的整数倍。

32位数据（如uint32\_t、float）：地址必须是4的倍数（如0x0000、0x0004、0x0008）；

16位数据（如uint16\_t）：地址必须是2的倍数；

8位数据（如uint8\_t）：地址可为任意值（无需对齐）。

非对齐访问即违反上述规则的访问行为，当程序尝试从非对齐地址加载或存储数据时，硬件会触发异常，死机日志中会出现关键字“Load address misaligned”或者“Store/AMO address misaligned”。例：

<a name="table182mcpsimp"></a>
<table><tbody><tr id="row186mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p188mcpsimp"><a name="p188mcpsimp"></a><a name="p188mcpsimp"></a>[2025-10-30 21:47:41] <strong id="b735514124410"><a name="b735514124410"></a><a name="b735514124410"></a>Load address misaligned</strong></p>
</td>
</tr>
</tbody>
</table>

### 原因分析<a name="ZH-CN_TOPIC_0000002555654503"></a>

-   **核心原因**

    内存访问模型与硬件对齐要求不匹配，芯片不支持非对齐访问，而软件未考虑。常出现在代码移植至新平台后突然崩溃的场景中，尤其在客户自研芯片架构上更为突出。

-   **典型场景**
    -   **结构体填充与对齐控制：**编译器默认会对结构体成员进行内存对齐填充，以提升访问效率。若使用 \#pragma pack\(1\) 等指令强制取消对齐，再直接访问结构体内成员，易引发非对齐访问。
    -   **指针类型转换：**将char\*或void\*指针强制转换为更严格对齐类型的指针（如int\*、float\*），并解引用，极易导致非对齐访问。
    -   **网络协议包/二进制文件解析**：在网络通信或文件解析场景中，常将原始字节缓冲区（char\[\]）直接映射为结构体指针。若协议定义的数据域本身未对齐，或解析时未考虑对齐，将导致非对齐访问。

### 问题定位<a name="ZH-CN_TOPIC_0000002524734552"></a>

**定位方法<a name="section1275482620501"></a>**

根据mepc（PC寄存器值）即可定位非对齐访问引起挂死的代码行。

**参考案例<a name="section184735382504"></a>**

<a name="table1296mcpsimp"></a>
<table><tbody><tr id="row1300mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p1302mcpsimp"><a name="p1302mcpsimp"></a><a name="p1302mcpsimp"></a>DOORBELL_VERSION = 20231205_V0.0001_test1</p>
<p id="p1303mcpsimp"><a name="p1303mcpsimp"></a><a name="p1303mcpsimp"></a>[app_main:903] p = 0x4886440</p>
<p id="p1304mcpsimp"><a name="p1304mcpsimp"></a><a name="p1304mcpsimp"></a>[app_main:907] p1 = 0x4886441</p>
<p id="p1305mcpsimp"><a name="p1305mcpsimp"></a><a name="p1305mcpsimp"></a>==================================================exception_analyze==================================================</p>
<p id="p1306mcpsimp"><a name="p1306mcpsimp"></a><a name="p1306mcpsimp"></a>CPU0 run addr = 0x403de7e</p>
<p id="p1307mcpsimp"><a name="p1307mcpsimp"></a><a name="p1307mcpsimp"></a>!!! cpu 1 exception_analyze: DEBUG_MSG = 0x0, DEBUG_PRP_NUM = 0xffffffff DSPCON=e00</p>
<p id="p1308mcpsimp"><a name="p1308mcpsimp"></a><a name="p1308mcpsimp"></a>R0: 42312a6</p>
<p id="p1309mcpsimp"><a name="p1309mcpsimp"></a><a name="p1309mcpsimp"></a>R1: 4224043</p>
<p id="p1310mcpsimp"><a name="p1310mcpsimp"></a><a name="p1310mcpsimp"></a>R2: 38d</p>
<p id="p1311mcpsimp"><a name="p1311mcpsimp"></a><a name="p1311mcpsimp"></a>R3: 0</p>
<p id="p1312mcpsimp"><a name="p1312mcpsimp"></a><a name="p1312mcpsimp"></a>R4: 4224043</p>
<p id="p1313mcpsimp"><a name="p1313mcpsimp"></a><a name="p1313mcpsimp"></a>R5: 42234e0</p>
<p id="p1314mcpsimp"><a name="p1314mcpsimp"></a><a name="p1314mcpsimp"></a>R6: 4886441</p>
<p id="p1315mcpsimp"><a name="p1315mcpsimp"></a><a name="p1315mcpsimp"></a>R7: 422b1f2</p>
<p id="p1316mcpsimp"><a name="p1316mcpsimp"></a><a name="p1316mcpsimp"></a>R8: 423128d</p>
<p id="p1317mcpsimp"><a name="p1317mcpsimp"></a><a name="p1317mcpsimp"></a>R9: 46d9e90</p>
<p id="p1318mcpsimp"><a name="p1318mcpsimp"></a><a name="p1318mcpsimp"></a>R10: 10101010</p>
<p id="p1319mcpsimp"><a name="p1319mcpsimp"></a><a name="p1319mcpsimp"></a>R11: 11111111</p>
<p id="p1320mcpsimp"><a name="p1320mcpsimp"></a><a name="p1320mcpsimp"></a>R12: 12121212</p>
<p id="p1321mcpsimp"><a name="p1321mcpsimp"></a><a name="p1321mcpsimp"></a>R13: 13131313</p>
<p id="p1322mcpsimp"><a name="p1322mcpsimp"></a><a name="p1322mcpsimp"></a>R14: 14141414</p>
<p id="p1323mcpsimp"><a name="p1323mcpsimp"></a><a name="p1323mcpsimp"></a>R15: 15151515</p>
<p id="p1324mcpsimp"><a name="p1324mcpsimp"></a><a name="p1324mcpsimp"></a>icfg: 7010280</p>
<p id="p1325mcpsimp"><a name="p1325mcpsimp"></a><a name="p1325mcpsimp"></a>psr: 0</p>
<p id="p1326mcpsimp"><a name="p1326mcpsimp"></a><a name="p1326mcpsimp"></a>rets: 0x40937c2</p>
<p id="p1327mcpsimp"><a name="p1327mcpsimp"></a><a name="p1327mcpsimp"></a>rete: 0x0</p>
<p id="p1328mcpsimp"><a name="p1328mcpsimp"></a><a name="p1328mcpsimp"></a>retx: 0x0</p>
<p id="p1329mcpsimp"><a name="p1329mcpsimp"></a><a name="p1329mcpsimp"></a>reti: 0x4000e32</p>
<p id="p1330mcpsimp"><a name="p1330mcpsimp"></a><a name="p1330mcpsimp"></a>usp: 489680c, ssp: 42eee68 sp: 42eee68</p>
</td>
</tr>
</tbody>
</table>

适配J客户时由于非对齐访问产生了上述挂死，该场景属于FBB-RTOS适配其他厂商的芯片，芯片不支持非对齐访问。（客户自己提供的打印信息未指示当前的异常类型）

解决方案：非对齐访问操作转化为按照单个字节去操作。

## 其他异常<a name="ZH-CN_TOPIC_0000002555814477"></a>

当系统异常日志与代码上下文无法直接定位根本原因，或通过上文提供的分析手段仅能锁定到某个变量、内存块或资源异常时，通常需怀疑发生了内存踩踏、内存不足或者内存布局非对齐问题。此类异常没有固定的异常类型，同时异常发生时往往非第一现场，导致问题定位困难。针对此类异常，FBB-RTOS提供了相应的DFX工具，能精准定位高频内存异常问题，包括动态分配内存越界、固定区域内存踩踏、指针重复释放引起的飞踩、内存泄露。




### 踩内存<a name="ZH-CN_TOPIC_0000002524574602"></a>



#### 原因分析<a name="ZH-CN_TOPIC_0000002555654505"></a>

发生踩内存类问题时，第一现场通常不会挂死，直到访问到被踩的内容时，通常才出现异常，出现系统死机、变量异常值、数据传输错误、业务预期结果异常、任务异常挂起或者退出、指令无效、内存合法性校验失败等问题。

踩内存的根本原因在于数组溢出、动态内存溢出、飞踩、重复释放等。

#### 问题定位<a name="ZH-CN_TOPIC_0000002524734554"></a>

**定位方法<a name="section1275482620501"></a>**

当前FBB-RTOS系统提供的DFX能力，主要解决踩内存的以下三类典型问题：

-   固定踩内存（每次踩踏都是同一片内存或者踩踏固定的变量）

    通过trigger监控固定区域的读写操作，捕获踩内存的第一现场，首先确定被踩的内存地址，然后使能LOSCFG\_TRIGGER\_ENABLE内核配置，使用trigger进行监控。目前提供了两种使用方式：

    方式一（推荐）：针对没有适配命令行的产品，可以直接调用trigger接口。

    <a name="table387mcpsimp"></a>
    <table><tbody><tr id="row391mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p393mcpsimp"><a name="p393mcpsimp"></a><a name="p393mcpsimp"></a>UINT32 ArchProtectionSetAddrRo(UINT32 addr, UINT32 size)</p>
    <p id="p394mcpsimp"><a name="p394mcpsimp"></a><a name="p394mcpsimp"></a>函数说明：根据提供的地址及范围配置保护；</p>
    <p id="p395mcpsimp"><a name="p395mcpsimp"></a><a name="p395mcpsimp"></a>参数说明：</p>
    <p id="p396mcpsimp"><a name="p396mcpsimp"></a><a name="p396mcpsimp"></a>addr: 被保护范围的起始地址，当size为4时，addr需要4对齐；</p>
    <p id="p397mcpsimp"><a name="p397mcpsimp"></a><a name="p397mcpsimp"></a>size: 被保护范围的大小；</p>
    <p id="p398mcpsimp"><a name="p398mcpsimp"></a><a name="p398mcpsimp"></a>使用举例：</p>
    <p id="p399mcpsimp"><a name="p399mcpsimp"></a><a name="p399mcpsimp"></a>ArchProtectionSetAddrRo(0x100000, 4)：被保护的地址范围为[0x100000, 0x100003]共4bytes；</p>
    <p id="p400mcpsimp"><a name="p400mcpsimp"></a><a name="p400mcpsimp"></a>************************************************************************************</p>
    <p id="p401mcpsimp"><a name="p401mcpsimp"></a><a name="p401mcpsimp"></a>UINT32 ArchProtectionUnSetAddrRo(UINT32 index)</p>
    <p id="p402mcpsimp"><a name="p402mcpsimp"></a><a name="p402mcpsimp"></a>函数说明：取消指定索引trigger的保护</p>
    <p id="p403mcpsimp"><a name="p403mcpsimp"></a><a name="p403mcpsimp"></a>参数说明：index: trigger索引，取值范围为[0, 3]，index == 0xffffffff标识取消所有trigger保护。</p>
    </td>
    </tr>
    </tbody>
    </table>

    方式二：在命令行使用命令

    <a name="table405mcpsimp"></a>
    <table><tbody><tr id="row409mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p411mcpsimp"><a name="p411mcpsimp"></a><a name="p411mcpsimp"></a>trigger set addr size</p>
    <p id="p412mcpsimp"><a name="p412mcpsimp"></a><a name="p412mcpsimp"></a>命令说明：设置一个trigger，保护addr地址开始的size个字节</p>
    <p id="p413mcpsimp"><a name="p413mcpsimp"></a><a name="p413mcpsimp"></a>Example：trigger set 0x1405ca4 4 表示保护从0x1405ca4地址开始的4个字节</p>
    <p id="p414mcpsimp"><a name="p414mcpsimp"></a><a name="p414mcpsimp"></a>注：设置trigger完成后，打印当前的trigger信息</p>
    <p id="p415mcpsimp"><a name="p415mcpsimp"></a><a name="p415mcpsimp"></a>************************************************************************************</p>
    <p id="p416mcpsimp"><a name="p416mcpsimp"></a><a name="p416mcpsimp"></a>trigger unset</p>
    <p id="p417mcpsimp"><a name="p417mcpsimp"></a><a name="p417mcpsimp"></a>命令说明：移出当前的trigger</p>
    <p id="p418mcpsimp"><a name="p418mcpsimp"></a><a name="p418mcpsimp"></a>Example：trigger unset</p>
    </td>
    </tr>
    </tbody>
    </table>

-   堆越界踩内存头

    利用FBB\_RTOS提供的DFX工具确定被踩内存结点，然后根据被踩内存结点中的taskId等信息进行定位。具体操作方法参考LiteOS开发指南8.3.3章节。

-   内存重复释放

    利用FBB\_RTOS提供的DFX工具确定重复释放指令，然后通过反汇编文件确定问题函数，并使用callerRA等信息逐步定位。

    具体步骤为：

    使能LOSCFG\_MEM\_DFX\_DOUBLE\_FREE\_CHECK内核配置。

    添加编译选项重新编译运行。

    <a name="table428mcpsimp"></a>
    <table><tbody><tr id="row432mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p434mcpsimp"><a name="p434mcpsimp"></a><a name="p434mcpsimp"></a># 添加编译选项</p>
    <p id="p435mcpsimp"><a name="p435mcpsimp"></a><a name="p435mcpsimp"></a># target_standard_sw21_application_template    linkflags添加编译选项</p>
    <p id="p436mcpsimp"><a name="p436mcpsimp"></a><a name="p436mcpsimp"></a>'linkflags': [</p>
    <p id="p437mcpsimp"><a name="p437mcpsimp"></a><a name="p437mcpsimp"></a>'-Wl,--wrap=LOS_MemFree'</p>
    <p id="p438mcpsimp"><a name="p438mcpsimp"></a><a name="p438mcpsimp"></a>],</p>
    </td>
    </tr>
    </tbody>
    </table>

    根据打印信息结合反汇编文件可以确定重复free函数的调用点。

    ![](figures/zh-cn_image_0000002563509625.png)

    详细使用方法和介绍参考《LiteOS 开发指南》的“内存重复释放检测”章节。

    其他飞踩等问题当前并没有特别好的手段，主要通过结合异常点业务代码做进一步分析，详细分析思路参考下图。

    ![](figures/zh-cn_image_0000002563589583.png)

**参考案例<a name="section33891826135114"></a>**

例1（堆越界踩内存头）：

<a name="table447mcpsimp"></a>
<table><tbody><tr id="row451mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p453mcpsimp"><a name="p453mcpsimp"></a><a name="p453mcpsimp"></a><strong id="b1565573020455"><a name="b1565573020455"></a><a name="b1565573020455"></a>The node:0x1137b4 has been damaged!</strong></p>
<p id="p454mcpsimp"><a name="p454mcpsimp"></a><a name="p454mcpsimp"></a><strong id="b665613010459"><a name="b665613010459"></a><a name="b665613010459"></a>node:0x1137b4 has been damaged! pre node:0x11379c，pre node CallerRA：0x100864</strong></p>
<p id="p455mcpsimp"><a name="p455mcpsimp"></a><a name="p455mcpsimp"></a>task:app_Task</p>
<p id="p456mcpsimp"><a name="p456mcpsimp"></a><a name="p456mcpsimp"></a>thrdPid:0x3</p>
<p id="p457mcpsimp"><a name="p457mcpsimp"></a><a name="p457mcpsimp"></a>type:0xb</p>
<p id="p458mcpsimp"><a name="p458mcpsimp"></a><a name="p458mcpsimp"></a>nestCnt:1</p>
<p id="p459mcpsimp"><a name="p459mcpsimp"></a><a name="p459mcpsimp"></a>phase:1</p>
<p id="p460mcpsimp"><a name="p460mcpsimp"></a><a name="p460mcpsimp"></a>ccause:0x0</p>
<p id="p461mcpsimp"><a name="p461mcpsimp"></a><a name="p461mcpsimp"></a>mcause:0xb</p>
<p id="p462mcpsimp"><a name="p462mcpsimp"></a><a name="p462mcpsimp"></a>mtval:0x0</p>
<p id="p463mcpsimp"><a name="p463mcpsimp"></a><a name="p463mcpsimp"></a>gp:0x10fda0</p>
<p id="p464mcpsimp"><a name="p464mcpsimp"></a><a name="p464mcpsimp"></a>mstatus:0x1800</p>
<p id="p465mcpsimp"><a name="p465mcpsimp"></a><a name="p465mcpsimp"></a>mepc:0x1008ae</p>
<p id="p466mcpsimp"><a name="p466mcpsimp"></a><a name="p466mcpsimp"></a>ra:0x1008ae</p>
<p id="p467mcpsimp"><a name="p467mcpsimp"></a><a name="p467mcpsimp"></a>sp:0x113540</p>
<p id="p468mcpsimp"><a name="p468mcpsimp"></a><a name="p468mcpsimp"></a>X4 :0x0</p>
<p id="p469mcpsimp"><a name="p469mcpsimp"></a><a name="p469mcpsimp"></a>X5 :0xffffd8f0</p>
<p id="p470mcpsimp"><a name="p470mcpsimp"></a><a name="p470mcpsimp"></a>X6 :0x1134bb</p>
<p id="p471mcpsimp"><a name="p471mcpsimp"></a><a name="p471mcpsimp"></a>X7 :0xa</p>
<p id="p472mcpsimp"><a name="p472mcpsimp"></a><a name="p472mcpsimp"></a>X8 :0x113670</p>
<p id="p473mcpsimp"><a name="p473mcpsimp"></a><a name="p473mcpsimp"></a>X9 :0x1137cc</p>
<p id="p474mcpsimp"><a name="p474mcpsimp"></a><a name="p474mcpsimp"></a>X10:0x11359c</p>
<p id="p475mcpsimp"><a name="p475mcpsimp"></a><a name="p475mcpsimp"></a>X11:0x40e5f3e4</p>
<p id="p476mcpsimp"><a name="p476mcpsimp"></a><a name="p476mcpsimp"></a>X12:0x24</p>
<p id="p477mcpsimp"><a name="p477mcpsimp"></a><a name="p477mcpsimp"></a>X13:0x1</p>
</td>
</tr>
</tbody>
</table>

通过关键信息打印 The node:0x1137b4 has been damaged确定是堆内存被踩踏，通常是前序节点越界才会踩踏后序节点的内存头，直接通过前序节点prenode的callerRA信息确定相邻前序节点申请者，进一步通过反汇编查找callerRA地址，确定到代码行即可。

例2（通过trigger定位踩固定区域）：

异常信息如下：

<a name="table481mcpsimp"></a>
<table><tbody><tr id="row485mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p487mcpsimp"><a name="p487mcpsimp"></a><a name="p487mcpsimp"></a>ptr_node:0x11cfdc</p>
<p id="p488mcpsimp"><a name="p488mcpsimp"></a><a name="p488mcpsimp"></a>task:app_Task</p>
<p id="p489mcpsimp"><a name="p489mcpsimp"></a><a name="p489mcpsimp"></a>thrdId:0x3</p>
<p id="p490mcpsimp"><a name="p490mcpsimp"></a><a name="p490mcpsimp"></a>type:0x5</p>
<p id="p491mcpsimp"><a name="p491mcpsimp"></a><a name="p491mcpsimp"></a>phase:1</p>
<p id="p492mcpsimp"><a name="p492mcpsimp"></a><a name="p492mcpsimp"></a>ccause:0x0</p>
<p id="p493mcpsimp"><a name="p493mcpsimp"></a><a name="p493mcpsimp"></a>mcause:0x0</p>
<p id="p494mcpsimp"><a name="p494mcpsimp"></a><a name="p494mcpsimp"></a>mtval:0xababeffc</p>
<p id="p495mcpsimp"><a name="p495mcpsimp"></a><a name="p495mcpsimp"></a>gp:0x115c50</p>
<p id="p496mcpsimp"><a name="p496mcpsimp"></a><a name="p496mcpsimp"></a>mstatus:0x1800</p>
<p id="p497mcpsimp"><a name="p497mcpsimp"></a><a name="p497mcpsimp"></a><strong id="b1116264134513"><a name="b1116264134513"></a><a name="b1116264134513"></a>mepc:0x102ee0</strong></p>
<p id="p498mcpsimp"><a name="p498mcpsimp"></a><a name="p498mcpsimp"></a>ra:0x2ed4</p>
<p id="p499mcpsimp"><a name="p499mcpsimp"></a><a name="p499mcpsimp"></a>sp:0x11a170</p>
</td>
</tr>
</tbody>
</table>

-   通过mepc: 0x102ee0找到异常发生时所运行的函数。

    <a name="table502mcpsimp"></a>
    <table><tbody><tr id="row506mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p508mcpsimp"><a name="p508mcpsimp"></a><a name="p508mcpsimp"></a>00102eac &lt;OsMemFreeNode&gt;:</p>
    <p id="p509mcpsimp"><a name="p509mcpsimp"></a><a name="p509mcpsimp"></a>102eac: 713d addi sp,sp,-32</p>
    <p id="p510mcpsimp"><a name="p510mcpsimp"></a><a name="p510mcpsimp"></a>102eae: ce06 sw ra,28(sp)</p>
    <p id="p511mcpsimp"><a name="p511mcpsimp"></a><a name="p511mcpsimp"></a>102eb0: cc22 sw s0,24(sp)</p>
    <p id="p512mcpsimp"><a name="p512mcpsimp"></a><a name="p512mcpsimp"></a>102eb2: ca26 sw s1,20(sp)</p>
    <p id="p513mcpsimp"><a name="p513mcpsimp"></a><a name="p513mcpsimp"></a>102eb4: c84a sw s2,16(sp)</p>
    <p id="p514mcpsimp"><a name="p514mcpsimp"></a><a name="p514mcpsimp"></a>102eb6: c64e sw s3,12(sp)</p>
    <p id="p515mcpsimp"><a name="p515mcpsimp"></a><a name="p515mcpsimp"></a>102eb8: 84aa mv s1,a0</p>
    <p id="p516mcpsimp"><a name="p516mcpsimp"></a><a name="p516mcpsimp"></a>102eba: 3fffffff099f l.li s3,3fffffff &lt;_heap_end+0x3fedffff&gt;</p>
    <p id="p517mcpsimp"><a name="p517mcpsimp"></a><a name="p517mcpsimp"></a>102ec0: 4554 lw a3,12(a0)</p>
    <p id="p518mcpsimp"><a name="p518mcpsimp"></a><a name="p518mcpsimp"></a>102ec2: 00455603 lhu a2,4(a0)</p>
    <p id="p519mcpsimp"><a name="p519mcpsimp"></a><a name="p519mcpsimp"></a>102ec6: 05c58913 addi s2,a1,92</p>
    <p id="p520mcpsimp"><a name="p520mcpsimp"></a><a name="p520mcpsimp"></a>102eca: 00858513 addi a0,a1,8</p>
    <p id="p521mcpsimp"><a name="p521mcpsimp"></a><a name="p521mcpsimp"></a>102ece: 0136f5b3 and a1,a3,s3</p>
    <p id="p522mcpsimp"><a name="p522mcpsimp"></a><a name="p522mcpsimp"></a>102ed2: 2ca9 jal ra,10312c</p>
    <p id="p523mcpsimp"><a name="p523mcpsimp"></a><a name="p523mcpsimp"></a>102ed4: 44c8 lw a0,12(s1)</p>
    <p id="p524mcpsimp"><a name="p524mcpsimp"></a><a name="p524mcpsimp"></a>102ed6: 4480 lw s0,8(s1)</p>
    <p id="p525mcpsimp"><a name="p525mcpsimp"></a><a name="p525mcpsimp"></a>102ed8: 013575b3 and a1,a0,s3</p>
    <p id="p526mcpsimp"><a name="p526mcpsimp"></a><a name="p526mcpsimp"></a>102edc: c4cc sw a1,12(s1)</p>
    <p id="p527mcpsimp"><a name="p527mcpsimp"></a><a name="p527mcpsimp"></a>102ede: cc2d beqz s0,102f58 &lt;OsMemFreeNode+0xac&gt;</p>
    <p id="p528mcpsimp"><a name="p528mcpsimp"></a><a name="p528mcpsimp"></a><strong id="b158478541455"><a name="b158478541455"></a><a name="b158478541455"></a>102ee0: 4448 lw a0,12(s0)</strong></p>
    </td>
    </tr>
    </tbody>
    </table>

-   发现异常指令是一条load操作，并且基地址存放在s0寄存器中，结合异常信息，发现s0寄存器是一个异常地址（0xababeff2）。
-   结合代码和反汇编，发现异常指令处是从结构体中取某一字段时异常，因此怀疑结构体被踩。

    <a name="table531mcpsimp"></a>
    <table><tbody><tr id="row535mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p537mcpsimp"><a name="p537mcpsimp"></a><a name="p537mcpsimp"></a>if ((<strong id="b13880675464"><a name="b13880675464"></a><a name="b13880675464"></a>node-&gt;selfNode.preNode != NULL</strong>) &amp;&amp;</p>
    <p id="p538mcpsimp"><a name="p538mcpsimp"></a><a name="p538mcpsimp"></a>!OS_MEM_NODE_GET_USED_FLAG(node-&gt;selfNode.preNode-&gt;selfNode.sizeAndFlag)) {</p>
    <p id="p539mcpsimp"><a name="p539mcpsimp"></a><a name="p539mcpsimp"></a>LosMemDynNode *preNode = node-&gt;selfNode.preNode;</p>
    <p id="p540mcpsimp"><a name="p540mcpsimp"></a><a name="p540mcpsimp"></a>OsMemMergeNode(node);</p>
    <p id="p541mcpsimp"><a name="p541mcpsimp"></a><a name="p541mcpsimp"></a>nextNode = OS_MEM_NEXT_NODE(preNode);</p>
    <p id="p542mcpsimp"><a name="p542mcpsimp"></a><a name="p542mcpsimp"></a>if (!OS_MEM_NODE_GET_USED_FLAG(nextNode-&gt;selfNode.sizeAndFlag)) {</p>
    <p id="p543mcpsimp"><a name="p543mcpsimp"></a><a name="p543mcpsimp"></a>OsMemListDelete(&amp;nextNode-&gt;selfNode.freeNodeInfo, firstNode);</p>
    <p id="p544mcpsimp"><a name="p544mcpsimp"></a><a name="p544mcpsimp"></a>OsMemMergeNode(nextNode);</p>
    <p id="p545mcpsimp"><a name="p545mcpsimp"></a><a name="p545mcpsimp"></a>}</p>
    </td>
    </tr>
    </tbody>
    </table>

-   由于被踩节点是一个动态申请的内存节点，因此通过使能CallerRA记录能力，记录内存节点申请的顶层函数地址，并在第3步中红框位置前增加打印，把内存节点的CallerRA打印出来。
-   结合CallerRA，找到内存节点申请的函数，并在申请到节点后使用Trigger将节点监控起来。

    <a name="table548mcpsimp"></a>
    <table><tbody><tr id="row552mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p554mcpsimp"><a name="p554mcpsimp"></a><a name="p554mcpsimp"></a>ArchProtectionSetAddrRo(node_addr, 4);</p>
    </td>
    </tr>
    </tbody>
    </table>

-   再次触发异常时，通过mepc可以得知踩内存的凶手为0x299f8。

    <a name="table556mcpsimp"></a>
    <table><tbody><tr id="row560mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p562mcpsimp"><a name="p562mcpsimp"></a><a name="p562mcpsimp"></a>task name: osMain, mepc: <strong id="b9651202574616"><a name="b9651202574616"></a><a name="b9651202574616"></a>0x299f8</strong>, caller: 0x2666e, diff addr: 0Ox22130304, old val: Ox0001254d, new val: 0xababeff2</p>
    </td>
    </tr>
    </tbody>
    </table>

### Out Of Memory\(oom\)<a name="ZH-CN_TOPIC_0000002555814479"></a>



#### 原因分析<a name="ZH-CN_TOPIC_0000002524574604"></a>

-   内存泄漏

    通常发生在函数反复被多次调用，函数内动态申请的内存未释放，内存泄露量随时间累积到一定程度，最终耗尽系统内存，触发系统异常挂死。

-   申请内存过大

    当程序尝试分配一块大内存，而系统提供的内存不足，内存分配函数可能会返回失败，未准确处理此类异常触发程序崩溃。

#### 问题定位<a name="ZH-CN_TOPIC_0000002555654507"></a>

**定位方法<a name="section1726125494915"></a>**

-   检查待申请内存大小是否符合业务预期

    若申请大小异常大，则需确认是否为业务逻辑错误或误操作。

    若申请大小合理，则需进一步排查是否存在内存泄露或系统内存资源不足。

-   查看当前内存状态

    若freesize < 申请量，则是内存耗尽，需进一步确认是内存泄露还是堆总大小不够。

    若freesize \> 申请量，但某个特定mem pool内存不足，可结合业务诉求调整对应mem pool内存大小。

    若freesize \> 申请量，且maxfreeNodeSize < 申请量，可能为内存泄露或者碎片化。

-   内存泄露定位流程

    使能LOSCFG\_MEM\_DFX\_SHOW\_CALLER\_RA内核配置，重新编译系统，启用内存泄露追踪功能。具体参考LiteOS开发指南8.4章节。

    在系统运行期间，多次执行task\_mem命令，查看内存节点的使用情况，关注相同callerRA（调用栈返回地址）的内存节点数量是否持续增长，若增长，则表明存在内存泄露，结合callerRA确认泄露点。

-   内存碎片定位与优化

    碎片化程度评估：通过LOS\_MemFragInfo接口获取各内存块大小区间内的空闲块数量，若小内存空闲块数量多则碎片化严重。

    碎片化优化策略：根据业务申请频次与大小，优化slab配置；高频申请/释放的业务模块独立使用内存池，降低碎片化影响。

-   若内存状态均正常可考虑内存小型化优化。

**参考案例<a name="section6505331155017"></a>**

-   解析dump文件可知当前PC所指的位置为panic\_deal，死机原因原因在g\_preserve\_data\_lib中，死机原因code = 0x0B。

    ![](figures/zh-cn_image_0000002532605220.png)

    ![](figures/zh-cn_image_0000002532765182.png)

    -   基于caller反查反汇编文件可知异常上下文为sysc\_monitor.c的124行。

    ![](figures/zh-cn_image_0000002563605099.png)

-   错误码0xB的含义REBOOT\_CAUSE\_MON\_MEM\_ALMOST\_EMPTY，即死机原因内存不足。结合业务侧代码逻辑分析很大概率存在内存泄漏，排查任务的内存使用情况，发现已使用的内存大小一直增加，基本坐实为内存泄露导致的oom，使用FBB-RTOS提供的内存泄露DFX工具定位到问题代码。

    <a name="table637mcpsimp"></a>
    <table><tbody><tr id="row641mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p643mcpsimp"><a name="p643mcpsimp"></a><a name="p643mcpsimp"></a>ID:11 Name:transmit, Priority:20, Status:8, Stack size:3072, Top stack:0x20013100, Heap Curr used:0, Heap Peak used:0</p>
    <p id="p644mcpsimp"><a name="p644mcpsimp"></a><a name="p644mcpsimp"></a>ID:12 Name:ux_task, Priority:10, Status:8, Stack size:8192, Top stack:0x20013d20, Heap Curr used:7240, Heap Peak used:7884</p>
    <p id="p645mcpsimp"><a name="p645mcpsimp"></a><a name="p645mcpsimp"></a>ID:12 Name:ux_task, Priority:10, Status:8, Stack size:8192, Top stack:0x20013d20, Heap Curr used:7332, Heap Peak used:7988</p>
    <p id="p646mcpsimp"><a name="p646mcpsimp"></a><a name="p646mcpsimp"></a>ID:12 Name:ux_task, Priority:10, Status:8, Stack size:8192, Top stack:0x20013d20, Heap Curr used:7424, Heap Peak used:8076</p>
    <p id="p647mcpsimp"><a name="p647mcpsimp"></a><a name="p647mcpsimp"></a>ID:12 Name:ux_task, Priority:10, Status:8, Stack size:8192, Top stack:0x20013d20, Heap Curr used:7516, Heap Peak used:8160</p>
    <p id="p648mcpsimp"><a name="p648mcpsimp"></a><a name="p648mcpsimp"></a>ID:12 Name:ux_task, Priority:10, Status:8, Stack size:8192, Top stack:0x20013d20, Heap Curr used:7608, Heap Peak used:8260</p>
    <p id="p649mcpsimp"><a name="p649mcpsimp"></a><a name="p649mcpsimp"></a>ID:12 Name:ux_task, Priority:10, Status:8, Stack size:8192, Top stack:0x20013d20, Heap Curr used:7700, Heap Peak used:8352</p>
    <p id="p650mcpsimp"><a name="p650mcpsimp"></a><a name="p650mcpsimp"></a>ID:12 Name:ux_task, Priority:10, Status:8, Stack size:8192, Top stack:0x20013d20, Heap Curr used:7792, Heap Peak used:8444</p>
    <p id="p651mcpsimp"><a name="p651mcpsimp"></a><a name="p651mcpsimp"></a>ID:12 Name:ux_task, Priority:10, Status:8, Stack size:8192, Top stack:0x20013d20, Heap Curr used:7884, Heap Peak used:8536</p>
    <p id="p652mcpsimp"><a name="p652mcpsimp"></a><a name="p652mcpsimp"></a>ID:12 Name:ux_task, Priority:10, Status:8, Stack size:8192, Top stack:0x20013d20, Heap Curr used:7976, Heap Peak used:8628</p>
    <p id="p653mcpsimp"><a name="p653mcpsimp"></a><a name="p653mcpsimp"></a>ID:12 Name:ux_task, Priority:10, Status:8, Stack size:8192, Top stack:0x20013d20, Heap Curr used:8068, Heap Peak used:8720</p>
    <p id="p654mcpsimp"><a name="p654mcpsimp"></a><a name="p654mcpsimp"></a>ID:12 Name:ux_task, Priority:10, Status:8, Stack size:8192, Top stack:0x20013d20, Heap Curr used:8160, Heap Peak used:8812</p>
    <p id="p655mcpsimp"><a name="p655mcpsimp"></a><a name="p655mcpsimp"></a>ID:12 Name:ux_task, Priority:10, Status:8, Stack size:8192, Top stack:0x20013d20, Heap Curr used:8252, Heap Peak used:8904</p>
    <p id="p656mcpsimp"><a name="p656mcpsimp"></a><a name="p656mcpsimp"></a>ID:12 Name:ux_task, Priority:10, Status:8, Stack size:8192, Top stack:0x20013d20, Heap Curr used:8344, Heap Peak used:8996</p>
    <p id="p657mcpsimp"><a name="p657mcpsimp"></a><a name="p657mcpsimp"></a>ID:12 Name:ux_task, Priority:10, Status:8, Stack size:8192, Top stack:0x20013d20, Heap Curr used:8436, Heap Peak used:9264</p>
    <p id="p658mcpsimp"><a name="p658mcpsimp"></a><a name="p658mcpsimp"></a>ID:12 Name:ux_task, Priority:10, Status:8, Stack size:8192, Top stack:0x20013d20, Heap Curr used:8528, Heap Peak used:9264</p>
    <p id="p659mcpsimp"><a name="p659mcpsimp"></a><a name="p659mcpsimp"></a>ID:12 Name:ux_task, Priority:10, Status:8, Stack size:8192, Top stack:0x20013d20, Heap Curr used:8620, Heap Peak used:9272</p>
    <p id="p660mcpsimp"><a name="p660mcpsimp"></a><a name="p660mcpsimp"></a>ID:12 Name:ux_task, Priority:10, Status:8, Stack size:8192, Top stack:0x20013d20, Heap Curr used:8712, Heap Peak used:9364</p>
    <p id="p661mcpsimp"><a name="p661mcpsimp"></a><a name="p661mcpsimp"></a>ID:12 Name:ux_task, Priority:10, Status:8, Stack size:8192, Top stack:0x20013d20, Heap Curr used:8804, Heap Peak used:9456</p>
    <p id="p662mcpsimp"><a name="p662mcpsimp"></a><a name="p662mcpsimp"></a>ID:12 Name:ux_task, Priority:10, Status:8, Stack size:8192, Top stack:0x20013d20, Heap Curr used:8896, Heap Peak used:9548</p>
    <p id="p663mcpsimp"><a name="p663mcpsimp"></a><a name="p663mcpsimp"></a>ID:12 Name:ux_task, Priority:10, Status:8, Stack size:8192, Top stack:0x20013d20, Heap Curr used:8988, Heap Peak used:9640</p>
    <p id="p664mcpsimp"><a name="p664mcpsimp"></a><a name="p664mcpsimp"></a>ID:12 Name:ux_task, Priority:10, Status:8, Stack size:8192, Top stack:0x20013d20, Heap Curr used:9080, Heap Peak used:9732</p>
    </td>
    </tr>
    </tbody>
    </table>

### 内存布局非对齐<a name="ZH-CN_TOPIC_0000002524734556"></a>



#### 原因分析<a name="ZH-CN_TOPIC_0000002555814481"></a>

该场景下产生异常的主要原因是链接脚本的错误配置。

#### 问题定位与解决<a name="ZH-CN_TOPIC_0000002524574606"></a>

**定位方法<a name="section5604638152416"></a>**

-   启动阶段异常优先检查链接脚本与内存布局。

    若系统异常发生在启动阶段，应首先排查链接脚本中各段（section）是否满足内存对齐要求，若满足对齐要求，使用工具dump RAM中的代码/数据段，验证其是否与链接脚本预期一致。

-   任务创建阶段异常优先检查栈起始地址对齐。

    若异常发生在任务创建过程中，优先通过任务创建接口的返回值判断。

-   结构体字段读取异常优先检查动态内存节点对齐。

    若异常发生在读取动态分配结构体字段时，且已排除内存踩踏等破坏性写入，优先通过打印方式将内存头结点地址打印出来，检查内存地址是否满足4bytes对齐。

**参考案例<a name="section1394575011241"></a>**

-   使用LOS\_MemTaskIdGet函数获取一个指针所属的task，返回异常（ret != taskID）。

    <a name="table1051mcpsimp"></a>
    <table><tbody><tr id="row1055mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p1057mcpsimp"><a name="p1057mcpsimp"></a><a name="p1057mcpsimp"></a>static UINT32 testcase(VOID)</p>
    <p id="p1058mcpsimp"><a name="p1058mcpsimp"></a><a name="p1058mcpsimp"></a>{</p>
    <p id="p1059mcpsimp"><a name="p1059mcpsimp"></a><a name="p1059mcpsimp"></a>UINT32 ret;</p>
    <p id="p1060mcpsimp"><a name="p1060mcpsimp"></a><a name="p1060mcpsimp"></a>UINT32 size = 0x100;</p>
    <p id="p1061mcpsimp"><a name="p1061mcpsimp"></a><a name="p1061mcpsimp"></a>void *p = NULL;</p>
    <p id="p1062mcpsimp"><a name="p1062mcpsimp"></a><a name="p1062mcpsimp"></a>ret = LOS_MemTaskIdGet(p);</p>
    <p id="p1063mcpsimp"><a name="p1063mcpsimp"></a><a name="p1063mcpsimp"></a>ICUNIT_ASSERT_EQUAL(ret, OS_INVALID, ret);</p>
    <p id="p1064mcpsimp"><a name="p1064mcpsimp"></a><a name="p1064mcpsimp"></a>p = LOS_MemAlloc(OS_SYS_MEM_ADDR, size);</p>
    <p id="p1065mcpsimp"><a name="p1065mcpsimp"></a><a name="p1065mcpsimp"></a>printf("p:%p\n", p);</p>
    <p id="p1066mcpsimp"><a name="p1066mcpsimp"></a><a name="p1066mcpsimp"></a>ICUNIT_ASSERT_NOT_EQUAL(p, NULL, p);</p>
    <p id="p1067mcpsimp"><a name="p1067mcpsimp"></a><a name="p1067mcpsimp"></a><strong id="b01390116485"><a name="b01390116485"></a><a name="b01390116485"></a>ret = LOS_MemTaskIdGet(p);   // 异常产生点</strong></p>
    <p id="p1068mcpsimp"><a name="p1068mcpsimp"></a><a name="p1068mcpsimp"></a>ICUNIT_ASSERT_EQUAL(ret, OsCurrTaskGet()-&gt;taskId, ret);</p>
    <p id="p1069mcpsimp"><a name="p1069mcpsimp"></a><a name="p1069mcpsimp"></a>ret = LOS_MemFree(OS_SYS_MEM_ADDR, p);</p>
    <p id="p1070mcpsimp"><a name="p1070mcpsimp"></a><a name="p1070mcpsimp"></a>ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);</p>
    <p id="p1071mcpsimp"><a name="p1071mcpsimp"></a><a name="p1071mcpsimp"></a>size = 0x1;</p>
    <p id="p1072mcpsimp"><a name="p1072mcpsimp"></a><a name="p1072mcpsimp"></a>p = LOS_MemAlloc(OS_SYS_MEM_ADDR, size);</p>
    <p id="p1073mcpsimp"><a name="p1073mcpsimp"></a><a name="p1073mcpsimp"></a>printf("p:%p\n", p);</p>
    <p id="p1074mcpsimp"><a name="p1074mcpsimp"></a><a name="p1074mcpsimp"></a>ICUNIT_ASSERT_NOT_EQUAL(p, NULL, p);</p>
    <p id="p1075mcpsimp"><a name="p1075mcpsimp"></a><a name="p1075mcpsimp"></a>}</p>
    </td>
    </tr>
    </tbody>
    </table>

-   增加打印，遍历内存节点时，出现大量非对齐内存节点（内存节点至少应该是2bytes对齐）。

    <a name="table1078mcpsimp"></a>
    <table><tbody><tr id="row1082mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p1084mcpsimp"><a name="p1084mcpsimp"></a><a name="p1084mcpsimp"></a>[XTS_I] ====&gt; ts [liteos_memory] start</p>
    <p id="p1085mcpsimp"><a name="p1085mcpsimp"></a><a name="p1085mcpsimp"></a>[XTS_I] ---&gt; tc [IT_LOS_MEM_043] start</p>
    <p id="p1086mcpsimp"><a name="p1086mcpsimp"></a><a name="p1086mcpsimp"></a>[ERR] input ptr is out of system memory range</p>
    <p id="p1087mcpsimp"><a name="p1087mcpsimp"></a><a name="p1087mcpsimp"></a>p:0x117f59        // 异常内存地址，非对齐</p>
    <p id="p1088mcpsimp"><a name="p1088mcpsimp"></a><a name="p1088mcpsimp"></a>tmpNode:0x116b59</p>
    <p id="p1089mcpsimp"><a name="p1089mcpsimp"></a><a name="p1089mcpsimp"></a>tmpNode:0x116e01</p>
    <p id="p1090mcpsimp"><a name="p1090mcpsimp"></a><a name="p1090mcpsimp"></a>tmpNode:0x117069</p>
    <p id="p1091mcpsimp"><a name="p1091mcpsimp"></a><a name="p1091mcpsimp"></a>tmpNode:0x1172ad</p>
    <p id="p1092mcpsimp"><a name="p1092mcpsimp"></a><a name="p1092mcpsimp"></a>tmpNode:0x1174e1</p>
    <p id="p1093mcpsimp"><a name="p1093mcpsimp"></a><a name="p1093mcpsimp"></a>tmpNode:0x117cfl</p>
    <p id="p1094mcpsimp"><a name="p1094mcpsimp"></a><a name="p1094mcpsimp"></a>tmpNode:0x117f49</p>
    <p id="p1095mcpsimp"><a name="p1095mcpsimp"></a><a name="p1095mcpsimp"></a>[XIS_E][testcase:46] ret=1</p>
    <p id="p1096mcpsimp"><a name="p1096mcpsimp"></a><a name="p1096mcpsimp"></a>[XIS_E][testcase:46] [step0: NULL] Check [(ret) == (0)] failed, errno = 0x78740002</p>
    </td>
    </tr>
    </tbody>
    </table>

-   检查ld文件，发现堆的起始位置未进行对齐处理，引起内存池异常。

    <a name="table1099mcpsimp"></a>
    <table><tbody><tr id="row1103mcpsimp"><td class="cellrowborder" valign="top" width="100%"><p id="p1105mcpsimp"><a name="p1105mcpsimp"></a><a name="p1105mcpsimp"></a>.bss : ALIGN(0x20) {</p>
    <p id="p1106mcpsimp"><a name="p1106mcpsimp"></a><a name="p1106mcpsimp"></a>__bss_start = .;</p>
    <p id="p1107mcpsimp"><a name="p1107mcpsimp"></a><a name="p1107mcpsimp"></a>*(.bss .bss.* .sbss* .gnu.linkonce.b.* COMMON)</p>
    <p id="p1108mcpsimp"><a name="p1108mcpsimp"></a><a name="p1108mcpsimp"></a>__bss_end = .;</p>
    <p id="p1109mcpsimp"><a name="p1109mcpsimp"></a><a name="p1109mcpsimp"></a><strong id="b311652624816"><a name="b311652624816"></a><a name="b311652624816"></a>. = ALIGN(0x10);  // 添加对齐</strong></p>
    <p id="p1110mcpsimp"><a name="p1110mcpsimp"></a><a name="p1110mcpsimp"></a>__heap_start = .;</p>
    <p id="p1111mcpsimp"><a name="p1111mcpsimp"></a><a name="p1111mcpsimp"></a>} &gt; SRAM_DATA</p>
    <p id="p1112mcpsimp"><a name="p1112mcpsimp"></a><a name="p1112mcpsimp"></a>__heap_end = ORIGIN(SRAM_DATA) + LENGTH(SRAM_DATA);</p>
    <p id="p1113mcpsimp"><a name="p1113mcpsimp"></a><a name="p1113mcpsimp"></a>__startup_stack = __heap_end - 0x20;</p>
    <p id="p1114mcpsimp"><a name="p1114mcpsimp"></a><a name="p1114mcpsimp"></a>__heap_size = __heap_end - __heap_start;</p>
    <p id="p1115mcpsimp"><a name="p1115mcpsimp"></a><a name="p1115mcpsimp"></a>. = ALIGN(0x10);</p>
    <p id="p1116mcpsimp"><a name="p1116mcpsimp"></a><a name="p1116mcpsimp"></a>__end = .;</p>
    </td>
    </tr>
    </tbody>
    </table>

## 总结<a name="ZH-CN_TOPIC_0000002555654509"></a>

<a name="table1184mcpsimp"></a>
<table><thead align="left"><tr id="row1190mcpsimp"><th class="cellrowborder" valign="top" width="18.81%" id="mcps1.1.4.1.1"><p id="p1192mcpsimp"><a name="p1192mcpsimp"></a><a name="p1192mcpsimp"></a><strong id="b1193mcpsimp"><a name="b1193mcpsimp"></a><a name="b1193mcpsimp"></a>关键异常信息</strong></p>
</th>
<th class="cellrowborder" valign="top" width="15.840000000000002%" id="mcps1.1.4.1.2"><p id="p1195mcpsimp"><a name="p1195mcpsimp"></a><a name="p1195mcpsimp"></a><strong id="b1196mcpsimp"><a name="b1196mcpsimp"></a><a name="b1196mcpsimp"></a>死机原因</strong></p>
</th>
<th class="cellrowborder" valign="top" width="65.35%" id="mcps1.1.4.1.3"><p id="p1198mcpsimp"><a name="p1198mcpsimp"></a><a name="p1198mcpsimp"></a><strong id="b1199mcpsimp"><a name="b1199mcpsimp"></a><a name="b1199mcpsimp"></a>根因排查步骤</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row1200mcpsimp"><td class="cellrowborder" valign="top" width="18.81%" headers="mcps1.1.4.1.1 "><p id="p1202mcpsimp"><a name="p1202mcpsimp"></a><a name="p1202mcpsimp"></a>Stack overflow</p>
</td>
<td class="cellrowborder" valign="top" width="15.840000000000002%" headers="mcps1.1.4.1.2 "><p id="p1204mcpsimp"><a name="p1204mcpsimp"></a><a name="p1204mcpsimp"></a>栈溢出</p>
</td>
<td class="cellrowborder" valign="top" width="65.35%" headers="mcps1.1.4.1.3 "><p id="p18189184394811"><a name="p18189184394811"></a><a name="p18189184394811"></a>通过日志中的任务名称和任务ID（task ID:）可确定发生栈溢出的具体线程，再结合栈估算工具调整栈大小或者整改代码。</p>
</td>
</tr>
<tr id="row1208mcpsimp"><td class="cellrowborder" valign="top" width="18.81%" headers="mcps1.1.4.1.1 "><p id="p1210mcpsimp"><a name="p1210mcpsimp"></a><a name="p1210mcpsimp"></a>Instruction access fault</p>
</td>
<td class="cellrowborder" valign="top" width="15.840000000000002%" headers="mcps1.1.4.1.2 "><p id="p1212mcpsimp"><a name="p1212mcpsimp"></a><a name="p1212mcpsimp"></a>取指异常</p>
</td>
<td class="cellrowborder" valign="top" width="65.35%" headers="mcps1.1.4.1.3 "><a name="ol1214mcpsimp"></a><a name="ol1214mcpsimp"></a><ol id="ol1214mcpsimp"><li>根据mtval、ra定位异常代码上下文，排查函数指针。</li><li>检查函数所在代码段是否有拷贝动作，若有则排查链接脚本。</li><li>排查代码段是否被踩，可通过PMP机制定位。</li></ol>
</td>
</tr>
<tr id="row1218mcpsimp"><td class="cellrowborder" valign="top" width="18.81%" headers="mcps1.1.4.1.1 "><p id="p1220mcpsimp"><a name="p1220mcpsimp"></a><a name="p1220mcpsimp"></a>Oops:NMI</p>
</td>
<td class="cellrowborder" valign="top" width="15.840000000000002%" headers="mcps1.1.4.1.2 "><p id="p1222mcpsimp"><a name="p1222mcpsimp"></a><a name="p1222mcpsimp"></a>狗超时</p>
</td>
<td class="cellrowborder" valign="top" width="65.35%" headers="mcps1.1.4.1.3 "><p id="p12484145812485"><a name="p12484145812485"></a><a name="p12484145812485"></a>根据超时前trace记录的调度信息、CPU占用信息和异常时所有任务的调用栈即可确定原因。</p>
</td>
</tr>
<tr id="row1226mcpsimp"><td class="cellrowborder" valign="top" width="18.81%" headers="mcps1.1.4.1.1 "><p id="p1228mcpsimp"><a name="p1228mcpsimp"></a><a name="p1228mcpsimp"></a>Panic</p>
</td>
<td class="cellrowborder" valign="top" width="15.840000000000002%" headers="mcps1.1.4.1.2 "><p id="p1230mcpsimp"><a name="p1230mcpsimp"></a><a name="p1230mcpsimp"></a>主动Panic</p>
</td>
<td class="cellrowborder" valign="top" width="65.35%" headers="mcps1.1.4.1.3 "><a name="ol1232mcpsimp"></a><a name="ol1232mcpsimp"></a><ol id="ol1232mcpsimp"><li>若打印中含文件名和行号，则可直接确认源码位置。</li><li>若断言失败，则检查断言表达式为何不成立。</li><li>搜索代码打印信息或根据mepc可定位挂死代码行。</li></ol>
</td>
</tr>
<tr id="row1236mcpsimp"><td class="cellrowborder" valign="top" width="18.81%" headers="mcps1.1.4.1.1 "><p id="p1238mcpsimp"><a name="p1238mcpsimp"></a><a name="p1238mcpsimp"></a>PMP access fault</p>
</td>
<td class="cellrowborder" valign="top" width="15.840000000000002%" headers="mcps1.1.4.1.2 "><p id="p1240mcpsimp"><a name="p1240mcpsimp"></a><a name="p1240mcpsimp"></a>访问PMP保护的地址</p>
</td>
<td class="cellrowborder" valign="top" width="65.35%" headers="mcps1.1.4.1.3 "><a name="ol1242mcpsimp"></a><a name="ol1242mcpsimp"></a><ol id="ol1242mcpsimp"><li>根据mepc、mtval可知权限异常的内存地址，排查PMP配置。</li><li>若为取指问题参考取指异常章节定位。</li><li>若为访问非PMP配置内存区间参考踩内存章节定位。</li></ol>
</td>
</tr>
<tr id="row1246mcpsimp"><td class="cellrowborder" valign="top" width="18.81%" headers="mcps1.1.4.1.1 "><p id="p1248mcpsimp"><a name="p1248mcpsimp"></a><a name="p1248mcpsimp"></a>Store/AMO access fault</p>
</td>
<td class="cellrowborder" valign="top" width="15.840000000000002%" headers="mcps1.1.4.1.2 "><p id="p1250mcpsimp"><a name="p1250mcpsimp"></a><a name="p1250mcpsimp"></a>访问保留内存</p>
</td>
<td class="cellrowborder" valign="top" width="65.35%" headers="mcps1.1.4.1.3 "><a name="ol1252mcpsimp"></a><a name="ol1252mcpsimp"></a><ol id="ol1252mcpsimp"><li>根据mtval定位异常访问地址并验证合法性。</li><li>通过mepc定位异常指令地址并反反汇编，定位异常源码。</li></ol>
</td>
</tr>
<tr id="row1255mcpsimp"><td class="cellrowborder" valign="top" width="18.81%" headers="mcps1.1.4.1.1 "><p id="p1257mcpsimp"><a name="p1257mcpsimp"></a><a name="p1257mcpsimp"></a>Dead lock</p>
</td>
<td class="cellrowborder" valign="top" width="15.840000000000002%" headers="mcps1.1.4.1.2 "><p id="p1259mcpsimp"><a name="p1259mcpsimp"></a><a name="p1259mcpsimp"></a>死锁</p>
</td>
<td class="cellrowborder" valign="top" width="65.35%" headers="mcps1.1.4.1.3 "><p id="p1344355620488"><a name="p1344355620488"></a><a name="p1344355620488"></a>反查请求地址定位异常锁函数。</p>
</td>
</tr>
<tr id="row1263mcpsimp"><td class="cellrowborder" valign="top" width="18.81%" headers="mcps1.1.4.1.1 "><p id="p1265mcpsimp"><a name="p1265mcpsimp"></a><a name="p1265mcpsimp"></a>Load/Store address misaligned</p>
</td>
<td class="cellrowborder" valign="top" width="15.840000000000002%" headers="mcps1.1.4.1.2 "><p id="p1267mcpsimp"><a name="p1267mcpsimp"></a><a name="p1267mcpsimp"></a>非对齐访问</p>
</td>
<td class="cellrowborder" valign="top" width="65.35%" headers="mcps1.1.4.1.3 "><p id="p08275564814"><a name="p08275564814"></a><a name="p08275564814"></a>根据mepc即可定位非对齐访问引起挂死的代码行。</p>
</td>
</tr>
<tr id="row1271mcpsimp"><td class="cellrowborder" valign="top" width="18.81%" headers="mcps1.1.4.1.1 "><p id="entry1272mcpsimpp0"><a name="entry1272mcpsimpp0"></a><a name="entry1272mcpsimpp0"></a>-</p>
</td>
<td class="cellrowborder" valign="top" width="15.840000000000002%" headers="mcps1.1.4.1.2 "><p id="p1274mcpsimp"><a name="p1274mcpsimp"></a><a name="p1274mcpsimp"></a>踩内存、OOM、内存布局非对齐</p>
</td>
<td class="cellrowborder" valign="top" width="65.35%" headers="mcps1.1.4.1.3 "><a name="ol1276mcpsimp"></a><a name="ol1276mcpsimp"></a><ol id="ol1276mcpsimp"><li>确认怀疑点后，利用现有DFX工具排查。</li><li>结合业务逻辑分析。</li></ol>
</td>
</tr>
</tbody>
</table>

