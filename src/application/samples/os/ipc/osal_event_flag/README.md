# OSAL Event Flag 示例

## 功能说明

本示例演示多个任务通过事件标志进行同步：

- Task A 延时 1 秒后置位 `EVENT_FLAG_TASK_A_READY`。
- Task B 延时 2 秒后置位 `EVENT_FLAG_TASK_B_READY`。
- Main Task 使用 `OSAL_WAITMODE_AND` 等待两个事件同时置位，并在读取后清除事件。

## 配置与编译

在 `menuconfig` 中选择：

```text
Application
  Enable Sample
    Enable the Sample of OS.
      Support OSAL event flag Sample.
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

烧录固件并复位开发板，串口将依次输出：

```text
[event_flag] started, waiting mask=0x3
[event_flag] task A ready
[event_flag] task B ready
[event_flag] all tasks ready, mask=0x3
```

出现 `all tasks ready` 表示 AND 等待和事件清除流程执行成功。
