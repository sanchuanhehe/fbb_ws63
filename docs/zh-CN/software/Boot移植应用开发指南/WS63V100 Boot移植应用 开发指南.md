# 前言<a name="ZH-CN_TOPIC_0000001732354354"></a>

**概述<a name="section4537382116410"></a>**

本文档描述了WS63V100 RomBoot、LoaderBoot及FlashBoot工作流程，用户可参考此文档对FlashBoot进行二次开发。

**产品版本<a name="section27775771"></a>**

与本文档相对应的产品版本如下。

<a name="table52250146"></a>
<table><thead align="left"><tr id="row55967882"><th class="cellrowborder" valign="top" width="39.39%" id="mcps1.1.3.1.1"><p id="p37104584"><a name="p37104584"></a><a name="p37104584"></a><strong id="b21211131516"><a name="b21211131516"></a><a name="b21211131516"></a>产品名称</strong></p>
</th>
<th class="cellrowborder" valign="top" width="60.61%" id="mcps1.1.3.1.2"><p id="p52681331"><a name="p52681331"></a><a name="p52681331"></a><strong id="b18261911191515"><a name="b18261911191515"></a><a name="b18261911191515"></a>产品版本</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row39329394"><td class="cellrowborder" valign="top" width="39.39%" headers="mcps1.1.3.1.1 "><p id="p31080012"><a name="p31080012"></a><a name="p31080012"></a>WS63</p>
</td>
<td class="cellrowborder" valign="top" width="60.61%" headers="mcps1.1.3.1.2 "><p id="p34453054"><a name="p34453054"></a><a name="p34453054"></a>V100</p>
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
<tbody><tr id="row1372280416410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p3734547016410"><a name="p3734547016410"></a><a name="p3734547016410"></a><a name="image2670064316410"></a><a name="image2670064316410"></a><span><img class="" id="image2670064316410" height="25.270000000000003" width="55.9265" src="figures/zh-cn_image_0000001732195226.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p1757432116410"><a name="p1757432116410"></a><a name="p1757432116410"></a>表示如不避免则将会导致死亡或严重伤害的具有高等级风险的危害。</p>
</td>
</tr>
<tr id="row466863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p1432579516410"><a name="p1432579516410"></a><a name="p1432579516410"></a><a name="image4895582316410"></a><a name="image4895582316410"></a><span><img class="" id="image4895582316410" height="25.270000000000003" width="55.9265" src="figures/zh-cn_image_0000001779394385.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p959197916410"><a name="p959197916410"></a><a name="p959197916410"></a>表示如不避免则可能导致死亡或严重伤害的具有中等级风险的危害。</p>
</td>
</tr>
<tr id="row123863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p1232579516410"><a name="p1232579516410"></a><a name="p1232579516410"></a><a name="image1235582316410"></a><a name="image1235582316410"></a><span><img class="" id="image1235582316410" height="25.270000000000003" width="55.9265" src="figures/zh-cn_image_0000001779234653.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p123197916410"><a name="p123197916410"></a><a name="p123197916410"></a>表示如不避免则可能导致轻微或中度伤害的具有低等级风险的危害。</p>
</td>
</tr>
<tr id="row5786682116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p2204984716410"><a name="p2204984716410"></a><a name="p2204984716410"></a><a name="image4504446716410"></a><a name="image4504446716410"></a><span><img class="" id="image4504446716410" height="25.270000000000003" width="55.9265" src="figures/zh-cn_image_0000001732195234.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p4388861916410"><a name="p4388861916410"></a><a name="p4388861916410"></a>用于传递设备或环境安全警示信息。如不避免则可能会导致设备损坏、数据丢失、设备性能降低或其它不可预知的结果。</p>
<p id="p1238861916410"><a name="p1238861916410"></a><a name="p1238861916410"></a>“须知”不涉及人身伤害。</p>
</td>
</tr>
<tr id="row2856923116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p5555360116410"><a name="p5555360116410"></a><a name="p5555360116410"></a><a name="image799324016410"></a><a name="image799324016410"></a><span><img class="" id="image799324016410" height="15.96" width="47.88" src="figures/zh-cn_image_0000001732195218.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p4612588116410"><a name="p4612588116410"></a><a name="p4612588116410"></a>对正文中重点信息的补充说明。</p>
<p id="p1232588116410"><a name="p1232588116410"></a><a name="p1232588116410"></a>“说明”不是安全警示信息，不涉及人身、设备及环境伤害信息。</p>
</td>
</tr>
</tbody>
</table>

**修改记录<a name="section2467512116410"></a>**

<a name="table1557726816410"></a>
<table><thead align="left"><tr id="row2942532716410"><th class="cellrowborder" valign="top" width="15.65%" id="mcps1.1.4.1.1"><p id="p3778275416410"><a name="p3778275416410"></a><a name="p3778275416410"></a><strong id="b5687322716410"><a name="b5687322716410"></a><a name="b5687322716410"></a>文档版本</strong></p>
</th>
<th class="cellrowborder" valign="top" width="20.89%" id="mcps1.1.4.1.2"><p id="p5627845516410"><a name="p5627845516410"></a><a name="p5627845516410"></a><strong id="b5800814916410"><a name="b5800814916410"></a><a name="b5800814916410"></a>发布日期</strong></p>
</th>
<th class="cellrowborder" valign="top" width="63.46000000000001%" id="mcps1.1.4.1.3"><p id="p2382284816410"><a name="p2382284816410"></a><a name="p2382284816410"></a><strong id="b3316380216410"><a name="b3316380216410"></a><a name="b3316380216410"></a>修改说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row820313201511"><td class="cellrowborder" valign="top" width="15.65%" headers="mcps1.1.4.1.1 "><p id="p220313211512"><a name="p220313211512"></a><a name="p220313211512"></a>01</p>
</td>
<td class="cellrowborder" valign="top" width="20.89%" headers="mcps1.1.4.1.2 "><p id="p52034321153"><a name="p52034321153"></a><a name="p52034321153"></a>2024-04-10</p>
</td>
<td class="cellrowborder" valign="top" width="63.46000000000001%" headers="mcps1.1.4.1.3 "><p id="p1031614161639"><a name="p1031614161639"></a><a name="p1031614161639"></a>第一次正式版本发布。</p>
</td>
</tr>
<tr id="row5947359616410"><td class="cellrowborder" valign="top" width="15.65%" headers="mcps1.1.4.1.1 "><p id="p2149706016410"><a name="p2149706016410"></a><a name="p2149706016410"></a>00B01</p>
</td>
<td class="cellrowborder" valign="top" width="20.89%" headers="mcps1.1.4.1.2 "><p id="p648803616410"><a name="p648803616410"></a><a name="p648803616410"></a>2023-12-18</p>
</td>
<td class="cellrowborder" valign="top" width="63.46000000000001%" headers="mcps1.1.4.1.3 "><p id="p1946537916410"><a name="p1946537916410"></a><a name="p1946537916410"></a>第一次临时版本发布。</p>
</td>
</tr>
</tbody>
</table>

# Boot简介<a name="ZH-CN_TOPIC_0000001779234629"></a>

WS63V100 Boot分为三部分：RomBoot、FlashBoot、LoaderBoot。

-   RomBoot功能包括：
    -   加载LoaderBoot到RAM，进一步利用LoaderBoot下载镜像到Flash，烧写efuse等。
    -   校验并引导FlashBoot。FlashBoot分为AB面，A面校验成功直接启动，校验失败会去校验B面，B面校验成功，则从B面启动，否则复位重启。

-   FlashBoot功能包括：
    -   升级固件。
    -   校验并引导固件。

-   LoaderBoot功能包括：
    -   下载镜像到Flash。
    -   烧写EFUSE（例如：安全启动/Flash加密相关密钥等）。

**图 1**  Boot启动流程<a name="fig2070413408573"></a>  
![](figures/Boot启动流程.png "Boot启动流程")

# RomBoot功能说明<a name="ZH-CN_TOPIC_0000001732195210"></a>



## 下载镜像及烧写EFUSE<a name="ZH-CN_TOPIC_0000001732195194"></a>

RomBoot通过加载Loaderboot实现下载镜像到Flash及烧写EFUSE的功能，具体操作请参见《WS63V100 BurnTool工具 使用指南》。

## 检验及引导Flashboot<a name="ZH-CN_TOPIC_0000001779394349"></a>

校验并引导FlashBoot流程如[图1](#fig1578634595518)所示。

**图 1**  校验并引导FlashBoot流程图<a name="fig1578634595518"></a>  
![](figures/校验并引导FlashBoot流程图.png "校验并引导FlashBoot流程图")

# LoaderBoot功能说明<a name="ZH-CN_TOPIC_0000001779394361"></a>

LoaderBoot是直接与BurnTool进行交互的组件，RomBoot无法直接实现烧写的功能，需要将LoaderBoot加载到RAM后，跳转到LoaderBoot，进一步通过LoaderBoot完成相关内容的烧写，LoaderBoot可烧写的内容包括：

-   FlashBoot
-   EFUSE参数配置文件
-   固件镜像（包括NV参数）
-   产测镜像

>![](public_sys-resources/icon-note.gif) **说明：** 
>LoaderBoot一般不涉及二次开发。

# FlashBoot说明<a name="ZH-CN_TOPIC_0000001732354346"></a>


## FlashBoot启动流程<a name="ZH-CN_TOPIC_0000001732195198"></a>

校验并引导固件流程如[图1](#fig921910011115)所示。

**图 1**  校验并引导固件流程图<a name="fig921910011115"></a>  
![](figures/校验并引导固件流程图.png "校验并引导固件流程图")

