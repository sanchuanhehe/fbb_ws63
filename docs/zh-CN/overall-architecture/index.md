---
hide:
  - toc
---

# 整体架构

本章节帮助你从整体上把握 WS63 SDK的软件架构与组成，包含软件分层、目录与代码边界、构建系统、LiteOS (Huawei LiteOS) 与 OSAL (Operating System Abstraction Layer)、启动流程、运行时任务和内存布局等内容。

## 快速导航

---

- [软件架构介绍](software-introduction.md)

    了解 WS63 SDK 的分层架构（应用层、中间件层、内核与驱动层）及 API 调用规则。

---

- [目录结构](source-tree/index.md)

    了解 SDK 源码组织方式，包括主应用入口、示例代码、内核、中间件等目录说明。

---

- [构建系统](build-output/index.md)

    了解基于 CMake + Kconfig 的组件化构建体系，使用 fbb CLI (Command Line Interface) 编译并生成可烧录的 .fwpkg 固件包。

---

- [OS 抽象层](kernel/os-abstract.md)

    了解 WS63 在 LiteOS 上提供的 OSAL 统一接口，以及应用为何应优先使用 OSAL 而不是 LiteOS 原生或 POSIX (Portable Operating System Interface) 接口。

---

- [LiteOS](kernel/liteos.md)

    了解华为轻量级 RTOS (Real-Time Operating System)，WS63 的默认内核。

---

- [启动流程](boot-flow/index.md)

    了解从上电 Boot ROM (Read-Only Memory) 到主应用运行 app_run() 的完整启动阶段。

---

- [运行时架构](runtime-architecture/index.md)

    了解 LiteOS 调度运行后的系统任务拓扑、执行上下文、通信边界、资源所有权和问题定位方法。

---

- [内存布局](memory-layout/index.md)

    了解 Flash 分区表与 RAM (Random Access Memory) 内存分配说明。
