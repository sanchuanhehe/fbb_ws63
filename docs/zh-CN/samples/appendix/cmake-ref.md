# CMake 参考

> 类型：内容页（参考） | 更新：2026-06-15

## 学习目标

- 理解 WS63 SDK的 CMake 构建结构
- 掌握为自己的 sample 添加 CMakeLists.txt 的方法

## 基本概念

### 构建流程

`build.py` 调用 CMake → 生成 Makefile → make 编译 → 链接 → 生成 .bin。

### CMake 变量

| 变量 | 说明 |
|------|------|
| `SOURCES` | 源文件列表 |
| `INCLUDES` | 头文件路径 |
| `LIBS` | 链接库 |
| `CONFIG_*` | Kconfig 条件编译 |

## 关键配置示例

```cmake
set(SOURCES ${SOURCES} main.c)
set(INCLUDES ${INCLUDES} inc/)
if(CONFIG_MY_FEATURE)
    list(APPEND SOURCES feature.c)
endif()
```

## 案例说明

新建 sample "my_app" → CMakeLists.txt → `fbb build` 编译通过。

