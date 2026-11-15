---
title: 文件系统 使用指南
---

# 前言<a name="ZH-CN_TOPIC_0000001787758664"></a>

**概述<a name="section4537382116410"></a>**

本文档主要针对WS63V100中LITTLE FILE SYSTEM（下简称LFS）文件系统模块的使用进行介绍。用于指导工程人员能够快速使用文件系统模块进行二次开发。

# LFS简介<a name="ZH-CN_TOPIC_0000001834437381"></a>

LFS文件系统是为小型嵌入式系统创建的一个文件系统，为用户提供了文件的打开、关闭、读取、写入、删除等功能，用户可以通过使用这些接口，将数据存储到NOR FLASH。

-   掉电恢复能力- LFS旨在处理随机掉电。所有文件操作都有强大的写时拷贝保证，如果断电，文件系统将回退到最后一个已知的良好状态。
-   动态磨损均衡- LFS在设计时考虑到了flash，并在动态块上提供了磨损均衡。此外，LFS可以检测坏块并解决它们。
-   有界RAM/ROM - LFS设计用于使用少量内存。RAM的使用是有严格限制的，这意味着RAM的使用不会随着文件系统的增长而改变。文件系统不包含无界递归，动态内存仅限于可以静态提供的可配置缓冲区。

# LFS编译预置<a name="ZH-CN_TOPIC_0000001834517317"></a>

当前SDK中提供了LFS特性，但默认不编译打开，如需使用，需按以下指导进行编译及特性配置

1.  加入编译。在需要LFS特性的编译目标中加入组件 'little\_fs', 'littlefs\_adapt\_ws63'
2.  特性宏。使用menuconfig，在需要LFS特性的编译目标中打开该特性对应的特性宏，特性宏配置路径：middleware-\>chip-\>Choose Chip \(ws63\)-\> Chip Configurations for ws63-\>打开littlefs adapt，即可打开CONFIG\_MIDDLEWARE\_SUPPORT\_LFS特性宏，打开宏后，LFS会在运行过程中自行完成挂载操作
3.  FLASH分配，关注代码（文件路径：middleware/chips/ws63/littlefs/littlefs\_adapt.c）接口littlefs\_adapt\_get\_block\_info 读取分区表信息对应分区为

    CONFIG\_LFS\_PARTITION\_ID；

    该宏需要替换或修改为分区表ID文件\(middleware/chips/ws63/partition/include/partition\_resource\_id.h\)中其他的分区ID宏，并在分区表配置文件（build/config/target\_config/ws63/param\_sector/param\_sector.json）中对该宏ID对应的FLASH地址与大小进行配置，作为LFS的运行基础；

    配置方式为通过menuconfig配置，在打开CONFIG\_MIDDLEWARE\_SUPPORT\_LFS宏后，会自动显示该宏，默认值为0x21,修改为分配的对应ID即可，不过在menuconfig中只允许输入十进制数，注意进制转换；需要注意的是，每次调整LFS的地址后，LFS中的内容会丢失；

4.  其他适配，为适配不同的上层VFS，对打开文件传入的oflag参数进行了转化，使得能够适配LFS的下层实现；为保证正常使用，需要重新实现fs\_adapt\_flag\_format接口来适配上层flag。
5.  支持posix文件接口函数open,read,write,lseek,close，该配置可按需开关，默认关闭。

    可以通过menuconfig配置“Middleware-\>Chips-\>Chip Configurations for ws63-\>littlefs adapt-\>littlefs support posix interface ”打开对应特性宏。在不冲突的情况下，能够支持open、read、write、lseek、close、unlink、sync接口。

# LFS接口API<a name="ZH-CN_TOPIC_0000001787757768"></a>



## API<a name="ZH-CN_TOPIC_0000001834600269"></a>

当前提供的API不代表LFS的所有能力，请按需使用

**表 1** 

<a name="table1316241613216"></a>
<table><thead align="left"><tr id="row201632164215"><th class="cellrowborder" colspan="2" valign="top" id="mcps1.2.4.1.1"><p id="p1316381682117"><a name="p1316381682117"></a><a name="p1316381682117"></a>功能分类</p>
</th>
<th class="cellrowborder" valign="top" id="mcps1.2.4.1.2"><p id="p14163116102110"><a name="p14163116102110"></a><a name="p14163116102110"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row7163316182111"><td class="cellrowborder" rowspan="2" align="center" valign="top" width="12.55125512551255%" headers="mcps1.2.4.1.1 "><p id="p1163151682110"><a name="p1163151682110"></a><a name="p1163151682110"></a>文件系统挂载/去挂载</p>
</td>
<td class="cellrowborder" valign="top" width="24.162416241624165%" headers="mcps1.2.4.1.1 "><p id="p1648161192411"><a name="p1648161192411"></a><a name="p1648161192411"></a>fs_adapt_mount</p>
</td>
<td class="cellrowborder" valign="top" width="63.28632863286329%" headers="mcps1.2.4.1.2 "><p id="p1116321618214"><a name="p1116321618214"></a><a name="p1116321618214"></a>挂载文件系统。</p>
</td>
</tr>
<tr id="row18485191247"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p690431920243"><a name="p690431920243"></a><a name="p690431920243"></a>fs_adapt_unmount</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p164871922415"><a name="p164871922415"></a><a name="p164871922415"></a>去挂载文件系统。</p>
</td>
</tr>
<tr id="row716314165211"><td class="cellrowborder" rowspan="5" valign="top" width="12.55125512551255%" headers="mcps1.2.4.1.1 "><p id="p19163181616217"><a name="p19163181616217"></a><a name="p19163181616217"></a>文件I/O操作</p>
</td>
<td class="cellrowborder" valign="top" width="24.162416241624165%" headers="mcps1.2.4.1.1 "><p id="p54371175260"><a name="p54371175260"></a><a name="p54371175260"></a>fs_adapt_open</p>
</td>
<td class="cellrowborder" valign="top" width="63.28632863286329%" headers="mcps1.2.4.1.2 "><p id="p31639161219"><a name="p31639161219"></a><a name="p31639161219"></a>打开一个文件，如果不存在，根据传入的oflag参数决定是否创建文件，该oflag是已进行转换后的参数，如果path参数中包含的路径不存在，则会默认创建该路径。</p>
</td>
</tr>
<tr id="row1232126112613"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p535617203267"><a name="p535617203267"></a><a name="p535617203267"></a>fs_adapt_close</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1123286172620"><a name="p1123286172620"></a><a name="p1123286172620"></a>关闭一个文件句柄。</p>
</td>
</tr>
<tr id="row1374910244267"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1990122792612"><a name="p1990122792612"></a><a name="p1990122792612"></a>fs_adapt_read</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p7177111618291"><a name="p7177111618291"></a><a name="p7177111618291"></a>读文件操作，成功返回读取的字节数，失败返回-1并设置错误码，如果在调用该接口前已到达文件末尾，则此次read返回0。</p>
</td>
</tr>
<tr id="row1772042252611"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1911153022617"><a name="p1911153022617"></a><a name="p1911153022617"></a>fs_adapt_write</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p15721182212618"><a name="p15721182212618"></a><a name="p15721182212618"></a>成功返回写入的字节数，出错返回-1并设置错误码。</p>
</td>
</tr>
<tr id="row1950483142613"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p831363312615"><a name="p831363312615"></a><a name="p831363312615"></a>fs_adapt_delete</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p115041932260"><a name="p115041932260"></a><a name="p115041932260"></a>通过路径名删除一个文件。</p>
</td>
</tr>
<tr id="row416371613215"><td class="cellrowborder" valign="top" width="12.55125512551255%" headers="mcps1.2.4.1.1 "><p id="p121636169211"><a name="p121636169211"></a><a name="p121636169211"></a>设置读/写偏移</p>
</td>
<td class="cellrowborder" valign="top" width="24.162416241624165%" headers="mcps1.2.4.1.1 "><p id="p8529153519267"><a name="p8529153519267"></a><a name="p8529153519267"></a>fs_adapt_seek</p>
</td>
<td class="cellrowborder" valign="top" width="63.28632863286329%" headers="mcps1.2.4.1.2 "><p id="p125423403015"><a name="p125423403015"></a><a name="p125423403015"></a>设置读/写文件偏移。</p>
<a name="ul023910419820"></a><a name="ul023910419820"></a><ul id="ul023910419820"><li>LFS_SEEK_SET：从文件头部开始偏移offset个字节。</li><li>LFS_SEEK_CUR：从文件当前读写的指针位置开始，增加offset个字节的偏移量。</li><li>LFS_SEEK_END：文件偏移量设置为文件的大小加上偏移量字节，偏移量offset只允许为负值</li></ul>
</td>
</tr>
<tr id="row18163716202118"><td class="cellrowborder" valign="top" width="12.55125512551255%" headers="mcps1.2.4.1.1 "><p id="p171639160214"><a name="p171639160214"></a><a name="p171639160214"></a>获取文件大小</p>
</td>
<td class="cellrowborder" valign="top" width="24.162416241624165%" headers="mcps1.2.4.1.1 "><p id="p11841389268"><a name="p11841389268"></a><a name="p11841389268"></a>fs_adapt_stat</p>
</td>
<td class="cellrowborder" valign="top" width="63.28632863286329%" headers="mcps1.2.4.1.2 "><p id="p916341615219"><a name="p916341615219"></a><a name="p916341615219"></a>通过文件名获取文件大小。</p>
</td>
</tr>
<tr id="row1249022215407"><td class="cellrowborder" valign="top" width="12.55125512551255%" headers="mcps1.2.4.1.1 "><p id="p14901322134019"><a name="p14901322134019"></a><a name="p14901322134019"></a>同步文件内容</p>
</td>
<td class="cellrowborder" valign="top" width="24.162416241624165%" headers="mcps1.2.4.1.1 "><p id="p15700734114013"><a name="p15700734114013"></a><a name="p15700734114013"></a>fs_adapt_sync</p>
</td>
<td class="cellrowborder" valign="top" width="63.28632863286329%" headers="mcps1.2.4.1.2 "><p id="p949012225400"><a name="p949012225400"></a><a name="p949012225400"></a><span>同步内存和片外存储，将缓冲数据写入到片外存储</span>。</p>
</td>
</tr>
<tr id="row18654205710404"><td class="cellrowborder" valign="top" width="12.55125512551255%" headers="mcps1.2.4.1.1 "><p id="p156549577409"><a name="p156549577409"></a><a name="p156549577409"></a>创建路径</p>
</td>
<td class="cellrowborder" valign="top" width="24.162416241624165%" headers="mcps1.2.4.1.1 "><p id="p186579024113"><a name="p186579024113"></a><a name="p186579024113"></a>fs_adapt_mkdir</p>
</td>
<td class="cellrowborder" valign="top" width="63.28632863286329%" headers="mcps1.2.4.1.2 "><p id="p1065405764016"><a name="p1065405764016"></a><a name="p1065405764016"></a>创建路径。</p>
</td>
</tr>
</tbody>
</table>

## 编程实例<a name="ZH-CN_TOPIC_0000001834640317"></a>

代码实现中提供一段基础调用示例lfs\_test，打开根目录下名为lfs\_test的文件，读取首个字节并按照整型数打印，字节加一后再写入到文件头，最后关闭文件；

```
void lfs_test(void)
{
    // read current count
    char boot_count = 0;
    int fp = fs_adapt_open("/lfs_test", O_RDWR | O_CREAT);
    if (fp < 0) {
        return;
    }
    int ret = fs_adapt_read(fp, &boot_count, sizeof(boot_count));
    lfs_debug_print_info("lfs_test read, ret = 0x%x\r\n", ret);
    // print the boot count
    lfs_debug_print_info("===========boot_count: %d=============\r\n", boot_count);
    // update boot count
    boot_count = (char)((uint8_t)boot_count + 1);
    ret = fs_adapt_seek(fp, 0, LFS_SEEK_SET);
    lfs_debug_print_info("lfs_test seek, ret = 0x%x\r\n", ret);
    if (ret < LFS_ERR_OK) {
        return;
    }
    ret = fs_adapt_write(fp, &boot_count, sizeof(boot_count));
    lfs_debug_print_info("lfs_test write, ret = 0x%x\r\n", ret);
    // remember the storage is not updated until the file is closed successfully
    ret = fs_adapt_close(fp);
    lfs_debug_print_info("lfs_test close, ret = 0x%x\r\n", ret);
    if (ret < LFS_ERR_OK) {
        return;
    }
}
```

