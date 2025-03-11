# 星闪代码编译

## 星闪代码下载

- 打开Vscode，在“远程资源管理器”打开“fbb_ws63”子系统

  ![image-20250311110526878](../docs/pic/tools/image-20250311110526878.png)

- 在“终端”新建一个终端

  ![image-20250311110825993](../docs/pic/tools/image-20250311110825993.png)


- 在新终端中输入`git clone https://gitee.com/HiSpark/fbb_ws63.git`指令下载代码，等待下载完成。

![image-20240807151920249](../docs/pic/tools/image-20240807151920249.png)

## 星闪代码导入到VSCode

- 下载完成后,在"资源管理器"中选择打开文件夹

  ![image-20250311111016027](../docs/pic/tools/image-20250311111016027.png)

- 在弹出框中选择"/root/fbb_ws63/src"目录，点击确定

  ![image-20250311111103585](../docs/pic/tools/image-20250311111103585.png)

  

## 镜像烧录


- 硬件搭建：Typec线将板端与PC端连接

  ![image-20240801173105245](E:/fbb_ws63_10/docs/pic/tools/image-20240801173105245.png)

- 安装驱动“CH341SER驱动”（[CH341SER驱动下载地址](https://www.wch.cn/downloads/CH341SER_EXE.html)，**如果该链接失效或者无法下载，用户自行百度下载即可**），安装CH341SER驱动，安装前单板需要与PC端相连，点击安装即可，显示**驱动安装成功代表成功**，如果出现**驱动预安装成功代表安装失败**

    ![image-20240801173439645](../docs/pic/tools/image-20240801173439645.png)

    ![image-20240801173618611](../docs/pic/tools/image-20240801173618611.png)

- 

## FAQ
-  如果根据文档没有编译成功，请参考https://developer.hisilicon.com/postDetail?tid=02110170392979486020
-  如果根据文档编译成功，但是在编写其他代码后，导致编译失败，可以在论坛提问，论坛链接：https://developer.hisilicon.com/forum/0133146886267870001


​    

  
