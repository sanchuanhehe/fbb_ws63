# 前言<a name="ZH-CN_TOPIC_0000001700134464"></a>

**概述<a name="section4537382116410"></a>**

本文档介绍WS63芯片SDK开发环境（包括：SDK编译、应用程序的开发等），用于帮助用户在快速了解开发环境后编译出可执行文件进行二次开发。

**产品版本<a name="section1423985410207"></a>**

与本文档相对应的产品版本如下。

<a name="table2270181717471"></a>
<table><thead align="left"><tr id="row15364171712479"><th class="cellrowborder" valign="top" width="31.759999999999998%" id="mcps1.1.3.1.1"><p id="p123646174478"><a name="p123646174478"></a><a name="p123646174478"></a><strong id="b12222191212104"><a name="b12222191212104"></a><a name="b12222191212104"></a>产品名称</strong></p>
</th>
<th class="cellrowborder" valign="top" width="68.24%" id="mcps1.1.3.1.2"><p id="p1936401717470"><a name="p1936401717470"></a><a name="p1936401717470"></a><strong id="b1523661211108"><a name="b1523661211108"></a><a name="b1523661211108"></a>产品版本</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row19364317104716"><td class="cellrowborder" valign="top" width="31.759999999999998%" headers="mcps1.1.3.1.1 "><p id="p14623132513473"><a name="p14623132513473"></a><a name="p14623132513473"></a>WS63</p>
</td>
<td class="cellrowborder" valign="top" width="68.24%" headers="mcps1.1.3.1.2 "><p id="p733963813395"><a name="p733963813395"></a><a name="p733963813395"></a>V100</p>
</td>
</tr>
</tbody>
</table>

**读者对象<a name="section4378592816410"></a>**

本文档主要适用于以下工程师：

-   技术支持工程师
-   软件开发工程师

**符号约定<a name="section133020216410"></a>**

在本文中可能出现下列标志，它们所代表的含义如下。

<a name="table2622507016410"></a>
<table><thead align="left"><tr id="row1530720816410"><th class="cellrowborder" valign="top" width="20.580000000000002%" id="mcps1.1.3.1.1"><p id="p6450074116410"><a name="p6450074116410"></a><a name="p6450074116410"></a><strong id="b2136615816410"><a name="b2136615816410"></a><a name="b2136615816410"></a>符号</strong></p>
</th>
<th class="cellrowborder" valign="top" width="79.42%" id="mcps1.1.3.1.2"><p id="p5435366816410"><a name="p5435366816410"></a><a name="p5435366816410"></a><strong id="b5941558116410"><a name="b5941558116410"></a><a name="b5941558116410"></a>说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row1372280416410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p3734547016410"><a name="p3734547016410"></a><a name="p3734547016410"></a><a name="image2670064316410"></a><a name="image2670064316410"></a><span><img class="" id="image2670064316410" height="25.270000000000003" width="55.9265" src="figures/zh-cn_image_0000001700294012.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p1757432116410"><a name="p1757432116410"></a><a name="p1757432116410"></a>表示如不避免则将会导致死亡或严重伤害的具有高等级风险的危害。</p>
</td>
</tr>
<tr id="row466863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p1432579516410"><a name="p1432579516410"></a><a name="p1432579516410"></a><a name="image4895582316410"></a><a name="image4895582316410"></a><span><img class="" id="image4895582316410" height="25.270000000000003" width="55.9265" src="figures/zh-cn_image_0000001748014061.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p959197916410"><a name="p959197916410"></a><a name="p959197916410"></a>表示如不避免则可能导致死亡或严重伤害的具有中等级风险的危害。</p>
</td>
</tr>
<tr id="row123863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p1232579516410"><a name="p1232579516410"></a><a name="p1232579516410"></a><a name="image1235582316410"></a><a name="image1235582316410"></a><span><img class="" id="image1235582316410" height="25.270000000000003" width="55.9265" src="figures/zh-cn_image_0000001748014069.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p123197916410"><a name="p123197916410"></a><a name="p123197916410"></a>表示如不避免则可能导致轻微或中度伤害的具有低等级风险的危害。</p>
</td>
</tr>
<tr id="row5786682116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p2204984716410"><a name="p2204984716410"></a><a name="p2204984716410"></a><a name="image4504446716410"></a><a name="image4504446716410"></a><span><img class="" id="image4504446716410" height="25.270000000000003" width="55.9265" src="figures/zh-cn_image_0000001700134520.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p4388861916410"><a name="p4388861916410"></a><a name="p4388861916410"></a>用于传递设备或环境安全警示信息。如不避免则可能会导致设备损坏、数据丢失、设备性能降低或其它不可预知的结果。</p>
<p id="p1238861916410"><a name="p1238861916410"></a><a name="p1238861916410"></a>“须知”不涉及人身伤害。</p>
</td>
</tr>
<tr id="row2856923116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p5555360116410"><a name="p5555360116410"></a><a name="p5555360116410"></a><a name="image799324016410"></a><a name="image799324016410"></a><span><img class="" id="image799324016410" height="15.96" width="47.88" src="figures/zh-cn_image_0000001700134508.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p4612588116410"><a name="p4612588116410"></a><a name="p4612588116410"></a>对正文中重点信息的补充说明。</p>
<p id="p1232588116410"><a name="p1232588116410"></a><a name="p1232588116410"></a>“说明”不是安全警示信息，不涉及人身、设备及环境伤害信息。</p>
</td>
</tr>
</tbody>
</table>

**修改记录<a name="section2467512116410"></a>**

<a name="table1557726816410"></a>
<table><thead align="left"><tr id="row2942532716410"><th class="cellrowborder" valign="top" width="19.6%" id="mcps1.1.4.1.1"><p id="p3778275416410"><a name="p3778275416410"></a><a name="p3778275416410"></a><strong id="b5687322716410"><a name="b5687322716410"></a><a name="b5687322716410"></a>文档版本</strong></p>
</th>
<th class="cellrowborder" valign="top" width="19.09%" id="mcps1.1.4.1.2"><p id="p5627845516410"><a name="p5627845516410"></a><a name="p5627845516410"></a><strong id="b5800814916410"><a name="b5800814916410"></a><a name="b5800814916410"></a>发布日期</strong></p>
</th>
<th class="cellrowborder" valign="top" width="61.309999999999995%" id="mcps1.1.4.1.3"><p id="p2382284816410"><a name="p2382284816410"></a><a name="p2382284816410"></a><strong id="b3316380216410"><a name="b3316380216410"></a><a name="b3316380216410"></a>修改说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row98621143304"><td class="cellrowborder" valign="top" width="19.6%" headers="mcps1.1.4.1.1 "><p id="p786244316018"><a name="p786244316018"></a><a name="p786244316018"></a>04</p>
</td>
<td class="cellrowborder" valign="top" width="19.09%" headers="mcps1.1.4.1.2 "><p id="p18862184313017"><a name="p18862184313017"></a><a name="p18862184313017"></a>2025-02-28</p>
</td>
<td class="cellrowborder" valign="top" width="61.309999999999995%" headers="mcps1.1.4.1.3 "><p id="p127681017917"><a name="p127681017917"></a><a name="p127681017917"></a>更新“<a href="新增链接外部静态库.md">新增链接外部静态库</a>”小节内容。</p>
</td>
</tr>
<tr id="row135791612125615"><td class="cellrowborder" valign="top" width="19.6%" headers="mcps1.1.4.1.1 "><p id="p857941245617"><a name="p857941245617"></a><a name="p857941245617"></a>03</p>
</td>
<td class="cellrowborder" valign="top" width="19.09%" headers="mcps1.1.4.1.2 "><p id="p8579612115620"><a name="p8579612115620"></a><a name="p8579612115620"></a>2024-10-30</p>
</td>
<td class="cellrowborder" valign="top" width="61.309999999999995%" headers="mcps1.1.4.1.3 "><p id="p16964152245620"><a name="p16964152245620"></a><a name="p16964152245620"></a>更新“<a href="Flash分区表配置.md">Flash分区表配置</a>”小节内容。</p>
</td>
</tr>
<tr id="row153682054121613"><td class="cellrowborder" valign="top" width="19.6%" headers="mcps1.1.4.1.1 "><p id="p1236815411619"><a name="p1236815411619"></a><a name="p1236815411619"></a>02</p>
</td>
<td class="cellrowborder" valign="top" width="19.09%" headers="mcps1.1.4.1.2 "><p id="p1236814546167"><a name="p1236814546167"></a><a name="p1236814546167"></a>2024-07-01</p>
</td>
<td class="cellrowborder" valign="top" width="61.309999999999995%" headers="mcps1.1.4.1.3 "><a name="ul1550619281711"></a><a name="ul1550619281711"></a><ul id="ul1550619281711"><li>更新“<a href="SDK目录结构介绍.md">SDK目录结构介绍</a>”小节内容。</li><li>更新“<a href="添加bin文件编译.md">添加bin文件编译</a>”小节内容。</li><li>更新“<a href="Menuconfig配置.md">Menuconfig配置</a>”小节内容。</li><li>新增“<a href="UART配置方法.md">UART配置方法</a>”小节内容。</li><li>更新“<a href="建立源码目录.md">建立源码目录</a>”小节内容。</li></ul>
</td>
</tr>
<tr id="row5241131133918"><td class="cellrowborder" valign="top" width="19.6%" headers="mcps1.1.4.1.1 "><p id="p3241031143914"><a name="p3241031143914"></a><a name="p3241031143914"></a>01</p>
</td>
<td class="cellrowborder" valign="top" width="19.09%" headers="mcps1.1.4.1.2 "><p id="p724143118391"><a name="p724143118391"></a><a name="p724143118391"></a>2024-04-10</p>
</td>
<td class="cellrowborder" valign="top" width="61.309999999999995%" headers="mcps1.1.4.1.3 "><p id="p1724153116391"><a name="p1724153116391"></a><a name="p1724153116391"></a>第一次正式版本发布。</p>
<a name="ul1422924953920"></a><a name="ul1422924953920"></a><ul id="ul1422924953920"><li>更新“<a href="编译参数详解.md">编译参数详解</a>”小节内容。</li><li>更新“<a href="添加bin文件编译.md">添加bin文件编译</a>”小节内容。</li><li>更新“<a href="Flash分区表配置.md">Flash分区表配置</a>”小节内容。</li></ul>
</td>
</tr>
<tr id="row52414385241"><td class="cellrowborder" valign="top" width="19.6%" headers="mcps1.1.4.1.1 "><p id="p172433822412"><a name="p172433822412"></a><a name="p172433822412"></a>00B03</p>
</td>
<td class="cellrowborder" valign="top" width="19.09%" headers="mcps1.1.4.1.2 "><p id="p824738112414"><a name="p824738112414"></a><a name="p824738112414"></a>2024-03-29</p>
</td>
<td class="cellrowborder" valign="top" width="61.309999999999995%" headers="mcps1.1.4.1.3 "><a name="ul9244850152412"></a><a name="ul9244850152412"></a><ul id="ul9244850152412"><li>更新“<a href="编译参数详解.md">编译参数详解</a>”小节内容。</li><li>更新“<a href="添加bin文件编译.md">添加bin文件编译</a>”小节内容。</li></ul>
</td>
</tr>
<tr id="row5947359616410"><td class="cellrowborder" valign="top" width="19.6%" headers="mcps1.1.4.1.1 "><p id="p1359161103310"><a name="p1359161103310"></a><a name="p1359161103310"></a>00B02</p>
</td>
<td class="cellrowborder" valign="top" width="19.09%" headers="mcps1.1.4.1.2 "><p id="p133594116335"><a name="p133594116335"></a><a name="p133594116335"></a>2024-01-10</p>
</td>
<td class="cellrowborder" valign="top" width="61.309999999999995%" headers="mcps1.1.4.1.3 "><p id="p12215184819505"><a name="p12215184819505"></a><a name="p12215184819505"></a>更新“<a href="安装Python环境.md">安装Python环境</a>”小节内容。</p>
</td>
</tr>
<tr id="row18512113914112"><td class="cellrowborder" valign="top" width="19.6%" headers="mcps1.1.4.1.1 "><p id="p2149706016410"><a name="p2149706016410"></a><a name="p2149706016410"></a>00B01</p>
</td>
<td class="cellrowborder" valign="top" width="19.09%" headers="mcps1.1.4.1.2 "><p id="p648803616410"><a name="p648803616410"></a><a name="p648803616410"></a>2023-11-27</p>
</td>
<td class="cellrowborder" valign="top" width="61.309999999999995%" headers="mcps1.1.4.1.3 "><p id="p1946537916410"><a name="p1946537916410"></a><a name="p1946537916410"></a>第一次临时版本发布。</p>
</td>
</tr>
</tbody>
</table>

# 开发环境搭建<a name="ZH-CN_TOPIC_0000001700293960"></a>



## SDK开发环境简介<a name="ZH-CN_TOPIC_0000001748093909"></a>

典型的SDK开发环境主要包括：

-   Linux服务器

    Linux服务器主要用于建立交叉编译环境，实现在Linux服务器上编译出可以在目标板上运行的可执行代码。

-   工作台

    工作台主要用于目标板烧录和调试，通过串口与目标板连接，开发人员可以在工作台中烧录目标板的镜像、调试程序。工作台通常需要安装终端工具，用于登录Linux服务器和目标板，查看目标板的打印输出信息。工作台一般为Windows或Linux操作系统，在Windows或Linux工作台运行的终端工具通常有SecureCRT、Putty、miniCom等，这些软件需要从其官网下载。

-   目标板

    本文的目标板以DEMO板为例，DEMO板与工作台通过USB转串口连接。工作台将交叉编译出来的DEMO板镜像通过串口烧录到DEMO板。如[图1](#fig1236915206315)所示。

    **图 1**  SDK 开发环境<a name="fig1236915206315"></a>  
    
    ![](figures/zh-cn_image_0000001751798221.png)

## 搭建Linux开发环境<a name="ZH-CN_TOPIC_0000001700293988"></a>

Linux系统推荐使用Ubuntu 20.04及以上版本，Shell使用bash ，SDK使用Cmake编译（3.14.1以上），编译工具还包括Python（3.8.0以上）等。




### 配置Shell<a name="ZH-CN_TOPIC_0000001700293972"></a>

配置默认使用 bash。打开Linux终端，执行命令“sudo dpkg-reconfigure dash”，选择 no。

### 安装Cmake<a name="ZH-CN_TOPIC_0000001758443013"></a>

打开Linux终端，执行命令“sudo apt install cmake”，完成Cmake的安装。

### 安装Python环境<a name="ZH-CN_TOPIC_0000001710481148"></a>

1.  打开Linux终端，输入命令“python3 -V”，查看Python版本号，推荐python3.8.0以上版本。
2.  如果Python版本太低，请使用命令“sudo apt-get update”更新系统到最新，或通过命令“sudo apt-get install python3 -y”安装Python3（需root/sudo权限安装），安装后再次确认Python版本。

    如果仍不能满足版本要求，请从“[https://www.python.org/downloads/source/](https://www.python.org/downloads/source/)  ”下载对应版本源码包，下载与安装方法请阅读  [https://wiki.python.org/moin/BeginnersGuide/Download](https://wiki.python.org/moin/BeginnersGuide/Download)  和源码包内README内容。

3.  安装Python包管理工具，运行命令“sudo apt-get install python3-setuptools python3-pip -y”（需root/sudo权限安装）。
4.  安装Kconfiglib 14.1.0+，使用命令“sudo pip3 install kconfiglib”（需root/sudo权限安装），或从“[https://pypi.org/project/kconfiglib](https://pypi.org/project/kconfiglib)”下载.whl文件（例如：kconfiglib-14.1.0-py2.py3-none-any.whl）后，使用“pip3 install kconfiglib-xxx.whl”进行安装（需root/sudo权限安装），或者下载源码包到本地并解压，使用“python setup.py install”进行安装（需root/sudo权限安装）。安装完成界面如[图1](#fig743717512220)所示。

    **图 1**  安装Kconfiglib组件包完成示例<a name="fig743717512220"></a>  
    ![](figures/安装Kconfiglib组件包完成示例.png "安装Kconfiglib组件包完成示例")

5.  安装升级文件签名依赖的Python组件包。

    安装pycparser：

    从“[https://pypi.org/project/pycparser/](https://pypi.org/project/pycparser/)”下载.whl文件（例如：pycparser-2.21-py2.py3-none-any.whl）后，使用“pip3 install pycparser-xxx.whl”进行安装（需root/sudo权限安装），或者下载源码包到本地并解压，使用“python setup.py install”进行安装（需root/sudo权限安装）。安装完成后界面会提示“Successfully intalled pycparser-2.21”。

>![](public_sys-resources/icon-note.gif) **说明：** 
>如果构建环境中包含多个python，特别是多个同版本的python，而用户无法辨认正在使用的是其中的哪个版本，此情况下，在安装python组件包时，推荐使用组件包源码进行安装。

# 编译SDK<a name="ZH-CN_TOPIC_0000001748014017"></a>



## SDK目录结构介绍<a name="ZH-CN_TOPIC_0000001748014045"></a>

SDK根目录结构如[表1](#table13927142512394)所示。

**表 1**  SDK根目录

<a name="table13927142512394"></a>
<table><thead align="left"><tr id="row15927132514396"><th class="cellrowborder" valign="top" width="27.38%" id="mcps1.2.3.1.1"><p id="p11927325113916"><a name="p11927325113916"></a><a name="p11927325113916"></a>目录</p>
</th>
<th class="cellrowborder" valign="top" width="72.61999999999999%" id="mcps1.2.3.1.2"><p id="p1292722593913"><a name="p1292722593913"></a><a name="p1292722593913"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row292882517399"><td class="cellrowborder" valign="top" width="27.38%" headers="mcps1.2.3.1.1 "><p id="p159281025163910"><a name="p159281025163910"></a><a name="p159281025163910"></a>application</p>
</td>
<td class="cellrowborder" valign="top" width="72.61999999999999%" headers="mcps1.2.3.1.2 "><p id="p417918234"><a name="p417918234"></a><a name="p417918234"></a>应用层代码（其中包含demo程序为参考示例）。</p>
</td>
</tr>
<tr id="row19928225163913"><td class="cellrowborder" valign="top" width="27.38%" headers="mcps1.2.3.1.1 "><p id="p2928122511393"><a name="p2928122511393"></a><a name="p2928122511393"></a>bootloader</p>
</td>
<td class="cellrowborder" valign="top" width="72.61999999999999%" headers="mcps1.2.3.1.2 "><p id="p1192882518398"><a name="p1192882518398"></a><a name="p1192882518398"></a>boot(Flashboot/SSB)代码。</p>
</td>
</tr>
<tr id="row12308241122019"><td class="cellrowborder" valign="top" width="27.38%" headers="mcps1.2.3.1.1 "><p id="p1566453261918"><a name="p1566453261918"></a><a name="p1566453261918"></a>build</p>
</td>
<td class="cellrowborder" valign="top" width="72.61999999999999%" headers="mcps1.2.3.1.2 "><p id="p466403211919"><a name="p466403211919"></a><a name="p466403211919"></a>SDK构建所需的脚本、配置文件。</p>
</td>
</tr>
<tr id="row1653555018202"><td class="cellrowborder" valign="top" width="27.38%" headers="mcps1.2.3.1.1 "><p id="p1066416323191"><a name="p1066416323191"></a><a name="p1066416323191"></a>build.py</p>
</td>
<td class="cellrowborder" valign="top" width="72.61999999999999%" headers="mcps1.2.3.1.2 "><p id="p14664123216198"><a name="p14664123216198"></a><a name="p14664123216198"></a>编译入口脚本。</p>
</td>
</tr>
<tr id="row10787191320215"><td class="cellrowborder" valign="top" width="27.38%" headers="mcps1.2.3.1.1 "><p id="p116642329192"><a name="p116642329192"></a><a name="p116642329192"></a>CMakeLists.txt</p>
</td>
<td class="cellrowborder" valign="top" width="72.61999999999999%" headers="mcps1.2.3.1.2 "><p id="p14664132191912"><a name="p14664132191912"></a><a name="p14664132191912"></a>Cmake工程顶层“CMakeLists.txt”文件。</p>
</td>
</tr>
<tr id="row109286253399"><td class="cellrowborder" valign="top" width="27.38%" headers="mcps1.2.3.1.1 "><p id="p17664432111917"><a name="p17664432111917"></a><a name="p17664432111917"></a>config.in</p>
</td>
<td class="cellrowborder" valign="top" width="72.61999999999999%" headers="mcps1.2.3.1.2 "><p id="p136644327197"><a name="p136644327197"></a><a name="p136644327197"></a>Kconfig配置文件。</p>
</td>
</tr>
<tr id="row15928132512396"><td class="cellrowborder" valign="top" width="27.38%" headers="mcps1.2.3.1.1 "><p id="p666414326194"><a name="p666414326194"></a><a name="p666414326194"></a>drivers</p>
</td>
<td class="cellrowborder" valign="top" width="72.61999999999999%" headers="mcps1.2.3.1.2 "><p id="p1566483281918"><a name="p1566483281918"></a><a name="p1566483281918"></a>驱动代码。</p>
</td>
</tr>
<tr id="row415218166102"><td class="cellrowborder" valign="top" width="27.38%" headers="mcps1.2.3.1.1 "><p id="p19664732181920"><a name="p19664732181920"></a><a name="p19664732181920"></a>include</p>
</td>
<td class="cellrowborder" valign="top" width="72.61999999999999%" headers="mcps1.2.3.1.2 "><p id="p1066453216197"><a name="p1066453216197"></a><a name="p1066453216197"></a>API头文件存放目录。</p>
</td>
</tr>
<tr id="row75842056117"><td class="cellrowborder" valign="top" width="27.38%" headers="mcps1.2.3.1.1 "><p id="p1166412325191"><a name="p1166412325191"></a><a name="p1166412325191"></a>interim_binary</p>
</td>
<td class="cellrowborder" valign="top" width="72.61999999999999%" headers="mcps1.2.3.1.2 "><p id="p1866403221917"><a name="p1866403221917"></a><a name="p1866403221917"></a>库存放目录。</p>
</td>
</tr>
<tr id="row152262035269"><td class="cellrowborder" valign="top" width="27.38%" headers="mcps1.2.3.1.1 "><p id="p1566413221912"><a name="p1566413221912"></a><a name="p1566413221912"></a>kernel</p>
</td>
<td class="cellrowborder" valign="top" width="72.61999999999999%" headers="mcps1.2.3.1.2 "><p id="p1066483281910"><a name="p1066483281910"></a><a name="p1066483281910"></a>内核代码和OS接口适配层代码。</p>
</td>
</tr>
<tr id="row07472124410"><td class="cellrowborder" valign="top" width="27.38%" headers="mcps1.2.3.1.1 "><p id="p1465510306495"><a name="p1465510306495"></a><a name="p1465510306495"></a>libs_url</p>
</td>
<td class="cellrowborder" valign="top" width="72.61999999999999%" headers="mcps1.2.3.1.2 "><p id="p11655153012490"><a name="p11655153012490"></a><a name="p11655153012490"></a>库文件。</p>
</td>
</tr>
<tr id="row26011201747"><td class="cellrowborder" valign="top" width="27.38%" headers="mcps1.2.3.1.1 "><p id="p433325713498"><a name="p433325713498"></a><a name="p433325713498"></a>middleware</p>
</td>
<td class="cellrowborder" valign="top" width="72.61999999999999%" headers="mcps1.2.3.1.2 "><p id="p233335715491"><a name="p233335715491"></a><a name="p233335715491"></a>中间件代码。</p>
</td>
</tr>
<tr id="row17392173512420"><td class="cellrowborder" valign="top" width="27.38%" headers="mcps1.2.3.1.1 "><p id="p14333185794910"><a name="p14333185794910"></a><a name="p14333185794910"></a>open_source</p>
</td>
<td class="cellrowborder" valign="top" width="72.61999999999999%" headers="mcps1.2.3.1.2 "><p id="p14333957164919"><a name="p14333957164919"></a><a name="p14333957164919"></a>开源代码。</p>
</td>
</tr>
<tr id="row17747172410413"><td class="cellrowborder" valign="top" width="27.38%" headers="mcps1.2.3.1.1 "><p id="p1333312572493"><a name="p1333312572493"></a><a name="p1333312572493"></a>protocol</p>
</td>
<td class="cellrowborder" valign="top" width="72.61999999999999%" headers="mcps1.2.3.1.2 "><p id="p6333157154912"><a name="p6333157154912"></a><a name="p6333157154912"></a>WiFi、BT、Radar等组件代码。</p>
</td>
</tr>
<tr id="row768611001510"><td class="cellrowborder" valign="top" width="27.38%" headers="mcps1.2.3.1.1 "><p id="p03333575492"><a name="p03333575492"></a><a name="p03333575492"></a>test</p>
</td>
<td class="cellrowborder" valign="top" width="72.61999999999999%" headers="mcps1.2.3.1.2 "><p id="p333345713495"><a name="p333345713495"></a><a name="p333345713495"></a>testsuite代码。</p>
</td>
</tr>
<tr id="row44171914181911"><td class="cellrowborder" valign="top" width="27.38%" headers="mcps1.2.3.1.1 "><p id="p93331557104916"><a name="p93331557104916"></a><a name="p93331557104916"></a>tools</p>
</td>
<td class="cellrowborder" valign="top" width="72.61999999999999%" headers="mcps1.2.3.1.2 "><p id="p1833415714912"><a name="p1833415714912"></a><a name="p1833415714912"></a>包含编译工具链（包括linux和windows）、镜像打包脚本、NV制作工具和签名脚本等。</p>
</td>
</tr>
<tr id="row15401556194817"><td class="cellrowborder" valign="top" width="27.38%" headers="mcps1.2.3.1.1 "><p id="p53342570499"><a name="p53342570499"></a><a name="p53342570499"></a>output</p>
</td>
<td class="cellrowborder" valign="top" width="72.61999999999999%" headers="mcps1.2.3.1.2 "><p id="p43341957154917"><a name="p43341957154917"></a><a name="p43341957154917"></a>编译时生成的目标文件与中间文件（包括库文件、打印log、生成的二进制文件等）。</p>
</td>
</tr>
</tbody>
</table>

解压缩SDK后的根目录，如[图2](#fig1447325620456)所示。注：上表所述的output目录是编译后生成的。

**图 1** <a name="fig112752111447"></a>  

**图 2**  解压缩SDK示例<a name="fig1447325620456"></a>  
![](figures/解压缩SDK示例.png "解压缩SDK示例")

## 编译SDK（Cmake）<a name="ZH-CN_TOPIC_0000001700134488"></a>










### 编译方法<a name="ZH-CN_TOPIC_0000001748093877"></a>

根目录下执行“python3 build.py”指令运行脚本编译，即可编译出对应的SDK程序。编译命令列表如[表1](#table1646491114816)所示。

**表 1**  build.sh参数列表

<a name="table1646491114816"></a>
<table><thead align="left"><tr id="row44654114810"><th class="cellrowborder" valign="top" width="12.76%" id="mcps1.2.4.1.1"><p id="p194651412487"><a name="p194651412487"></a><a name="p194651412487"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="37.480000000000004%" id="mcps1.2.4.1.2"><p id="p6461872507"><a name="p6461872507"></a><a name="p6461872507"></a>示例</p>
</th>
<th class="cellrowborder" valign="top" width="49.76%" id="mcps1.2.4.1.3"><p id="p1246515144820"><a name="p1246515144820"></a><a name="p1246515144820"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row746513144812"><td class="cellrowborder" valign="top" width="12.76%" headers="mcps1.2.4.1.1 "><p id="p2465219482"><a name="p2465219482"></a><a name="p2465219482"></a>无</p>
</td>
<td class="cellrowborder" valign="top" width="37.480000000000004%" headers="mcps1.2.4.1.2 "><p id="p1215022142715"><a name="p1215022142715"></a><a name="p1215022142715"></a>python3 build.py ws63-liteos-app</p>
</td>
<td class="cellrowborder" valign="top" width="49.76%" headers="mcps1.2.4.1.3 "><p id="p144651117489"><a name="p144651117489"></a><a name="p144651117489"></a>启动ws63-liteos-app目标的增量编译。</p>
</td>
</tr>
<tr id="row04651218489"><td class="cellrowborder" valign="top" width="12.76%" headers="mcps1.2.4.1.1 "><p id="p24654119480"><a name="p24654119480"></a><a name="p24654119480"></a>-c</p>
</td>
<td class="cellrowborder" valign="top" width="37.480000000000004%" headers="mcps1.2.4.1.2 "><p id="p16463717500"><a name="p16463717500"></a><a name="p16463717500"></a>python3 build.py -c ws63-liteos-app</p>
</td>
<td class="cellrowborder" valign="top" width="49.76%" headers="mcps1.2.4.1.3 "><p id="p1046516134810"><a name="p1046516134810"></a><a name="p1046516134810"></a>启动ws63-liteos-app目标的全量编译。</p>
</td>
</tr>
<tr id="row11696675533"><td class="cellrowborder" valign="top" width="12.76%" headers="mcps1.2.4.1.1 "><p id="p206971172535"><a name="p206971172535"></a><a name="p206971172535"></a>menuconfig</p>
</td>
<td class="cellrowborder" valign="top" width="37.480000000000004%" headers="mcps1.2.4.1.2 "><p id="p1669710713535"><a name="p1669710713535"></a><a name="p1669710713535"></a>python3 build.py  ws63-liteos-app menuconfig</p>
</td>
<td class="cellrowborder" valign="top" width="49.76%" headers="mcps1.2.4.1.3 "><p id="p18697274534"><a name="p18697274534"></a><a name="p18697274534"></a>启动ws63-liteos-app目标的menuconfig图形配置界面。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  编译目标介绍

<a name="table16988747155411"></a>
<table><thead align="left"><tr id="row1898820470542"><th class="cellrowborder" valign="top" width="42.96%" id="mcps1.2.3.1.1"><p id="p69881047105420"><a name="p69881047105420"></a><a name="p69881047105420"></a>编译目标</p>
</th>
<th class="cellrowborder" valign="top" width="57.04%" id="mcps1.2.3.1.2"><p id="p898824715413"><a name="p898824715413"></a><a name="p898824715413"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row20988114713546"><td class="cellrowborder" valign="top" width="42.96%" headers="mcps1.2.3.1.1 "><p id="p1098804755419"><a name="p1098804755419"></a><a name="p1098804755419"></a>python3 build.py -c ws63-liteos-app</p>
</td>
<td class="cellrowborder" valign="top" width="57.04%" headers="mcps1.2.3.1.2 "><p id="p7989144715418"><a name="p7989144715418"></a><a name="p7989144715418"></a>app版本编译目标（自动包含flashboot编译）</p>
</td>
</tr>
<tr id="row14862228125611"><td class="cellrowborder" valign="top" width="42.96%" headers="mcps1.2.3.1.1 "><p id="p757993465615"><a name="p757993465615"></a><a name="p757993465615"></a>python3 build.py -c ws63-flashboot</p>
</td>
<td class="cellrowborder" valign="top" width="57.04%" headers="mcps1.2.3.1.2 "><p id="p4863132885617"><a name="p4863132885617"></a><a name="p4863132885617"></a>flashboot镜像编译目标</p>
</td>
</tr>
<tr id="row1798914714548"><td class="cellrowborder" valign="top" width="42.96%" headers="mcps1.2.3.1.1 "><p id="p59897476544"><a name="p59897476544"></a><a name="p59897476544"></a>python3 build.py -c ws63-liteos-xts</p>
</td>
<td class="cellrowborder" valign="top" width="57.04%" headers="mcps1.2.3.1.2 "><p id="p15989747145416"><a name="p15989747145416"></a><a name="p15989747145416"></a>openharmony xts认证版本编译目标（详情请参考“鸿蒙XTS认证指导书”）</p>
</td>
</tr>
<tr id="row2170141317581"><td class="cellrowborder" valign="top" width="42.96%" headers="mcps1.2.3.1.1 "><p id="p17171161316588"><a name="p17171161316588"></a><a name="p17171161316588"></a>python3 build.py -c ws63-liteos-app-iot</p>
</td>
<td class="cellrowborder" valign="top" width="57.04%" headers="mcps1.2.3.1.2 "><p id="p7171191365813"><a name="p7171191365813"></a><a name="p7171191365813"></a>Harmony connect版本编译目标（详情请参考“HiLink编译使用指南”）</p>
</td>
</tr>
<tr id="row398924715412"><td class="cellrowborder" valign="top" width="42.96%" headers="mcps1.2.3.1.1 "><p id="p2098914711549"><a name="p2098914711549"></a><a name="p2098914711549"></a>python3 build.py -c ws63-liteos-hilink</p>
</td>
<td class="cellrowborder" valign="top" width="57.04%" headers="mcps1.2.3.1.2 "><p id="p497215559918"><a name="p497215559918"></a><a name="p497215559918"></a>Harmony connect独立升级版本编译目标（详情请参考“HiLink编译使用指南”）</p>
</td>
</tr>
</tbody>
</table>

编译得到的烧录镜像在“output/ws63/fwpkg/ws63-liteos-app”目录下（如[表3](#table5535429403)所示）。

**表 3**  烧录镜像

<a name="table5535429403"></a>
<table><thead align="left"><tr id="row1353722184019"><th class="cellrowborder" valign="top" width="21.45%" id="mcps1.2.3.1.1"><p id="p97691634164219"><a name="p97691634164219"></a><a name="p97691634164219"></a>文件名</p>
</th>
<th class="cellrowborder" valign="top" width="78.55%" id="mcps1.2.3.1.2"><p id="p253772164019"><a name="p253772164019"></a><a name="p253772164019"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row75376234019"><td class="cellrowborder" valign="top" width="21.45%" headers="mcps1.2.3.1.1 "><p id="p1740682318402"><a name="p1740682318402"></a><a name="p1740682318402"></a>ws63-liteos-app_all.fwpkg</p>
</td>
<td class="cellrowborder" valign="top" width="78.55%" headers="mcps1.2.3.1.2 "><p id="p10903201011919"><a name="p10903201011919"></a><a name="p10903201011919"></a>空片烧录时，需要烧录此文件。包含了所有的需要升级的内容。包含：root_loaderboot_sign.bin、root_params.bin、flashboot_sign.bin、ws63_all_nv.bin、ws63-liteos-app-sign.bin。</p>
<p id="p7548577443"><a name="p7548577443"></a><a name="p7548577443"></a>各文件介绍如下：</p>
<p id="p195743193449"><a name="p195743193449"></a><a name="p195743193449"></a>root_loaderboot_sign.bin：loaderboot的镜像文件。升级开始时，芯片中固化的romboot会接收此镜像文件，加载到内存并运行，loadboot负责接收后续的镜像文件。注：此镜像只在升级阶段放在RAM中运行，并不存放在flash中。</p>
<p id="p5426121718517"><a name="p5426121718517"></a><a name="p5426121718517"></a>root_params.bin：flash分区信息的镜像文件。分区信息供romboot、loaderboot和flashboot使用。</p>
<p id="p205118146539"><a name="p205118146539"></a><a name="p205118146539"></a>flashboot_sign.bin：flashboot的镜像文件。</p>
<p id="p4313165175313"><a name="p4313165175313"></a><a name="p4313165175313"></a>ws63_all_nv.bin：参数区的镜像文件。</p>
<p id="p123521010192719"><a name="p123521010192719"></a><a name="p123521010192719"></a>ws63-liteos-app-sign.bin：版本镜像文件。</p>
</td>
</tr>
<tr id="row15537132114020"><td class="cellrowborder" valign="top" width="21.45%" headers="mcps1.2.3.1.1 "><p id="p20351123274017"><a name="p20351123274017"></a><a name="p20351123274017"></a>ws63-liteos-app_load_only.fwpkg</p>
</td>
<td class="cellrowborder" valign="top" width="78.55%" headers="mcps1.2.3.1.2 "><p id="p109031510191919"><a name="p109031510191919"></a><a name="p109031510191919"></a>版本升级打包文件，包含：root_loaderboot_sign.bin、ws63-liteos-app-sign.bin。不包含flashboot相关内容。</p>
<p id="p7556838163116"><a name="p7556838163116"></a><a name="p7556838163116"></a>当芯片烧录过“ws63-liteos-app_all.fwpkg”镜像后，如果后续修改不涉及root_params、flash_boot、nv的修改，则可以用此文件升级。</p>
</td>
</tr>
</tbody>
</table>

注：编译得到的中间文件在“output/ws63/acore/ws63-liteos-app”目录下。

### 编译参数详解<a name="ZH-CN_TOPIC_0000001882476501"></a>

编译命令接收参数及解释如[表1](#table36913222319)所示。

**表 1**  编译参数信息表

<a name="table36913222319"></a>
<table><thead align="left"><tr id="row205261342910"><th class="cellrowborder" valign="top" width="20.65%" id="mcps1.2.3.1.1"><p id="p6526649913"><a name="p6526649913"></a><a name="p6526649913"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="79.35%" id="mcps1.2.3.1.2"><p id="p135275415919"><a name="p135275415919"></a><a name="p135275415919"></a>参数信息</p>
</th>
</tr>
</thead>
<tbody><tr id="row1289314403919"><td class="cellrowborder" valign="top" width="20.65%" headers="mcps1.2.3.1.1 "><p id="p1089312404913"><a name="p1089312404913"></a><a name="p1089312404913"></a>-c</p>
</td>
<td class="cellrowborder" valign="top" width="79.35%" headers="mcps1.2.3.1.2 "><p id="p489312401193"><a name="p489312401193"></a><a name="p489312401193"></a>clean后编译</p>
</td>
</tr>
<tr id="row9701422193117"><td class="cellrowborder" valign="top" width="20.65%" headers="mcps1.2.3.1.1 "><p id="p157010226318"><a name="p157010226318"></a><a name="p157010226318"></a>-j</p>
</td>
<td class="cellrowborder" valign="top" width="79.35%" headers="mcps1.2.3.1.2 "><p id="p17092210313"><a name="p17092210313"></a><a name="p17092210313"></a>-j&lt;num&gt;，以num线程数执行编译，如-j16，-j8</p>
<p id="p194801456193213"><a name="p194801456193213"></a><a name="p194801456193213"></a>默认最大线程</p>
</td>
</tr>
<tr id="row270622113120"><td class="cellrowborder" valign="top" width="20.65%" headers="mcps1.2.3.1.1 "><p id="p327613157339"><a name="p327613157339"></a><a name="p327613157339"></a>-def=</p>
</td>
<td class="cellrowborder" valign="top" width="79.35%" headers="mcps1.2.3.1.2 "><p id="p570622133113"><a name="p570622133113"></a><a name="p570622133113"></a>-def=XXX,YYY,ZZZ=x,...  向本次编译target中添加XXX、YYY、ZZZ=x编译宏</p>
<p id="p687205165210"><a name="p687205165210"></a><a name="p687205165210"></a>可使用-def=-:XXX 来屏蔽XXX宏</p>
<p id="p7751710105219"><a name="p7751710105219"></a><a name="p7751710105219"></a>可使用-def=-:ZZZ=x 来添加或者修改ZZZ宏</p>
</td>
</tr>
<tr id="row5701722153118"><td class="cellrowborder" valign="top" width="20.65%" headers="mcps1.2.3.1.1 "><p id="p147012225311"><a name="p147012225311"></a><a name="p147012225311"></a>-component=</p>
</td>
<td class="cellrowborder" valign="top" width="79.35%" headers="mcps1.2.3.1.2 "><p id="p170922163112"><a name="p170922163112"></a><a name="p170922163112"></a>-component=XXX,YYY,...  仅编译XXX,YYY组件</p>
</td>
</tr>
<tr id="row12380153694910"><td class="cellrowborder" valign="top" width="20.65%" headers="mcps1.2.3.1.1 "><p id="p183801736124914"><a name="p183801736124914"></a><a name="p183801736124914"></a>-ninja</p>
</td>
<td class="cellrowborder" valign="top" width="79.35%" headers="mcps1.2.3.1.2 "><p id="p6744245135819"><a name="p6744245135819"></a><a name="p6744245135819"></a>使用ninja生成中间文件，默认使用Unix makefile</p>
</td>
</tr>
<tr id="row57010229319"><td class="cellrowborder" valign="top" width="20.65%" headers="mcps1.2.3.1.1 "><p id="p8706227315"><a name="p8706227315"></a><a name="p8706227315"></a>-[release / debug]</p>
</td>
<td class="cellrowborder" valign="top" width="79.35%" headers="mcps1.2.3.1.2 "><p id="p270112211312"><a name="p270112211312"></a><a name="p270112211312"></a>release:  在生成反汇编文件时节省时间</p>
<p id="p16649163112211"><a name="p16649163112211"></a><a name="p16649163112211"></a>debug:   在生成反汇编文件时信息更加全面但也更耗时</p>
<p id="p18807163914214"><a name="p18807163914214"></a><a name="p18807163914214"></a>默认为debug</p>
</td>
</tr>
<tr id="row187072220311"><td class="cellrowborder" valign="top" width="20.65%" headers="mcps1.2.3.1.1 "><p id="p770422133120"><a name="p770422133120"></a><a name="p770422133120"></a>-dump</p>
</td>
<td class="cellrowborder" valign="top" width="79.35%" headers="mcps1.2.3.1.2 "><p id="p4701622143115"><a name="p4701622143115"></a><a name="p4701622143115"></a>编译时在终端输出target的所有参数列表 -- 包括编译宏、组件、编译选项等</p>
</td>
</tr>
<tr id="row1170152283114"><td class="cellrowborder" valign="top" width="20.65%" headers="mcps1.2.3.1.1 "><p id="p5701222103111"><a name="p5701222103111"></a><a name="p5701222103111"></a>-nhso</p>
</td>
<td class="cellrowborder" valign="top" width="79.35%" headers="mcps1.2.3.1.2 "><p id="p117012221318"><a name="p117012221318"></a><a name="p117012221318"></a>不更新HSO数据库</p>
</td>
</tr>
<tr id="row87082218313"><td class="cellrowborder" valign="top" width="20.65%" headers="mcps1.2.3.1.1 "><p id="p1870102213317"><a name="p1870102213317"></a><a name="p1870102213317"></a>-out_libs</p>
</td>
<td class="cellrowborder" valign="top" width="79.35%" headers="mcps1.2.3.1.2 "><p id="p370192214311"><a name="p370192214311"></a><a name="p370192214311"></a>-out_libs=file_path, 不再链接成elf, 转而将所有.a打包成一个大的.a</p>
</td>
</tr>
<tr id="row57062212312"><td class="cellrowborder" valign="top" width="20.65%" headers="mcps1.2.3.1.1 "><p id="p1270182211310"><a name="p1270182211310"></a><a name="p1270182211310"></a>others</p>
</td>
<td class="cellrowborder" valign="top" width="79.35%" headers="mcps1.2.3.1.2 "><p id="p17019227313"><a name="p17019227313"></a><a name="p17019227313"></a>作为匹配编译target_names的关键字</p>
</td>
</tr>
</tbody>
</table>

### 编译选项详解<a name="ZH-CN_TOPIC_0000001885439217"></a>

WS63在不同目录下的.py文件下配置编译选项，如[表1](#table20340122212538)所示。

**表 1**  WS63通用组件编译选项

<a name="table20340122212538"></a>
<table><thead align="left"><tr id="row5340132295310"><th class="cellrowborder" align="center" valign="top" width="15.85%" id="mcps1.2.5.1.1"><p id="p1934072235311"><a name="p1934072235311"></a><a name="p1934072235311"></a>编译选项类型</p>
</th>
<th class="cellrowborder" align="center" valign="top" width="16.68%" id="mcps1.2.5.1.2"><p id="p9340122218538"><a name="p9340122218538"></a><a name="p9340122218538"></a>说明</p>
</th>
<th class="cellrowborder" align="center" valign="top" width="42.47%" id="mcps1.2.5.1.3"><p id="p12340192213533"><a name="p12340192213533"></a><a name="p12340192213533"></a>内容</p>
</th>
<th class="cellrowborder" align="center" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="p183401122125318"><a name="p183401122125318"></a><a name="p183401122125318"></a>对应文件控制路径</p>
</th>
</tr>
</thead>
<tbody><tr id="row1234092285320"><td class="cellrowborder" align="left" valign="top" width="15.85%" headers="mcps1.2.5.1.1 "><p id="p2034012205315"><a name="p2034012205315"></a><a name="p2034012205315"></a>common_ccflags</p>
</td>
<td class="cellrowborder" align="left" valign="top" width="16.68%" headers="mcps1.2.5.1.2 "><p id="p6340172295318"><a name="p6340172295318"></a><a name="p6340172295318"></a>基础编译选项</p>
</td>
<td class="cellrowborder" align="left" valign="top" width="42.47%" headers="mcps1.2.5.1.3 "><p id="p334017226530"><a name="p334017226530"></a><a name="p334017226530"></a>-std=gnu99 -Wall -Werror -Wextra -Winit-self -Wpointer-arith -Wstrict-prototypes -Wno-type-limits -fno-strict-aliasing -Os -fno-unwind-tables</p>
</td>
<td class="cellrowborder" align="left" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p33404224537"><a name="p33404224537"></a><a name="p33404224537"></a>\sdk\build\config\target_config\common_config.py</p>
</td>
</tr>
<tr id="row1534012216539"><td class="cellrowborder" align="left" valign="top" width="15.85%" headers="mcps1.2.5.1.1 "><p id="p63407223532"><a name="p63407223532"></a><a name="p63407223532"></a>riscv31</p>
</td>
<td class="cellrowborder" align="left" valign="top" width="16.68%" headers="mcps1.2.5.1.2 "><p id="p12340142265320"><a name="p12340142265320"></a><a name="p12340142265320"></a>芯片类型编译选项</p>
</td>
<td class="cellrowborder" align="left" valign="top" width="42.47%" headers="mcps1.2.5.1.3 "><p id="p18340922175319"><a name="p18340922175319"></a><a name="p18340922175319"></a>-ffreestanding -fdata-sections -Wno-implicit-fallthrough -ffunction-sections -nostdlib -pipe -fno-tree-scev-cprop -fno-common -mpush-pop -msmall-data-limit=0 -fno-ipa-ra -Wtrampolines -Wlogical-op -Wjump-misses-init -Wa,-enable-c-lbu-sb -Wa,-enable-c-lhu-sh -fimm-compare -femit-muliadd -fmerge-immshf -femit-uxtb-uxth -femit-lli -femit-clz -fldm-stm-optimize -g</p>
</td>
<td class="cellrowborder" align="left" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p17340422185318"><a name="p17340422185318"></a><a name="p17340422185318"></a>\sdk\build\config\target_config\common_config.py</p>
</td>
</tr>
<tr id="row143404224531"><td class="cellrowborder" align="left" valign="top" width="15.85%" headers="mcps1.2.5.1.1 "><p id="p16340152214539"><a name="p16340152214539"></a><a name="p16340152214539"></a>fp_flags</p>
</td>
<td class="cellrowborder" align="left" valign="top" width="16.68%" headers="mcps1.2.5.1.2 "><p id="p12340182275318"><a name="p12340182275318"></a><a name="p12340182275318"></a>硬浮点编译选项</p>
</td>
<td class="cellrowborder" align="left" valign="top" width="42.47%" headers="mcps1.2.5.1.3 "><p id="p113401322195311"><a name="p113401322195311"></a><a name="p113401322195311"></a>-march=rv32imfc -mabi=ilp32f</p>
</td>
<td class="cellrowborder" align="left" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p1734013223533"><a name="p1734013223533"></a><a name="p1734013223533"></a>\sdk\build\config\target_config\ws63\target_config.py</p>
</td>
</tr>
<tr id="row20340322205313"><td class="cellrowborder" align="left" valign="top" width="15.85%" headers="mcps1.2.5.1.1 "><p id="p334013226531"><a name="p334013226531"></a><a name="p334013226531"></a>codesize_flags</p>
</td>
<td class="cellrowborder" align="left" valign="top" width="16.68%" headers="mcps1.2.5.1.2 "><p id="p434012235317"><a name="p434012235317"></a><a name="p434012235317"></a>codesize优化选项</p>
</td>
<td class="cellrowborder" align="left" valign="top" width="42.47%" headers="mcps1.2.5.1.3 "><p id="p43401422205318"><a name="p43401422205318"></a><a name="p43401422205318"></a>--short-enums -madjust-regorder -madjust-const-cost -freorder-commu-args -fimm-compare-expand -frmv-str-zero -mfp-const-opt -mswitch-jump-table -frtl-sequence-abstract -frtl-hoist-sink -fsafe-alias-multipointer -finline-optimize-size -fmuliadd-expand -mlli-expand -Wa,-mcjal-expand -foptimize-reg-alloc -fsplit-multi-zero-assignments -floop-optimize-size -Wa,-mlli-relax -mpattern-abstract -foptimize-pro-and-epilogue</p>
</td>
<td class="cellrowborder" align="left" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p16341142215317"><a name="p16341142215317"></a><a name="p16341142215317"></a>\sdk\build\config\target_config\ws63\target_config.py</p>
</td>
</tr>
</tbody>
</table>

其中，编译选项的详细说明如[表2](#table5190336213)所示。

**表 2**  编译选项详细说明

<a name="table5190336213"></a>
<table><thead align="left"><tr id="row1319033617116"><th class="cellrowborder" valign="top" width="23.27%" id="mcps1.2.3.1.1"><p id="p17190636711"><a name="p17190636711"></a><a name="p17190636711"></a>选项</p>
</th>
<th class="cellrowborder" valign="top" width="76.73%" id="mcps1.2.3.1.2"><p id="p11190936911"><a name="p11190936911"></a><a name="p11190936911"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row71905363117"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p519017366118"><a name="p519017366118"></a><a name="p519017366118"></a>-std=gnu99</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p2019017361613"><a name="p2019017361613"></a><a name="p2019017361613"></a>使用 ISO C99 标准再加上GNU的扩展</p>
</td>
</tr>
<tr id="row12190103614112"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p2190236315"><a name="p2190236315"></a><a name="p2190236315"></a>-Wall</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p81901236214"><a name="p81901236214"></a><a name="p81901236214"></a>选项意思是编译后显示所有警告</p>
</td>
</tr>
<tr id="row101901836410"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p2190143614118"><a name="p2190143614118"></a><a name="p2190143614118"></a>-Werror</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p10190163611115"><a name="p10190163611115"></a><a name="p10190163611115"></a>用于将所有警告升级成错误</p>
</td>
</tr>
<tr id="row1119017361713"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p019020361111"><a name="p019020361111"></a><a name="p019020361111"></a>-Wextra</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p20190163620117"><a name="p20190163620117"></a><a name="p20190163620117"></a>用于开启额外的警告信息（-Wall的补充）</p>
</td>
</tr>
<tr id="row619013367118"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p019015360118"><a name="p019015360118"></a><a name="p019015360118"></a>-Winit-self</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p161900368113"><a name="p161900368113"></a><a name="p161900368113"></a>警告使用自己初始化的未初始化变量</p>
</td>
</tr>
<tr id="row719019363117"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p119014366115"><a name="p119014366115"></a><a name="p119014366115"></a>-Wpointer-arith</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p1619043612113"><a name="p1619043612113"></a><a name="p1619043612113"></a>警告任何取决于“功能类型”或“功能类型”的大小void</p>
</td>
</tr>
<tr id="row14190153619116"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p111901367112"><a name="p111901367112"></a><a name="p111901367112"></a>-Wstrict-prototypes</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p91904361411"><a name="p91904361411"></a><a name="p91904361411"></a>警告如果一个函数被声明或定义而没有指定参数类型</p>
</td>
</tr>
<tr id="row1291829722"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p109181091128"><a name="p109181091128"></a><a name="p109181091128"></a>-Wno-type-limits</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p09187916216"><a name="p09187916216"></a><a name="p09187916216"></a>屏蔽由于数据类型范围有限而导致比较始终为真或始终为false的告警</p>
</td>
</tr>
<tr id="row358111191722"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p1658117191624"><a name="p1658117191624"></a><a name="p1658117191624"></a>-fno-strict-aliasing</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p35814192026"><a name="p35814192026"></a><a name="p35814192026"></a>禁用 strict-aliasing优化规则：不同类型的指针绝对不会指向同一块内存区域</p>
</td>
</tr>
<tr id="row8581919926"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p205811519027"><a name="p205811519027"></a><a name="p205811519027"></a>-Os</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p95813198217"><a name="p95813198217"></a><a name="p95813198217"></a>专门优化目标文件大小，执行所有的不增加目标文件大小的-O2优化选项，同时-Os还会执行更加优化程序的选项</p>
</td>
</tr>
<tr id="row16125102412213"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p1412517241221"><a name="p1412517241221"></a><a name="p1412517241221"></a>-fno-unwind-tables</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p131259244217"><a name="p131259244217"></a><a name="p131259244217"></a>删除unwind调试信息</p>
</td>
</tr>
<tr id="row1012518241212"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p1312512243217"><a name="p1312512243217"></a><a name="p1312512243217"></a>-ffreestanding</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p612515241523"><a name="p612515241523"></a><a name="p612515241523"></a>断言编译发生在独立环境中</p>
</td>
</tr>
<tr id="row31251624524"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p13125152413211"><a name="p13125152413211"></a><a name="p13125152413211"></a>-fdata-sections</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p1512552417212"><a name="p1512552417212"></a><a name="p1512552417212"></a>将每个数据放入自己的部分（仅限ELF）</p>
</td>
</tr>
<tr id="row1912517240210"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p1112512417210"><a name="p1112512417210"></a><a name="p1112512417210"></a>-Wno-implicit-fallthrough</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p3125924522"><a name="p3125924522"></a><a name="p3125924522"></a>可忽略编译时switch-case中缺少break的错误</p>
</td>
</tr>
<tr id="row19253297218"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p32522910212"><a name="p32522910212"></a><a name="p32522910212"></a>-ffunction-sections</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p112517292029"><a name="p112517292029"></a><a name="p112517292029"></a>将每个函数放在自己的节中（仅限ELF）</p>
</td>
</tr>
<tr id="row0223859152611"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p12801157122614"><a name="p12801157122614"></a><a name="p12801157122614"></a>-nostdlib</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p580145752611"><a name="p580145752611"></a><a name="p580145752611"></a>关闭默认头文件与库文件搜索目录</p>
</td>
</tr>
<tr id="row82231359192613"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p480114577263"><a name="p480114577263"></a><a name="p480114577263"></a>-pipe</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p08011257132610"><a name="p08011257132610"></a><a name="p08011257132610"></a>编译过程中使用管道，借助 GCC 的管道功能来提高编译速度</p>
</td>
</tr>
<tr id="row4223185992617"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p780155782616"><a name="p780155782616"></a><a name="p780155782616"></a>-fno-tree-scev-cprop</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p11801115713269"><a name="p11801115713269"></a><a name="p11801115713269"></a>禁用标量演化信息进行复写传递，代码空间优化相关</p>
</td>
</tr>
<tr id="row42231559132610"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p38013573266"><a name="p38013573266"></a><a name="p38013573266"></a>-fno-common</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p12801195752613"><a name="p12801195752613"></a><a name="p12801195752613"></a>可以把静态库中的没有初始化的全局变量从弱符号变成强符号，当所有静态库链接成可执行文件，如果同时有两个以上“重名强符号”，链接器会报错。</p>
</td>
</tr>
<tr id="row222315919261"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p1280155716262"><a name="p1280155716262"></a><a name="p1280155716262"></a>-mpush-pop</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p10801457172618"><a name="p10801457172618"></a><a name="p10801457172618"></a>CodeSize 优化，改编译选项需要CPU版本支持push/pop/popret/lwm/swm 等指令</p>
</td>
</tr>
<tr id="row52231259132616"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p38011357172611"><a name="p38011357172611"></a><a name="p38011357172611"></a>-msmall-data-limit=0</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p13801105716267"><a name="p13801105716267"></a><a name="p13801105716267"></a>CodeSize 优化，改编译选项需要CPU版本支持push/pop/popret/lwm/swm 等指令</p>
</td>
</tr>
<tr id="row1422318591268"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p7801115712618"><a name="p7801115712618"></a><a name="p7801115712618"></a>-fno-ipa-ra</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p480110573264"><a name="p480110573264"></a><a name="p480110573264"></a>禁用编译器针对叶子函数的编译优化（编译选项中添加了-O2优化选项时-fipa-ra参数导致）</p>
</td>
</tr>
<tr id="row112231859142619"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p2801135712616"><a name="p2801135712616"></a><a name="p2801135712616"></a>-Wtrampolines</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p118011557192613"><a name="p118011557192613"></a><a name="p118011557192613"></a>该选项用于检查代码中是否包含内嵌函数,gcc对内嵌函数有个专门的称呼:trampoline</p>
</td>
</tr>
<tr id="row822315592263"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p58016579269"><a name="p58016579269"></a><a name="p58016579269"></a>-Wlogical-op</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p2801185716269"><a name="p2801185716269"></a><a name="p2801185716269"></a>当逻辑操作结果似乎总为真或假时给出警告</p>
</td>
</tr>
<tr id="row3223159172612"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p98011357202616"><a name="p98011357202616"></a><a name="p98011357202616"></a>-Wjump-misses-init</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p4801657112615"><a name="p4801657112615"></a><a name="p4801657112615"></a>switch或者goto语句后声明并且初始化变量，进行告警。</p>
</td>
</tr>
<tr id="row16223205992611"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p2801857142613"><a name="p2801857142613"></a><a name="p2801857142613"></a>-Wa,-enable-c-lbu-sb</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p980145762611"><a name="p980145762611"></a><a name="p980145762611"></a>汇编器优化，默认禁用。如果启用此优化，汇编器将使用压缩的 lbu &amp; sb 来替换 lbu &amp; sb。</p>
</td>
</tr>
<tr id="row12223145992612"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p68019577260"><a name="p68019577260"></a><a name="p68019577260"></a>-Wa,-enable-c-lhu-sh</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p780116576263"><a name="p780116576263"></a><a name="p780116576263"></a>汇编器优化，默认禁用。如果启用此优化，汇编器将使用压缩的 lhu &amp; sh 来替换 lhu &amp; sh</p>
</td>
</tr>
<tr id="row922385932618"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p8801125718262"><a name="p8801125718262"></a><a name="p8801125718262"></a>-fimm-compare</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p1380125712267"><a name="p1380125712267"></a><a name="p1380125712267"></a>代码大小的优化，可以将非零立即比较的两条指令（li，bxx）合并到一条指令（bxxi）</p>
</td>
</tr>
<tr id="row17223155916263"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p138011557162617"><a name="p138011557162617"></a><a name="p138011557162617"></a>-femit-muliadd</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p780114574266"><a name="p780114574266"></a><a name="p780114574266"></a>CodeSize优化，可以将多条加树指令合并为一条指令</p>
</td>
</tr>
<tr id="row14223145912610"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p118019579264"><a name="p118019579264"></a><a name="p118019579264"></a>-fmerge-immshf</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p580113575267"><a name="p580113575267"></a><a name="p580113575267"></a>CodeSize优化，可以将立即移位合并为一条指令。组合仅在 -O1 以上的选项中生效</p>
</td>
</tr>
<tr id="row122385912614"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p78011257162613"><a name="p78011257162613"></a><a name="p78011257162613"></a>-femit-uxtb-uxth</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p9802195772618"><a name="p9802195772618"></a><a name="p9802195772618"></a>CodeSize优化，将无符号扩展字节和无符号扩展半字优化为uxtb、uxth(16字节)。组合仅在 -O1 以上的选项中</p>
</td>
</tr>
<tr id="row10223135911262"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p12802175762619"><a name="p12802175762619"></a><a name="p12802175762619"></a>-femit-lli</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p15802135719269"><a name="p15802135719269"></a><a name="p15802135719269"></a>使用48位l.li指令代替64位指令lui + addi进行32位长立即加载。此优化与insn组合结合使用</p>
</td>
</tr>
<tr id="row522395932614"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p16802155782615"><a name="p16802155782615"></a><a name="p16802155782615"></a>-femit-clz</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p158021557112619"><a name="p158021557112619"></a><a name="p158021557112619"></a>支持CLZ指令，所有__builtin_clz函数的调用都会优化为CLZ指令。</p>
</td>
</tr>
<tr id="row152221259102610"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p1180215717269"><a name="p1180215717269"></a><a name="p1180215717269"></a>-fldm-stm-optimize</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p58021057112610"><a name="p58021057112610"></a><a name="p58021057112610"></a>启用用 ldmia/stmia 替换连续 WORD 加载/存储的优化，默认禁用。</p>
</td>
</tr>
<tr id="row19222195911264"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p18802205772613"><a name="p18802205772613"></a><a name="p18802205772613"></a>-g</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p1580235712268"><a name="p1580235712268"></a><a name="p1580235712268"></a>调试编译选项，对于可执行二进制文件可通过以下方法确定是否包含调试信息</p>
</td>
</tr>
<tr id="row8222125972613"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p13802105710266"><a name="p13802105710266"></a><a name="p13802105710266"></a>-mabi=ilp32f</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p880285711261"><a name="p880285711261"></a><a name="p880285711261"></a>支持硬浮点（指定整数和浮点调用约定）</p>
</td>
</tr>
<tr id="row62223599267"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p11802125782610"><a name="p11802125782610"></a><a name="p11802125782610"></a>-march=rv32imfc</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p180225712268"><a name="p180225712268"></a><a name="p180225712268"></a>支持硬浮点（为给定的 RISC-V ISA 生成代码）</p>
</td>
</tr>
<tr id="row022216592268"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p11802115732615"><a name="p11802115732615"></a><a name="p11802115732615"></a>--short-enums</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p1280214577264"><a name="p1280214577264"></a><a name="p1280214577264"></a>CodeSize 优化，enum类型等于大小足够的最小整数类型</p>
</td>
</tr>
<tr id="row17222125972619"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p880285772615"><a name="p880285772615"></a><a name="p880285772615"></a>-madjust-regorder</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p4802125762613"><a name="p4802125762613"></a><a name="p4802125762613"></a>寄存器分配优化-寄存器分配顺序调整优化</p>
</td>
</tr>
<tr id="row42226593261"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p10802657102615"><a name="p10802657102615"></a><a name="p10802657102615"></a>-madjust-const-cost</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p5802957152612"><a name="p5802957152612"></a><a name="p5802957152612"></a>立即数重复加载优化</p>
</td>
</tr>
<tr id="row7222165972619"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p580265710263"><a name="p580265710263"></a><a name="p580265710263"></a>-freorder-commu-args</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p2802105719261"><a name="p2802105719261"></a><a name="p2802105719261"></a>浮点运算可交换操作数优化</p>
</td>
</tr>
<tr id="row1922295942618"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p128021057142619"><a name="p128021057142619"></a><a name="p128021057142619"></a>-fimm-compare-expand</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p13802057142614"><a name="p13802057142614"></a><a name="p13802057142614"></a>扩展指令常量比较指令优化</p>
</td>
</tr>
<tr id="row62227599269"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p198027575269"><a name="p198027575269"></a><a name="p198027575269"></a>-frmv-str-zero</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p180218572268"><a name="p180218572268"></a><a name="p180218572268"></a>rodata段常量字符串对齐优化</p>
</td>
</tr>
<tr id="row20222125911269"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p2802125702610"><a name="p2802125702610"></a><a name="p2802125702610"></a>-mfp-const-opt</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p48021057162618"><a name="p48021057162618"></a><a name="p48021057162618"></a>浮点常量加载优化</p>
</td>
</tr>
<tr id="row322255912263"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p680220574266"><a name="p680220574266"></a><a name="p680220574266"></a>-mswitch-jump-table</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p1080265714268"><a name="p1080265714268"></a><a name="p1080265714268"></a>switch   case跳转表优化</p>
</td>
</tr>
<tr id="row1422275916266"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p15802165792620"><a name="p15802165792620"></a><a name="p15802165792620"></a>-frtl-sequence-abstract</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p880218573260"><a name="p880218573260"></a><a name="p880218573260"></a>函数内过程优化</p>
</td>
</tr>
<tr id="row192221459112614"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p198020573260"><a name="p198020573260"></a><a name="p198020573260"></a>-frtl-hoist-sink</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p980255715269"><a name="p980255715269"></a><a name="p980255715269"></a>代码移动优化</p>
</td>
</tr>
<tr id="row22221659202615"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p780210572261"><a name="p780210572261"></a><a name="p780210572261"></a>-fsafe-alias-multipointer</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p14802155772613"><a name="p14802155772613"></a><a name="p14802155772613"></a>多级指针重复加载优化</p>
</td>
</tr>
<tr id="row52220590262"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p580275711266"><a name="p580275711266"></a><a name="p580275711266"></a>-finline-optimize-size</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p9802115722617"><a name="p9802115722617"></a><a name="p9802115722617"></a>inline内联代价模型优化</p>
</td>
</tr>
<tr id="row422255910268"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p10802165719268"><a name="p10802165719268"></a><a name="p10802165719268"></a>-fmuliadd-expand</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p15802125772620"><a name="p15802125772620"></a><a name="p15802125772620"></a>扩展指令乘加指令优化（muliadd优化）</p>
</td>
</tr>
<tr id="row1022225915261"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p8802185715264"><a name="p8802185715264"></a><a name="p8802185715264"></a>-mlli-expand</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p12802175712265"><a name="p12802175712265"></a><a name="p12802175712265"></a>扩展指令l.li指令优化</p>
</td>
</tr>
<tr id="row162222059172614"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p2802195718261"><a name="p2802195718261"></a><a name="p2802195718261"></a>-Wa,-mcjal-expand</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p1980295715265"><a name="p1980295715265"></a><a name="p1980295715265"></a>汇编器上jal压缩指令优化</p>
</td>
</tr>
<tr id="row9222135920268"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p680211576264"><a name="p680211576264"></a><a name="p680211576264"></a>-foptimize-reg-alloc</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p1980295717268"><a name="p1980295717268"></a><a name="p1980295717268"></a>寄存器分配优化-寄存器分配优先级调整优化</p>
</td>
</tr>
<tr id="row102227596265"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p18802175792618"><a name="p18802175792618"></a><a name="p18802175792618"></a>-fsplit-multi-zero-assignments</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p8802125715260"><a name="p8802125715260"></a><a name="p8802125715260"></a>连续赋零值优化</p>
</td>
</tr>
<tr id="row18222135932618"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p1280225714264"><a name="p1280225714264"></a><a name="p1280225714264"></a>-floop-optimize-size</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p13802165792611"><a name="p13802165792611"></a><a name="p13802165792611"></a>循环结构优化</p>
</td>
</tr>
<tr id="row622212598262"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p3802957102613"><a name="p3802957102613"></a><a name="p3802957102613"></a>-Wa,-mlli-relax</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p480225719264"><a name="p480225719264"></a><a name="p480225719264"></a>高频立即数加载优化（汇编器和链接器协同优化）</p>
</td>
</tr>
<tr id="row19222459132612"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p6802657122612"><a name="p6802657122612"></a><a name="p6802657122612"></a>-mpattern-abstract</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p1280375711260"><a name="p1280375711260"></a><a name="p1280375711260"></a>过程间抽象优化(根据已知pattern抽象优化)</p>
</td>
</tr>
<tr id="row112215590261"><td class="cellrowborder" valign="top" width="23.27%" headers="mcps1.2.3.1.1 "><p id="p38031257182617"><a name="p38031257182617"></a><a name="p38031257182617"></a>-foptimize-pro-and-epilogue</p>
</td>
<td class="cellrowborder" valign="top" width="76.73%" headers="mcps1.2.3.1.2 "><p id="p108038575262"><a name="p108038575262"></a><a name="p108038575262"></a>函数prologue和epilogue优化</p>
</td>
</tr>
</tbody>
</table>

### 新增链接外部静态库<a name="ZH-CN_TOPIC_0000002188128385"></a>

链接外部静态库，请参考sdk\\application\\samples\\wifi\\ohos\_connect\\CMakeLists.txt中引用1个外部静态库文件hilinkbtsdk.a的示例：

```
set(COMPONENT_NAME "hilinkbtsdk")
set(LIBS ${ROOT_DIR}/application/samples/wifi/libhilink/lib${COMPONENT_NAME}.a)
set(WHOLE_LINK
true
)
build_component()
```

然后在sdk\\build\\config\\target\_config\\ws63\\config.py 中对应的编译target中添加'hilinkbtsdk'。

### 添加bin文件编译<a name="ZH-CN_TOPIC_0000001872047653"></a>

当前ws63编译ws63-liteos-app\_all.fwpkg时，默认编译root\_loaderboot\_sign.bin、root\_params.bin、flashboot\_sign.bin、ws63\_all\_nv.bin、ws63-liteos-app-sign.bin，文件介绍如[表3](编译方法.md#table5535429403)所示。

若需编译其他bin文件，可按如下步骤添加：

1.  在根路径下打开/tools/pkg/chip\_packet/ws63/packet.py文件
2.  在make\_all\_in\_one\_packet函数中添加代码，如[图1](#fig951946172817)所示

    **图 1**  文件路径和函数<a name="fig951946172817"></a>  
    ![](figures/文件路径和函数.png "文件路径和函数")

3.  在函数中添加bin文件路径

    其中每个拼接字符串代表一层目录（文件夹名和文件名）

    最终拼接完成的test\_add\_bin实际值：sdk\\interim\_binary\\ws63\\bin\\rom\_bin\\pke\_rom.bin

    **图 2**  bin文件路径图<a name="fig1516335210303"></a>  
    ![](figures/bin文件路径图.png "bin文件路径图")

4.  设置编译参数，参数之间用"|"分割

    **图 3**  bin文件编译参数<a name="fig1499631915318"></a>  
    ![](figures/bin文件编译参数.png "bin文件编译参数")

    1. 烧录位置，单板剩余地址可在sdk\\build\\config\\target\_config\\ws63\\param\_sector\\param\_sector.json文件中查看

    2. 占用空间大小

    3. 文件类型，0代表loader，1代表普通烧写文件，3是efuse，4是otp

    **图 4**  单板剩余地址<a name="fig15441153283112"></a>  
    ![](figures/单板剩余地址.png "单板剩余地址")

5.  在函数末尾，将设置好编译参数和路径的变量添加到编译列表

    **图 5**  添加路径到编译列表<a name="fig43951143193119"></a>  
    ![](figures/添加路径到编译列表.png "添加路径到编译列表")

6.  编译结果展示

    **图 6**  编译结果<a name="fig64574383219"></a>  
    ![](figures/编译结果.png "编译结果")

>![](public_sys-resources/icon-note.gif) **说明：** 
>如果新增的bin文件有通过OTA升级的需求，请参见《WS63V100 FOTA 开发指南》中对应内容，适配新增bin文件的OTA升级支持。

### Flash分区表配置<a name="ZH-CN_TOPIC_0000001883133629"></a>

分区表配置文件路径sdk\\build\\config\\target\_config\\ws63\\param\_sector\\param\_sector.json

![](figures/zh-cn_image_0000001883312345.png)

>![](public_sys-resources/icon-note.gif) **说明：** 
>上图内容仅作文件内容说明，具体分区信息请参考《WS63V100 FOTA 开发指南》“4.2 注意事项”章节中分区信息。
>分区表ID限制16个分区数量，默认Flash共4M大小，预留6个分区ID，可通过uapi\_partition\_get\_info接口传入分区ID获取对应地址和长度。

根据当前Flash分区方案，Flash划分情况如[图1](#fig01859287557)所示。

**图 1**  Flash划分情况<a name="fig01859287557"></a>  

![](figures/zh-cn_image_0000002089281337.png)

调整分区时，需要遵守以下原则：

1.  地址段0x000000\~0x030000为不可调整区，**任何改动都可能会导致无法启动而变砖**。
2.  地址段0x030000\~0x270000为APP镜像区，地址段信息来源于分区表中ID为0x20对应的APP镜像区，**该地址段仅支持调整分区大小，分区首地址不支持调整**，调整分区首地址同样会导致无法启动。
3.  地址段0x270000\~0x3F3000为FOTA镜像区，地址段信息来源于分区表中ID为0x21对应的压缩分区/OTA升级分区/产测镜像分区/B面分区，**该分区首地址必须为APP镜像区的结束地址**；**使用压缩升级方案时，该分区大小至少配置为APP镜像区大小的0.7倍及以上**。
4.  地址段0x3F3000\~0x3FB000为预留分区，该分区首地址为FOTA镜像区的结束地址，支持划分为4个不同分区ID的单独分区。
5.  地址段0x3FB000\~0x400000为其他功能区，包括死机信息\(0x11\)以及NV分区\(0x10\)，**该分区不支持调整**。
6.  调整分区时，除不可调整区以外，其他分区的首地址以及大小均要4K对齐。

调整分区示例：

根据分区表中分区信息，想要调整分区ID为0x30的预留分区的大小。

1.  假设在APP镜像分区有8K的空间余量，则调整APP镜像区的大小，减少8K，分区结束地址同时减少8K，同步修改文件drivers/boards/ws63/evb/memory\_config/include/memory\_config\_common.h，将文件中宏APP\_PROGRAM\_LENGTH的值\(默认为'\(0x240000 - 0x000300\)'\)修改为调整后分区大小\(如'\(0x23E000 - 0x000300\)'\)。
2.  FOTA分区受APP镜像区的影响，首地址前移8K，调整为0x26E000，压缩升级方案中FOTA分区大小需同步调整，4K对齐后为减少4K，最终FOTA分区结束地址为0x3F0000。
3.  APP镜像分区以及FOTA分区总计调整出12K的余量，可合并到预留分区中，预留分区首地址前移12K，为0x3F0000，同时大小增加12K，结束地址保持为0x3FB000。

### Menuconfig配置<a name="ZH-CN_TOPIC_0000001748014029"></a>

运行“python3 build.py -c ws63-liteos-app menuconfig”脚本会启动Menuconfig程序，用户可通过Menuconfig对编译和系统功能进行配置，如[图1](#fig155343385597)所示。

SDK集成了默认配置，但建议用户首次运行时进行相应配置，从而减少因为配置原因引起的问题。用户随时可以运行“python3 build.py -c ws63-liteos-app menuconfig”更改配置。

**图 1**  Menuconfig运行界面<a name="fig155343385597"></a>  
![](figures/Menuconfig运行界面.png "Menuconfig运行界面")

注：界面如存在差异，以实际版本为准。

Menuconfig操作说明如[表1](#table364152210248)所示，在Menuconfig界面中可输入快捷键进行配置。

**表 1**  Menuconfig常用操作命令

<a name="table364152210248"></a>
<table><thead align="left"><tr id="row2642122213247"><th class="cellrowborder" valign="top" width="17%" id="mcps1.2.3.1.1"><p id="p10343125916259"><a name="p10343125916259"></a><a name="p10343125916259"></a>快捷键</p>
</th>
<th class="cellrowborder" valign="top" width="83%" id="mcps1.2.3.1.2"><p id="p0642102212419"><a name="p0642102212419"></a><a name="p0642102212419"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row146421622162417"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.2.3.1.1 "><p id="p66421322192415"><a name="p66421322192415"></a><a name="p66421322192415"></a>空格、回车</p>
</td>
<td class="cellrowborder" valign="top" width="83%" headers="mcps1.2.3.1.2 "><p id="p464282282416"><a name="p464282282416"></a><a name="p464282282416"></a>选中，反选。</p>
</td>
</tr>
<tr id="row0235155732813"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.2.3.1.1 "><p id="p123512571284"><a name="p123512571284"></a><a name="p123512571284"></a>ESC</p>
</td>
<td class="cellrowborder" valign="top" width="83%" headers="mcps1.2.3.1.2 "><p id="p4235125718282"><a name="p4235125718282"></a><a name="p4235125718282"></a>返回上级菜单，退出界面。</p>
</td>
</tr>
<tr id="row1425985152914"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.2.3.1.1 "><p id="p02597515295"><a name="p02597515295"></a><a name="p02597515295"></a>Q</p>
</td>
<td class="cellrowborder" valign="top" width="83%" headers="mcps1.2.3.1.2 "><p id="p1825945119290"><a name="p1825945119290"></a><a name="p1825945119290"></a>退出界面。</p>
</td>
</tr>
<tr id="row161871942143019"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.2.3.1.1 "><p id="p718744220300"><a name="p718744220300"></a><a name="p718744220300"></a>S</p>
</td>
<td class="cellrowborder" valign="top" width="83%" headers="mcps1.2.3.1.2 "><p id="p1818734211305"><a name="p1818734211305"></a><a name="p1818734211305"></a>保存配置。</p>
</td>
</tr>
<tr id="row1661115053113"><td class="cellrowborder" valign="top" width="17%" headers="mcps1.2.3.1.1 "><p id="p1861165015311"><a name="p1861165015311"></a><a name="p1861165015311"></a>F</p>
</td>
<td class="cellrowborder" valign="top" width="83%" headers="mcps1.2.3.1.2 "><p id="p17611125012312"><a name="p17611125012312"></a><a name="p17611125012312"></a>显示帮助菜单。</p>
</td>
</tr>
</tbody>
</table>

所有命令可在Menuconfig界面的下方查看Menuconfig官方说明解释，如[图2](#fig14504171214012)所示。

**图 2**  Menuconfig命令帮助栏<a name="fig14504171214012"></a>  
![](figures/Menuconfig命令帮助栏.png "Menuconfig命令帮助栏")

**表 2**  Menuconfig菜单项说明

<a name="table111109185019"></a>
<table><thead align="left"><tr id="row31102115020"><th class="cellrowborder" valign="top" width="28.48%" id="mcps1.2.3.1.1"><p id="p1511071155010"><a name="p1511071155010"></a><a name="p1511071155010"></a>菜单</p>
</th>
<th class="cellrowborder" valign="top" width="71.52%" id="mcps1.2.3.1.2"><p id="p1511016155013"><a name="p1511016155013"></a><a name="p1511016155013"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row171109113504"><td class="cellrowborder" valign="top" width="28.48%" headers="mcps1.2.3.1.1 "><p id="p21103135016"><a name="p21103135016"></a><a name="p21103135016"></a>Targets</p>
</td>
<td class="cellrowborder" valign="top" width="71.52%" headers="mcps1.2.3.1.2 "><p id="p171101818501"><a name="p171101818501"></a><a name="p171101818501"></a>编译target相关配置。</p>
</td>
</tr>
<tr id="row211021195018"><td class="cellrowborder" valign="top" width="28.48%" headers="mcps1.2.3.1.1 "><p id="p1311017165013"><a name="p1311017165013"></a><a name="p1311017165013"></a>Application</p>
</td>
<td class="cellrowborder" valign="top" width="71.52%" headers="mcps1.2.3.1.2 "><p id="p31103195010"><a name="p31103195010"></a><a name="p31103195010"></a>应用相关配置（主要是sample相关）。</p>
</td>
</tr>
<tr id="row161103165010"><td class="cellrowborder" valign="top" width="28.48%" headers="mcps1.2.3.1.1 "><p id="p01104117505"><a name="p01104117505"></a><a name="p01104117505"></a>Bootloader</p>
</td>
<td class="cellrowborder" valign="top" width="71.52%" headers="mcps1.2.3.1.2 "><p id="p121105116505"><a name="p121105116505"></a><a name="p121105116505"></a>boot相关配置。</p>
</td>
</tr>
<tr id="row1211071175013"><td class="cellrowborder" valign="top" width="28.48%" headers="mcps1.2.3.1.1 "><p id="p8110141155011"><a name="p8110141155011"></a><a name="p8110141155011"></a>Drivers</p>
</td>
<td class="cellrowborder" valign="top" width="71.52%" headers="mcps1.2.3.1.2 "><p id="p11692144810534"><a name="p11692144810534"></a><a name="p11692144810534"></a>外设驱动相关配置和板级相关配置。</p>
</td>
</tr>
<tr id="row1511031125010"><td class="cellrowborder" valign="top" width="28.48%" headers="mcps1.2.3.1.1 "><p id="p141101111500"><a name="p141101111500"></a><a name="p141101111500"></a>Kernel</p>
</td>
<td class="cellrowborder" valign="top" width="71.52%" headers="mcps1.2.3.1.2 "><p id="p15798145395311"><a name="p15798145395311"></a><a name="p15798145395311"></a>内核相关配置。</p>
</td>
</tr>
<tr id="row161106115504"><td class="cellrowborder" valign="top" width="28.48%" headers="mcps1.2.3.1.1 "><p id="p91105115502"><a name="p91105115502"></a><a name="p91105115502"></a>Middleware</p>
</td>
<td class="cellrowborder" valign="top" width="71.52%" headers="mcps1.2.3.1.2 "><p id="p199165471300"><a name="p199165471300"></a><a name="p199165471300"></a>中间件（NV、FOTA、AT、DFX、PM等）相关配置。</p>
</td>
</tr>
<tr id="row94651621525"><td class="cellrowborder" valign="top" width="28.48%" headers="mcps1.2.3.1.1 "><p id="p8671200105210"><a name="p8671200105210"></a><a name="p8671200105210"></a>Protocol</p>
</td>
<td class="cellrowborder" valign="top" width="71.52%" headers="mcps1.2.3.1.2 "><p id="p8251182085411"><a name="p8251182085411"></a><a name="p8251182085411"></a>WiFi、蓝牙相关配置。</p>
</td>
</tr>
<tr id="row1453145185216"><td class="cellrowborder" valign="top" width="28.48%" headers="mcps1.2.3.1.1 "><p id="p1760612418522"><a name="p1760612418522"></a><a name="p1760612418522"></a>Test</p>
</td>
<td class="cellrowborder" valign="top" width="71.52%" headers="mcps1.2.3.1.2 "><p id="p1960613475215"><a name="p1960613475215"></a><a name="p1960613475215"></a>testsuite相关配置。</p>
</td>
</tr>
</tbody>
</table>

### UART配置方法<a name="ZH-CN_TOPIC_0000001964047897"></a>

WS63芯片总共有3个UART，SDK默认配置如下。

<a name="table179737495303"></a>
<table><thead align="left"><tr id="row12271050193018"><th class="cellrowborder" valign="top" width="20.61%" id="mcps1.1.4.1.1"><p id="p4274501303"><a name="p4274501303"></a><a name="p4274501303"></a>uart序号</p>
</th>
<th class="cellrowborder" valign="top" width="29.310000000000002%" id="mcps1.1.4.1.2"><p id="p182755019307"><a name="p182755019307"></a><a name="p182755019307"></a>波特率</p>
</th>
<th class="cellrowborder" valign="top" width="50.080000000000005%" id="mcps1.1.4.1.3"><p id="p20272509302"><a name="p20272509302"></a><a name="p20272509302"></a>用途</p>
</th>
</tr>
</thead>
<tbody><tr id="row8279503302"><td class="cellrowborder" valign="top" width="20.61%" headers="mcps1.1.4.1.1 "><p id="p11274502303"><a name="p11274502303"></a><a name="p11274502303"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="29.310000000000002%" headers="mcps1.1.4.1.2 "><p id="p327850183010"><a name="p327850183010"></a><a name="p327850183010"></a>115200</p>
</td>
<td class="cellrowborder" valign="top" width="50.080000000000005%" headers="mcps1.1.4.1.3 "><p id="p1227450143011"><a name="p1227450143011"></a><a name="p1227450143011"></a>debug调试打印/烧录/AT。</p>
</td>
</tr>
<tr id="row182785073016"><td class="cellrowborder" valign="top" width="20.61%" headers="mcps1.1.4.1.1 "><p id="p92715013015"><a name="p92715013015"></a><a name="p92715013015"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="29.310000000000002%" headers="mcps1.1.4.1.2 "><p id="p927195023011"><a name="p927195023011"></a><a name="p927195023011"></a>921600</p>
</td>
<td class="cellrowborder" valign="top" width="50.080000000000005%" headers="mcps1.1.4.1.3 "><p id="p7272050103014"><a name="p7272050103014"></a><a name="p7272050103014"></a>debugkits调试口。</p>
</td>
</tr>
<tr id="row1327155093018"><td class="cellrowborder" valign="top" width="20.61%" headers="mcps1.1.4.1.1 "><p id="p152735013013"><a name="p152735013013"></a><a name="p152735013013"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="29.310000000000002%" headers="mcps1.1.4.1.2 "><p id="p15271150183018"><a name="p15271150183018"></a><a name="p15271150183018"></a>115200</p>
</td>
<td class="cellrowborder" valign="top" width="50.080000000000005%" headers="mcps1.1.4.1.3 "><p id="p627750153017"><a name="p627750153017"></a><a name="p627750153017"></a>空闲，暂未使用。</p>
</td>
</tr>
</tbody>
</table>

烧录功能，固定用uart0，不可更改。

debug调试打印/AT/debugkits调试口，可通过menuconfig进行配置，以便适应不同硬件板级uart连接，波特率也可以通过menuconfig进行定制。

menuconfig的配置路径为Drivers-\>Chips-\>Chip Configurations for ws63：

![](figures/zh-cn_image_0000001937101784.png)

>![](public_sys-resources/icon-notice.gif) **须知：** 
>1.  debugkits调试口不能与其他功能共用同一个UART口，必须独占。
>2.  UART波特率建议配置典型值，如115200/921600/1M等，考虑到兼容性，不建议配置不常用的特殊值，比如115623此类波特率值。
>3.  修改UART序号请慎重，必须要与板级硬件工程师确认uart硬件连接，确保软件配置与硬件板级的实际电路连接是对得上的，否则无法正常工作。

### 注意事项<a name="ZH-CN_TOPIC_0000001700134460"></a>

-   如果执行“./build.py”提示无权限，可执行命令“chmod +x build.py”添加执行权限或执行“python3./build.py”。
-   编译过程中，报错找不到某个包，请检查环境中的python是否已经安装了相应组件。如果构建环境中包含多个python，特别是多个同版本的python，而用户无法辨认正在使用的是其中的哪个版本，此情况下，在安装python组件包时，推荐使用组件包源码进行安装。
-   系统优先使用用户通过Menuconfig所做的配置，如果用户未配置，系统将使用默认配置进行编译。

# 新建APP<a name="ZH-CN_TOPIC_0000001748014009"></a>




## 建立源码目录<a name="ZH-CN_TOPIC_0000001700134428"></a>

>![](public_sys-resources/icon-note.gif) **说明：** 
>用户可在“application/ws63”同级目录下参考“ws63\_liteos\_application”目录建立app，以下均以建立“my\_demo”为例。

步骤如下：

1.  新建“application/ws63/my\_demo”目录，用来存放“my\_demo”的源文件。
2.  复制“application/ws63/ws63\_liteos\_application/CMakeLists.txt”到“application/ws63/my\_demo/CmakeLists.txt”，并将源文件放在“application/ws63/my\_demo”目录下。
3.  修改“application/ws63/my\_demo/CmakeLists.txt”文件。其中各个变量的含义如[表1](#table89969106362)所示。

    **表 1**  组件的CmakeLists.txt中的变量含义

    <a name="table89969106362"></a>
    <table><thead align="left"><tr id="row69971710143612"><th class="cellrowborder" valign="top" width="27.900000000000002%" id="mcps1.2.3.1.1"><p id="p8997181043611"><a name="p8997181043611"></a><a name="p8997181043611"></a>变量名称</p>
    </th>
    <th class="cellrowborder" valign="top" width="72.1%" id="mcps1.2.3.1.2"><p id="p1799771019361"><a name="p1799771019361"></a><a name="p1799771019361"></a>变量含义</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row77088920486"><td class="cellrowborder" valign="top" width="27.900000000000002%" headers="mcps1.2.3.1.1 "><p id="p167083915484"><a name="p167083915484"></a><a name="p167083915484"></a>COMPONENT_NAME</p>
    </td>
    <td class="cellrowborder" valign="top" width="72.1%" headers="mcps1.2.3.1.2 "><p id="p570849194814"><a name="p570849194814"></a><a name="p570849194814"></a>当前组件名称，如“my_demo”。</p>
    </td>
    </tr>
    <tr id="row99971710133619"><td class="cellrowborder" valign="top" width="27.900000000000002%" headers="mcps1.2.3.1.1 "><p id="p199976101361"><a name="p199976101361"></a><a name="p199976101361"></a>SOURCES</p>
    </td>
    <td class="cellrowborder" valign="top" width="72.1%" headers="mcps1.2.3.1.2 "><p id="p499751023618"><a name="p499751023618"></a><a name="p499751023618"></a>当前组件的C文件列表，其中CMAKE_CURRENT_SOURCE_DIR变量标识当前CMakeLists.txt所在的路径。</p>
    </td>
    </tr>
    <tr id="row5997910163618"><td class="cellrowborder" valign="top" width="27.900000000000002%" headers="mcps1.2.3.1.1 "><p id="p129971210143615"><a name="p129971210143615"></a><a name="p129971210143615"></a>PUBLIC_HEADER</p>
    </td>
    <td class="cellrowborder" valign="top" width="72.1%" headers="mcps1.2.3.1.2 "><p id="p69971109360"><a name="p69971109360"></a><a name="p69971109360"></a>当前组件需要对外提供的头文件的路径。</p>
    </td>
    </tr>
    <tr id="row1199791011363"><td class="cellrowborder" valign="top" width="27.900000000000002%" headers="mcps1.2.3.1.1 "><p id="p1699711105364"><a name="p1699711105364"></a><a name="p1699711105364"></a>PRIVATE_HEADER</p>
    </td>
    <td class="cellrowborder" valign="top" width="72.1%" headers="mcps1.2.3.1.2 "><p id="p1799771033615"><a name="p1799771033615"></a><a name="p1799771033615"></a>当前组件内部的头文件搜索路径。</p>
    </td>
    </tr>
    <tr id="row99971610193616"><td class="cellrowborder" valign="top" width="27.900000000000002%" headers="mcps1.2.3.1.1 "><p id="p169971610133618"><a name="p169971610133618"></a><a name="p169971610133618"></a>PRIVATE_DEFINES</p>
    </td>
    <td class="cellrowborder" valign="top" width="72.1%" headers="mcps1.2.3.1.2 "><p id="p14997610103613"><a name="p14997610103613"></a><a name="p14997610103613"></a>当前组件内部生效的宏定义。</p>
    </td>
    </tr>
    <tr id="row12997210203618"><td class="cellrowborder" valign="top" width="27.900000000000002%" headers="mcps1.2.3.1.1 "><p id="p59971910163611"><a name="p59971910163611"></a><a name="p59971910163611"></a>PUBLIC_DEFINES</p>
    </td>
    <td class="cellrowborder" valign="top" width="72.1%" headers="mcps1.2.3.1.2 "><p id="p02671050184018"><a name="p02671050184018"></a><a name="p02671050184018"></a>当前组件需要对外提供的宏定义。</p>
    </td>
    </tr>
    <tr id="row12716914103914"><td class="cellrowborder" valign="top" width="27.900000000000002%" headers="mcps1.2.3.1.1 "><p id="p5717214153911"><a name="p5717214153911"></a><a name="p5717214153911"></a>COMPONENT_PUBLIC_CCFLAGS</p>
    </td>
    <td class="cellrowborder" valign="top" width="72.1%" headers="mcps1.2.3.1.2 "><p id="p14717111473912"><a name="p14717111473912"></a><a name="p14717111473912"></a>当前组件需要对外提供的编译选项。</p>
    </td>
    </tr>
    <tr id="row22992182396"><td class="cellrowborder" valign="top" width="27.900000000000002%" headers="mcps1.2.3.1.1 "><p id="p152993185398"><a name="p152993185398"></a><a name="p152993185398"></a>COMPONENT_CCFLAGS</p>
    </td>
    <td class="cellrowborder" valign="top" width="72.1%" headers="mcps1.2.3.1.2 "><p id="p132991118123913"><a name="p132991118123913"></a><a name="p132991118123913"></a>当前组件内部生效的编译选项。</p>
    </td>
    </tr>
    </tbody>
    </table>

4.  修改“application/ws63/CMakeLists.txt”，将my\_demo目录加入编译。
5.  修改“build/config/target\_config/ws63/config.py”，在ram\_component字段中加入‘my\_demo’，向编译系统中注册my\_demo组件。

## 开发代码<a name="ZH-CN_TOPIC_0000001700134432"></a>

目录结构建立完成后开始启动开发代码（用户可参考“application/samples”进行移植），代码开发完成后即可使用“python3 build.py -c ws63-liteos-app -component=my\_demo”编译my\_demo进行代码编译调试。

## 镜像烧录<a name="ZH-CN_TOPIC_0000001768396593"></a>

镜像烧录方法，请参见《WS63V100 BurnTool工具 使用指南》中“操作指南”章节。

