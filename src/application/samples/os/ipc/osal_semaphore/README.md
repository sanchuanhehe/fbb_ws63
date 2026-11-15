# OSAL Semaphore 示例

## 功能说明

本示例通过计数信号量实现生产者和消费者同步：

- Producer Task 每秒释放一次信号量。
- Consumer Task 阻塞等待信号量。
- Consumer Task 每次被唤醒后递增并打印通知计数。

## 配置与编译

在 `menuconfig` 中选择：

```text
Application
  Enable Sample
    Enable the Sample of OS.
      Support OSAL semaphore Sample.
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
[semaphore] started
[semaphore] consumer notified: 1
[semaphore] consumer notified: 2
```

通知计数每秒递增表示生产者和消费者同步正常。
