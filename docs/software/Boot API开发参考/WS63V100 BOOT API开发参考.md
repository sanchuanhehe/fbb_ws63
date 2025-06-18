# 前言<a name="ZH-CN_TOPIC_0000001803116112"></a>

**概述<a name="section4537382116410"></a>**

本文主要介绍WS63 Flashboot中升级相关的API接口，具体参考《WS63V100 FOTA开发指南》中的接口介绍。

**产品版本<a name="section111371595118"></a>**

与本文档相对应的产品版本如下。

<a name="table22377277"></a>
<table><thead align="left"><tr id="row63051425"><th class="cellrowborder" valign="top" width="40.400000000000006%" id="mcps1.1.3.1.1"><p id="p6891761"><a name="p6891761"></a><a name="p6891761"></a><strong id="b87075663813"><a name="b87075663813"></a><a name="b87075663813"></a>产品名称</strong></p>
</th>
<th class="cellrowborder" valign="top" width="59.599999999999994%" id="mcps1.1.3.1.2"><p id="p21361741"><a name="p21361741"></a><a name="p21361741"></a><strong id="b1175145653811"><a name="b1175145653811"></a><a name="b1175145653811"></a>产品版本</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row52579486"><td class="cellrowborder" valign="top" width="40.400000000000006%" headers="mcps1.1.3.1.1 "><p id="p31080012"><a name="p31080012"></a><a name="p31080012"></a>WS63</p>
</td>
<td class="cellrowborder" valign="top" width="59.599999999999994%" headers="mcps1.1.3.1.2 "><p id="p34453054"><a name="p34453054"></a><a name="p34453054"></a>V100</p>
</td>
</tr>
</tbody>
</table>

**读者对象<a name="section0192844173119"></a>**

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
<tbody><tr id="row1372280416410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p3734547016410"><a name="p3734547016410"></a><a name="p3734547016410"></a><a name="image2670064316410"></a><a name="image2670064316410"></a><span><img class="" id="image2670064316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000001849835093.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p1757432116410"><a name="p1757432116410"></a><a name="p1757432116410"></a>表示如不避免则将会导致死亡或严重伤害的具有高等级风险的危害。</p>
</td>
</tr>
<tr id="row466863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p1432579516410"><a name="p1432579516410"></a><a name="p1432579516410"></a><a name="image4895582316410"></a><a name="image4895582316410"></a><span><img class="" id="image4895582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000001849755165.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p959197916410"><a name="p959197916410"></a><a name="p959197916410"></a>表示如不避免则可能导致死亡或严重伤害的具有中等级风险的危害。</p>
</td>
</tr>
<tr id="row123863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p1232579516410"><a name="p1232579516410"></a><a name="p1232579516410"></a><a name="image1235582316410"></a><a name="image1235582316410"></a><span><img class="" id="image1235582316410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000001802956316.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p123197916410"><a name="p123197916410"></a><a name="p123197916410"></a>表示如不避免则可能导致轻微或中度伤害的具有低等级风险的危害。</p>
</td>
</tr>
<tr id="row5786682116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p2204984716410"><a name="p2204984716410"></a><a name="p2204984716410"></a><a name="image4504446716410"></a><a name="image4504446716410"></a><span><img class="" id="image4504446716410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000001803116124.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p4388861916410"><a name="p4388861916410"></a><a name="p4388861916410"></a>用于传递设备或环境安全警示信息。如不避免则可能会导致设备损坏、数据丢失、设备性能降低或其它不可预知的结果。</p>
<p id="p1238861916410"><a name="p1238861916410"></a><a name="p1238861916410"></a>“须知”不涉及人身伤害。</p>
</td>
</tr>
<tr id="row2856923116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p5555360116410"><a name="p5555360116410"></a><a name="p5555360116410"></a><a name="image799324016410"></a><a name="image799324016410"></a><span><img class="" id="image799324016410" height="25.270000000000003" width="67.83" src="figures/zh-cn_image_0000001849835105.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p4612588116410"><a name="p4612588116410"></a><a name="p4612588116410"></a>对正文中重点信息的补充说明。</p>
<p id="p1232588116410"><a name="p1232588116410"></a><a name="p1232588116410"></a>“说明”不是安全警示信息，不涉及人身、设备及环境伤害信息。</p>
</td>
</tr>
</tbody>
</table>

**修改记录<a name="section2467512116410"></a>**

<a name="table1557726816410"></a>
<table><thead align="left"><tr id="row2942532716410"><th class="cellrowborder" valign="top" width="20.72%" id="mcps1.1.4.1.1"><p id="p3778275416410"><a name="p3778275416410"></a><a name="p3778275416410"></a><strong id="b5687322716410"><a name="b5687322716410"></a><a name="b5687322716410"></a>文档版本</strong></p>
</th>
<th class="cellrowborder" valign="top" width="26.119999999999997%" id="mcps1.1.4.1.2"><p id="p5627845516410"><a name="p5627845516410"></a><a name="p5627845516410"></a><strong id="b5800814916410"><a name="b5800814916410"></a><a name="b5800814916410"></a>发布日期</strong></p>
</th>
<th class="cellrowborder" valign="top" width="53.16%" id="mcps1.1.4.1.3"><p id="p2382284816410"><a name="p2382284816410"></a><a name="p2382284816410"></a><strong id="b3316380216410"><a name="b3316380216410"></a><a name="b3316380216410"></a>修改说明</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row1699734715172"><td class="cellrowborder" valign="top" width="20.72%" headers="mcps1.1.4.1.1 "><p id="p220313211512"><a name="p220313211512"></a><a name="p220313211512"></a>01</p>
</td>
<td class="cellrowborder" valign="top" width="26.119999999999997%" headers="mcps1.1.4.1.2 "><p id="p52034321153"><a name="p52034321153"></a><a name="p52034321153"></a>2024-04-10</p>
</td>
<td class="cellrowborder" valign="top" width="53.16%" headers="mcps1.1.4.1.3 "><p id="p1031614161639"><a name="p1031614161639"></a><a name="p1031614161639"></a>第一次正式版本发布。</p>
</td>
</tr>
<tr id="row5947359616410"><td class="cellrowborder" valign="top" width="20.72%" headers="mcps1.1.4.1.1 "><p id="p2149706016410"><a name="p2149706016410"></a><a name="p2149706016410"></a>00B01</p>
</td>
<td class="cellrowborder" valign="top" width="26.119999999999997%" headers="mcps1.1.4.1.2 "><p id="p648803616410"><a name="p648803616410"></a><a name="p648803616410"></a>2024-02-22</p>
</td>
<td class="cellrowborder" valign="top" width="53.16%" headers="mcps1.1.4.1.3 "><p id="p1946537916410"><a name="p1946537916410"></a><a name="p1946537916410"></a>第一次临时版本发布。</p>
</td>
</tr>
</tbody>
</table>

# 接口介绍<a name="ZH-CN_TOPIC_0000001849825997"></a>

**表 1**  升级接口（升级包存储部分）描述

<a name="table1585372681620"></a>
<table><thead align="left"><tr id="row385412616166"><th class="cellrowborder" valign="top" width="37.74%" id="mcps1.2.3.1.1"><p id="p373695616166"><a name="p373695616166"></a><a name="p373695616166"></a>接口名称</p>
</th>
<th class="cellrowborder" valign="top" width="62.260000000000005%" id="mcps1.2.3.1.2"><p id="p15736756151620"><a name="p15736756151620"></a><a name="p15736756151620"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row1486763081710"><td class="cellrowborder" valign="top" width="37.74%" headers="mcps1.2.3.1.1 "><p id="p18867133051718"><a name="p18867133051718"></a><a name="p18867133051718"></a>uapi_upg_init</p>
</td>
<td class="cellrowborder" valign="top" width="62.260000000000005%" headers="mcps1.2.3.1.2 "><p id="p1486713307174"><a name="p1486713307174"></a><a name="p1486713307174"></a>升级模块初始化。</p>
</td>
</tr>
<tr id="row11854122671616"><td class="cellrowborder" valign="top" width="37.74%" headers="mcps1.2.3.1.1 "><p id="p126138161710"><a name="p126138161710"></a><a name="p126138161710"></a>uapi_upg_prepare</p>
</td>
<td class="cellrowborder" valign="top" width="62.260000000000005%" headers="mcps1.2.3.1.2 "><p id="p19611380177"><a name="p19611380177"></a><a name="p19611380177"></a>保存升级包到本地存储器前的准备工作。</p>
</td>
</tr>
<tr id="row685452616161"><td class="cellrowborder" valign="top" width="37.74%" headers="mcps1.2.3.1.1 "><p id="p16616811719"><a name="p16616811719"></a><a name="p16616811719"></a>uapi_upg_write_package_async/uapi_upg_write_package_sync</p>
</td>
<td class="cellrowborder" valign="top" width="62.260000000000005%" headers="mcps1.2.3.1.2 "><p id="p3611486173"><a name="p3611486173"></a><a name="p3611486173"></a>将升级包数据写入本地存储器。（异步方式/同步方式）</p>
</td>
</tr>
<tr id="row98547262169"><td class="cellrowborder" valign="top" width="37.74%" headers="mcps1.2.3.1.1 "><p id="p259718589312"><a name="p259718589312"></a><a name="p259718589312"></a>uapi_upg_read_package</p>
</td>
<td class="cellrowborder" valign="top" width="62.260000000000005%" headers="mcps1.2.3.1.2 "><p id="p5615811714"><a name="p5615811714"></a><a name="p5615811714"></a>从本地存储器读取升级包数据。</p>
</td>
</tr>
<tr id="row177421859153715"><td class="cellrowborder" valign="top" width="37.74%" headers="mcps1.2.3.1.1 "><p id="p107431159183718"><a name="p107431159183718"></a><a name="p107431159183718"></a>uapi_upg_request_upgrade</p>
</td>
<td class="cellrowborder" valign="top" width="62.260000000000005%" headers="mcps1.2.3.1.2 "><p id="p57432059133712"><a name="p57432059133712"></a><a name="p57432059133712"></a>申请开始进行本地升级，所有升级包数据全部保存完成后，调用此接口。</p>
</td>
</tr>
<tr id="row3828154123820"><td class="cellrowborder" valign="top" width="37.74%" headers="mcps1.2.3.1.1 "><p id="p1066210362417"><a name="p1066210362417"></a><a name="p1066210362417"></a>uapi_upg_get_storage_size</p>
</td>
<td class="cellrowborder" valign="top" width="62.260000000000005%" headers="mcps1.2.3.1.2 "><p id="p1182824143810"><a name="p1182824143810"></a><a name="p1182824143810"></a>获取可存放升级包的空间大小。</p>
</td>
</tr>
</tbody>
</table>

**表 2**  升级接口（本地升级部分）描述

<a name="table1660824542212"></a>
<table><thead align="left"><tr id="row1860864518228"><th class="cellrowborder" valign="top" width="37.74%" id="mcps1.2.3.1.1"><p id="p1060916458228"><a name="p1060916458228"></a><a name="p1060916458228"></a>接口名称</p>
</th>
<th class="cellrowborder" valign="top" width="62.260000000000005%" id="mcps1.2.3.1.2"><p id="p13609184532213"><a name="p13609184532213"></a><a name="p13609184532213"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row560934513229"><td class="cellrowborder" valign="top" width="37.74%" headers="mcps1.2.3.1.1 "><p id="p960934513228"><a name="p960934513228"></a><a name="p960934513228"></a>uapi_upg_init</p>
</td>
<td class="cellrowborder" valign="top" width="62.260000000000005%" headers="mcps1.2.3.1.2 "><p id="p176091045102215"><a name="p176091045102215"></a><a name="p176091045102215"></a>升级模块初始化。</p>
</td>
</tr>
<tr id="row35427913505"><td class="cellrowborder" valign="top" width="37.74%" headers="mcps1.2.3.1.1 "><p id="p5856111017502"><a name="p5856111017502"></a><a name="p5856111017502"></a>uapi_upg_register_progress_callback</p>
</td>
<td class="cellrowborder" valign="top" width="62.260000000000005%" headers="mcps1.2.3.1.2 "><p id="p7856161015507"><a name="p7856161015507"></a><a name="p7856161015507"></a>注册升级进度通知回调函数，注册后，在本地升级过程中会调用回调函数通知当前进度。</p>
</td>
</tr>
<tr id="row19609154562210"><td class="cellrowborder" valign="top" width="37.74%" headers="mcps1.2.3.1.1 "><p id="p5609745202212"><a name="p5609745202212"></a><a name="p5609745202212"></a>uapi_upg_start</p>
</td>
<td class="cellrowborder" valign="top" width="62.260000000000005%" headers="mcps1.2.3.1.2 "><p id="p20609245172211"><a name="p20609245172211"></a><a name="p20609245172211"></a>开始本地升级。</p>
</td>
</tr>
<tr id="row1612214129385"><td class="cellrowborder" valign="top" width="37.74%" headers="mcps1.2.3.1.1 "><p id="p9861102283820"><a name="p9861102283820"></a><a name="p9861102283820"></a>uapi_upg_get_result</p>
</td>
<td class="cellrowborder" valign="top" width="62.260000000000005%" headers="mcps1.2.3.1.2 "><p id="p386142213387"><a name="p386142213387"></a><a name="p386142213387"></a>获取升级结果。</p>
</td>
</tr>
<tr id="row1260944511226"><td class="cellrowborder" valign="top" width="37.74%" headers="mcps1.2.3.1.1 "><p id="p1821544917386"><a name="p1821544917386"></a><a name="p1821544917386"></a>uapi_upg_verify_file_head</p>
</td>
<td class="cellrowborder" valign="top" width="62.260000000000005%" headers="mcps1.2.3.1.2 "><p id="p122151749113812"><a name="p122151749113812"></a><a name="p122151749113812"></a>校验升级包头结构。</p>
</td>
</tr>
<tr id="row126175548388"><td class="cellrowborder" valign="top" width="37.74%" headers="mcps1.2.3.1.1 "><p id="p1355184103910"><a name="p1355184103910"></a><a name="p1355184103910"></a>uapi_upg_verify_file_image</p>
</td>
<td class="cellrowborder" valign="top" width="62.260000000000005%" headers="mcps1.2.3.1.2 "><p id="p1355154163918"><a name="p1355154163918"></a><a name="p1355154163918"></a>校验升级包中的升级镜像。</p>
</td>
</tr>
<tr id="row18734135723817"><td class="cellrowborder" valign="top" width="37.74%" headers="mcps1.2.3.1.1 "><p id="p18827963914"><a name="p18827963914"></a><a name="p18827963914"></a>uapi_upg_verify_file</p>
</td>
<td class="cellrowborder" valign="top" width="62.260000000000005%" headers="mcps1.2.3.1.2 "><p id="p10821694394"><a name="p10821694394"></a><a name="p10821694394"></a>校验整个升级包。</p>
</td>
</tr>
<tr id="row43871712163916"><td class="cellrowborder" valign="top" width="37.74%" headers="mcps1.2.3.1.1 "><p id="p9285101916398"><a name="p9285101916398"></a><a name="p9285101916398"></a>uapi_upg_register_user_defined_verify_func</p>
</td>
<td class="cellrowborder" valign="top" width="62.260000000000005%" headers="mcps1.2.3.1.2 "><p id="p152851419163917"><a name="p152851419163917"></a><a name="p152851419163917"></a>注册用户自定义字段的校验函数。</p>
<p id="p32851119103910"><a name="p32851119103910"></a><a name="p32851119103910"></a>升级包结构中预留了48Byte用于用户自定义数据的校验。注册自定义校验函数后，被注册的函数会在调用uapi_upg_verify_file_head和uapi_upg_verify_file函数时被调用到。如果自定义数据校验失败uapi_upg_verify_file_head和uapi_upg_verify_file会返回失败。</p>
</td>
</tr>
</tbody>
</table>

**表 3**  升级接口入参及返回值描述

<a name="table543918111242"></a>
<table><thead align="left"><tr id="row64392119417"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p1343919111645"><a name="p1343919111645"></a><a name="p1343919111645"></a>接口原型</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p174391011448"><a name="p174391011448"></a><a name="p174391011448"></a>参数及返回值说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row19439131110414"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1969441615612"><a name="p1969441615612"></a><a name="p1969441615612"></a>errcode_t uapi_upg_init(const upg_func_t *func_list)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><a name="ul1384395376"></a><a name="ul1384395376"></a><ul id="ul1384395376"><li>入参说明：<p id="p743931115420"><a name="p743931115420"></a><a name="p743931115420"></a>func_list：注册回调列表，upg_func_t类型。</p>
</li></ul>
<a name="ul368214575374"></a><a name="ul368214575374"></a><ul id="ul368214575374"><li>返回值：<a name="ul13561878384"></a><a name="ul13561878384"></a><ul id="ul13561878384"><li>ERRCODE_SUCC：成功。</li><li>其他：失败。</li></ul>
</li></ul>
</td>
</tr>
<tr id="row174394111548"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p172791358273"><a name="p172791358273"></a><a name="p172791358273"></a>errcode_t uapi_upg_start(void)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><a name="ul1417191953814"></a><a name="ul1417191953814"></a><ul id="ul1417191953814"><li>入参说明：无。</li><li>返回值：<a name="ul4292432183815"></a><a name="ul4292432183815"></a><ul id="ul4292432183815"><li>ERRCODE_SUCC：成功。</li><li>其他：失败。</li></ul>
</li></ul>
</td>
</tr>
<tr id="row1943941111416"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p7912209817"><a name="p7912209817"></a><a name="p7912209817"></a>errcode_t uapi_upg_register_progress_callback(uapi_upg_progress_cb func)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><a name="ul18751138203816"></a><a name="ul18751138203816"></a><ul id="ul18751138203816"><li>入参说明：<p id="p874642615811"><a name="p874642615811"></a><a name="p874642615811"></a>func：回调函数，该函数需业务实现。</p>
</li><li>返回值：<a name="ul1534612502382"></a><a name="ul1534612502382"></a><ul id="ul1534612502382"><li>ERRCODE_SUCC：成功。</li><li>其他：失败。</li></ul>
</li></ul>
</td>
</tr>
<tr id="row443915117411"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p413413720912"><a name="p413413720912"></a><a name="p413413720912"></a>errcode_t uapi_upg_get_result(upg_result_t *result, uint32_t *last_image_index)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><a name="ul5512203183912"></a><a name="ul5512203183912"></a><ul id="ul5512203183912"><li>入参说明：<a name="ul1285514154399"></a><a name="ul1285514154399"></a><ul id="ul1285514154399"><li>result：出参，保存升级结果的内存地址，类型upg_result_t。</li><li>last_image_index：出参，保存最后一个处理的镜像的索引。</li></ul>
</li><li>返回值：<a name="ul93412817391"></a><a name="ul93412817391"></a><ul id="ul93412817391"><li>ERRCODE_SUCC：成功。</li><li>其他：失败。</li></ul>
</li></ul>
</td>
</tr>
<tr id="row17439131117411"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p115282109111"><a name="p115282109111"></a><a name="p115282109111"></a>errcode_t uapi_upg_prepare(upg_prepare_info_t *prepare_info)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><a name="ul28461128173912"></a><a name="ul28461128173912"></a><ul id="ul28461128173912"><li>入参说明：<p id="p182641143164"><a name="p182641143164"></a><a name="p182641143164"></a>prepare_info：入参，upg_prepare_info_t*类型，准备信息的指针。</p>
</li><li>返回值：<a name="ul101051345399"></a><a name="ul101051345399"></a><ul id="ul101051345399"><li>ERRCODE_SUCC：成功。</li><li>其他：失败。</li></ul>
</li></ul>
</td>
</tr>
<tr id="row1972014489105"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p59591437498"><a name="p59591437498"></a><a name="p59591437498"></a>errcode_t uapi_upg_write_package_async(uint32_t offset, const uint8_t *buff, uint16_t len, uapi_upg_write_done_cb callback)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><a name="ul13324126134011"></a><a name="ul13324126134011"></a><ul id="ul13324126134011"><li>入参说明：<a name="ul6565151412407"></a><a name="ul6565151412407"></a><ul id="ul6565151412407"><li>offset：入参，uint32_t类型，相对升级包开头的偏移。</li><li>buff：入参，const uint8_t *类型，存放升级包数据的buffer。</li><li>len：入参，uint16_t类型，升级包数据buffer的长度。</li><li>callback：入参，uapi_upg_write_done_cb类型，写入完成的回调函数。</li></ul>
</li><li>返回值：<a name="ul135574198410"></a><a name="ul135574198410"></a><ul id="ul135574198410"><li>ERRCODE_SUCC：成功。</li><li>其他：失败。</li></ul>
</li></ul>
</td>
</tr>
<tr id="row18571245918"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p96711599112"><a name="p96711599112"></a><a name="p96711599112"></a>errcode_t uapi_upg_write_package_sync(uint32_t offset, const uint8_t *buff, uint16_t len)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><a name="ul6730191311914"></a><a name="ul6730191311914"></a><ul id="ul6730191311914"><li>入参说明：<a name="ul1730513290"></a><a name="ul1730513290"></a><ul id="ul1730513290"><li>offset：入参，uint32_t类型，相对升级包开头的偏移。</li><li>buff：入参，const uint8_t *类型，存放升级包数据的buffer。</li><li>len：入参，uint16_t类型，升级包数据buffer的长度。</li></ul>
</li><li>返回值：<a name="ul13731513999"></a><a name="ul13731513999"></a><ul id="ul13731513999"><li>ERRCODE_SUCC：成功。</li><li>其他：失败。</li></ul>
</li></ul>
</td>
</tr>
<tr id="row16820205217100"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1415605316124"><a name="p1415605316124"></a><a name="p1415605316124"></a>errcode_t uapi_upg_read_package(uint32_t offset, uint8_t *buff, uint32_t len)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><a name="ul942818368418"></a><a name="ul942818368418"></a><ul id="ul942818368418"><li>入参说明：<a name="ul540820427411"></a><a name="ul540820427411"></a><ul id="ul540820427411"><li>offset：入参，uint32_t类型，相对升级包开头的偏移。</li><li>buff：出参，uint8_t *类型，存放升级包数据的buffer。</li><li>len：入参，uint32_t类型，读取数据buffer的长度。</li></ul>
</li><li>返回值：<a name="ul92624479419"></a><a name="ul92624479419"></a><ul id="ul92624479419"><li>ERRCODE_SUCC：成功。</li><li>其他：失败。</li></ul>
</li></ul>
</td>
</tr>
<tr id="row9731115618106"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1642912184138"><a name="p1642912184138"></a><a name="p1642912184138"></a>uint32_t uapi_upg_get_storage_size(void)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><a name="ul11468249184212"></a><a name="ul11468249184212"></a><ul id="ul11468249184212"><li>入参说明：无。</li><li>返回值：<a name="ul1471417015437"></a><a name="ul1471417015437"></a><ul id="ul1471417015437"><li>0：失败返回0。</li><li>其他：成功返回空间大小。</li></ul>
</li></ul>
</td>
</tr>
<tr id="row99221759121014"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p17782242205"><a name="p17782242205"></a><a name="p17782242205"></a>errcode_t uapi_upg_request_upgrade(bool reset)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><a name="ul93401214124320"></a><a name="ul93401214124320"></a><ul id="ul93401214124320"><li>入参说明：<p id="p12873129121711"><a name="p12873129121711"></a><a name="p12873129121711"></a>reset：入参，bool类型，申请流程结束后是否重启系统。</p>
</li><li>返回值：<a name="ul112601188436"></a><a name="ul112601188436"></a><ul id="ul112601188436"><li>ERRCODE_SUCC：成功。</li><li>其他：失败。</li></ul>
</li></ul>
</td>
</tr>
<tr id="row1168225117190"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p17933151517497"><a name="p17933151517497"></a><a name="p17933151517497"></a>errcode_t uapi_upg_verify_file_head(const upg_package_header_t *pkg_header)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><a name="ul172091039114315"></a><a name="ul172091039114315"></a><ul id="ul172091039114315"><li>入参说明：<p id="p577525916436"><a name="p577525916436"></a><a name="p577525916436"></a>pkg_header：入参，upg_package_header_t *类型，指向升级包头结构的指针。</p>
</li><li>返回值：<a name="ul106384575434"></a><a name="ul106384575434"></a><ul id="ul106384575434"><li>ERRCODE_SUCC：成功。</li><li>其他：失败。</li></ul>
</li></ul>
</td>
</tr>
<tr id="row16741055171920"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p15451248507"><a name="p15451248507"></a><a name="p15451248507"></a>errcode_t uapi_upg_verify_file_image(const upg_image_header_t *img_header, const uint8_t *hash, uint32_t hash_len, bool verify_old)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><a name="ul083121716446"></a><a name="ul083121716446"></a><ul id="ul083121716446"><li>入参说明：<a name="ul195551433164420"></a><a name="ul195551433164420"></a><ul id="ul195551433164420"><li>img_header：入参，upg_image_header_t*类型，指向升级包中升级镜像头结构的指针。</li><li>hash：入参，uint8_t*类型，升级镜像的HASH值。</li><li>hash_len：入参，uint32_t类型，HASH的长度（单位：Byte）。</li><li>verify_old：入参，bool类型，是否校验旧镜像。</li></ul>
</li><li>返回值：<a name="ul1095818223448"></a><a name="ul1095818223448"></a><ul id="ul1095818223448"><li>ERRCODE_SUCC：成功。</li><li>其他：失败。</li></ul>
</li></ul>
</td>
</tr>
<tr id="row1456615911192"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p194291932165215"><a name="p194291932165215"></a><a name="p194291932165215"></a>errcode_t uapi_upg_verify_file(const upg_package_header_t *pkg_header)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><a name="ul7532122417456"></a><a name="ul7532122417456"></a><ul id="ul7532122417456"><li>入参说明：<p id="p492743714514"><a name="p492743714514"></a><a name="p492743714514"></a>pkg_header：入参，upg_package_header_t *类型，指向升级包头结构的指针。</p>
</li><li>返回值：<a name="ul668363524515"></a><a name="ul668363524515"></a><ul id="ul668363524515"><li>ERRCODE_SUCC：成功。</li><li>其他：失败。</li></ul>
</li></ul>
</td>
</tr>
<tr id="row1521203202011"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p44763015318"><a name="p44763015318"></a><a name="p44763015318"></a>void uapi_upg_register_user_defined_verify_func(uapi_upg_user_defined_check func, uintptr_t param)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><a name="ul643116487459"></a><a name="ul643116487459"></a><ul id="ul643116487459"><li>入参说明：<a name="ul123651753194513"></a><a name="ul123651753194513"></a><ul id="ul123651753194513"><li>func：入参，upg_package_header_t *类型，用于校验用户自定义字段的校验函数。</li><li>param：入参，uintptr_t类型，注册参数。</li></ul>
</li><li>返回值：无。</li></ul>
</td>
</tr>
</tbody>
</table>

