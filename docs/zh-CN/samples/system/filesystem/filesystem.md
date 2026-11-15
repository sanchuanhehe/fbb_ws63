# LittleFS 文件系统

> 使用技术：LittleFS、VFS (Virtual File System)

## 学习目标

- 理解 LittleFS 在 WS63 上的定位——为 Flash 优化的嵌入式文件系统
- 掌握挂载/卸载/文件读写/目录操作的基本 API
- 理解 LittleFS 的磨损均衡和掉电保护特性

## 基本概念

### LittleFS vs FAT/EXT

| 特性 | LittleFS | FAT |
|------|:---:|:---:|
| 掉电保护 | 支持（copy-on-write） | 可能损坏 |
| 磨损均衡 | 内置 | 无 |
| 目录大小 | 动态增长 | 固定 |
| 设计目标 | NOR/NAND Flash | 磁盘/存储卡 |

### VFS 抽象层

通过标准 POSIX (Portable Operating System Interface) 接口（`mount`/`open`/`read`/`write`/`close`）访问 LittleFS——与 Linux 文件操作一致。

## 涉及 API

| API | 用途 |
|-----|------|
| `mount(NULL, "/data", "littlefs", 0, NULL)` | 挂载文件系统 |
| `open("/data/log.txt", O_CREAT \| O_WRONLY)` | 创建/打开文件 |
| `write(fd, buf, len)` / `read(fd, buf, len)` | 文件读写 |
| `close(fd)` / `unlink("/data/log.txt")` | 关闭/删除文件 |

## 案例说明

### 案例简介

挂载 `/data` 分区 → 创建文件写入日志 → 读取文件验证 → 卸载文件系统。

## 关键配置

| 参数 | 说明 |
|------|------|
| 分区大小 | Kconfig 中配置 `/data` 分区 |
| 文件数量 | 无硬限制（受 Flash 容量影响） |

## 代码详解

```c
#include <sys/mount.h>
#include <fcntl.h>
#include <unistd.h>

/* 挂载 */
mount(NULL, "/data", "littlefs", 0, NULL);

/* 创建并写入文件 */
int fd = open("/data/log.txt", O_CREAT | O_WRONLY);
const char *log = "sensor: 25.5C\n";
write(fd, log, strlen(log));
close(fd);

/* 读取文件 */
char buf[256];
fd = open("/data/log.txt", O_RDONLY);
int len = read(fd, buf, sizeof(buf) - 1);
buf[len] = '\0';
printf("log: %s\n", buf);
close(fd);

/* 卸载 */
umount("/data");
```

---

