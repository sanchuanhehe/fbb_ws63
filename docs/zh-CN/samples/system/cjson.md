# cJSON 解析

> cJSON v1.7.15

## 学习目标

- 理解 cJSON 的核心用途——解析云端下发的 JSON 配置、构造传感器数据上报的 JSON 报文
- 掌握 JSON 解析：`cJSON_Parse()` → `cJSON_GetObjectItem()` → 提取值 → `cJSON_Delete()`
- 掌握 JSON 构造：`cJSON_CreateObject()` → `cJSON_AddXxxToObject()` → `cJSON_Print()` → `cJSON_Delete()`
- 理解 cJSON 内存管理——每次 Parse/Create 必须对应 Delete/Free

## 基本概念

### cJSON 的设计理念

轻量级单文件 JSON 解析器——树状结构，`cJSON_Parse()` 将 JSON 字符串解析为 cJSON 节点树，通过 key 查找子节点。

```mermaid
flowchart LR
    JSON["'{\"temp\":25.5, \"humidity\":60}'"] --> PARSE[cJSON_Parse]
    PARSE --> ROOT[cJSON 根节点]
    ROOT --> TEMP["cJSON_GetObjectItem(root, \"temp\")"]
    ROOT --> HUM["cJSON_GetObjectItem(root, \"humidity\")"]
    TEMP --> V1["item->valuedouble = 25.5"]
    HUM --> V2["item->valueint = 60"]
    V1 --> DEL[cJSON_Delete(root)]
    V2 --> DEL
```

### 典型使用场景

| 场景 | 说明 |
|------|------|
| 云端下发配置 | MQTT (Message Queuing Telemetry Transport) 收到 `{"interval":5000}` → 解析 interval |
| 传感器上报 | 构造 `{"temp":25.5,"humidity":60}` → MQTT publish |
| OTA (Over-The-Air) 元数据 | 解析 `{"version":"2.0.1","size":524288}` |

### 解析 vs 构造

解析：JSON 字符串 → `cJSON_Parse()` → cJSON 树 → `cJSON_GetObjectItem()` 提取值
构造：`cJSON_CreateObject()` → `cJSON_AddXxxToObject()` → `cJSON_Print()` → JSON 字符串

### 内存管理陷阱

每个 `cJSON_Parse()` 返回的根节点必须用 `cJSON_Delete(root)` 释放。`cJSON_Print()` 返回的字符串必须用 `cJSON_free(str)` 释放。**忘记释放是内存泄漏最常见的原因**。

## 涉及 API

| API | 用途 | 内存 |
|-----|------|:---:|
| `cJSON_Parse(str)` | 将 JSON 字符串解析为节点树 | 需 Delete |
| `cJSON_GetObjectItem(obj, key)` | 按 key 查找子节点 | — |
| `cJSON_GetArraySize(arr)` | 获取数组长度 | — |
| `cJSON_GetArrayItem(arr, index)` | 按索引取数组元素 | — |
| `cJSON_GetStringValue(item)` | 获取 String 值 | — |
| `cJSON_IsNumber(item)` / `item->valueint` / `item->valuedouble` | 获取 Number 值 | — |
| `cJSON_CreateObject()` | 创建 `{}` | 需 Delete |
| `cJSON_CreateArray()` | 创建 `[]` | 需 Delete |
| `cJSON_AddStringToObject(obj, key, val)` | 添加 String 字段 | — |
| `cJSON_AddNumberToObject(obj, key, num)` | 添加 Number 字段 | — |
| `cJSON_AddItemToArray(arr, item)` | 数组添加元素 | — |
| `cJSON_Print(item)` | 序列化为 JSON 字符串 | 需 free |
| `cJSON_PrintUnformatted(item)` | 序列化（无缩进） | 需 free |
| `cJSON_Delete(item)` | 释放节点树 | — |
| `cJSON_free(ptr)` | 释放 Print 返回的字符串 | — |

## 案例说明

### 案例简介

**子场景 1 — 解析云端配置**：串口输入 `{"interval": 5000, "threshold": 80}` → 解析 interval 和 threshold → 应用到设备参数。

**子场景 2 — 构造上报数据**：读取传感器 → 构造 `{"temp": 25.5, "humidity": 60}` → 串口打印 JSON。

## 案例操作指导

### 集成 cJSON

添加 `cJSON.c` / `cJSON.h` 到工程，或在 Kconfig 中启用 cJSON 组件。

### 编译


```bash
fbb build ws63-liteos-app
```

### 验证

串口输入 `{"threshold":80}` → 解析输出 `threshold = 80`。构造的温度 JSON 正确输出。

## 关键配置

| 参数 | 值 | 说明 |
|------|:---:|------|
| 嵌套深度限制 | 256 | `CJSON_NESTING_LIMIT`，超出则解析失败 |
| 数字精度 | `double` | `item->valuedouble`；整数用 `item->valueint` |
| 单节点内存 | ~64B | 100 个节点 ≈ 6.4KB |

## 代码详解

### JSON 解析标准流程

```c
#include "cJSON.h"

void parse_config(const char *json_str) {
    /* 1. 解析 */
    cJSON *root = cJSON_Parse(json_str);
    if (root == NULL) {
        const char *err = cJSON_GetErrorPtr();
        printf("parse error at: %s\n", err ? err : "unknown");
        return;
    }

    /* 2. 提取字段——检查类型后取值 */
    cJSON *item = cJSON_GetObjectItem(root, "interval");
    if (item != NULL && cJSON_IsNumber(item)) {
        int interval = item->valueint;
        printf("interval = %d\n", interval);
    }

    item = cJSON_GetObjectItem(root, "threshold");
    if (item != NULL && cJSON_IsNumber(item)) {
        double threshold = item->valuedouble;
        printf("threshold = %.1f\n", threshold);
    }

    /* 3. 释放 */
    cJSON_Delete(root);  // 别忘了！
}
```

### JSON 构造标准流程

```c
void build_report(double temp, double humidity) {
    /* 1. 创建根对象 */
    cJSON *root = cJSON_CreateObject();

    /* 2. 添加字段 */
    cJSON_AddNumberToObject(root, "temp", temp);
    cJSON_AddNumberToObject(root, "humidity", humidity);

    /* 3. 序列化 */
    char *str = cJSON_PrintUnformatted(root);
    printf("report: %s\n", str);

    /* 4. 释放——顺序：先 free 字符串，再 Delete 树 */
    cJSON_free(str);
    cJSON_Delete(root);
}
```

### 嵌套 JSON 解析

```c
cJSON *sensor = cJSON_GetObjectItem(root, "sensor");
if (sensor != NULL) {
    cJSON *temp = cJSON_GetObjectItem(sensor, "temp");
    printf("sensor.temp = %d\n", temp->valueint);
}
```

### 数组遍历

```c
cJSON *arr = cJSON_GetObjectItem(root, "devices");
if (arr != NULL && cJSON_IsArray(arr)) {
    int count = cJSON_GetArraySize(arr);
    for (int i = 0; i < count; i++) {
        cJSON *item = cJSON_GetArrayItem(arr, i);
        cJSON *name = cJSON_GetObjectItem(item, "name");
        printf("device[%d]: %s\n", i, cJSON_GetStringValue(name));
    }
}
```

### 内存泄漏检测

```c
/* 每个 Parse / Create 必须匹配 Delete */
cJSON *root1 = cJSON_Parse(str1);   // alloc
// ... 使用 ...
cJSON_Delete(root1);                 // free —— 不能少！

cJSON *root2 = cJSON_CreateObject(); // alloc
char *s = cJSON_Print(root2);        // alloc
// ... 发送 s ...
cJSON_free(s);                       // free —— 不能少！
cJSON_Delete(root2);                 // free —— 不能少！
```

---

