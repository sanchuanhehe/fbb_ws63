# 前言<a name="ZH-CN_TOPIC_0000001856218661"></a>

**概述<a name="section4537382116410"></a>**

本文档主要介绍基于cJson 1.7.15的Json报文解析功能开发实现示例，以及接口说明。

cJson 1.7.15的详细说明请参见官方说明文档：[#README](#README)

**产品版本<a name="section12266191774710"></a>**

与本文档相对应的产品版本如下。

<a name="table2270181717471"></a>
<table><thead align="left"><tr id="row15364171712479"><th class="cellrowborder" valign="top" width="31.759999999999998%" id="mcps1.1.3.1.1"><p id="p123646174478"><a name="p123646174478"></a><a name="p123646174478"></a><strong id="b1564684310201"><a name="b1564684310201"></a><a name="b1564684310201"></a>产品名称</strong></p>
</th>
<th class="cellrowborder" valign="top" width="68.24%" id="mcps1.1.3.1.2"><p id="p1936401717470"><a name="p1936401717470"></a><a name="p1936401717470"></a><strong id="b16664184332015"><a name="b16664184332015"></a><a name="b16664184332015"></a>产品版本</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row19364317104716"><td class="cellrowborder" valign="top" width="31.759999999999998%" headers="mcps1.1.3.1.1 "><p id="p31080012"><a name="p31080012"></a><a name="p31080012"></a>WS63</p>
</td>
<td class="cellrowborder" valign="top" width="68.24%" headers="mcps1.1.3.1.2 "><p id="p34453054"><a name="p34453054"></a><a name="p34453054"></a>V100</p>
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
<tbody><tr id="row1372280416410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p3734547016410"><a name="p3734547016410"></a><a name="p3734547016410"></a><a name="image2670064316410"></a><a name="image2670064316410"></a><span><img class="" id="image2670064316410" height="25.270000000000003" width="55.9265" src="figures/zh-cn_image_0000001809499880.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p1757432116410"><a name="p1757432116410"></a><a name="p1757432116410"></a>表示如不避免则将会导致死亡或严重伤害的具有高等级风险的危害。</p>
</td>
</tr>
<tr id="row466863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p1432579516410"><a name="p1432579516410"></a><a name="p1432579516410"></a><a name="image4895582316410"></a><a name="image4895582316410"></a><span><img class="" id="image4895582316410" height="25.270000000000003" width="55.9265" src="figures/zh-cn_image_0000001809340020.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p959197916410"><a name="p959197916410"></a><a name="p959197916410"></a>表示如不避免则可能导致死亡或严重伤害的具有中等级风险的危害。</p>
</td>
</tr>
<tr id="row123863216410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p1232579516410"><a name="p1232579516410"></a><a name="p1232579516410"></a><a name="image1235582316410"></a><a name="image1235582316410"></a><span><img class="" id="image1235582316410" height="25.270000000000003" width="55.9265" src="figures/zh-cn_image_0000001809499884.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p123197916410"><a name="p123197916410"></a><a name="p123197916410"></a>表示如不避免则可能导致轻微或中度伤害的具有低等级风险的危害。</p>
</td>
</tr>
<tr id="row5786682116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p2204984716410"><a name="p2204984716410"></a><a name="p2204984716410"></a><a name="image4504446716410"></a><a name="image4504446716410"></a><span><img class="" id="image4504446716410" height="25.270000000000003" width="55.9265" src="figures/zh-cn_image_0000001856138661.png"></span></p>
</td>
<td class="cellrowborder" valign="top" width="79.42%" headers="mcps1.1.3.1.2 "><p id="p4388861916410"><a name="p4388861916410"></a><a name="p4388861916410"></a>用于传递设备或环境安全警示信息。如不避免则可能会导致设备损坏、数据丢失、设备性能降低或其它不可预知的结果。</p>
<p id="p1238861916410"><a name="p1238861916410"></a><a name="p1238861916410"></a>“须知”不涉及人身伤害。</p>
</td>
</tr>
<tr id="row2856923116410"><td class="cellrowborder" valign="top" width="20.580000000000002%" headers="mcps1.1.3.1.1 "><p id="p5555360116410"><a name="p5555360116410"></a><a name="p5555360116410"></a><a name="image799324016410"></a><a name="image799324016410"></a><span><img class="" id="image799324016410" height="15.96" width="47.88" src="figures/zh-cn_image_0000001856218669.png"></span></p>
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
<tbody><tr id="row1349661710815"><td class="cellrowborder" valign="top" width="20.72%" headers="mcps1.1.4.1.1 "><p id="p14971217988"><a name="p14971217988"></a><a name="p14971217988"></a>01</p>
</td>
<td class="cellrowborder" valign="top" width="26.119999999999997%" headers="mcps1.1.4.1.2 "><p id="p349771716819"><a name="p349771716819"></a><a name="p349771716819"></a>2024-04-10</p>
</td>
<td class="cellrowborder" valign="top" width="53.16%" headers="mcps1.1.4.1.3 "><p id="p6497117787"><a name="p6497117787"></a><a name="p6497117787"></a>第一次正式版本发布。</p>
</td>
</tr>
<tr id="row1672911371419"><td class="cellrowborder" valign="top" width="20.72%" headers="mcps1.1.4.1.1 "><p id="p77291037518"><a name="p77291037518"></a><a name="p77291037518"></a>00B01</p>
</td>
<td class="cellrowborder" valign="top" width="26.119999999999997%" headers="mcps1.1.4.1.2 "><p id="p137300374116"><a name="p137300374116"></a><a name="p137300374116"></a>2024-03-15</p>
</td>
<td class="cellrowborder" valign="top" width="53.16%" headers="mcps1.1.4.1.3 "><p id="p87301237616"><a name="p87301237616"></a><a name="p87301237616"></a>第一次临时版本发布。</p>
</td>
</tr>
</tbody>
</table>

# API接口说明<a name="ZH-CN_TOPIC_0000001809340008"></a>

>![](public_sys-resources/icon-note.gif) **说明：** 
>cJson 1.7.15为开源软件，以下为部分结构体和API描述，其余部分请参考官方说明。



## 结构体说明<a name="ZH-CN_TOPIC_0000001809340016"></a>



### struct cJSON<a name="ZH-CN_TOPIC_0000001856138649"></a>

```
/* The cJSON structure: */
typedef struct cJSON
{
struct cJSON *next;
struct cJSON *prev;
struct cJSON *child;
int type;
char *valuestring;
/* writing to valueint is DEPRECATED, use cJSON_SetNumberValue instead */
int valueint;
double valuedouble;
char *string;
} cJSON;
```

### struct cJSON\_Hooks<a name="ZH-CN_TOPIC_0000001809340012"></a>

```
typedef struct cJSON_Hooks
{
/* malloc/free are CDECL on Windows regardless of the default calling convention of the compiler, so ensure the hooks allow passing those functions directly. */
void *(CJSON_CDECL *malloc_fn)(size_t sz);
void (CJSON_CDECL *free_fn)(void *ptr);
} cJSON_Hooks;
```

## API列表<a name="ZH-CN_TOPIC_0000001856138653"></a>

<a name="table898945520214"></a>
<table><thead align="left"><tr id="row8962561229"><th class="cellrowborder" valign="top" width="62.03999999999999%" id="mcps1.1.3.1.1"><p id="p1796195615215"><a name="p1796195615215"></a><a name="p1796195615215"></a>cJSON API</p>
</th>
<th class="cellrowborder" valign="top" width="37.96%" id="mcps1.1.3.1.2"><p id="p196135616212"><a name="p196135616212"></a><a name="p196135616212"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row12969564214"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p34281722101513"><a name="p34281722101513"></a><a name="p34281722101513"></a>CJSON_PUBLIC(const char*) cJSON_Version(void);</p>
</td>
<td class="cellrowborder" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p199612565214"><a name="p199612565214"></a><a name="p199612565214"></a>获得cJSON的版本。</p>
</td>
</tr>
<tr id="row49685614218"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p86661128131511"><a name="p86661128131511"></a><a name="p86661128131511"></a>CJSON_PUBLIC(void) cJSON_InitHooks(cJSON_Hooks* hooks);</p>
</td>
<td class="cellrowborder" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p996125618214"><a name="p996125618214"></a><a name="p996125618214"></a>初始化cJSON_Hooks。</p>
</td>
</tr>
<tr id="row19617565217"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p9860163613154"><a name="p9860163613154"></a><a name="p9860163613154"></a>CJSON_PUBLIC(cJSON *) cJSON_Parse(const char *value);</p>
</td>
<td class="cellrowborder" rowspan="2" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p12967561928"><a name="p12967561928"></a><a name="p12967561928"></a>将字符串解析成cJSON结构体。</p>
</td>
</tr>
<tr id="row4968566216"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p1796856228"><a name="p1796856228"></a><a name="p1796856228"></a>CJSON_PUBLIC(cJSON *) cJSON_ParseWithOpts(const char *value, const char **return_parse_end, cJSON_bool require_null_terminated);</p>
</td>
</tr>
<tr id="row149695613217"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p198261857171515"><a name="p198261857171515"></a><a name="p198261857171515"></a>CJSON_PUBLIC(char *) cJSON_Print(const cJSON *item);</p>
</td>
<td class="cellrowborder" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p159616565210"><a name="p159616565210"></a><a name="p159616565210"></a>将cJSON结构体转换成格式化的字符串。</p>
</td>
</tr>
<tr id="row129612568215"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p2962562211"><a name="p2962562211"></a><a name="p2962562211"></a>CJSON_PUBLIC(char *) cJSON_PrintUnformatted(const cJSON *item);</p>
</td>
<td class="cellrowborder" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p1296125618215"><a name="p1296125618215"></a><a name="p1296125618215"></a>将cJSON结构体转换成未格式化的字符串。</p>
</td>
</tr>
<tr id="row196956428"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p1087181311166"><a name="p1087181311166"></a><a name="p1087181311166"></a>CJSON_PUBLIC(char *) cJSON_PrintBuffered(const cJSON *item, int prebuffer, cJSON_bool fmt);</p>
</td>
<td class="cellrowborder" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p09685618215"><a name="p09685618215"></a><a name="p09685618215"></a>将cJSON结构体转换为字符串，通过入参传入预估结果尺寸和是否格式化。</p>
</td>
</tr>
<tr id="row89618561822"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p12541224169"><a name="p12541224169"></a><a name="p12541224169"></a>CJSON_PUBLIC(cJSON_bool) cJSON_PrintPreallocated(cJSON *item, char *buffer, const int length, const cJSON_bool format);</p>
</td>
<td class="cellrowborder" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p1697656225"><a name="p1697656225"></a><a name="p1697656225"></a>将cJSON结构体转换为字符串，并存入预先申请的长度确定的缓冲空间，通过传入参数决定是否格式化。</p>
</td>
</tr>
<tr id="row109745618217"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p297256023"><a name="p297256023"></a><a name="p297256023"></a>CJSON_PUBLIC(void) cJSON_Delete(cJSON *c);</p>
</td>
<td class="cellrowborder" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p159716567212"><a name="p159716567212"></a><a name="p159716567212"></a>删除cJSON结构体。</p>
</td>
</tr>
<tr id="row1097165612215"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p179561844101611"><a name="p179561844101611"></a><a name="p179561844101611"></a>CJSON_PUBLIC(int) cJSON_GetArraySize(const cJSON *array);</p>
</td>
<td class="cellrowborder" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p199725619218"><a name="p199725619218"></a><a name="p199725619218"></a>返回Array类型的大小，对Object类型也有效。</p>
</td>
</tr>
<tr id="row17973568210"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p17471175331610"><a name="p17471175331610"></a><a name="p17471175331610"></a>CJSON_PUBLIC(cJSON *) cJSON_GetArrayItem(const cJSON *array, int index);</p>
</td>
<td class="cellrowborder" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p3977562218"><a name="p3977562218"></a><a name="p3977562218"></a>返回Array类型的index的值，对Object类型也有效。</p>
</td>
</tr>
<tr id="row7977564211"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p15162661717"><a name="p15162661717"></a><a name="p15162661717"></a>CJSON_PUBLIC(cJSON *) cJSON_GetObjectItem(const cJSON * const object, const char * const string);</p>
</td>
<td class="cellrowborder" rowspan="2" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p14977567218"><a name="p14977567218"></a><a name="p14977567218"></a>使用string获得对应的value。</p>
</td>
</tr>
<tr id="row189719561326"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p12974569217"><a name="p12974569217"></a><a name="p12974569217"></a>CJSON_PUBLIC(cJSON *) cJSON_GetObjectItemCaseSensitive(const cJSON * const object, const char * const string);</p>
</td>
</tr>
<tr id="row39712561210"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p15456125171716"><a name="p15456125171716"></a><a name="p15456125171716"></a>CJSON_PUBLIC(cJSON_bool) cJSON_HasObjectItem(const cJSON *object, const char *string);</p>
</td>
<td class="cellrowborder" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p39711561211"><a name="p39711561211"></a><a name="p39711561211"></a>判断string在Object中是否存在。</p>
</td>
</tr>
<tr id="row39785617210"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p159733211711"><a name="p159733211711"></a><a name="p159733211711"></a>CJSON_PUBLIC(const char *) cJSON_GetErrorPtr(void);</p>
</td>
<td class="cellrowborder" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p397195615219"><a name="p397195615219"></a><a name="p397195615219"></a>获得错误信息。</p>
</td>
</tr>
<tr id="row1916919167418"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p5169101614414"><a name="p5169101614414"></a><a name="p5169101614414"></a>CJSON_PUBLIC(char *) cJSON_GetStringValue(cJSON *item);</p>
</td>
<td class="cellrowborder" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p141697167411"><a name="p141697167411"></a><a name="p141697167411"></a>判断Item是否为字符串，返回键值字符串或NULL。</p>
</td>
</tr>
<tr id="row10971656324"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p5811104801717"><a name="p5811104801717"></a><a name="p5811104801717"></a>CJSON_PUBLIC(cJSON_bool) cJSON_IsInvalid(const cJSON * const item);</p>
</td>
<td class="cellrowborder" rowspan="10" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p597145614213"><a name="p597145614213"></a><a name="p597145614213"></a>Item类型判断。</p>
</td>
</tr>
<tr id="row16971756529"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p1151165416174"><a name="p1151165416174"></a><a name="p1151165416174"></a>CJSON_PUBLIC(cJSON_bool) cJSON_IsFalse(const cJSON * const item);</p>
</td>
</tr>
<tr id="row109715569218"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p133839011816"><a name="p133839011816"></a><a name="p133839011816"></a>CJSON_PUBLIC(cJSON_bool) cJSON_IsTrue(const cJSON * const item);</p>
</td>
</tr>
<tr id="row11971561722"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p1647205111814"><a name="p1647205111814"></a><a name="p1647205111814"></a>CJSON_PUBLIC(cJSON_bool) cJSON_IsBool(const cJSON * const item);</p>
</td>
</tr>
<tr id="row1197155618219"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p2941101221819"><a name="p2941101221819"></a><a name="p2941101221819"></a>CJSON_PUBLIC(cJSON_bool) cJSON_IsNull(const cJSON * const item);</p>
</td>
</tr>
<tr id="row69705614218"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p1192721831816"><a name="p1192721831816"></a><a name="p1192721831816"></a>CJSON_PUBLIC(cJSON_bool) cJSON_IsNumber(const cJSON * const item);</p>
</td>
</tr>
<tr id="row189717569219"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p998592361818"><a name="p998592361818"></a><a name="p998592361818"></a>CJSON_PUBLIC(cJSON_bool) cJSON_IsString(const cJSON * const item);</p>
</td>
</tr>
<tr id="row29716561628"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p3954201114190"><a name="p3954201114190"></a><a name="p3954201114190"></a>CJSON_PUBLIC(cJSON_bool) cJSON_IsArray(const cJSON * const item);</p>
</td>
</tr>
<tr id="row497856421"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p895551614199"><a name="p895551614199"></a><a name="p895551614199"></a>CJSON_PUBLIC(cJSON_bool) cJSON_IsObject(const cJSON * const item);</p>
</td>
</tr>
<tr id="row199719564211"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p20279132318197"><a name="p20279132318197"></a><a name="p20279132318197"></a>CJSON_PUBLIC(cJSON_bool) cJSON_IsRaw(const cJSON * const item);</p>
</td>
</tr>
<tr id="row129717561627"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p44139308193"><a name="p44139308193"></a><a name="p44139308193"></a>CJSON_PUBLIC(cJSON *) cJSON_CreateNull(void);</p>
</td>
<td class="cellrowborder" rowspan="9" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p15978569212"><a name="p15978569212"></a><a name="p15978569212"></a>创造对应类型的Item。</p>
</td>
</tr>
<tr id="row139715617214"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p19464836101916"><a name="p19464836101916"></a><a name="p19464836101916"></a>CJSON_PUBLIC(cJSON *) cJSON_CreateTrue(void);</p>
</td>
</tr>
<tr id="row109718561216"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p11824184181912"><a name="p11824184181912"></a><a name="p11824184181912"></a>CJSON_PUBLIC(cJSON *) cJSON_CreateFalse(void);</p>
</td>
</tr>
<tr id="row5972561217"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p152841747141913"><a name="p152841747141913"></a><a name="p152841747141913"></a>CJSON_PUBLIC(cJSON *) cJSON_CreateBool(cJSON_bool boolean);</p>
</td>
</tr>
<tr id="row1597656421"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p1567214521199"><a name="p1567214521199"></a><a name="p1567214521199"></a>CJSON_PUBLIC(cJSON *) cJSON_CreateNumber(double num);</p>
</td>
</tr>
<tr id="row8971561025"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p9400165811196"><a name="p9400165811196"></a><a name="p9400165811196"></a>CJSON_PUBLIC(cJSON *) cJSON_CreateString(const char *string);</p>
</td>
</tr>
<tr id="row8971956920"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p178536418202"><a name="p178536418202"></a><a name="p178536418202"></a>CJSON_PUBLIC(cJSON *) cJSON_CreateRaw(const char *raw);</p>
</td>
</tr>
<tr id="row1398165611220"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p62492114204"><a name="p62492114204"></a><a name="p62492114204"></a>CJSON_PUBLIC(cJSON *) cJSON_CreateArray(void);</p>
</td>
</tr>
<tr id="row79813561224"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p3616184207"><a name="p3616184207"></a><a name="p3616184207"></a>CJSON_PUBLIC(cJSON *) cJSON_CreateObject(void);</p>
</td>
</tr>
<tr id="row109810569218"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p09815565215"><a name="p09815565215"></a><a name="p09815565215"></a>CJSON_PUBLIC(cJSON *) cJSON_CreateIntArray(const int *numbers, int count);</p>
</td>
<td class="cellrowborder" rowspan="4" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p1098756325"><a name="p1098756325"></a><a name="p1098756325"></a>批量创造包含对应类型和数量Items的Array。</p>
</td>
</tr>
<tr id="row11983561524"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p141117102113"><a name="p141117102113"></a><a name="p141117102113"></a>CJSON_PUBLIC(cJSON *) cJSON_CreateFloatArray(const float *numbers, int count);</p>
</td>
</tr>
<tr id="row1498056326"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p998456924"><a name="p998456924"></a><a name="p998456924"></a>CJSON_PUBLIC(cJSON *) cJSON_CreateDoubleArray(const double *numbers, int count);</p>
</td>
</tr>
<tr id="row159811563214"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p1698056329"><a name="p1698056329"></a><a name="p1698056329"></a>CJSON_PUBLIC(cJSON *) cJSON_CreateStringArray(const char **strings, int count);</p>
</td>
</tr>
<tr id="row39812567218"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p6216172320218"><a name="p6216172320218"></a><a name="p6216172320218"></a>CJSON_PUBLIC(void) cJSON_AddItemToArray(cJSON *array, cJSON *item);</p>
</td>
<td class="cellrowborder" rowspan="5" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p798165616211"><a name="p798165616211"></a><a name="p798165616211"></a>向Array或Object增加Item。</p>
</td>
</tr>
<tr id="row1198125618211"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p153323062116"><a name="p153323062116"></a><a name="p153323062116"></a>CJSON_PUBLIC(void) cJSON_AddItemToObject(cJSON *object, const char *string, cJSON *item);</p>
</td>
</tr>
<tr id="row12982561228"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p146324811219"><a name="p146324811219"></a><a name="p146324811219"></a>CJSON_PUBLIC(void) cJSON_AddItemToObjectCS(cJSON *object, const char *string, cJSON *item);</p>
</td>
</tr>
<tr id="row189815611213"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p818125513218"><a name="p818125513218"></a><a name="p818125513218"></a>CJSON_PUBLIC(void) cJSON_AddItemReferenceToArray(cJSON *array, cJSON *item);</p>
</td>
</tr>
<tr id="row2145194893415"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p26310543345"><a name="p26310543345"></a><a name="p26310543345"></a>CJSON_PUBLIC(void) cJSON_AddItemReferenceToObject(cJSON *object, const char *string, cJSON *item);</p>
</td>
</tr>
<tr id="row119855618218"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p189819569213"><a name="p189819569213"></a><a name="p189819569213"></a>CJSON_PUBLIC(cJSON *) cJSON_DetachItemViaPointer(cJSON *parent, cJSON * const item);</p>
</td>
<td class="cellrowborder" rowspan="7" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p12529155653315"><a name="p12529155653315"></a><a name="p12529155653315"></a>从Arrays或Objects中删除Items。</p>
</td>
</tr>
<tr id="row109812561622"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p1029431302216"><a name="p1029431302216"></a><a name="p1029431302216"></a>CJSON_PUBLIC(cJSON *) cJSON_DetachItemFromArray(cJSON *array, int which);</p>
</td>
</tr>
<tr id="row1598175614217"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p112974198223"><a name="p112974198223"></a><a name="p112974198223"></a>CJSON_PUBLIC(void) cJSON_DeleteItemFromArray(cJSON *array, int which);</p>
</td>
</tr>
<tr id="row1398185614218"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p109816561128"><a name="p109816561128"></a><a name="p109816561128"></a>CJSON_PUBLIC(cJSON *) cJSON_DetachItemFromObject(cJSON *object, const char *string);</p>
</td>
</tr>
<tr id="row149818561728"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p941603420224"><a name="p941603420224"></a><a name="p941603420224"></a>CJSON_PUBLIC(cJSON *) cJSON_DetachItemFromObjectCaseSensitive(cJSON *object, const char *string);</p>
</td>
</tr>
<tr id="row199815561022"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p159811565213"><a name="p159811565213"></a><a name="p159811565213"></a>CJSON_PUBLIC(void) cJSON_DeleteItemFromObject(cJSON *object, const char *string);</p>
</td>
</tr>
<tr id="row49811568216"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p1982056724"><a name="p1982056724"></a><a name="p1982056724"></a>CJSON_PUBLIC(void) cJSON_DeleteItemFromObjectCaseSensitive(cJSON *object, const char *string);</p>
</td>
</tr>
<tr id="row498145620214"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p1747175742219"><a name="p1747175742219"></a><a name="p1747175742219"></a>CJSON_PUBLIC(void) cJSON_InsertItemInArray(cJSON *array, int which, cJSON *newitem);</p>
</td>
<td class="cellrowborder" rowspan="5" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p1962154716326"><a name="p1962154716326"></a><a name="p1962154716326"></a>更新Items。</p>
</td>
</tr>
<tr id="row59845618210"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p1396217147237"><a name="p1396217147237"></a><a name="p1396217147237"></a>CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemViaPointer(cJSON * const parent, cJSON * const item, cJSON * replacement);</p>
</td>
</tr>
<tr id="row6981856521"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p99731522112320"><a name="p99731522112320"></a><a name="p99731522112320"></a>CJSON_PUBLIC(void) cJSON_ReplaceItemInArray(cJSON *array, int which, cJSON *newitem);</p>
</td>
</tr>
<tr id="row69816561021"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p139861029122318"><a name="p139861029122318"></a><a name="p139861029122318"></a>CJSON_PUBLIC(void) cJSON_ReplaceItemInObject(cJSON *object,const char *string,cJSON *newitem);</p>
</td>
</tr>
<tr id="row14983561829"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p95102383234"><a name="p95102383234"></a><a name="p95102383234"></a>CJSON_PUBLIC(void) cJSON_ReplaceItemInObjectCaseSensitive(cJSON *object,const char *string,cJSON *newitem);</p>
</td>
</tr>
<tr id="row119905610220"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p1111155012316"><a name="p1111155012316"></a><a name="p1111155012316"></a>CJSON_PUBLIC(cJSON *) cJSON_Duplicate(const cJSON *item, cJSON_bool recurse);</p>
</td>
<td class="cellrowborder" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p169975611212"><a name="p169975611212"></a><a name="p169975611212"></a>复制cJSON结构体。</p>
</td>
</tr>
<tr id="row139935612210"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p14939145562315"><a name="p14939145562315"></a><a name="p14939145562315"></a>CJSON_PUBLIC(cJSON_bool) cJSON_Compare(const cJSON * const a, const cJSON * const b, const cJSON_bool case_sensitive);</p>
</td>
<td class="cellrowborder" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p79912561425"><a name="p79912561425"></a><a name="p79912561425"></a>比较两个cJSON结构体。</p>
</td>
</tr>
<tr id="row12998561424"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p82797122417"><a name="p82797122417"></a><a name="p82797122417"></a>CJSON_PUBLIC(void) cJSON_Minify(char *json);</p>
</td>
<td class="cellrowborder" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p1699145613218"><a name="p1699145613218"></a><a name="p1699145613218"></a>将格式化的字符串压缩。</p>
</td>
</tr>
<tr id="row199905614217"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p84229155243"><a name="p84229155243"></a><a name="p84229155243"></a>CJSON_PUBLIC(cJSON*) cJSON_AddNullToObject(cJSON * const object, const char * const name);</p>
</td>
<td class="cellrowborder" rowspan="9" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p97771742712"><a name="p97771742712"></a><a name="p97771742712"></a>用于同时创建对象和将Items添加到Object。返回添加的Items或返回NULL表示失败。</p>
</td>
</tr>
<tr id="row149965611212"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p23281323162420"><a name="p23281323162420"></a><a name="p23281323162420"></a>CJSON_PUBLIC(cJSON*) cJSON_AddTrueToObject(cJSON * const object, const char * const name);</p>
</td>
</tr>
<tr id="row69945616220"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p4504122920242"><a name="p4504122920242"></a><a name="p4504122920242"></a>CJSON_PUBLIC(cJSON*) cJSON_AddFalseToObject(cJSON * const object, const char * const name);</p>
</td>
</tr>
<tr id="row17100956423"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p196746364246"><a name="p196746364246"></a><a name="p196746364246"></a>CJSON_PUBLIC(cJSON*) cJSON_AddBoolToObject(cJSON * const object, const char * const name, const cJSON_bool boolean);</p>
</td>
</tr>
<tr id="row710075611220"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p297534482415"><a name="p297534482415"></a><a name="p297534482415"></a>CJSON_PUBLIC(cJSON*) cJSON_AddNumberToObject(cJSON * const object, const char * const name, const double number);</p>
</td>
</tr>
<tr id="row101001356923"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p77921515248"><a name="p77921515248"></a><a name="p77921515248"></a>CJSON_PUBLIC(cJSON*) cJSON_AddStringToObject(cJSON * const object, const char * const name, const char * const string);</p>
</td>
</tr>
<tr id="row210017561528"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p11668857152412"><a name="p11668857152412"></a><a name="p11668857152412"></a>CJSON_PUBLIC(cJSON*) cJSON_AddRawToObject(cJSON * const object, const char * const name, const char * const raw);</p>
</td>
</tr>
<tr id="row57771712272"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p4777167172711"><a name="p4777167172711"></a><a name="p4777167172711"></a>CJSON_PUBLIC(cJSON*) cJSON_AddObjectToObject(cJSON * const object, const char * const name);</p>
</td>
</tr>
<tr id="row31521554272"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p10535191813274"><a name="p10535191813274"></a><a name="p10535191813274"></a>CJSON_PUBLIC(cJSON*) cJSON_AddArrayToObject(cJSON * const object, const char * const name);</p>
</td>
</tr>
<tr id="row10100195613215"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p8444811477"><a name="p8444811477"></a><a name="p8444811477"></a>cJSON_SetIntValue(object, number)</p>
</td>
<td class="cellrowborder" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p132575083210"><a name="p132575083210"></a><a name="p132575083210"></a>设置整形键值，同时设置双精度浮点型键值。</p>
</td>
</tr>
<tr id="row171001856321"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p185462215473"><a name="p185462215473"></a><a name="p185462215473"></a>cJSON_SetNumberValue(object, number)</p>
</td>
<td class="cellrowborder" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p1767312594224"><a name="p1767312594224"></a><a name="p1767312594224"></a>设置双精度浮点型键值，同时设置整形键值。</p>
</td>
</tr>
<tr id="row9175193714617"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p1917512378461"><a name="p1917512378461"></a><a name="p1917512378461"></a>cJSON_ArrayForEach(element, array)</p>
</td>
<td class="cellrowborder" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p2039314610238"><a name="p2039314610238"></a><a name="p2039314610238"></a>遍历数组。</p>
</td>
</tr>
<tr id="row2010012563220"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p9364243112517"><a name="p9364243112517"></a><a name="p9364243112517"></a>CJSON_PUBLIC(double) cJSON_SetNumberHelper(cJSON *object, double number);</p>
</td>
<td class="cellrowborder" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p162561006325"><a name="p162561006325"></a><a name="p162561006325"></a>设置整形和双精度浮点型键值，超过极限值的输入将按照极限值设置。</p>
</td>
</tr>
<tr id="row14100175618210"><td class="cellrowborder" valign="top" width="62.03999999999999%" headers="mcps1.1.3.1.1 "><p id="p4100656927"><a name="p4100656927"></a><a name="p4100656927"></a>CJSON_PUBLIC(void *) cJSON_malloc(size_t size);</p>
</td>
<td class="cellrowborder" rowspan="2" valign="top" width="37.96%" headers="mcps1.1.3.1.2 "><p id="p11004561029"><a name="p11004561029"></a><a name="p11004561029"></a>使用cJSON_InitHooks中注册的函数实现malloc/free操作。</p>
</td>
</tr>
<tr id="row19461165232519"><td class="cellrowborder" valign="top" headers="mcps1.1.3.1.1 "><p id="p5462155216252"><a name="p5462155216252"></a><a name="p5462155216252"></a>CJSON_PUBLIC(void) cJSON_free(void *object);</p>
</td>
</tr>
</tbody>
</table>

# 开发指南<a name="ZH-CN_TOPIC_0000001809499872"></a>

以下为组织Json字符串和解析Json字符串的官方代码样例。



## 创建Json字符串示例<a name="ZH-CN_TOPIC_0000001856218665"></a>

```
//NOTE: Returns a heap allocated string, you are required to free it after use.
char *create_monitor_with_helpers(void)
{
    const unsigned int resolution_numbers[3][2] = {
        {1280, 720},
        {1920, 1080},
        {3840, 2160}
    };
    char *string = NULL;
    cJSON *resolutions = NULL;
    size_t index = 0;

    cJSON *monitor = cJSON_CreateObject();

    if (cJSON_AddStringToObject(monitor, "name", "Awesome 4K") == NULL)
    {
        goto end;
    }

    resolutions = cJSON_AddArrayToObject(monitor, "resolutions");
    if (resolutions == NULL)
    {
        goto end;
    }

    for (index = 0; index < (sizeof(resolution_numbers) / (2 * sizeof(int))); ++index)
    {
        cJSON *resolution = cJSON_CreateObject();

        if (cJSON_AddNumberToObject(resolution, "width", resolution_numbers[index][0]) == NULL)
        {
            goto end;
        }

        if(cJSON_AddNumberToObject(resolution, "height", resolution_numbers[index][1]) == NULL)
        {
            goto end;
        }

        cJSON_AddItemToArray(resolutions, resolution);
    }

    string = cJSON_Print(monitor);
    if (string == NULL) {
        fprintf(stderr, "Failed to print monitor.\n");
    }

end:
    cJSON_Delete(monitor);
    return string;
}
```

## 解析Json字符串示例<a name="ZH-CN_TOPIC_0000001809499876"></a>

```
/* return 1 if the monitor supports full hd, 0 otherwise */
int supports_full_hd(const char * const monitor)
{
    const cJSON *resolution = NULL;
    const cJSON *resolutions = NULL;
    const cJSON *name = NULL;
    int status = 0;
    cJSON *monitor_json = cJSON_Parse(monitor);
    if (monitor_json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        status = 0;
        goto end;
    }

    name = cJSON_GetObjectItemCaseSensitive(monitor_json, "name");
    if (cJSON_IsString(name) && (name->valuestring != NULL))
    {
        printf("Checking monitor \"%s\"\n", name->valuestring);
    }

    resolutions = cJSON_GetObjectItemCaseSensitive(monitor_json, "resolutions");
    cJSON_ArrayForEach(resolution, resolutions)
    {
        cJSON *width = cJSON_GetObjectItemCaseSensitive(resolution, "width");
        cJSON *height = cJSON_GetObjectItemCaseSensitive(resolution, "height");

        if (!cJSON_IsNumber(width) || !cJSON_IsNumber(height))
        {
            status = 0;
            goto end;
        }

        if ((width->valuedouble == 1920) && (height->valuedouble == 1080))
        {
            status = 1;
            goto end;
        }
    }

end:
    cJSON_Delete(monitor_json);
    return status;
}
```

