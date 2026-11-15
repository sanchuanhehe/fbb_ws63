# OSAL Message Queue 示例

## 功能说明

本示例演示从中断到任务的消息传递流程：

1. Timer 2 每秒产生一次中断。
2. 中断处理函数清除中断并释放二值信号量。
3. Bridge Task 获取信号量，将事件写入消息队列。
4. Worker Task 从消息队列读取事件并打印序号。

中断处理函数只完成必要操作，消息构造和处理均放在任务上下文执行。

## 配置与编译

在 `menuconfig` 中选择：

```text
Application
  Enable Sample
    Enable the Sample of OS.
      Support OSAL interrupt-to-task message queue Sample.
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
[msg_queue] started: ISR -> semaphore -> queue -> worker
[msg_queue] worker received type=1 sequence=1
[msg_queue] worker received type=1 sequence=2
```

事件序号持续递增表示定时器中断、信号量和消息队列链路运行正常。

## 注意事项

示例占用 Timer 2 及其对应中断，和其他使用相同定时器的功能同时启用时需要重新分配定时器资源。
