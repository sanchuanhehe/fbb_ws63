# OSAL Mutex 示例

## 功能说明

本示例创建两个同优先级任务。两个任务周期性申请同一个互斥锁，并在临界区内打印日志，用于演示：

- 互斥锁初始化和销毁。
- 带超时的互斥锁申请。
- 多任务对共享资源的排他访问。

## 配置与编译

在 `menuconfig` 中选择：

```text
Application
  Enable Sample
    Enable the Sample of OS.
      Support OSAL mutex Sample.
```

在 `src` 目录执行：

```bash
python build.py -c ws63-liteos-app
```

生成的固件位于：

```text
output/ws63/fwpkg/ws63-liteos-app/ws63-liteos-app_all.fwpkg
```

## 运行结果

烧录固件并复位开发板，串口将输出：

```text
[mutex] started
[mutex] TaskA: Hello World
[mutex] TaskB: Hello World
```

Task A 和 Task B 持续交替输出且没有出现 `lock timeout`，表示共享打印资源受到互斥锁保护。
