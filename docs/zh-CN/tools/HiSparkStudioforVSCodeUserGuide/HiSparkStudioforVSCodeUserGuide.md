# 工具简介<a name="ZH-CN_TOPIC_0000002293219986"></a>

HiSpark Studio for VS Code插件面向智能设备开发者提供一站式集成开发环境。它为开发者提供代码编辑、编译、烧录和调试等功能，并支持C/C++语言，兼容64位Windows 10/Windows 11和Ubuntu 20.04及以上版本的Linux/WSL环境。该插件具有以下特点：

- 支持代码查找、代码高亮、代码自动补齐、代码输入提示、代码检查等，开发者可以轻松、高效编码。
- 支持单步调试和查看内存、变量、调用栈等调试信息。
- 支持自动检测各芯片/开发板依赖的工具链是否完备，并提供一键下载和安装缺失的工具链。

HiSpark Studio for VS Code插件主要分为以下6个功能区域（如图1所示）：

① WELCOME：提供欢迎页、使用指南、创建导入项目等选项。

② PROJECT EXPLORER：工程区文件展示区。

③ COMMANDS：提供新建工程、打开工程、清除、编译、烧录等功能按钮，并可控制状态栏中按钮的显隐。

④ 代码编辑区：提供代码的查看、编写、跳转、高亮等功能。

⑤ 输出控制台：提供操作日志的打印、调试命令的输入及命令行工具等功能。

⑥ 状态栏：提供常用功能按钮，包括新建工程、导入工程、工程配置、清除、编译、烧录等功能，并显示当前文件的编码格式、行数、列数等信息。

**图 1**  功能分区图<a name="fig19904573445"></a>
<img src="figures/功能分区图.png" width="700" alt="功能分区图">

HiSpark Studio for VS Code插件当前支持的芯片和对应特性如表1所示。

**表 1**  HiSpark Studio for VS Code插件支持的芯片及其特性（一）

<table>
<thead>
<tr>
<th>芯片系列</th>
<th>芯片名称</th>
<th>工程管理</th>
<th>编译运行</th>
<th>一键烧录</th>
<th>烧录配置</th>
<th>栈分析/镜像分析</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="11"><strong>短距物联</strong></td>
<td style="background-color: #e6f7e6; color: #1a1a1a;">BS20</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td>BS20C</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td>BS21</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td style="background-color: #e6f7e6; color: #1a1a1a;">BS21E</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td style="background-color: #e6f7e6; color: #1a1a1a;">BS22</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td>BS26</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td>BS21A</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td>BS25</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td>BS27A</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td>WS53</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td style="background-color: #e6f7e6; color: #1a1a1a;">WS63</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td rowspan="4"><strong>手机穿戴</strong></td>
<td>BRANDY</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td>SOCMN2</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td>SW21</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td style="background-color: #e6f7e6; color: #1a1a1a;">3322</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td rowspan="6"><strong>广域物联</strong></td>
<td>NB17</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td>NB17E</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td>NB18</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td>Hi2113</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td>Hi2131</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td>Hi2131C</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
</tbody>
</table>

**表 1**  HiSpark Studio for VS Code插件支持的芯片及其特性（二）

<table>
<thead>
<tr>
<th>芯片系列</th>
<th>芯片名称</th>
<th>工程调试</th>
<th>串口控制台</th>
<th>Kconfig</th>
<th>CodeSize</th>
<th>GUI</th>
<th>远程编译</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="11"><strong>短距物联</strong></td>
<td style="background-color: #e6f7e6; color: #1a1a1a;">BS20</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>×</td>
<td>√</td>
</tr>
<tr>
<td>BS20C</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>×</td>
<td>√</td>
</tr>
<tr>
<td>BS21</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>×</td>
<td>√</td>
</tr>
<tr>
<td style="background-color: #e6f7e6; color: #1a1a1a;">BS21E</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>×</td>
<td>√</td>
</tr>
<tr>
<td style="background-color: #e6f7e6; color: #1a1a1a;">BS22</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>×</td>
<td>√</td>
</tr>
<tr>
<td>BS26</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>×</td>
<td>√</td>
</tr>
<tr>
<td>BS21A</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>×</td>
<td>√</td>
</tr>
<tr>
<td>BS25</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>×</td>
<td>√</td>
</tr>
<tr>
<td>BS27A</td>
<td>×</td>
<td>×</td>
<td>×</td>
<td>×</td>
<td>×</td>
<td>√</td>
</tr>
<tr>
<td>WS53</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>×</td>
<td>√</td>
</tr>
<tr>
<td style="background-color: #e6f7e6; color: #1a1a1a;">WS63</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>×</td>
<td>√</td>
</tr>
<tr>
<td rowspan="4"><strong>手机穿戴</strong></td>
<td>BRANDY</td>
<td>√</td>
<td>√</td>
<td>×</td>
<td>√</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td>SOCMN2</td>
<td>√</td>
<td>√</td>
<td>×</td>
<td>√</td>
<td>×</td>
<td>√</td>
</tr>
<tr>
<td>SW21</td>
<td>√</td>
<td>√</td>
<td>×</td>
<td>×</td>
<td>×</td>
<td>√</td>
</tr>
<tr>
<td style="background-color: #e6f7e6; color: #1a1a1a;">3322</td>
<td>√</td>
<td>√</td>
<td>×</td>
<td>×</td>
<td>√</td>
<td>√</td>
</tr>
<tr>
<td rowspan="6"><strong>广域物联</strong></td>
<td>NB17</td>
<td>√</td>
<td>√</td>
<td>×</td>
<td>×</td>
<td>×</td>
<td>√</td>
</tr>
<tr>
<td>NB17E</td>
<td>√</td>
<td>√</td>
<td>×</td>
<td>×</td>
<td>×</td>
<td>√</td>
</tr>
<tr>
<td>NB18</td>
<td>√</td>
<td>√</td>
<td>×</td>
<td>×</td>
<td>×</td>
<td>√</td>
</tr>
<tr>
<td>Hi2113</td>
<td>√</td>
<td>√</td>
<td>×</td>
<td>×</td>
<td>×</td>
<td>√</td>
</tr>
<tr>
<td>Hi2131</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>×</td>
<td>√</td>
</tr>
<tr>
<td>Hi2131C</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>√</td>
<td>×</td>
<td>√</td>
</tr>
</tbody>
</table>

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - HiSpark Studio for VS Code所支持的芯片中，BS20、BS21E、BS22、WS63以及3322中的HiDiTing的SDK已经开放至gitcode社区，可通过“SDK下载”章节获取详细信息。
> - 芯片系列中，"短距物联"、"手机穿戴"、"广域物联"统一属于FBB系列，文档中FBB系列芯片指代这三个芯片系列。
> - [HiSpark Studio for VS Code用户指南](https://docs.hisilicon.com/repos/vscode-hispark-studio/zh-CN/main/HiSparkStudioforVSCodeUserGuide/index.html)
> - [海思官方网站](https://www.hisilicon.com/)
> - [海思生态论坛](https://developers.hisilicon.com/forum/0101108112079179002/)
> - [扩展插件市场](https://marketplace.ide.huaweicloud.com/)
> - [HiSpark 开发文档](https://docs.hisilicon.com/)
> - 3322包含3322和HiDiTing芯片。
> - Windows和Linux环境下都支持主页、新建工程、编译、栈/镜像分析功能、系统配置、命令行工具、高阶分析、烧录、下载管理、监视器、远程开发工具、调试功能。

# 开发环境搭建<a name="ZH-CN_TOPIC_0000002327179601"></a>

- **概述**
- **安装要求**
- **安装HiSpark Studio for VS Code插件**
- **工具链toolchain配置**
- **SDK下载**
- **MinGW安装**
- **bash环境准备**
- **CONFIG\_PUBLIC\_OPTION\_IN\_FILES宏开启**

## 概述<a name="ZH-CN_TOPIC_0000002293379654"></a>

当前版本支持Windows10/Windows11系统、Ubuntu 20.04及以上版本的Linux/WSL环境，本章节主要介绍Windows10/Windows11系统上的开发环境搭建。

## 安装要求<a name="ZH-CN_TOPIC_0000002327219377"></a>

- 操作系统要求：支持64位Windows10或64位Windows11，Linux/WSL环境支持Ubuntu20.04版本及以上。
- VS Code版本要求：1.85.0及以上。
- 硬盘要求：至少有900MB的硬盘空间来安装HiSpark Studio for VS Code插件。
- 内存要求：HiSpark Studio for VS Code插件最低要求为1GB RAM，建议至少有4GB RAM来安装运行HiSpark Studio for VS Code插件。
- CPU：HiSpark Studio for VS Code插件最低要求为1.6GHz或者更高的处理器。
- 运行空间：建议至少保留1GB可用空间。

## 安装HiSpark Studio for VS Code插件<a name="ZH-CN_TOPIC_0000002293219990"></a>

> ![](public_sys-resources/icon-note.gif) **说明：**
> 若之前已经安装过DevEco相关的插件，请手动禁用或者卸载所有与DevEco相关的插件，否则会与HiSpark Studio for VS Code插件功能冲突。
> Linux/WSL环境下请先连接到远程环境，再安装HiSpark Studio for VS Code插件。

1. 打开VS Code插件市场，在搜索框中搜索HiSpark Studio。
   
   **图 1**  VS Code插件市场搜索框<a name="fig155444514303"></a>
   <img src="figures/VS-Code插件市场搜索框.png" width="700" alt="VS-Code插件市场搜索框">
2. 在搜索结果中选择HiSpark Studio for VS Code，点击安装即可。如果不需要安装最新版本，可以在扩展页面点击“齿轮”按钮弹出的“安装特定版本”，或者在安装插件后的扩展页面中点击“卸载”下拉框中的“安装特定版本”功能，安装所需插件的以往版本。
   
   **图 2**  安装插件以往版本<a name="fig163656101744"></a>
   <img src="figures/安装插件以往版本.png" width="700" alt="安装插件以往版本">
   
   <img src="figures/zh-cn_image_0000002537983659.png" width="700" alt="zh-cn_image_0000002537983659">

## 工具链toolchain配置<a name="ZH-CN_TOPIC_0000002337256433"></a>

HiSpark Studio for VS Code插件在对工程进行编译等操作时，需要依赖工具链、Python以及pip.pyz等环境。可以使用插件中的Download Toolchain功能进行工具链等的自动下载和安装。

**图 1**  Download Toolchain功能<a name="fig4608724245"></a>
<img src="figures/Download-Toolchain功能.png" width="500" alt="Download-Toolchain功能">

点击Download Toolchain功能后，会弹出一个文件夹选择框，用于选择工具链的下载和安装位置。

**图 2**  工具链保存位置选择<a name="fig76821526434"></a>
<img src="figures/工具链保存位置选择.png" width="700" alt="工具链保存位置选择">

> ![](public_sys-resources/icon-notice.gif) **须知：**
> 
> - 首次选择文件夹进行工具链的配置时，为避免影响该文件夹内已有文件，建议选择一个空文件夹，用于下载和配置工具链。
> - Linux/WSL环境下，工具链安装位置建议选择“home/用户/xxx”目录下。

文件夹选择路径不能包含中文或者空格，否则会有提示框弹出，此时选择“再次选择文件夹路径”会重新进入到工具链保存位置选择界面，选择“取消”则会退出工具链的选择安装。

**图 3**  文件夹路径选择中文或者空格后的提示弹窗<a name="fig162591729204619"></a>
<img src="figures/文件夹路径选择中文或者空格后的提示弹窗.png" width="500" alt="文件夹路径选择中文或者空格后的提示弹窗">

选择完文件夹路径后，会依次下载Python 3.11.4、pip.pyz、pip.pyz的依赖（wheel、setuptools、cmake、kconfiglib、pycparser、pillow、numpy、opencv\-python、windows\-curses、ffmpeg\-python、pyelftools、openpyxl、pandas以及tkinter-embed），以及编译所需的tools工具。如果本地没有安装git，即命令提示符（cmd）中输入“git --version”时没有显示对应版本，系统会同时下载git安装包。下载过程VS Code右下角通知栏会有对应的工具下载提示以及进度展示。compiler由于文件比较大，下载时间较长，请耐心等待。

**图 4**  Toolchain下载进度展示（部分）<a name="fig13993195610574"></a>
<img src="figures/Toolchain下载进度展示（部分）.png" width="500" alt="Toolchain下载进度展示（部分）">

下载结束后，具体的下载内容保存在选择目录的downloads目录下，以选择路径为“D:\\toolchain”为例，工具链下载目录如图5所示。如果下载了git安装包，会在downloads目录下生成一个“Git.tar.gz”压缩包。

**图 5**  Toolchain下载目录<a name="fig1964666125817"></a>
<img src="figures/Toolchain下载目录.png" width="700" alt="Toolchain下载目录">

Python 3.11.4安装好后，会对pip.pyz的依赖进行安装，并对编译工具链进行解压，右下角会有对应的通知弹框提示，全部安装完成会提示环境准备完成。

**图 6**  python3.11.4安装完成后的pip.pyz依赖及工具链的解压提示<a name="fig7778922205811"></a>
<img src="figures/python3-11-4安装完成后的pip-pyz依赖及工具链的解压提示.png" width="500" alt="python3-11-4安装完成后的pip-pyz依赖及工具链的解压提示">

在环境准备中，会将文件夹下载路径如“D:\\toolchain\\downloads\\HiSparkStudioToolchain.zip”的这个zip压缩包解压到与downloads目录同级的tools目录（如“D:\\toolchain\\tools”中）。如果下载了git安装包，tools目录下会存在一个Git文件夹。

**图 7**  编译工具链解压结果<a name="fig136041725175413"></a>
<img src="figures/编译工具链解压结果.png" width="700" alt="编译工具链解压结果">

Python环境可以通过在tools目录下的python目录如“D:\\toolchain\\tools\\python”路径下打开cmd（命令提示符）窗口中进行验证。执行“.\\python.exe --version”输出结果为‘Python 3.11.4’，以及执行“python.exe ..\\..\\downloads\\pip.pyz list”命令输出结果显示有wheel、setuptools、cmake、kconfiglib、pycparser、pillow、numpy、opencv\-python、windows\-curses、ffmpeg\-python、pyelftools、openpyxl、pandas以及tkinter-embed这些pip.pyz的依赖及其对应版本，则说明Python环境配置正确。

**图 8**  python环境验证<a name="fig1757924515812"></a>
<img src="figures/python环境验证.png" width="700" alt="python环境验证">

工具链下载完成，环境配置完成后，会自动在用户环境变量中添加一个变量名为“HISPARK\_TOOL\_PATH”，变量值为选择的工具链存放位置如“D:\\toolchain”的环境变量，便于插件获取工具链的路径。

**图 9**  为工具链添加用户环境变量<a name="fig94491142417"></a>
<img src="figures/为工具链添加用户环境变量.png" width="700" alt="为工具链添加用户环境变量">

> ![](public_sys-resources/icon-note.gif) **说明：**
> **手动配置toolchain环境：**
> 如果由于环境或其他原因导致自动配置toolchain环境失败，用户也可以手动进行配置，首先创建一个空文件夹用于存放下载的工具链，如“D:\\toolchain”，并在该路径下创建两个文件夹downloads和tools分别用于存放下载的文件以及安装的工具链：
> 
> - python下载链接：[python](https://mirrors.huaweicloud.com/python/3.11.4/python-3.11.4-embed-amd64.zip)，将该文件下载到downloads目录下（如“D:\\toolchain\\downloads”），并解压到tools目录下（如“D:\\toolchain\\tools”），将解压后文件命名为“python”。
>   <img src="figures/python安装3.png" width="700" alt="python安装3">
> - 下载“pip.pyz”文件至downloads目录下（如“D:\\toolchain\\downloads”），并将下载后的“pip.pyz”文件复制到tools\\python目录下（如“D:\\toolchain\\tools\\python”）。下载链接：[pip.pyz](https://hispark-obs.obs.cn-east-3.myhuaweicloud.com/pip.pyz)
> - 修改..\\tools\\python\\python311.\_pth文件的内容，删除“\#import site”前的\#号。
>   <img src="figures/python安装4.png" width="700" alt="python安装4">
> - pip.pyz依赖准备需要下载对应依赖的whl文件到downloads目录（如“D:\\toolchain\\downloads”）下，并复制到tools\\python目录下（如“D:\\toolchain\\tools\\python”），并在“D:\\toolchain\\tools\\python”目录下打开cmd执行“.\\python.exe pip.pyz install xx.whl”命令，如下载cmake的whl文件为“cmake-3.20.5-py2.py3-none-win\_amd64.whl”，则在“D:\\toolchain\\tools\\python”目录下打开cmd执行“.\\python.exe pip.pyz install cmake-3.20.5-py2.py3-none-win\_amd64.whl”即可。各依赖的whl文件下载链接如下：
>   [wheel](https://mirrors.cloud.tencent.com/pypi/packages/0b/2c/87f3254fd8ffd29e4c02732eee68a83a1d3c346ae39bc6822dcbcb697f2b/wheel-0.45.1-py3-none-any.whl)、[setuptools](https://mirrors.cloud.tencent.com/pypi/packages/a3/dc/17031897dae0efacfea57dfd3a82fdd2a2aeb58e0ff71b77b87e44edc772/setuptools-80.9.0-py3-none-any.whl)、[cmake](https://mirrors.cloud.tencent.com/pypi/packages/65/7f/80cf681cd376834b442af8af48e6f17b4197d20b7255aa2f76d8d93a9e44/cmake-3.20.5-py2.py3-none-win_amd64.whl)、[kconfiglib](https://mirrors.cloud.tencent.com/pypi/packages/8a/f1/d98a89231e779b079b977590efcc31249d959c8f1d4b5858cad69695ff9c/kconfiglib-14.1.0-py2.py3-none-any.whl)、[pycparser](https://mirrors.cloud.tencent.com/pypi/packages/62/d5/5f610ebe421e85889f2e55e33b7f9a6795bd982198517d912eb1c76e1a53/pycparser-2.21-py2.py3-none-any.whl)、[pillow](https://mirrors.cloud.tencent.com/pypi/packages/c1/d0/5866318eec2b801cdb8c82abf190c8343d8a1cd8bf5a0c17444a6f268291/pillow-10.4.0-cp311-cp311-win_amd64.whl)、[numpy](https://mirrors.cloud.tencent.com/pypi/packages/9b/0f/022ca4783b6e6239a53b988a4d315d67f9ae7126227fb2255054a558bd72/numpy-2.0.0-cp311-cp311-win_amd64.whl)、[opencv\-python](https://mirrors.cloud.tencent.com/pypi/packages/fa/80/eb88edc2e2b11cd2dd2e56f1c80b5784d11d6e6b7f04a1145df64df40065/opencv_python-4.12.0.88-cp37-abi3-win_amd64.whl)、[windows\-curses](https://mirrors.cloud.tencent.com/pypi/packages/18/1b/e06eb41dad1c74f0d3124218084f258f73a5e76c67112da0ba174162670f/windows_curses-2.3.3-cp311-cp311-win_amd64.whl)、[ffmpeg\-python](https://mirrors.cloud.tencent.com/pypi/packages/d7/0c/56be52741f75bad4dc6555991fabd2e07b432d333da82c11ad701123888a/ffmpeg_python-0.2.0-py3-none-any.whl)、[pyelftools](https://mirrors.cloud.tencent.com/pypi/packages/af/43/700932c4f0638c3421177144a2e86448c0d75dbaee2c7936bda3f9fd0878/pyelftools-0.32-py3-none-any.whl)、[openpyxl](https://mirrors.cloud.tencent.com/pypi/packages/c0/da/977ded879c29cbd04de313843e76868e6e13408a94ed6b987245dc7c8506/openpyxl-3.1.5-py2.py3-none-any.whl)、[pandas](https://mirrors.cloud.tencent.com/pypi/packages/44/a0/97a6339859d4acb2536efb24feb6708e82f7d33b2ed7e036f2983fcced82/pandas-3.0.2-cp311-cp311-win_amd64.whl)
> - 下载tkinter压缩包到downloads目录下（如“D:\\toolchain\\downloads”），并复制到“tools\\python”目录下（如“D:\\toolchain\\tools\\python”），并在“D:\\toolchain\\tools\\python”目录下打开cmd执行“.\\python.exe pip.pyz install --target .\\ tkinter\_embed-3.11.0.tar.gz”命令。
>   下载链接：[tkinter压缩包](https://mirrors.cloud.tencent.com/pypi/packages/a2/b5/01fa4f6b1b78b01c1602d8e6e28879dcbef2d399d934f28d3324c1114552/tkinter_embed-3.11.0.tar.gz)
> - 新建“toolChain.json”文件到tools目录下（如“D:\\toolchain\\tools”），并添加python路径如“d:\\\\toolchain\\\\tools\\\\python”内容至“toolChain.json”中。
> 
> ```
> {
>     "pythonDir": "d:\\toolchain\\tools\\python"
> }
> ```
> 
> <img src="figures/python安装5.png" width="700" alt="python安装5">
> 
> - 工具链可以手动通过[工具链包](https://hispark-obs.obs.cn-east-3.myhuaweicloud.com/HiSparkStudioToolchain.zip)链接下载到downloads目录下（如“D:\\toolchain\\downloads”），并解压到tools目录下（如“D:\\toolchain\\tools”）。
> - 若本地没有配置git环境，可以手动下载git压缩包并解压到tools目录，git压缩包下载地址：[git包](https://hispark-obs.obs.cn-east-3.myhuaweicloud.com/Git.tar.gz)。解压后的目录结构如下：
>   <img src="figures/Git.png" width="700" alt="Git">
> - 工具链的环境和Python环境验证可参考图7和图8。
> - 验证成功后需手动将创建的用于存放下载工具链的文件夹目录（如“D:\\toolchain”）添加至用户环境变量中，变量名为“HISPARK\_TOOL\_PATH”，变量值为新建的文件夹目录（如“D:\\toolchain”），可参考图9。

> ![](public_sys-resources/icon-note.gif) **说明：**
> **半手动配置toolchain环境：**
> 如果只是少数文件下载失败，用户可以根据上述“手动配置toolchain环境”中相应文件的下载路径，手动下载相应的文件到downloads目录，然后再次点击英文“Download Toolchain”或者中文“下载工具链”按钮，HiSpark Studio会自动完成后续的文件的下载、安装以及配置操作。

## SDK下载<a name="ZH-CN_TOPIC_0000002303416852"></a>

HiSpark Studio for VS Code插件中创建工程需要依赖SDK软件包，插件当前提供WS63、BS2X系列以及HiDiTing的SDK下载。

**图 1**  Download SDK from HiSpark功能<a name="fig16959181457"></a>
<img src="figures/Download-SDK-from-HiSpark功能.png" width="500" alt="Download-SDK-from-HiSpark功能">

点击“Download SDK from HiSpark”后，弹出SDK下载列表。

**图 2**  SDK下载列表<a name="fig862702418516"></a>
<img src="figures/SDK下载列表.png" width="700" alt="SDK下载列表">

选中任意SDK，会弹出“选择SDK保存位置”选项。

**图 3**  SDK保存位置选择<a name="fig109016331355"></a>
<img src="figures/SDK保存位置选择.png" width="700" alt="SDK保存位置选择">

选中保存文件夹后，会优先使用本地系统中配置的git环境进行SDK下载，如果本地没有配置git环境，并且也没有进行下载工具链的操作，会提示“本地未安装配置git环境，请重新下载工具链配置git环境”。下载工具链的操作可以参考“工具链toolchain配置”。

**图 4**  下载SDK提示框<a name="fig1437317573111"></a>
<img src="figures/下载SDK提示框.png" width="500" alt="下载SDK提示框">

SDK下载完成后，用户可根据SDK来进行工程创建。

> ![](public_sys-resources/icon-notice.gif) **须知：**
> 
> - 为避免SDK下载后与其他文件夹相互影响，建议选择SDK保存位置时选择一个空文件夹。
> - 由于Windows 10和Windows 11的路径存在260Byte的长度限制，为后续编译操作，建议SDK存放路径不宜过长。
> - WS63 SDK通过git clone [https://gitcode.com/HiSpark/fbb_ws63.git](https://gitcode.com/HiSpark/fbb_ws63.git)在线下载。
> - BS2X SDK通过git clone [https://gitcode.com/HiSpark/fbb_bs2x.git](https://gitcode.com/HiSpark/fbb_bs2x.git)在线下载。
> - HiDiTing SDK通过git clone [https://gitcode.com/HiSpark/hs-fbb.git](https://gitcode.com/HiSpark/hs-fbb.git)在线下载。

## MinGW安装<a name="ZH-CN_TOPIC_0000002524323169"></a>

使用GUI模拟器，需要安装CMake和MinGW。MinGW安装过程如下：

1. 下载7.3.0版本的[MinGW](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/7.3.0/threads-posix/seh/)。
   
   **图 1**  MinGW-W64下载版本选择<a name="fig3726173721912"></a>
   <img src="figures/MinGW-W64下载版本选择.png" width="500" alt="MinGW-W64下载版本选择">
   
   下载得到如下压缩包：
   
   **图 2**  MinGW-W64安装包<a name="fig18488603204"></a>
   <img src="figures/MinGW-W64安装包.png" width="500" alt="MinGW-W64安装包">
2. 在文件夹下解压，并将安装路径“/mingw64/bin”添加到系统环境变量，通过“gcc -v”命令查看是否成功安装。安装目录切勿包含中文路径，否则可能会导致编译失败。
   
   **图 3**  MinGW-W64安装后添加环境变量验证<a name="fig15348183017207"></a>
   <img src="figures/MinGW-W64安装后添加环境变量验证.png" width="700" alt="MinGW-W64安装后添加环境变量验证">

## bash环境准备<a name="ZH-CN_TOPIC_0000002304004976"></a>

该小节仅针对需要依赖bash环境才可以编译的Hi2131、Hi2131C、3322工程。

如果需要编译Hi2131、Hi2131C、3322的工程，用户需手动下载[git](https://github.com/git-for-windows/git/releases/download/v2.48.1.windows.1/Git-2.48.1-64-bit.exe)
并将bash路径添加到环境变量中。

如果Windows系统System32下默认有“bash.exe”，需要删除其系统下的“bash.exe”，使用安装的git下内置的“bash.exe”。

完成后，重启VS Code使新环境变量生效，即可正常编译工程。

**图 1**  删除Windows系统库的bash<a name="fig9843191112367"></a>
<img src="figures/删除Windows系统库的bash.png" width="700" alt="删除Windows系统库的bash">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 若删除Windows系统库下的bash时提示没权限，可以在需要删除的bash.exe上右键点击，选择“属性”，在弹出的窗口中依次点击“安全”标签页→“高级”→点击“所有者”标签页下的“更改”按钮。选择一个新的所有者（例如当前用户），点击“应用”和“确定”。返回“安全”标签页，点击“编辑”。为当前添加的用户或组分配适当的权限（如完全控制、读取等）。点“应用”和“确定”后即可删掉选择的bash.exe文件。

**图 2**  配置bash.exe的删除权限<a name="fig31821532142019"></a>
<img src="figures/配置bash-exe的删除权限.png" width="700" alt="配置bash-exe的删除权限">

<img src="figures/zh-cn_image_0000002478125622.png" width="700" alt="zh-cn_image_0000002478125622">

**图 3**  Git下的bash路径<a name="fig664202311363"></a>
<img src="figures/Git下的bash路径.png" width="700" alt="Git下的bash路径">

**图 4**  添加到环境变量中<a name="fig1687216294365"></a>
<img src="figures/添加到环境变量中.png" width="500" alt="添加到环境变量中">

**图 5**  保证Git下的bash被调用<a name="fig14591538123614"></a>
<img src="figures/保证Git下的bash被调用.png" width="700" alt="保证Git下的bash被调用">

对于3322的工程，涉及用tar进行解压的操作，如果使用系统默认自带的tar会解压失败，需要对bash环境进行配置，使用bash环境中的tar来解压，保证编译正常运行。

1. 在bash环境准备好的基础上，将Git目录下的“..\\Git\\usr\\bin”目录添加到系统环境变量中，且优先顺序在默认的System32之上。如图6所示。
   
   **图 6**  添加系统环境变量<a name="fig16428419174718"></a>
   <img src="figures/添加系统环境变量.png" width="500" alt="添加系统环境变量">
2. 将“..\\Git\\usr\\bin”添加到环境变量后，打开一个命令提示符（cmd），输入“where tar”，保证最新添加的“..\\Git\\usr\\bin”在系统默认的tar的环境变量之前，但需保留默认的System32目录下的tar，如图7所示。环境配置完成后，需要删除工程目录下的output文件夹，关闭所有Visual Studio Code窗口后重新打开，再次编译，即可编译成功。
   
   **图 7**  tar环境验证<a name="fig1022633464711"></a>
   <img src="figures/tar环境验证.png" width="700" alt="tar环境验证">

## CONFIG\_PUBLIC\_OPTION\_IN\_FILES宏开启<a name="ZH-CN_TOPIC_0000002622344905"></a>

对于3322工程（包含3322和diting工程），进行编译时，需要手动修改工程目录下的“build/config/target\_config/3322/config.py”文件中的代码，将默认注释的宏“CONFIG\_PUBLIC\_OPTION\_IN\_FILES”开放，如不开放，在编译过程中会报“is command line too long”的错误，导致编译失败。

**图 1**  开启”CONFIG\_PUBLIC\_OPTION\_IN\_FILES”宏<a name=”fig1360193117574”></a>
<img src="figures/开启-CONFIG_PUBLIC_OPTION_IN_FILES-宏.png" width="700" alt="开启-CONFIG_PUBLIC_OPTION_IN_FILES-宏">

需要编译哪个target就需要开放哪个target的宏，如上图所示，“3322-native-js”target中的“CONFIG\_PUBLIC\_OPTION\_IN\_FILES”宏默认关闭，在编译“3322-native-js”target时需要将宏的注释去掉，开放该宏，保证编译通过。

> ![](public_sys-resources/icon-notice.gif) **须知：**
> 
> - diting工程的编译同样需要修改开放对应的宏。
> - 如果某一target中的参数没有“CONFIG\_PUBLIC\_OPTION\_IN\_FILES”宏，并且在编译时报“is command line too long”的错误，可以手动在“config.py”文件中的target下手动增加这一宏选项。
> - 开启CONFIG_PUBLIC_OPTION_IN_FILES宏后，Windows会读取.rsp文件执行编译，rsp文件读取是以流式方式读取，头文件的读取存在先后顺序问题，当存在同名头文件时，可能存在头文件引用错误问题，因此用户开发过程中需避免创建同名头文件。

# 工程管理<a name="ZH-CN_TOPIC_0000002293219994"></a>

- **新建工程**
- **导入工程**

## 新建工程<a name="ZH-CN_TOPIC_0000002327179609"></a>

1. 打开HiSpark Studio for VS Code插件，进入欢迎页面，点击“新建工程”，进入新建工程页面。
   
   **图 1**  HiSpark Studio for VS Code插件欢迎页面<a name="fig6514428113718"></a>
   <img src="figures/HiSpark-Studio-for-VS-Code插件欢迎页面.png" width="700" alt="HiSpark-Studio-for-VS-Code插件欢迎页面">
2. <a name="li4852172114422"></a>在图2界面配置工程参数，点击“完成”。
   
   **图 2**  新建工程窗口<a name="fig2784311111318"></a>
<img src="figures/新建工程窗口.png" width="700" alt="新建工程窗口">
   
   - 芯片：选择工程使用的芯片名称。
   - 开发板：选择工程使用的开发板名称。当用户不需要自定义开发板时，默认芯片名作为开发板名。
   - 工程类型：选择创建的工程类型。包括示例工程和三方示例工程，其中三方示例工程当前仅支持WS63芯片。
   - 工程名：输入工程名称。
   - 工程路径：选择用于存放工程文件的目录。
   - 软件包：选择工程使用的软件开发驱动包（SDK）文件夹根目录。
   
   当芯片选择WS63，且工程类型选择“三方示例工程”后，工程界面会新增“sample路径”和“sample选择”选项，如图3所示。
   
   **图 3**  sample工程创建页面<a name="fig16606922141519"></a>
   <img src="figures/sample工程创建页面.png" width="700" alt="sample工程创建页面">
   
   sample路径可选择WS63 SDK下载后与SDK同级目录下的vendor下的任意文件夹（所选文件夹下需包含“build\_config.json”文件及demo文件夹），然后点击sample选择，弹出如图4所示界面。
   
   **图 4**  sample选择页面<a name="fig111743454160"></a>
   <img src="figures/sample选择页面.png" width="700" alt="sample选择页面">
   
   选择需要的sample后，关闭sample选择页面，选择的sample选项名会填充到新建工程页面中的“sample选择”中，如图5所示。
   
   **图 5**  sample选择完成后页面<a name="fig9923152471712"></a>
   <img src="figures/sample选择完成后页面.png" width="700" alt="sample选择完成后页面">
   
   后续选择合适的工程名、工程路径和WS63的软件包，点击“完成”即可创建工程。
3. 查看工程创建结果。
   
   HiSpark Studio for VS Code插件会自动打开新创建的工程，并在欢迎界面工程列表中显示创建的工程，如图6所示。如果是WS63创建“三方示例工程”，工程创建后会在“application/sample”工程目录下增加一个与已选sample同名的文件夹。
   
   **图 6**  工程创建结果页面<a name="fig551738161718"></a>
   <img src="figures/工程创建结果页面.png" width="700" alt="工程创建结果页面">

## 导入工程<a name="ZH-CN_TOPIC_0000002293379666"></a>

1. 打开HiSpark Studio for VS Code插件，进入欢迎页面，点击“导入工程”，进入导入工程页面。
   
   **图 1**  HiSpark Studio for VS Code插件导入工程页面<a name="fig19540125184016"></a>
   <img src="figures/HiSpark-Studio-for-VS-Code插件导入工程页面.png" width="700" alt="HiSpark-Studio-for-VS-Code插件导入工程页面">
2. 选择导入的路径，即可查找该路径下所有的工程，勾选需要导入的工程，单击“完成”。
   
   **图 2**  导入工程配置页面<a name="fig3640203910157"></a>
   <img src="figures/导入工程配置页面.png" width="700" alt="导入工程配置页面">
3. 导入工程完成后，会在工程区展示出工程的文件夹，并在欢迎界面的工程列表中展示已导入的工程。
   
   **图 3**  导入工程完成<a name="fig1617152231818"></a>
   <img src="figures/导入工程完成.png" width="700" alt="导入工程完成">

# 工程配置<a name="ZH-CN_TOPIC_0000002327179613"></a>

工程配置主要用于配置工程的基本信息、编译、调试、烧录等工程配置项，单击“<img src="figures/config.png" width="20">”按钮可以打开工程配置界面，如图1所示。

**图 1**  单击工程配置按钮<a name="fig672755464817"></a>
<img src="figures/单击工程配置按钮.png" width="700" alt="单击工程配置按钮">

- **基本信息配置**
- **编译器配置**
- **烧录器配置**
- **调试器配置**

## 基本信息配置<a name="ZH-CN_TOPIC_0000002293379670"></a>

单击工程配置界面左侧“基本信息”页签进入基本信息配置界面，如图1所示。该界面包含工程芯片系列、开发板型号、软件包路径，可以修改工程所对应的软件包路径。

**图 1**  基本信息配置界面<a name="fig18947542448"></a>

<img src="figures/zh-cn_image_0000002517475768.png" width="700">

- 基本系列配置界面中，Target选择框以及关联的Target管理功能仅在部分FBB芯片中生效：
  
  BS20、BS20C、BS21、BS21E、BS21A、BS22、BS26既支持Target选择也支持Target管理，其余含有Target的工程仅支持Target选择。
- 以下指南仅针对上述涉及到Target功能的芯片：
  
  1. 如图2所示，通过选取不同的Target，可配合编译生成不同的编译产物。
     
     **图 2**  Target选择界面<a name="fig1942333014511"></a>
     <img src="figures/Target选择界面.png" width="700" alt="Target选择界面">
  2. 单击Target选项框下方的 "Target 管理"，可进入Target管理界面，如图3所示。
     
     **图 3**  Target管理界面<a name="fig1648301431310"></a>
     <img src="figures/Target管理界面.png" width="700" alt="Target管理界面">
     
     ① 不支持编辑和删除的SDK默认Target。
     
     ② 支持编辑和删除的Target。
  3. 单击Target管理界面的“添加”，可进入添加Target界面，如图4所示。
     
     **图 4**  添加Target界面<a name="fig13409104010130"></a>
     <img src="figures/添加Target界面.png" width="700" alt="添加Target界面">
  4. 当用户添加或删除Target时，工程配置界面的Target选项也会同步生效。以添加Target为例，当新增一个demo的Target后，如图5，工程配置界面Target下拉框也会同步更新DEMO选项，如图6。
     
     **图 5**  添加名称为demo的Target<a name="fig899411161410"></a>
     <img src="figures/添加名称为demo的Target.png" width="700" alt="添加名称为demo的Target">
     
     **图 6**  Target下拉框同步更新DEMO<a name="fig38170493913"></a>
     <img src="figures/Target下拉框同步更新DEMO.png" width="700" alt="Target下拉框同步更新DEMO">
     
     配置后的Target参数宏信息会生成在target\_config目录下的“.config”文件中，如图7所示。
     
     **图 7**  Target配置文件<a name="fig8566412205415"></a>
     <img src="figures/Target配置文件.png" width="700" alt="Target配置文件">
     
     Target支持选择和自定义，单击Target选项框可以从Target列表中选择需要的Target，也可以自定义Target和编译的指令。
     
     **图 8**  Target自定义<a name="fig94764713276"></a>
<img src="figures/Target自定义.png" width="700" alt="Target自定义">
     
     > ![](public_sys-resources/icon-notice.gif) **须知：**
     > 
     > - Target支持选择和自定义，每次自定义时均需要手动输入完整的自定义Target及命令，不支持在选择或自定义的Target基础上二次编辑，即每次自定义Target时均需从第一个字符开始输入；
     > - Target自定义时，如果输入的Target为列表中已有的Target，在此基础上新增其他编译命令，如果编译能正常通过，调试和烧录均可正常执行。
     > - Target自定义时，如果输入的Target为列表中没有的Target，在此基础上进行编译，如果编译能正常通过，由于没有绑定elf和map文件，调试和栈分析、镜像分析功能会受到影响。此外，若使用自定义列表中没有的Target，重新打开工程配置时会提示launch.json缺失并影响调试功能，此为正常现象。

## 编译器配置<a name="ZH-CN_TOPIC_0000002327219389"></a>

单击工程配置界面左侧“编译器”页签进入编译器配置界面，如图1所示，该界面可配置工程是否开启编译问题分析功能和生成analyzerJson功能，默认不开启这两个功能。对于3322和3321工程会增加一个“-nhso构建参数”编译选项，默认开启，以加快3322和3321工程编译速度。

**图 1**  编译器配置界面-1<a name="fig177671416389"></a>

<img src="figures/ScreenShot_20260303145647.png" width="700">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 修改配置之后会自动保存并生效。文本输入框中的修改会在失焦时自动保存并生效。

## 烧录器配置<a name="ZH-CN_TOPIC_0000002327179617"></a>

单击工程配置界面左侧“程序加载”页签进入程序加载配置界面，如图1所示，该界面支持配置烧录传输方式以及传输方式对应的参数。

**图 1**  程序加载配置界面<a name="fig1851473982716"></a>
<img src="figures/程序加载配置界面.png" width="700" alt="程序加载配置界面">

- 传输方式：选择数据传输方式。选择不同的传输方式，会出现不同的参数配置项。
  
  - serial：选择通过串口传输。如图2所示。
    
    - 端口
    - 波特率
    
    **图 2**  选择serial配置界面<a name="fig58771855184310"></a>
    
    <img src="figures/zh-cn_image_0000002339310954.png" width="700">
  - usb：选择通过USB设备完成烧写升级。如图3所示。（当前usb模式支持BS20、BS20C、BS21、BS21A、BS21E、BS22、BS25、BS26系列芯片。）
    
    - usb设备列表
      
      **图 3**  选择usb配置界面<a name="fig10468263017"></a>
      
      <img src="figures/zh-cn_image_0000002373349237.png" width="700">
    - 如图所示，切换usb模式时，会修改默认Bin文件，且仅支持程序加载，不支持烧录配置。
      
      **图 4**  传输方式改为usb模式<a name="fig9131134814544"></a>
      <img src="figures/传输方式改为usb模式.png" width="700" alt="传输方式改为usb模式">
- Bin文件：指定需要烧录的文件。
- 烧录后复位：烧录完成后，会进行单板软复位。
- 烧录后校验：烧录后，会将烧录文件进行回读对比，校验文件的完整性。

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - 修改配置之后会自动保存并生效。

## 调试器配置<a name="ZH-CN_TOPIC_0000002340080528"></a>

单击工程配置界面左侧“调试器”页签，在JlinkGDBServerCL路径中选择Jlink的执行软件，如图1所示。

**图 1**  工程配置界面中JlinkGDBServerCL路径选择<a name="fig205561635163111"></a>
<img src="figures/工程配置界面中JlinkGDBServerCL路径选择.png" width="700" alt="工程配置界面中JlinkGDBServerCL路径选择">

# 编译运行<a name="ZH-CN_TOPIC_0000002327183609"></a>

- **编译按钮介绍**
- **编译结果**
- **命令行编译工具**
- **脚本执行命令**

## 编译按钮介绍<a name="ZH-CN_TOPIC_0000002293383670"></a>

**图 1**  编译按钮<a name="fig7871826336"></a>
<img src="figures/编译按钮.png" width="700" alt="编译按钮">

图1中按钮依次为：清除、编译、重编译和停止编译。

- <img src="figures/zh-cn_image_0000002339324210.png" width="15">：单击触发工程清理，删除编译中间生成的文件。
- <img src="figures/zh-cn_image_0000002339323986.png" width="17">：单击触发工程编译。
- <img src="figures/zh-cn_image_0000002373241301.png" width="17">：单击触发先清理target再编译。
- <img src="figures/zh-cn_image_0000002373362865.png" width="17">：单击触发停止编译。

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - 重编译功能会先清除output目录下当前target的内容，如acore目录和fwpkg目录下名为target的文件夹。如果存在该文件夹则会删除；如果没有，则不会清除任何文件或文件夹。清除完毕后会再执行编译功能。
> - 如果想删除output文件夹，执行“清除”功能即可。

## 编译结果<a name="ZH-CN_TOPIC_0000002293224006"></a>

单击“<img src="figures/zh-cn_image_0000002373367797.png" width="17">”按钮开始编译，编译成功后终端窗口输出如图1所示，且工程目录中生成output目录如图2所示。

**图 1**  编译成功<a name="fig738217204550"></a>
<img src="figures/编译成功.png" width="700" alt="编译成功">

**图 2**  编译生成output目录<a name="fig114873277206"></a>

<img src="figures/zh-cn_image_0000002339329450.png" width="300">

单击<img src="figures/zh-cn_image_0000002339329878.png" width="15">按钮开始清除编程生成的文件，成功后终端窗口输出如图3所示。工程清理会清除工程目录下的output文件夹。

**图 3**  清除工程编译成功<a name="fig1710335292011"></a>

<img src="figures/zh-cn_image_0000002327183953.png" width="700">

## 命令行编译工具<a name="ZH-CN_TOPIC_0000002413445032"></a>

点击COMMANDS中的Command Line功能或状态栏中的Command Line图标，可执行命令行工具功能。

**图 1**  Command Line功能入口<a name="fig22561015171811"></a>
<img src="figures/Command-Line功能入口.png" width="700" alt="Command-Line功能入口">

执行命令行工具功能后，会自动配置临时环境变量，并启动一个CommandLine终端。如果缺少编译所必需的工具链或其他依赖，会提示需先执行Download Toolchain功能配置环境。以Python环境配置失败为例，当Python环境未配置时点击Command Line会有信息提示先执行Download Toolchain配置环境。

**图 2**  环境配置失败提示信息<a name="fig820222016210"></a>
<img src="figures/环境配置失败提示信息.png" width="700" alt="环境配置失败提示信息">

环境配置完成后，会打开一个CommandLine终端，默认在C盘根目录下，输入“d:”会切换至D盘根目录，然后cd到SDK目录，执行“python build.py -c”命令运行编译脚本，选择编译选项（“-c”为清除编译命令，可视情况选择是否在编译命令中带上“-c”）。

**图 3**  命令行编译<a name="fig126501319153419"></a>
<img src="figures/命令行编译.png" width="700" alt="命令行编译">

## 脚本执行命令<a name="ZH-CN_TOPIC_0000002526068312"></a>

在完成“工具链toolchain配置”后，在下载的工具链的tools目录下，如“D:\\toolChain\\tools”目录下，存在一个“hispark\_studio.bat”的脚本。

**图 1**  hispark\_studio脚本所在位置<a name="fig2214951121219"></a>
<img src="figures/hispark_studio脚本所在位置.png" width="700" alt="hispark_studio脚本所在位置">

双击该脚本后，cd到SDK所在目录，执行“python build.py -c”并选择所需要的target，即可不依赖VS Code工具直接对SDK或工程进行编译操作。以WS63工程为例，操作过程如图2所示。

**图 2**  脚本起编译过程<a name="fig41347152166"></a>
<img src="figures/脚本起编译过程.png" width="700" alt="脚本起编译过程">

# 软件包烧录<a name="ZH-CN_TOPIC_0000002293224022"></a>

烧录功能只支持串口烧录。

- **连接烧录串口线**
- **配置工程的烧录选项**
- **烧录配置**

> ![](public_sys-resources/icon-note.gif) **说明：**
> **Linux/WSL挂载串口步骤：**
> 1、下载usbipd-win：访问usbipd-win的GitHub仓库下载最新版本的安装包（https://gitcode.com/gh_mirrors/us/usbipd-win?source_module=search_result_repo 查看该链接中的README，下载“最新版本”的.msi文件）。或者，在PowerShell中使用winget命令在线安装：winget install --interactive --exact dorssel.usbipd-win。
> 2、安装usbipd-win：双击下载的.msi文件，按照安装向导完成安装。
> 3、列出所有USB设备：在Windows搜索栏中输入“PowerShell”，右键点击“Windows PowerShell”，选择“以管理员身份运行”。在PowerShell中运行usbipd list命令，列出所有连接到Windows的USB设备及其总线ID。
> 4、选择并共享USB设备：根据需要，找到并复制想要在WSL中使用的串口设备的总线ID。运行usbipd bind --busid <BUSID>命令来绑定该设备，允许它被共享到WSL。其中<BUSID>为要共享的设备总线ID。注意：在某些情况下，用户可能不需要显式运行usbipd bind命令，直接执行usbipd attach即可。但根据最新信息，建议首先使用bind命令确保设备被正确共享。
> 5、附加USB设备到WSL：在PowerShell中，使用usbipd attach --wsl --busid <BUSID>命令将USB设备附加到WSL。其中<BUSID>为之前复制的设备总线ID。
> **设置执行权限报错**
> Linux/WSL环境下，如果BurnToolCmd设置执行权限报错，请在终端执行`sudo chmod +x XXX/tool/BurnToolCmd/BurnToolCmd`后重新尝试烧录。
> XXX：工具链安装目录

## 连接烧录串口线<a name="ZH-CN_TOPIC_0000002327223389"></a>

软件镜像烧录使用串口通信协议，需要将运行HiSpark Studio for VS Code插件的电脑与目标板用串口线连接，常见的串口线有标准的串口线和USB转串口线两种。如果使用USB转串口线，需提前安装USB转串口驱动。

**图 1**  烧录串口连接示意图<a name="fig8881486226"></a>
<img src="figures/烧录串口连接示意图.png" width="700" alt="烧录串口连接示意图">

## 配置工程的烧录选项<a name="ZH-CN_TOPIC_0000002327183641"></a>

1. 配置好硬件环境。
   
   请用串口线连接好电脑和待烧录开发板。
2. <a name="li207851059104511"></a>确定所连接的串口号。
   
   打开电脑的设备管理器，查看并记录串口线对应的串口号。
   
   **图 1**  串口选择<a name="fig55913473219"></a>
<img src="figures/串口选择.png" width="700" alt="串口选择">
   
   > ![](public_sys-resources/icon-notice.gif) **须知：**
   > 如果使用USB转串口方式烧录，请安装USB转串口的驱动程序。
3. 进入工程配置界面。
   
   打开要烧录的工程后，单击工程配置的“<img src="figures/config-0.png" width="20">”按钮，进入工程配置界面。
   
   **图 2**  工程配置入口<a name="fig337113913219"></a>
   <img src="figures/工程配置入口.png" width="700" alt="工程配置入口">
4. 单击“程序加载”中的“传输方式”，默认选择“serial”串口传输，“Bin文件”中，会默认选择烧录的Bin文件，按步骤2选择端口号，波特率默认115200。
   
   **图 3**  串口烧录配置<a name="fig99981772213"></a>
<img src="figures/串口烧录配置.png" width="700" alt="串口烧录配置">
   
   > ![](public_sys-resources/icon-notice.gif) **须知：**
   > FBB系列芯片选择“serial”烧写模式时，由于是USB转串口的方式，硬件设备的差异可能会对芯片支持的烧写波特率有限制，如果想支持更高的波特率，需要改板。其中，BRANDY系列芯片烧写波特率默认限制为不超过500000，BS2X系列芯片烧写波特率默认限制为不超过2000000，3322系列芯片烧写波特率默认限制为不超过750000。
5. 单击工具栏中的烧录按钮  <img src="figures/shaolu.png" width="20">，开始执行烧写。
   
   **图 4**  烧录按钮入口<a name="fig1518855418224"></a>
   <img src="figures/烧录按钮入口.png" width="700" alt="烧录按钮入口">
6. 根据终端打印的"Connection"或其他提示，对单板进行复位，烧录成功后终端窗口输出如图5所示。
   
   **图 5**  烧录成功<a name="fig9672125742319"></a>
   <img src="figures/烧录成功.png" width="700" alt="烧录成功">

## 烧录配置<a name="ZH-CN_TOPIC_0000002293383702"></a>

本章节主要介绍支持选择性烧录bin文件的方法。使用此功能前如果编译成功则直接从本章节1开始，如果未编译，请参见“编译结果”章节进行编译，然后再根据本章节步骤进行操作。

1. <a name="p913mcpsimp"></a>单击工具栏中的“烧录配置”  <img src="figures/sp.png" width="20">  按钮，进入烧录配置界面。
   
   **图 1**  烧录配置按钮及界面<a name="fig18205858181719"></a>
   
   <img src="figures/zh-cn_image_0000002340660566.png" width="700">
2. FBB分区文件默认为打包好的.fwpkg文件，或者单击“浏览”按钮从本地文件中选择打包好的.fwpkg文件。选择完成后，烧录工具会自动地将.fwpkg文件中包含的bin内容列出。
   
   **图 2**  FBB烧录配置分区文件路径<a name="fig7273738121914"></a>
   
   <img src="figures/zh-cn_image_0000002374785221.png" width="700">
3. 勾选需要烧录的.bin文件。默认会勾选全部.bin文件，且不支持修改表格中包含loader.bin或ssb.bin的分区名属性所在行的编辑状态。而其他.bin文件可以根据烧写的需求勾选或者取消勾选。
4. 烧录之前需要配置传输方式及其他参数信息，具体操作请参见“烧录器配置”章节。
5. 单击“烧录”按钮，根据提示重启开发板，即可开始烧录。
   
   **图 3**  烧录<a name="fig7508701238"></a>
   
   <img src="figures/zh-cn_image_0000002374672533.png" width="700">
6. 开始烧录后，在分区文件下方会显示出烧录进度条，方便查看烧录进度。烧录开始后会在终端打印对应指令信息，最后烧录成功后会在界面显示“successfully”字样。
   
   **图 4**  烧录进度<a name="fig19626111222410"></a>
   
   <img src="figures/zh-cn_image_0000002374674917.png" width="700">
   
   **图 5**  烧写开始打印<a name="fig377133292415"></a>
   
   <img src="figures/zh-cn_image_0000002374810285.png" width="700">
   
   **图 6**  烧写开始打印<a name="fig17310339133719"></a>
   <img src="figures/烧写开始打印.png" width="700" alt="烧写开始打印">

# 栈分析和镜像分析<a name="ZH-CN_TOPIC_0000002293383718"></a>

HiSpark Studio for VS Code插件集成了Stack Analysis栈分析工具和Image Analysis镜像分析工具，用于分析开发过程中的内存不足、内存溢出等问题，帮助开发者更加精准地分析、定位问题。

- Stack Analysis栈分析工具是基于静态二进制分析手段，提供任务栈开销估算值和函数调用关系图示，为栈内存使用、分析、优化、问题定位等开发场景提供较为准确的静态内存分析数据参考。
- Image Analysis镜像分析工具对工程构建出的.elf文件进行内存占用分析，帮助开发者快速评估内存段、符号表使用情况。
- **栈分析**
- **镜像分析**

## 栈分析<a name="ZH-CN_TOPIC_0000002293224062"></a>

**功能介绍<a name="section148321529151417"></a>**

栈分析工具基于静态二进制分析手段，提供任务栈开销估算值和函数调用关系图示，为栈内存使用、分析、优化和问题定位等开发场景提供较为准确的静态内存分析数据参考。

**功能入口<a name="section17325123961615"></a>**

创建工程并成功编译后，单击工具栏中“<img src="figures/zh-cn_image_0000002293384078.png" width="18">”按钮进行栈分析。

**栈分析功能页面<a name="section125419179175"></a>**

栈分析结果按照函数列表和调用关系进行展示。如图1所示，功能列表页面展示每个函数的名称、内部栈开销和位置信息，其中内部栈开销单位为Byte，支持关键字搜索和排序功能。

**图 1**  功能列表页面<a name="fig4596122219257"></a>
<img src="figures/功能列表页面.png" width="700" alt="功能列表页面">

调用关系界面如图2所示，显示每个函数的调用关系，包括函数名称、调用深度、函数最大栈开销和内部栈开销，支持关键字搜索和排序功能。

**图 2**  调用图页面<a name="fig1384164712519"></a>
<img src="figures/调用图页面.png" width="700" alt="调用图页面">

**统计项说明<a name="section618mcpsimp"></a>**

- 最大开销：为当前函数所有子函数中最大栈开销与循环次数的乘积，再加上自身的开销。
  
  计算公式：max（子函数1的自身栈开销，子函数2的自身栈开销，子函数3的自身栈开销，…）× 循环次数＋函数的自身栈开销
- 本地开销：当前函数的自身栈开销。
- 深度：当前函数每增加一层子函数，深度增加一层。

## 镜像分析<a name="ZH-CN_TOPIC_0000002327223425"></a>

**功能介绍<a name="section538803119292"></a>**

镜像分析工具通过分析.elf文件，图形化展示RAM和ROM的使用情况。

**功能入口<a name="section1582295611297"></a>**

创建工程并成功编译后，单击工具栏中的“<img src="figures/jing.png" width="20">”按钮。

**功能界面<a name="section1533673619307"></a>**

内存区域页面（如图1所示）评估分析工程对内存的细分使用情况。例如WS63，显示的内存区域region包含RAM、SRAM、ITCM等，展示的信息包含每个内存区域的名称、起始内存地址、结束内存地址、总大小、空闲大小、已用大小以及使用比例，支持关键字搜索和排序功能，如图1所示。

**图 1**  内存区域页面<a name="fig1787391122717"></a>
<img src="figures/内存区域页面.png" width="700" alt="内存区域页面">

内存详细信息页面（如图2所示）展示每个内存区域包含的内存段section和内存段包含的symbol的详细信息。比如FLASH下面包含.text、.entry、.data等内存段，内存段又包含分配在该段的程序符号，支持关键字搜索和排序功能。

每一行展示的信息包含运行地址VMA（Virtual Memory Address，表示程序运行时的内存地址）、装载地址LMA（Load Memory Address，表示程序装载的内存地址）、内存段/符号的大小。

**图 2**  内存详细信息页面<a name="fig15591937172713"></a>
<img src="figures/内存详细信息页面.png" width="700" alt="内存详细信息页面">

文件大小页面（如图3所示）展示每个链接进来的.o文件所占的内存区域及其大小，支持关键字搜索和排序功能。

**图 3**  文件大小页面<a name="fig9261641289"></a>
<img src="figures/文件大小页面.png" width="700" alt="文件大小页面">

模块大小页面（如图4所示）展示了模块和组件的层级关系以及不同模块的内存占用，支持关键字搜索和排序功能。

**图 4**  模块大小页面<a name="fig1367222122813"></a>
<img src="figures/模块大小页面.png" width="700" alt="模块大小页面">

文件夹大小页面（如图5所示）展示了不同文件夹中模块的内存占用，支持关键字搜索和排序功能，支持导出Excel。

**图 5**  文件夹大小<a name="fig1075993892817"></a>
<img src="figures/文件夹大小.png" width="700" alt="文件夹大小">

# 工程调试<a name="ZH-CN_TOPIC_0000002327183673"></a>

- **调试配置选项**
- **启动调试**
- **常用调试功能**

## 调试配置选项<a name="ZH-CN_TOPIC_0000002293383734"></a>

> ![](public_sys-resources/icon-note.gif) **说明：**
> 调试配置选项中的JlinkGDBServerCL驱动需要在[J-Link官网](https://www.segger.com/products/debug-probes/j-link/models/j-link-base/)下载。

参考下图所示进行调试选项配置：

1. 选择要调试的工程：在HiSpark Studio for VS Code插件主界面中，选择要调试的工程，打开“工程配置”。
2. 修改调试选项，选择对应的调试器。
   
   **图 1**  调试选项修改<a name="fig4891141904416"></a>
   <img src="figures/调试选项修改.png" width="700" alt="调试选项修改">

## 启动调试<a name="ZH-CN_TOPIC_0000002293224078"></a>

1. 单击IDE工具栏调试按钮“<img src="figures/34.png" width="20">”，在顶部弹框中选择需要的调试模式。
   
   **图 1**  调试模式选择<a name="fig0277550163411"></a>
<img src="figures/调试模式选择.png" width="700" alt="调试模式选择">
   
   - GDB Launch（Acore）：A核重启，暂停CPU，设置PC指针从头开始运行程序（A核开头设置了一个虚拟断点）。
   - GDB Attach（Acore）：A核正在运行中，暂停CPU，程序直接停在CPU Halt处。
   - GDB Launch（Pcore）：P核重启，暂停CPU，设置PC指针从头开始运行程序（P核开头设置了一个虚拟断点）。
   - GDB Attach（Pcore）：P核正在运行中，暂停CPU，程序直接停在CPU Halt处。
   
   > ![](public_sys-resources/icon-note.gif) **说明：**
   > 3322芯片低功耗场景不支持launch模式，只支持attach模式。
2. 调试成功后示例如下图，若出现下面提示信息与工具栏调试图标，则说明已成功启动调试。
   
   **图 2**  调试成功后提示信息、调试图标及调试界面<a name="fig1214081317358"></a>
   <img src="figures/调试成功后提示信息-调试图标及调试界面.png" width="700" alt="调试成功后提示信息-调试图标及调试界面">
   
   <img src="figures/3534.png" width="300">
   
   <img src="figures/777.png" width="700">

## 常用调试功能<a name="ZH-CN_TOPIC_0000002327223441"></a>

- **调试页面**

### 调试页面<a name="ZH-CN_TOPIC_0000002327183693"></a>

调试工作界面如图1所示，主要由以下3个部分组成：

① 调试侧边栏

② 调试功能区

③ 调试控制台

**图 1**  调试工作界面<a name="fig132922116210"></a>
<img src="figures/调试工作界面.png" width="700" alt="调试工作界面">

- **调试侧边栏**
- **调试功能区**
- **调试控制台**

#### 调试侧边栏<a name="ZH-CN_TOPIC_0000002293383750"></a>

调试侧边栏集合了调试常用功能，包括变量、监视、调用堆栈、断点、内存信息查看等。

#### 调试功能区<a name="ZH-CN_TOPIC_0000002293224090"></a>

启动调试功能后，当代码执行到设置的断点时，程序会暂停，用户可根据调试功能区的按钮进行代码调试。

**图 1**  调试图标<a name="fig1952516244357"></a>
<img src="figures/调试图标.png" width="700" alt="调试图标">

- <img src="figures/zh-cn_image_0000002327223797.png" width="18">：继续运行（“F5”），当程序执行到断点时停止执行，单击此按钮程序继续执行。
- <img src="figures/zh-cn_image_0000002293384106.png" width="19">：单步跳过（“F10”），在单步调试时，直接前进到下一行（如果当前函数存在子函数调用，不会进入子函数内单步执行，而是将整个子函数当作一步执行）。
- <img src="figures/zh-cn_image_0000002293224438.png" width="17">：单步执行（“F11”），在单步调试时，遇到子函数后，进入子函数并继续单步执行。
- <img src="figures/zh-cn_image_0000002327223801.png" width="18">：单步跳出（“Shift+F11”），在单步调试执行到子函数内时，单击单步跳出会执行完子函数剩余部分，并跳出返回到上一层函数。
- <img src="figures/zh-cn_image_0000002327184057.png" width="19">：重启调试（“Ctrl+Shift+F5”），重新启动调试。
- <img src="figures/tt7.png" width="20">：停止调试（“Shift+F5”），停止调试任务，断开连接。

#### 调试控制台<a name="ZH-CN_TOPIC_0000002327223477"></a>

调试控制台用来输出调试时的打印信息，也可以输入命令与调试器交互。

- 变量查看
  
  当运行到断点处暂停时，可以在变量界面查看变量的当前值。
  
  **图 1**  查看变量当前值<a name="fig122981731183513"></a>
  <img src="figures/查看变量当前值.png" width="700" alt="查看变量当前值">
  
  支持如下4种变量类型：
  
  - 局部变量
  - 全局变量（可能会被编译器优化，可以使用关键字volatile来规避此问题）
  - 静态变量（可能会被编译器优化，可以使用关键字volatile来规避此问题）
  - 寄存器
- 监视功能
  
  在调试过程中，可以通过“监视”查看变量（包括局部变量、全局变量以及静态变量）和特定地址的取值来判断程序的运行结果是否有误。
  
  **图 2**  监视功能<a name="fig154504387355"></a>
  <img src="figures/监视功能.png" width="700" alt="监视功能">
- 查看调用栈
  
  在调试过程中，可以通过查看调用栈来分析主程序调用的各子程序的调用关系，如下图所示。
  
  **图 3**  调用堆栈功能<a name="fig102391744183516"></a>
  <img src="figures/调用堆栈功能.png" width="700" alt="调用堆栈功能">
- 内存信息查看
  
  调试过程中，可以在内存查看界面查看指定内存地址的当前信息。
  
  **图 4**  查看内存信息菜单<a name="fig1536463013146"></a>
  <img src="figures/查看内存信息菜单.png" width="700" alt="查看内存信息菜单">
  
  点击侧边栏“MEMORY”-\>“open memory view”菜单项，弹出内存信息查看窗口。
  
  **图 5**  内存信息查看窗口<a name="fig126654372298"></a>
  <img src="figures/内存信息查看窗口.png" width="700" alt="内存信息查看窗口">
  
  在输入框“address”、“offset”、“Length”中分别输入起始地址、偏移量和长度，可查看指定范围的内存地址信息。默认情况下，查看从“address”指定的地址开始、连续128个字节的地址的值。
  
  **图 6**  查看内存信息<a name="fig416563263713"></a>
  <img src="figures/查看内存信息.png" width="700" alt="查看内存信息">
  
  点击“Save”按钮，可将当前地址信息以表格的形式保存到本地。
  
  点击侧边栏“MEMORY”-\>“open memory view”菜单项，可同时打开多个Memory窗口。

# 远程开发工具<a name="ZH-CN_TOPIC_0000002519102031"></a>

本章节主要介绍HiSpark Studio for VS Code插件中的远程开发工具。该工具可监测本地工程的文件变化并实时传输至服务器端，在服务器端编译后，将编译产物复制到本地工程目录，供本地执行调试、烧录等功能。

主要包括四个功能：连接服务器、打开远程配置管理、执行编译命令并传输文件、断开服务器。

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - 在使用远程开发工具之前，建议本地创建的工程目录与服务端需要传输文件的SDK目录保持一致，如：
> - 服务端目录：/home/developer/1a21
> - 本地工程目录：D:\\\\home\\\\developer\\\\1a21
> - 如果已经按照bash环境准备进行tar的环境配置，需要去除添加的Git目录下的“..\\Git\\usr\\bin”的系统环境变量，保证where tar指向的是系统默认的tar，而不是用户手动安装的git目录下的tar。

- **连接服务器**
- **打开远程配置管理**
- **执行编译命令并传输文件**
- **断开服务器**

## 连接服务器<a name="ZH-CN_TOPIC_0000002519102873"></a>

在本地和服务端分别创建工程并保持路径一致后，打开本地创建的工程目录，单击“远程开发工具”，会弹出远程开发选项框。

**图 1**  远程开发工具<a name="fig6230341121613"></a>
<img src="figures/远程开发工具.png" width="700" alt="远程开发工具">

**图 2**  远程开发选项框<a name="fig99090381711"></a>
<img src="figures/远程开发选项框.png" width="700" alt="远程开发选项框">

在弹出的远程开发选项框中选择“连接服务器”。按照提示进行服务器的连接，在选择模式中选择“文件同步Linux连接”，输入需要连接的服务器地址、连接端口号、用户名，选择认证方式（一般为“密码”），最后输入密码，连接步骤完成后，会在“输出”界面显示连接成功。

**图 3**  选择连接模式<a name="fig113221229102415"></a>
<img src="figures/选择连接模式.png" width="700" alt="选择连接模式">

**图 4**  输入服务器地址<a name="fig5884613249"></a>
<img src="figures/输入服务器地址.png" width="700" alt="输入服务器地址">

**图 5**  输入端口号<a name="fig135241815142515"></a>
<img src="figures/输入端口号.png" width="700" alt="输入端口号">

**图 6**  选择认证方式，一般选择密码<a name="fig073063119252"></a>
<img src="figures/选择认证方式-一般选择密码.png" width="700" alt="选择认证方式-一般选择密码">

**图 7**  输入密码<a name="fig12219125619253"></a>
<img src="figures/输入密码.png" width="700" alt="输入密码">

**图 8**  连接成功状态<a name="fig63161839172614"></a>
<img src="figures/连接成功状态.png" width="700" alt="连接成功状态">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 若已经在该文件夹目录下连接过远程环境，在点击“连接服务器”后，会弹出是否使用已有的连接和重新配置连接的窗口。如果选择使用现有配置快速连接，则会直接跳转到输入密码阶段，输入密码后即可重新连接成功；如果选择重新连接，则会从连接选择模式开始重复上述操作。
> **图 9**  非首次连接在点击“连接服务器”后的界面显示<a name="fig18468435102816"></a>
> <img src="figures/非首次连接在点击-连接服务器-后的界面显示.png" width="700" alt="非首次连接在点击-连接服务器-后的界面显示">

## 打开远程配置管理<a name="ZH-CN_TOPIC_0000002486863030"></a>

在首次连接或者点击“远程开发工具”后弹出的“打开远程配置管理”选项框，会弹出“远程编译执行配置”界面。

**图 1**  远程编译执行配置界面<a name="fig655612429325"></a>
<img src="figures/远程编译执行配置界面.png" width="700" alt="远程编译执行配置界面">

各选项框介绍说明：

- Windows工程目录：选择本地工程即当前打开的文件夹，用于与服务器目录的数据同步与传输。
- Linux工程目录：选择服务器端的工程目录，用于与本地端目录的数据同步。
- 编译命令配置：需要在服务器端执行的命令操作，一般为cd到服务器端的工程目录下，再执行编译命令，即能够编译出需要传输给Windows目录的编译产物（即output目录）。
- 编译成功后自动下载产物：默认勾选，选择后执行编译命令会默认将Linux工程目录下的output文件夹复制到“Windows工程目录”选择的路径（即本地工程路径）；如果编译后生成的烧录fwpkg文件在“tools/pkg/fwpkg”下，会额外复制一份“tools/pkg/fwpkg”文件夹到本地工程目录，如bs系列和brandy工程。
- 文件同步忽略规则：与服务端建立连接后，会监测本地文件的变化改动，再自动上传到远程服务端，忽略规则可以输入不需要监测的文件夹。如果工程目录过大需要监测的文件过多，会导致性能变差，IDE工具出现卡顿等情况。

各选项选择完后，需要点击保存配置。

## 执行编译命令并传输文件<a name="ZH-CN_TOPIC_0000002487022994"></a>

点击“远程开发工具”后弹出的“执行编译命令并传输文件”选项框，会自动先执行“编译命令配置”中选择的编译命令，编译结束后，会自动将服务器端的编译产物传输至本地工程目录。

> ![](public_sys-resources/icon-note.gif) **说明：**
> 为提升传输速率，远程传输文件至本地采取先压缩编译产物为tar.gz压缩包，再传输单个压缩包文件，传输到本地后，再执行解压的操作。解压完成后会自动删除服务器和本地的tar.gz压缩包。如果编译产物（即output文件夹）下文件或者文件夹过多，会影响压缩和解压时间，造成传输时间过长。

服务器端的编译产物传输至本地后，可正常在本地使用调试、烧录等功能。

## 断开服务器<a name="ZH-CN_TOPIC_0000002519022891"></a>

点击“远程开发工具”后弹出的“断开服务器”选项框，会断开与服务器的连接。

# 串口控制台工具<a name="ZH-CN_TOPIC_0000002293383834"></a>

本章节主要介绍HiSpark Studio for VS Code插件中关于串口操作的工具监视器（Monitor）。主要功能包括显示串口列表、连接串口、断开串口连接、接收串口消息、给串口发送消息、清空串口输出区、开启\\关闭屏幕自动滚动等。

> ![](public_sys-resources/icon-note.gif) **说明：**
> 串口工具以“\\r\\n“作为每行的分隔符，所以要求被打印的每行字符串都要以“\\r\\n“结尾，否则可能会出现程序结尾打印丢失的情况。

- **打开监视器**
- **连接串口**
- **查看消息**
- **发送消息**
- **断开连接**
- **扩展工具使用**

## 打开监视器<a name="ZH-CN_TOPIC_0000002293224166"></a>

运行HiSpark Studio for VS Code插件后打开VS Code终端，找到终端区域、切换到“监视器”选项卡，如图1所示。

**图 1**  切换到“监视器”选项卡<a name="fig17771812125220"></a>
<img src="figures/切换到-监视器-选项卡.png" width="700" alt="切换到-监视器-选项卡">

**图 2**  监视器界面介绍<a name="fig14318546512"></a>

<img src="figures/terminal.png" width="700">

- ①：串口配置区
  
  端口：显示当前电脑所连接的串口设备，单击“<img src="figures/zh-cn_image_0000002327184165.png" width="19">”按钮刷新串口列表。
  
  波特率：选择串口波特率，范围：300～250000。
  
  行尾：当给串口发送消息时，工具会根据此选项自动添加字符。
  
  - CRLF代表“\\r\\n”。
  - CR代表“\\r”。
  - LF代表“\\n”。
- ②：功能按钮区
  
  <img src="figures/zh-cn_image_0000002293384214.png" width="88">：连接串口按钮。当连接串口后，按钮状态会变成<img src="figures/zh-cn_image_0000002293224546.png" width="88">，单击此按钮会断开串口连接。
  
  <img src="figures/zh-cn_image_0000002327223909.png" width="24">：时间戳按钮。开启时会在每行输出前加上时间戳显示，如果按钮处于关闭状态则不显示时间戳，如图3所示。
  
  **图 3**  时间戳设置效果示例<a name="fig1040102615572"></a>
  <img src="figures/时间戳设置效果示例.png" width="700" alt="时间戳设置效果示例">
  
  <img src="figures/zh-cn_image_0000002293384218.png" width="22">：隐藏输入框，隐藏/显示输入框界面如图4、图5所示。
  
  **图 4**  显示输入框<a name="fig209583373583"></a>
  <img src="figures/显示输入框.png" width="700" alt="显示输入框">
  
  **图 5**  隐藏输入框<a name="fig17214483580"></a>
  <img src="figures/隐藏输入框.png" width="700" alt="隐藏输入框">
  
  <img src="figures/zh-cn_image_0000002327184173.png" width="16">：开启/关闭屏幕自动滚动。
  
  <img src="figures/zh-cn_image_0000002293384226.png" width="20">：清空输出区。
  
  <img src="figures/zh-cn_image_0000002293224562.png" width="20">：最大化面板。
  
  <img src="figures/zh-cn_image_0000002327223921.png" width="19">：关闭面板。
- ③：输出区。
- ④：输入区。
- ⑤：发送消息按钮，单击按钮或按回车键向串口发送输入区信息，默认编码为UTF-8。

## 连接串口<a name="ZH-CN_TOPIC_0000002327223529"></a>

单击“<img src="figures/zh-cn_image_0000002327184177.png" width="91">”即可连接串口。连接串口前，输入区默认处于未激活状态（不可输入、不可点击）；连接串口后，输入区变为激活状态，串口配置区的监视模式选项变为未激活状态。

**图 1**  开始监视功能<a name="fig58721954163517"></a>
<img src="figures/开始监视功能.png" width="700" alt="开始监视功能">

## 查看消息<a name="ZH-CN_TOPIC_0000002327183793"></a>

在输出区可以查看串口发送的消息。

**图 1**  查看串口消息<a name="fig15659205953510"></a>
<img src="figures/查看串口消息.png" width="700" alt="查看串口消息">

## 发送消息<a name="ZH-CN_TOPIC_0000002293383846"></a>

在下方输入区输入消息后单击发送按钮或者单击键盘回车按钮发送消息。

**图 1**  发送消息<a name="fig1571552215364"></a>
<img src="figures/发送消息.png" width="700" alt="发送消息">

## 断开连接<a name="ZH-CN_TOPIC_0000002293224182"></a>

单击“<img src="figures/zh-cn_image_0000002327184181.png" width="86">”按钮断开串口连接。

**图 1**  停止监视<a name="fig4728173953619"></a>
<img src="figures/停止监视.png" width="700" alt="停止监视">

## 扩展工具使用<a name="ZH-CN_TOPIC_0000002327223541"></a>

扩展工具主要用于添加用户常用的命令，添加后点击名称即可发送命令。扩展工具在出厂时预置了一些常用命令，如果用户用不到这些命令，可以选择清空表格，然后手动添加所需命令，或通过Excel表格导入方式进行添加。除此之外，扩展工具还有循环发送功能，在后面章节中会详细介绍。

- 打开扩展工具。单击扩展按钮，会在右侧展开扩展页面。
  
  **图 1**  扩展工具<a name="fig1786951684210"></a>
  <img src="figures/扩展工具.png" width="700" alt="扩展工具">
  
  单击此按钮可最大化面板。
  
  **图 2**  监视器面板最大化按钮<a name="fig11226132364210"></a>
  <img src="figures/监视器面板最大化按钮.png" width="700" alt="监视器面板最大化按钮">
- 单击清空列表按钮可清空列表。
  
  **图 3**  清空列表功能<a name="fig6150152913426"></a>
  <img src="figures/清空列表功能.png" width="700" alt="清空列表功能">
  
  如果清空出厂表格后需要恢复，可导入安装目录下“C:\\Users\\用户xxx\\.vscode\\extensions\\hispark.hisparkstudio-x.x.x\\dist\\resources\\terminal\\resources\\excelFile”中的“rawData.xlsx”表格。
- 手动新增命令行。单击“增加一行”按钮。
  
  **图 4**  新增一行功能<a name="fig1455312345424"></a>
  <img src="figures/新增一行功能.png" width="700" alt="新增一行功能">
  
  添加数据规则：
  
  数据格式：下拉框模式，下拉选项有utf8、bin、hex。
  
  命令：发送给串口的命令字符串。
  
  名称：识别命令功能的字符串。单击此按钮可立即发送命令给串口（前提是工具处于监测状态）。
  
  顺序：在使用循环发送功能时，顺序大于0，才会进行循环发送。如果有多个命令的顺序大于0，并且数值一样，则按照由上到下的顺序发送。
  
  延时发送：先发送一次，再进行延时，单位：ms。
  
  操作：有编辑和删除两个选项，单击编辑可对命令行进行修改，单击删除可删除命令行。
- 批量添加命令。
  
  1. 下载导入模板。导入模板会自动保存到“C:\\Users\\用户xxx\\.vscode\\extensions\\hispark.hisparkstudio-x.x.x\\dist\\resources\\terminal\\resources\\excelFile\\template.xlsx”。
     
     **图 5**  下载导入模板功能<a name="fig131081446114215"></a>
     <img src="figures/下载导入模板功能.png" width="700" alt="下载导入模板功能">
     
     导入模板中会显示需要导入的列，以及每个列的规则。
     
     **图 6**  导入模板<a name="fig4205125119425"></a>
     <img src="figures/导入模板.png" width="700" alt="导入模板">
  2. 填写导入模板。
     
     **图 7**  导入模板数据填写<a name="fig249315718422"></a>
     <img src="figures/导入模板数据填写.png" width="700" alt="导入模板数据填写">
  3. 导入“导入模板“文件。
     
     **图 8**  导入模板数据<a name="fig11424622434"></a>
     <img src="figures/导入模板数据.png" width="700" alt="导入模板数据">
     
     <img src="figures/Snipaste_2025-05-26_19-25-41.png" width="700">
  4. 查看导入数据。
     
     **图 9**  查看导入数据<a name="fig18481158134314"></a>
     <img src="figures/查看导入数据.png" width="700" alt="查看导入数据">
     
     <img src="figures/zh-cn_image_0000002327184197.png" width="700">
  5. 查看导入结果表格。导入结果列会显示导入不成功的原因。
     
     **图 10**  查看导入结果<a name="fig1017011484311"></a>
     <img src="figures/查看导入结果.png" width="700" alt="查看导入结果">
- 循环发送。
  
  1. 设置顺序和延时。
     
     **图 11**  设置顺序和延时功能<a name="fig6466102611434"></a>
     <img src="figures/设置顺序和延时功能.png" width="700" alt="设置顺序和延时功能">
     
     上图表格执行命令的顺序：
     
     首先，发送“起蓝牙“命令，延时1000ms；然后，发送“键盘模式“命令，延时1000ms；最后，发送“鼠标模式“命令，延时1000ms；结束。这是一个循环发送的周期，循环发送会持续重复该周期。
     
     “修改蓝牙地址”命令并不会发送，因为它的顺序不大于0。
  2. 打开串口监测。
     
     **图 12**  串口监测功能<a name="fig5219203116435"></a>
     <img src="figures/串口监测功能.png" width="700" alt="串口监测功能">
  3. 打开循环发送开关（发送的命令自带“回车换行”）。开启后，会在左侧输出栏看到发送的命令。
     
     **图 13**  循环发送功能<a name="fig734614365431"></a>
     <img src="figures/循环发送功能.png" width="700" alt="循环发送功能">
     
     红色字体表明正在发送此条命令或者正在延时。

# Kconfig配置<a name="ZH-CN_TOPIC_0000002327183797"></a>

本功能主要用于控制工程的编译构建，支持通过图形化界面管理编译配置。

打开工程后，单击工具栏中的系统配置“<img src="figures/zh-cn_image_0000002327291945.png" width="34">”按钮，如图1所示。

**图 1**  系统配置入口<a name="fig1334674314438"></a>
<img src="figures/系统配置入口.png" width="700" alt="系统配置入口">

系统配置界面如图2所示。

**图 2**  系统配置界面<a name="fig11641828725"></a>
<img src="figures/系统配置界面.png" width="700" alt="系统配置界面">

- **按钮功能介绍**

## 按钮功能介绍<a name="ZH-CN_TOPIC_0000002293383854"></a>

系统配置界面按钮功能如下：

① save：配置文件默认保存至\`$\{menu\_config\_build\_target\}\`下。

② save as：自定义保存路径，默认配置文件名 \`$\{menu\_config\_build\_target\}\`.config。

③ save\(minimal\)：自定义保存路径和配置文件名称，且只保存修改过的配置项。

④ open：自定义加载配置文件。

⑤ jump to：配置项搜索。

⑥ show name：显示列名（Option-Name）。

⑦ show all：显示隐藏配置项。

⑧ Single-menu mode：单个菜单模式。

> ![](public_sys-resources/icon-note.gif) **说明：**
> Linux/WSL环境下没有图形化界面，操作界面显示为命令行模式。

# GUI工程创建与使用<a name="ZH-CN_TOPIC_0000002399739733"></a>

HiSpark Studio支持创建GUI工程和GUI模拟器的使用。按照“新建工程”指导新建Brandy工程后，单击工具栏菜单下的“创建GUI应用”图标可在Brandy的工程中创建GUI工程。

> ![](public_sys-resources/icon-note.gif) **说明：**
> Linux/WSL环境下不支持本章节功能。
> 仅Brandy/3322系列芯片支持本章节功能。

**图 1**  创建GUI工程<a name="fig1874373411538"></a>
<img src="figures/创建GUI工程.png" width="700" alt="创建GUI工程">

- 像素值：默认454×454对应生成的画布大小，在形状为circle（圆形）时，分辨率为直径×直径，当创建圆形且分辨率的两个像素值不相等时，取默认466×466，在形状为rectangle（矩形）时，分辨率为长×宽。像素值可以自定义，取值范围为1～600。
- 形状：分为circle（圆形）和rectangle（矩形），如果不主动选择，默认为圆形。

> ![](public_sys-resources/icon-note.gif) **说明：**
> 使用GUI模拟器前，需要将版本转测包中的res复制到“software\\code\\sdk\\application\\wearable”中。

- **GUI界面介绍**
- **模拟器效果展示方法**
- **图形工具介绍**
- **GUI工程使用常见问题**

## GUI界面介绍<a name="ZH-CN_TOPIC_0000002366099950"></a>

- **GUI拖拽界面介绍**
- **页面树介绍**
- **组件介绍**

### GUI拖拽界面介绍<a name="ZH-CN_TOPIC_0000002399619865"></a>

GUI工程创建后，会在工程目录“application/wearable/nativeapp/nativeui/应用名”下创建一个以工程名命名的GUI文件，打开后会呈现表盘形状，可自定义表盘内容。

当前支持的组件包括Button、Image、Progress、List、Label、ToggleButton、CheckBox、RadioButton、LabelButton、ScrollView、CircleProgress、ArcLabel、SwipeView、TextureMapper、Slider、Picker、TimePicker、Chart、EditText、ImageAnimator、DigitalClock、Qrcode、AnalogClock、SweepClock、Coverflow、Barcode、CardPage、CrossView、MapView、RollerView、HexagonsList、IcosahedronView、CanvasExt、TransformList、SlipflowView、TransformGroup、Coverflow2、ParticleView组件和Root组件。

**图 1**  GUI拖拽界面<a name="fig1973444016553"></a>

<img src="figures/gui8.png" width="700">

- FileTree：页面树，可以选择添加、删除页面、修改页面标题、设置主页面。
- UI Control：UI组件栏，可以将相应的组件选中并拖动到画布（Canvas）中，实现组件的添加。
- Component Tree：组件树，在低代码开发界面中，开发者可以直观地看到组件的层级结构和摘要信息。开发者可以通过选中组件树中的组件（画布中对应的组件被同步选中），实现画布内组件的快速定位；单击组件后的“<img src="figures/Y.png" width="17">”，可以隐藏/显示相应的组件。
- Panel：功能面板，包括CrossView、Coverflow2组件用到的子组件切换，以及常用的画布缩小放大、组件左对齐、垂直居中对齐、右对齐、顶对齐、水平居中对齐、底对齐、图层切换、撤销、显示/隐藏组件虚拟边框、一键生成C++代码等。
- Canvas：画布，开发者可在此区域对组件进行拖拽、拉伸等可视化操作，构建UI界面布局效果。
- Attributes & Styles：属性样式栏，选中画布中的相应组件后，在右侧属性样式栏可以对该组件的属性样式进行配置。
  - General：对应图标<img src="figures/zh-cn_image_0000002366100070.png" width="17">，用于设置Width、Height、Background、Position、Display等常规样式。
  - Feature：对应图标<img src="figures/zh-cn_image_0000002399619989.png" width="27">，用于设置组件的特有样式，如描述Text组件文字大小的FontSize样式等。
  - Events：对应图标<img src="figures/zh-cn_image_0000002365940174.png" width="22">，为组件绑定相关事件，并设置绑定事件的回调函数。

### 页面树介绍<a name="ZH-CN_TOPIC_0000002484446082"></a>

- **页面的新增删除修改标题设置主页**

#### 页面的新增删除修改标题设置主页<a name="ZH-CN_TOPIC_0000002516526041"></a>

- 页面新增
  
  点击FileTree新增页面按钮，可新增空白页面。
  
  **图 1**  页面新增<a name="fig84442050152819"></a>
  <img src="figures/页面新增.png" width="700" alt="页面新增">
- 页面删除
  
  将鼠标悬浮在需要删除的页面右侧，弹出操作框后，点击Delete按钮，可删除页面。
  
  **图 2**  页面删除<a name="fig854641933212"></a>
  <img src="figures/页面删除.png" width="700" alt="页面删除">
- 页面修改标题
  
  将鼠标悬浮在需要修改标题的页面右侧，弹出操作框后，点击Rename按钮，可修改标题。重命名只能包含字母（A-Z，a-z）、数字（0-9）和下划线（\_），必须以字母开头。
  
  **图 3**  页面修改标题<a name="fig99631158173419"></a>
  <img src="figures/页面修改标题.png" width="700" alt="页面修改标题">
- 页面设置主页
  
  将鼠标悬浮在需要设置主页的页面右侧，弹出操作框后，点击Set HomePage按钮，可设置主页。
  
  **图 4**  页面设置主页<a name="fig5146716103813"></a>
  <img src="figures/页面设置主页.png" width="700" alt="页面设置主页">

### 组件介绍<a name="ZH-CN_TOPIC_0000002365940054"></a>

- **组件的复制粘贴删除**
- **组件的共有属性**
- **Button组件**
- **Image组件**
- **Progress组件**
- **List组件**
- **Label组件**
- **ToggleButton组件**
- **CheckBox组件**
- **RadioButton组件**
- **LabelButton组件**
- **ScrollView组件**
- **CircleProgress组件**
- **ArcLabel组件**
- **SwipeView组件**
- **TextureMapper组件**
- **Slider组件**
- **Picker组件**
- **TimePicker组件**
- **Chart组件**
- **EditText组件**
- **DigitalClock组件**
- **ImageAnimator组件**
- **Qrcode组件**
- **AnalogClock组件**
- **SweepClock组件**
- **Coverflow组件**
- **Barcode组件**
- **CardPage组件**
- **CrossView组件**
- **MapView组件**
- **RollerView组件**
- **HexagonsList组件**
- **IcosahedronView组件**
- **CanvasExt组件**
- **TransformList组件**
- **SlipflowView组件**
- **TransformGroup组件**
- **Coverflow2组件**
- **ParticleView组件**
- **LabelExt组件**
- **ChartPillarExt组件**
- **ListNested组件**
- **SwipeViewNested组件**
- **LottieView组件**
- **Root组件**
- **用户代码编辑**
- **组件对齐**
- **组件层级移动**
- **高斯模糊属性**

#### 组件的复制粘贴删除<a name="ZH-CN_TOPIC_0000002399739737"></a>

以Button组件为例，在画布内已经拖入Button组件的前提下进行操作：

- 组件删除
  
  组件可以通过两种方式删除：
  
  - 右键点击画布中的Button组件显示操作栏，在弹出的操作栏点击Delete。
  - 选中Button组件，通过键盘Del键删除。
  
  **图 1**  组件删除<a name="fig1276601114010"></a>
  <img src="figures/组件删除.png" width="700" alt="组件删除">
- 组件复制
  
  组件可以通过两种方式复制：
  
  - 右键点击画布中的Button组件显示操作栏，在弹出的操作栏点击Copy，再次右键后点击Paste，实现复制粘贴。
  - 选中Button组件，通过键盘Ctrl+C和Ctrl+V实现复制粘贴。
  
  **图 2**  组件复制<a name="fig1175415591409"></a>
  <img src="figures/组件复制.png" width="700" alt="组件复制">
  
  **图 3**  组件粘贴<a name="fig14301013164114"></a>
  <img src="figures/组件粘贴.png" width="700" alt="组件粘贴">

#### 组件的共有属性<a name="ZH-CN_TOPIC_0000002366099954"></a>

以Button组件为例，选中组件栏中的Button组件，将其拖拽至中央画布区域，松开鼠标，实现一个Button组件的添加。（组件会生成在画布上坐标(100,100)的位置）。

**图 1**  拖拽组件<a name="fig10331115148"></a>
<img src="figures/拖拽组件.png" width="700" alt="拖拽组件">

以下操作需在画布内已经拖入Button组件的前提下进行操作。

- 选中画布内的Button组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399739865.png" width="20">样式图标（General），在展开的General栏中修改Button组件的ID。ID并非生成代码时的变量名，因此多个组件可以设置同一个ID。
  
  **图 2**  通用属性配置：Id<a name="fig1333343519513"></a>
  <img src="figures/通用属性配置-Id.png" width="700" alt="通用属性配置-Id">
- 选中画布内的Button组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399620009.png" width="20">样式图标（General），在展开的General栏中修改Button组件的宽高、位置；另外还可以通过拖拽的方式放大缩小Button组件以及改变组件的位置。
  
  **图 3**  通用属性的配置<a name="fig9966191695113"></a>
  <img src="figures/通用属性的配置.png" width="700" alt="通用属性的配置">
- 选中Button组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399739869.png" width="20">样式图标（General），在展开的General栏中修改Button组件的BackgroundColor属性改变背景颜色。
  
  **图 4**  修改背景颜色<a name="fig759121615415"></a>
  <img src="figures/修改背景颜色.png" width="700" alt="修改背景颜色">
- 选中画布内的Button组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399620013.png" width="20">样式图标（General），在展开的General栏中修改Button组件GaussOption框中的FullScreenState、GaussianBlur属性以改变组件的高斯模糊。该操作需在同时选中两个组件的前提下进行。FullScreenState为false时高斯模糊设置不生效，FullScreenState为true时高斯模糊设置生效。GaussianBlur属性控制高斯模糊的模糊程度。详情请参见“高斯模糊属性”。
  
  **图 5**  高斯模糊<a name="fig48601252103118"></a>
  <img src="figures/高斯模糊.png" width="700" alt="高斯模糊">
- 选中画布内的Button组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399739877.png" width="20">样式图标（General），在展开的General栏中修改Button组件Border框中的Width、Color、Radius属性改变边框的样式。
  
  **图 6**  修改组件边框样式<a name="fig421010341275"></a>
  <img src="figures/修改组件边框样式.png" width="700" alt="修改组件边框样式">
- 选中Button组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399620017.png" width="20">样式图标（General），在展开的General栏中修改Button组件Margin框中的MarginTop和MarginLeft属性调整组件相对表盘的位置。
  
  **图 7**  修改组件相对窗口位置<a name="fig19389527155313"></a>
  <img src="figures/修改组件相对窗口位置.png" width="700" alt="修改组件相对窗口位置">
- 选中Button组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399739897.png" width="20">样式图标（General），在展开的General栏中修改Padding改变组件padding部分四个方向的宽度。
  
  **图 8**  组件相对边框位置<a name="fig88161839549"></a>
  <img src="figures/组件相对边框位置.png" width="700" alt="组件相对边框位置">

#### Button组件<a name="ZH-CN_TOPIC_0000002399619869"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中画布内的Button组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399620053.png" width="36">样式图标（Feature），在展开的Feature栏中修改Button组件的DefaultImgSrc属性加载图片。
  
  **图 1**  DefaultImgSrc属性配置<a name="fig1930105513132"></a>
  <img src="figures/DefaultImgSrc属性配置.png" width="700" alt="DefaultImgSrc属性配置">
- 选中画布内的Button组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399739953.png" width="36">样式图标（Feature），在展开的Feature栏中修改Button组件的ImageX和ImageY属性调整图片位置（修改前需要设置DefaultImgSrc属性）。
  
  **图 2**  ImageX/ImageY属性配置<a name="fig11684412161613"></a>
  <img src="figures/ImageX-ImageY属性配置.png" width="700" alt="ImageX-ImageY属性配置">
- 选中画布内的Button组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399620097.png" width="36">样式图标（Feature），在展开的Feature栏中修改Button组件的ImageOpacity属性调整图片透明度（需要设置DefaultImgSrc）。
  
  **图 3**  ImageOpacity属性配置<a name="fig11511623928"></a>
  <img src="figures/ImageOpacity属性配置.png" width="700" alt="ImageOpacity属性配置">
- 选中画布内的Button组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399739961.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  Button组件支持的事件有：OnClick、OnLongPress、OnPress、OnCancel和OnRelease。
  
  **图 4**  回调事件配置<a name="fig141561846154620"></a>
  <img src="figures/回调事件配置.png" width="700" alt="回调事件配置">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - General页面下的MarginBottom属性和MarginRight属性无界面渲染效果。
> - Feature页面下的TriggeredImgSrc属性、TriggeredBackgroundColor属性、TriggeredBorderColor属性无界面渲染效果，可以在模拟器上查看渲染效果。
> - DefaultImgSrc和TriggeredImgSrc不支持中文路径。

#### Image组件<a name="ZH-CN_TOPIC_0000002365940058"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中画布内的Image组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399620105.png" width="36">样式图标（Feature），在展开的Feature栏中修改Image组件的ImagePath属性加载图片。
  
  在已有选中图片情况下再次选择相同图片，或者在浏览文件时关闭窗口或单击取消时，界面渲染效果将不会改变。需要清空图片可以删除输入框中的文本。
  
  **图 1**  特有属性配置：ImagePath<a name="fig1383616220353"></a>
<img src="figures/特有属性配置-ImagePath.png" width="700" alt="特有属性配置-ImagePath">
  
  > ![](public_sys-resources/icon-note.gif) **说明：**
  > ImagePath不支持中文路径。
  > 导入的Bin文件必须是图片转的Bin，否则模拟器会异常。
- 选中画布内的Image组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399739965.png" width="36">样式图标（Feature），在展开的Feature栏中修改Image组件的ImageOpacity属性调整图片透明度。
  
  **图 2**  特有属性配置：ImageOpacity<a name="fig19953128113813"></a>
  <img src="figures/特有属性配置-ImageOpacity.png" width="700" alt="特有属性配置-ImageOpacity">
- 选中画布内的Image组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399620109.png" width="36">样式图标（Feature），在展开的Feature栏中修改Image组件的AutoEnable属性。
  
  - AutoEnable属性设置为true时，组件会被限制为图片本身的大小，无法通过拖拽组件或直接修改Width/Height值来修改组件的显示效果。
  - AutoEnable属性设置为false时，会按照组件大小展示图片，设置不同的ImageResizeMode属性值可以呈现不同的显示效果。
  
  **图 3**  特有属性配置：AutoEnable<a name="fig32031181480"></a>
  <img src="figures/特有属性配置-AutoEnable.png" width="700" alt="特有属性配置-AutoEnable">
- 选中画布内的Image组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399739969.png" width="36">样式图标（Feature），在展开的Feature栏中修改Image组件的ImageResizeMode属性。
  
  - AutoEnable属性设置为true时，ImageResizeMode属性不生效。
  - AutoEnable属性设置为false时，ImageResizeMode生效，对应不同的显示效果。
  
  当前共有6种模式对应的效果示例如下（图片原始尺寸为80×80）：
  
  - ImageResizeMode - None
    
    **图 4**  图片平铺（图中组件大小为200×200）<a name="fig137143716018"></a>
    <img src="figures/图片平铺（图中组件大小为200-200）.png" width="700" alt="图片平铺（图中组件大小为200-200）">
  - ImageResizeMode - Cover
    
    **图 5**  图片覆盖组件1（图中组件大小为100×200）<a name="fig203183291602"></a>
    <img src="figures/图片覆盖组件1（图中组件大小为100-200）.png" width="700" alt="图片覆盖组件1（图中组件大小为100-200）">
    
    **图 6**  图片覆盖组件2（图中组件大小为200×100）<a name="fig119815431203"></a>
    <img src="figures/图片覆盖组件2（图中组件大小为200-100）.png" width="700" alt="图片覆盖组件2（图中组件大小为200-100）">
  - ImageResizeMode - Contain
    
    **图 7**  图片被组件包含在内（图中组件大小为100×200）<a name="fig9776119513"></a>
    <img src="figures/图片被组件包含在内（图中组件大小为100-200）.png" width="700" alt="图片被组件包含在内（图中组件大小为100-200）">
    
    **图 8**  图片被组件包含在内（图中组件大小为200×100）<a name="fig1741815478118"></a>
    <img src="figures/图片被组件包含在内（图中组件大小为200-100）.png" width="700" alt="图片被组件包含在内（图中组件大小为200-100）">
  - ImageResizeMode - Fill
    
    **图 9**  图片填满组件（图中组件大小为200×100）<a name="fig143226171166"></a>
    <img src="figures/图片填满组件（图中组件大小为200-100）.png" width="700" alt="图片填满组件（图中组件大小为200-100）">
  - ImageResizeMode - Center
    
    **图 10**  图片处在组件中心（图中组件大小为100×100）<a name="fig78523241168"></a>
<img src="figures/图片处在组件中心（图中组件大小为100-100）.png" width="700" alt="图片处在组件中心（图中组件大小为100-100）">
    
    > ![](public_sys-resources/icon-note.gif) **说明：**
    > 组件宽高任一为0时，图片隐藏。
  - ImageResizeMode - Scale Down
    
    **图 11**  组件宽高较大时，图片按原有尺寸显示<a name="fig1319195718418"></a>
    <img src="figures/组件宽高较大时-图片按原有尺寸显示.png" width="700" alt="组件宽高较大时-图片按原有尺寸显示">
    
    **图 12**  组件宽高较小时，图片会缩小并被组件包含在内<a name="fig89661415359"></a>
    <img src="figures/组件宽高较小时-图片会缩小并被组件包含在内.png" width="700" alt="组件宽高较小时-图片会缩小并被组件包含在内">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - 组件的特有属性ImagePath未设置时，模拟器中不会显示组件。
> - General页面下的MarginBottom属性和MarginRight属性没有任何界面渲染效果，可在模拟器上查看效果。
> - Feature页面下的BlurLevel属性和Algorithm属性没有任何界面渲染效果，可在模拟器上查看效果。
> - Image组件不支持配置回调事件。

#### Progress组件<a name="ZH-CN_TOPIC_0000002399739741"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中Progress组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399620153.png" width="36">样式图标（Feature），在展开的Feature栏中ForegroundStyle窗口中修改Progress组件的前景色。
  
  **图 1**  修改前景色<a name="fig117331435123510"></a>
  <img src="figures/修改前景色.png" width="700" alt="修改前景色">
- 选中Progress组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399740017.png" width="36">样式图标（Feature），在展开的Feature栏中BackgroundStyle窗口中修改Progress组件的背景色。
  
  **图 2**  修改背景色<a name="fig879142417363"></a>
  <img src="figures/修改背景色.png" width="700" alt="修改背景色">
- 选中Progress组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399620157.png" width="36">样式图标（Feature），在展开的Feature栏中修改Progress组件的ProgressWidth和ProgressHeight属性改变组件的宽度和高度。
  
  **图 3**  修改组件宽高<a name="fig119251539922"></a>
  <img src="figures/修改组件宽高.png" width="700" alt="修改组件宽高">
- 选中Progress组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399740029.png" width="36">样式图标（Feature），在展开的Feature栏中修改Progress组件的Direction属性以修改进度条方向。
  
  **图 4**  修改进度条方向<a name="fig4795118183012"></a>
  <img src="figures/修改进度条方向.png" width="700" alt="修改进度条方向">
- 选中Progress组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399620269.png" width="36">样式图标（Feature），在展开的Feature栏中修改Progress组件的Value属性以修改进度。
  
  **图 5**  修改进度<a name="fig18362191916365"></a>
  <img src="figures/修改进度.png" width="700" alt="修改进度">
- 选中Progress组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399740281.png" width="36">样式图标（Feature），在展开的Feature栏中修改Progress组件的ForeImage属性添加已走过进度的图片和BackImage属性添加进度条整体的图片。路径不能为空，不支持中文路径。
  
  **图 6**  添加图片<a name="fig9386125012810"></a>
  <img src="figures/添加图片.png" width="700" alt="添加图片">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - General页面下的BackgroungColor属性，MarginBottom属性和MarginRight属性暂无渲染效果。
> - Feature页面下的ForegroundStyle和BackgroundStyle的子属性BorderWidth、BorderColor、BorderRadius、LineWidth、LineHeight、LineColor、ImageOpacity属性暂无渲染效果。
> - Progress组件不支持配置回调事件。

#### List组件<a name="ZH-CN_TOPIC_0000002366099958"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中List组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399620549.png" width="36">样式图标（Feature），在展开的Feature栏中修改itemHeight属性设置所有子项的高度；修改itemWidth属性设置所有子项的宽度；修改imageWidth属性设置所有子项图片的宽度；修改imageHeight属性设置所有子项图片的高度；修改FontSize属性设置所有子项的字体大小；点击Item栏右侧的加号添加子项，修改子项中的Text属性和ImagePath属性添加文本和图片。
  
  **图 1**  添加子项<a name="fig28061923305"></a>
  <img src="figures/添加子项.png" width="700" alt="添加子项">
- 选中List组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399740433.png" width="36">样式图标（Feature），在展开的Feature栏中修改List组件的OffsetX属性设置所有子项的横向偏移，修改OffsetY属性设置所有子项的纵向偏移。
  
  **图 2**  修改子项的偏移<a name="fig1020210122313"></a>
  <img src="figures/修改子项的偏移.png" width="700" alt="修改子项的偏移">
- 选中List组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399620589.png" width="36">样式图标（Feature），在展开的Feature栏中修改List组件的Direction属性设置列表的排列方向。
  
  **图 3**  修改列表的排列方向<a name="fig56791215114412"></a>
  <img src="figures/修改列表的排列方向.png" width="700" alt="修改列表的排列方向">
- 选中画布内的List组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399740481.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  List组件支持的事件有：OnClick、OnLongPress、OnDrag、OnDragStart、OnDragEnd、OnPress、OnCancel、OnRelease、OnFocus和OnBlur。
  
  **图 4**  回调事件配置<a name="fig186332171075"></a>
  <img src="figures/回调事件配置-1.png" width="700" alt="回调事件配置-1">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - General页面下的MarginBottom属性和MarginRight属性无渲染效果。
> - Feature页面下的Isloop属性、Autoalign属性、Aligntime属性、Startindex属性均没有界面渲染效果，在添加子元素后可在模拟器查看效果。
> - 目前删除子项的最后一个元素会出现无法删除的问题，如需删除，可以在关闭界面的状态下在index.visual中将属性listItemText和listItemPath删除。

#### Label组件<a name="ZH-CN_TOPIC_0000002399619873"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中画布内的Label组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399620633.png" width="36">样式图标（Feature），在展开的Feature栏中修改Label组件的Text属性来编辑文本内容（新添加组件默认文本内容为“default”）。
  
  **图 1**  特有属性配置：Text文本内容<a name="fig455274919252"></a>
  <img src="figures/特有属性配置-Text文本内容.png" width="700" alt="特有属性配置-Text文本内容">
- 选中画布内的Label组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399740537.png" width="36">样式图标（Feature），在展开的Feature栏中设置Label组件的Font属性：
  
  - Color：字体颜色
  - Size：字体大小
  - LetterSpace：字母间距
  - LineSpace：行间距
  - LineHeight：行高（行高小于字体大小时将以字体大小为准，最终行高＝行高＋行间距）
  - TextDirection：文本方向
  
  **图 2**  通用属性配置：文本相关属性<a name="fig15826115915219"></a>
  <img src="figures/通用属性配置-文本相关属性.png" width="700" alt="通用属性配置-文本相关属性">
- 选中画布内的Label组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399620677.png" width="36">样式图标（Feature），在展开的Feature栏中修改Label组件的TextAlign属性改变文字的横向与纵向排布。
  
  **图 3**  改变文字排布<a name="fig195425918268"></a>
  <img src="figures/改变文字排布.png" width="700" alt="改变文字排布">
- 选中画布内的Label组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399740593.png" width="36">样式图标（Feature），在展开的Feature栏中修改Label组件的LineBreakMode属性。LineBreakMode对应6种换行模式，详细介绍如下：
  
  - Adapt：组件大小自适应文本，且不会对文本做自动换行。
    
    > ![](public_sys-resources/icon-note.gif) **说明：**
    > 组件宽高不受通用属性Width/Height值影响，因此不可通过拖拽组件或直接修改Width/Height值的方式改变组件宽高。
    
    **图 4**  LineBreakMode - Adapt<a name="fig1679161320461"></a>
    <img src="figures/LineBreakMode---Adapt.png" width="700" alt="LineBreakMode---Adapt">
  - Stretch：组件高度按照设置的Height值显示，宽度由文本中最长的行决定，文本不会自动换行。
    
    > ![](public_sys-resources/icon-note.gif) **说明：**
    > 组件宽度不受通用属性Width值影响，因此不可通过拖拽组件或直接修改Width值的方式改变组件宽度。
    
    **图 5**  LineBreakMode - Stretch<a name="fig8791181319462"></a>
    <img src="figures/LineBreakMode---Stretch.png" width="700" alt="LineBreakMode---Stretch">
  - Wrap：组件宽度按照设置的Width值显示，文本自动换行，组件高度由文本行数决定。
    
    > ![](public_sys-resources/icon-note.gif) **说明：**
    > 组件高度不受通用属性Height值影响，因此不可通过拖拽组件或直接修改Height值的方式改变组件高度。
    
    **图 6**  LineBreakMode - Wrap<a name="fig177911513124619"></a>
    <img src="figures/LineBreakMode---Wrap.png" width="700" alt="LineBreakMode---Wrap">
  - Ellipsis：组件大小按照设置的Height和Width值显示，文本自动换行，超出组件的文本将在末尾以省略号的形式显示。
    
    **图 7**  LineBreakMode - Ellipsis<a name="fig1179181314462"></a>
    <img src="figures/LineBreakMode---Ellipsis.png" width="700" alt="LineBreakMode---Ellipsis">
  - Clip：组件大小按照设置的Height和Width值显示，文本自动换行，超出组件的文本将在末尾自动隐藏。
    
    **图 8**  LineBreakMode - Clip<a name="fig14791191324610"></a>
    <img src="figures/LineBreakMode---Clip.png" width="700" alt="LineBreakMode---Clip">
  - Marquee：组件大小按照设置的Height和Width值显示，文本不自动换行，超出组件的文本将自动隐藏。
    
    > ![](public_sys-resources/icon-note.gif) **说明：**
    > 此模式下如果单行文本长度不超出组件时，文本按照设置的TextAlign进行对齐。
    > 文本长度超出组件时，文本会自动向左循环滚动播放。
    
    **图 9**  LineBreakMode - Marquee<a name="fig530611174710"></a>
    <img src="figures/LineBreakMode---Marquee.png" width="700" alt="LineBreakMode---Marquee">
- 选中画布内的Label组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941030.png" width="36">样式图标（Feature），在展开的Feature栏中修改Label组件的RollAnimation属性。
  
  RollAnimation属性只在LineBreakMode为Marquee且文本长度超出时生效。RollAnimation包含滚动速度（Speed）和滚动起始位置（Pos）。
  
  **图 10**  特有属性配置：RollAnimation<a name="fig19196141194615"></a>
  <img src="figures/特有属性配置-RollAnimation.png" width="700" alt="特有属性配置-RollAnimation">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - General页面下的MarginBottom属性和MarginRight属性没有任何界面渲染效果，可在模拟器上查看效果。
> - Label组件不支持配置回调事件。

#### ToggleButton组件<a name="ZH-CN_TOPIC_0000002365940062"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中画布内的ToggleButton组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366100938.png" width="36">样式图标（Feature），在展开的Feature栏中修改ToggleButton组件的ImageOpacity属性以修改图片透明度。
  
  **图 1**  修改图片透明度<a name="fig18996545134711"></a>
  <img src="figures/修改图片透明度.png" width="700" alt="修改图片透明度">
- 选中ToggleButton组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941090.png" width="36">样式图标（Feature），在展开的Feature栏中修改ToggleButton组件的State属性改变组件的状态，当属性值为true时为选中状态，反之为未选中状态。
  
  **图 2**  修改组件状态<a name="fig28061923305"></a>
  <img src="figures/修改组件状态.png" width="700" alt="修改组件状态">
- 选中ToggleButton组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366100998.png" width="36">样式图标（Feature），在展开的Feature栏中修改ToggleButton组件的UnSelectedImg属性添加未选中时的图片和SelectedImg属性添加选中时的图片，两张图片必须同时存在时才会产生渲染效果，通过将属性UnSelectedImg和SelectedImg恢复成空（手动删除），可以实现图片的删除。
  
  > ![](public_sys-resources/icon-note.gif) **说明：**
  > 图片不支持中文路径，且路径中不能包含空格。
  
  **图 3**  添加图片<a name="fig1150615503413"></a>
  <img src="figures/添加图片-2.png" width="700" alt="添加图片-2">
- 选中画布内的ToggleButton组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941142.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  ToggleButton组件支持的事件有：OnClick、OnLongPress、OnPress、OnCancel、OnRelease和OnChange。
  
  **图 4**  回调事件配置<a name="fig560310204504"></a>
  <img src="figures/回调事件配置-3.png" width="700" alt="回调事件配置-3">

> ![](public_sys-resources/icon-note.gif) **说明：**
> General页面下的MarginBottom属性、MarginRight属性暂无渲染效果。

#### CheckBox组件<a name="ZH-CN_TOPIC_0000002399739745"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中画布内的CheckBox组件，在已经添加图片的前提下，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101018.png" width="36">样式图标（Feature），在展开的Feature栏中修改CheckBox组件的ImageOpacity属性以修改图片透明度。
  
  **图 1**  修改图片透明度<a name="fig2853208115915"></a>
  <img src="figures/修改图片透明度-4.png" width="700" alt="修改图片透明度-4">
- 选中CheckBox组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941146.png" width="36">样式图标（Feature），在展开的Feature栏中修改CheckBox组件的State属性改变组件的状态，当属性值为Selected时为选中状态，反之为未选中状态。
  
  **图 2**  修改组件状态<a name="fig3771124145810"></a>
  <img src="figures/修改组件状态-5.png" width="700" alt="修改组件状态-5">
- 选中CheckBox组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101022.png" width="36">样式图标（Feature），在展开的Feature栏中修改CheckBox组件的UnSelectedImg属性添加未选中时的图片和SelectedImg属性添加选中时的图片，两张图片必须同时存在才会产生渲染效果，通过将属性UnSelectedImg和SelectedImg恢复成空（手动删除），可以实现图片的删除。
  
  > ![](public_sys-resources/icon-note.gif) **说明：**
  > 图片不支持中文路径，且路径中不能包含空格。
  
  **图 3**  添加图片<a name="fig1581184025416"></a>
  <img src="figures/添加图片-6.png" width="700" alt="添加图片-6">
- 选中画布内的CheckBox组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941150.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  CheckBox组件支持的事件有：OnClick、OnLongPress、OnPress、OnCancel、OnRelease和OnChange。
  
  **图 4**  回调事件配置<a name="fig15393131421110"></a>
  <img src="figures/回调事件配置-7.png" width="700" alt="回调事件配置-7">

> ![](public_sys-resources/icon-note.gif) **说明：**
> General页面下的MarginTop属性、MarginRight属性无渲染效果。

#### RadioButton组件<a name="ZH-CN_TOPIC_0000002366099962"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中画布内的RadioButton组件，在已经添加图片的前提下，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101026.png" width="36">样式图标（Feature），在展开的Feature栏中修改RadioButton组件的ImageOpacity属性以修改图片透明度。
  
  **图 1**  修改图片透明度<a name="fig2853208115915"></a>
  <img src="figures/修改图片透明度-8.png" width="700" alt="修改图片透明度-8">
- 选中RadioButton组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941158.png" width="36">样式图标（Feature），在展开的Feature栏中修改RadioButton组件的State改变组件的状态，当属性值为Selected时为选中状态，反之为未选中状态。
  
  **图 2**  修改组件状态<a name="fig82077222339"></a>
  <img src="figures/修改组件状态-9.png" width="700" alt="修改组件状态-9">
- 选中RadioButton组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101034.png" width="36">样式图标（Feature），在展开的Feature栏中修改RadioButton组件的SelectedImg属性添加选中时的图片和UnSelectedImg属性添加未选中时的图片，两张图片必须同时存在才会产生渲染效果，通过将SelectedImg和UnSelectedImg属性恢复成空（手动删除），可以实现图片的删除。
  
  **图 3**  添加图片效果展示<a name="fig02076225333"></a>
<img src="figures/添加图片效果展示.png" width="700" alt="添加图片效果展示">
  
  > ![](public_sys-resources/icon-note.gif) **说明：**
  > SelectedImg属性和UnSelectedImg属性不支持中文路径。
- 选中画布内的RadioButton组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941162.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  RadioButton组件支持的事件有：OnClick、OnLongPress、OnPress、OnCancel、OnRelease和OnChange。
  
  **图 4**  回调事件配置<a name="fig14875143917147"></a>
  <img src="figures/回调事件配置-10.png" width="700" alt="回调事件配置-10">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - General页面下的MarginBottom属性和MarginRight属性无渲染效果。
> - Feature页面下的Name属性没有任何界面渲染效果。当界面上有2个组件，并且Name属性一样时，模拟器才可以实现状态的切换。

#### LabelButton组件<a name="ZH-CN_TOPIC_0000002399619877"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中LabelButton组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101038.png" width="36">样式图标（Feature），在展开的Feature栏中修改LabelButton组件的ImageOpacity属性可以修改图片的透明度。
  
  **图 1**  修改图片透明度<a name="fig59341711182514"></a>
  <img src="figures/修改图片透明度-11.png" width="700" alt="修改图片透明度-11">
- 选中LabelButton组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941166.png" width="36">样式图标（Feature），在展开的Feature栏中修改LabelButton组件的ImageX属性和ImageY属性可以对图片的位置进行调整。
  
  **图 2**  修改ImageX属性和ImageY属性的效果<a name="fig1747911404612"></a>
  <img src="figures/修改ImageX属性和ImageY属性的效果.png" width="700" alt="修改ImageX属性和ImageY属性的效果">
- 选中LabelButton组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101042.png" width="36">样式图标（Feature），在展开的Feature栏中修改LabelButton组件的Text属性添加文本。
  
  **图 3**  添加文本<a name="fig115151856956"></a>
  <img src="figures/添加文本.png" width="700" alt="添加文本">
- 选中LabelButton组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941170.png" width="36">样式图标（Feature），在展开的Feature栏中设置LabelButton组件的Font属性：
  
  - Color：字体颜色
  - FontSize：字体大小
  - TextDirection：文本方向
  
  **图 4**  文本相关属性<a name="fig11349122424720"></a>
  <img src="figures/文本相关属性.png" width="700" alt="文本相关属性">
- 选中LabelButton组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101046.png" width="36">样式图标（Feature），在展开的Feature栏中修改LabelButton组件的Text Xoffset属性和Text Yoffset属性对文字的位置进行调整。
  
  **图 5**  对文字位置进行调整<a name="fig335933815438"></a>
  <img src="figures/对文字位置进行调整.png" width="700" alt="对文字位置进行调整">
- 选中LabelButton组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941186.png" width="36">样式图标（Feature），在展开的Feature栏中修改LabelButton组件的TextAlign属性对文字的水平对齐方式进行调整。
  
  **图 6**  修改文字的对齐方式<a name="fig0812161817195"></a>
  <img src="figures/修改文字的对齐方式.png" width="700" alt="修改文字的对齐方式">
- 选中LabelButton组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101066.png" width="36">样式图标（Feature），在展开的Feature栏中修改LabelButton组件的DefaultImgSrc属性可以添加图片，通过将属性DefaultImgSrc恢复成空（手动删除），可以实现图片的删除。
  
  **图 7**  添加图片<a name="fig245413171610"></a>
<img src="figures/添加图片-12.png" width="700" alt="添加图片-12">
  
  > ![](public_sys-resources/icon-note.gif) **说明：**
  > DefaultImgSrc属性不支持中文路径。
- 选中画布内的LabelButton组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941198.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  LabelButton组件支持的事件有：OnClick、OnLongPress、OnPress、OnCancel和OnRelease。
  
  **图 8**  回调事件配置<a name="fig1068120358476"></a>
  <img src="figures/回调事件配置-13.png" width="700" alt="回调事件配置-13">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - General页面下的MarginBottom属性和MarginRight属性没有任何的渲染效果。
> - Feature页面下的TriggeredImgSrc属性、TriggeredBackgroundColor属性、TriggeredborderColor属性没有任何界面渲染效果，可在模拟器上查看效果。

#### ScrollView组件<a name="ZH-CN_TOPIC_0000002365940066"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中组件栏中的其他组件，可以将其拖入ScrollView组件中，并且对其他组件进行样式调整。子组件的位置是相对于父容器的左上角，当组件已经存在于界面上时，点击组件中心位置可以将组件拖入容器中。
  
  **图 1**  将其他组件拖入ScrollView组件中<a name="fig192648563920"></a>
  <img src="figures/将其他组件拖入ScrollView组件中.png" width="700" alt="将其他组件拖入ScrollView组件中">
- 选中ScrollView组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621001.png" width="36">样式图标（Feature），在展开的Feature栏中修改ScrollView组件的Direction属性可以控制组件哪个方向可以滚动。
  
  **图 2**  修改组件状态<a name="fig1335145711333"></a>
  <img src="figures/修改组件状态-14.png" width="700" alt="修改组件状态-14">
- 选中画布内的ScrollView组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399740877.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  ScrollView组件支持的事件有：OnDrag、OnDragStart、OnDragEnd、OnFocus、OnBlur和OnScroll。
  
  **图 3**  回调事件配置<a name="fig159041157202714"></a>
  <img src="figures/回调事件配置-15.png" width="700" alt="回调事件配置-15">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - 当其他组件不是直接从左侧UI Control栏拖入时可能会发生其他组件出现在ScrollView组件外面且无法看到的情况，并且当多次从左侧UI Control栏拖入组件时，它们会重复出现在画布(100,100)坐标位置。
> - General页面下的MarginBottom属性和MarginRight属性没有任何的渲染效果。
> - Feature页面下的除Direction属性外其他属性均没有界面渲染效果，可在模拟器上查看效果。
> - ScrollView的滚动条一直处于隐藏状态，但仍保持正常的拖拽方式。

#### CircleProgress组件<a name="ZH-CN_TOPIC_0000002399739749"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中画布内的CircleProgress组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621005.png" width="36">样式图标（Feature），在展开的Feature栏中修改CircleProgress组件的ForegroundStyle和BackgroundStyle窗口中LineWidth属性改变进度条宽度；修改LineColor属性改变进度条颜色。
  
  **图 1**  修改进度条宽度<a name="fig815243422317"></a>
  <img src="figures/修改进度条宽度.png" width="700" alt="修改进度条宽度">
  
  **图 2**  修改进度条颜色<a name="fig969345712413"></a>
  <img src="figures/修改进度条颜色.png" width="700" alt="修改进度条颜色">
- 选中画布内的CircleProgress组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101078.png" width="36">样式图标（Feature），在展开的Feature栏中修改CircleProgress组件的RangeMax和RangeMin用来限制进度条的value，修改value来改变进度值。
  
  **图 3**  修改进度值<a name="fig762919369274"></a>
  <img src="figures/修改进度值.png" width="700" alt="修改进度值">
- 选中画布内的CircleProgress组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941210.png" width="36">样式图标（Feature），在展开的Feature栏中修改CircleProgress组件的Radius和CenterX属性和CenterY用来限定圆环。
  
  **图 4**  修改圆环大小<a name="fig1581711211515"></a>
  <img src="figures/修改圆环大小.png" width="700" alt="修改圆环大小">
- 选中画布内的CircleProgress组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101082.png" width="36">样式图标（Feature），在展开的Feature栏中修改CircleProgress组件的StartAngel和EndAngel属性用来限制绘制角度。
  
  **图 5**  修改角度<a name="fig1267654717373"></a>
  <img src="figures/修改角度.png" width="700" alt="修改角度">
- 选中画布内的CircleProgress组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941214.png" width="36">样式图标（Feature），在展开的Feature栏中修改CircleProgress组件的ForeImage或者BackImage的子属性来添加前景或者背景图片并且修改图片位置。
  
  > ![](public_sys-resources/icon-note.gif) **说明：**
  > 图片不支持中文路径，且路径中不能包含空格。
  
  **图 6**  添加图片<a name="fig836417515157"></a>
  <img src="figures/添加图片-16.png" width="700" alt="添加图片-16">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - General页面下的MarginBottom属性、MarginRight属性无渲染效果。
> - Feature页面下的ForegroundStyle和BackgroundStyle的子属性BackgroundColor、BorderWidth、BorderColor、BorderRadius、LineHeight、ImageOpacity无渲染效果。
> - CircleProgress组件不支持回调事件配置。

#### ArcLabel组件<a name="ZH-CN_TOPIC_0000002366099966"></a>

本组件的共有属性不包含Position属性与Size属性，其他共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中画布内的ArcLabel组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101086.png" width="36">样式图标（Feature），在展开的Feature栏中修改ArcLabel组件的LetterSpace属性来修改文本之间的间隔。
  
  **图 1**  修改文字颜色<a name="fig19959125910167"></a>
  <img src="figures/修改文字颜色.png" width="700" alt="修改文字颜色">
- 选中画布内的ArcLabel组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941218.png" width="36">样式图标（Feature），在展开的Feature栏中修改ArcLabel组件的TextAlign属性改变文本在圆环上的对齐方式。
  
  **图 2**  文本对齐方式修改<a name="fig6210165422115"></a>
  <img src="figures/文本对齐方式修改.png" width="700" alt="文本对齐方式修改">
- 选中画布内的ArcLabel组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101094.png" width="36">样式图标（Feature），在展开的Feature栏中修改ArcLabel组件的Text属性来改变显示的文本。
  
  **图 3**  显示文本的修改<a name="fig10896149152219"></a>
  <img src="figures/显示文本的修改.png" width="700" alt="显示文本的修改">
- 选中画布内的ArcLabel组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941222.png" width="36">样式图标（Feature），在展开的Feature栏中修改ArcLabel组件的TextColor属性来修改文字的颜色。
  
  **图 4**  修改文字颜色<a name="fig181046357190"></a>
  <img src="figures/修改文字颜色-17.png" width="700" alt="修改文字颜色-17">
- 选中画布内的ArcLabel组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101098.png" width="36">样式图标（Feature），在展开的Feature栏中修改ArcLabel组件的TextSize改变文本中文字的大小。
  
  **图 5**  文本中文字大小修改<a name="fig13754615153218"></a>
  <img src="figures/文本中文字大小修改.png" width="700" alt="文本中文字大小修改">
- 选中画布内的ArcLabel组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941230.png" width="36">样式图标（Feature），在展开的Feature栏中修改ArcLabel组件的CenterX、CenterY属性改变文字所在圆的圆心位置。
  
  **图 6**  文本所在圆圆心修改<a name="fig16333532113418"></a>
  <img src="figures/文本所在圆圆心修改.png" width="700" alt="文本所在圆圆心修改">
- 选中画布内的ArcLabel组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101102.png" width="36">样式图标（Feature），在展开的Feature栏中修改ArcLabel组件的TextRadius改变文字所在圆的半径。
  
  **图 7**  文本所在圆半径修改<a name="fig13296111602013"></a>
  <img src="figures/文本所在圆半径修改.png" width="700" alt="文本所在圆半径修改">
- 选中画布内的ArcLabel组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941234.png" width="36">样式图标（Feature），在展开的Feature栏中修改ArcLabel组件的StartAngle、EndAngle属性改变文本在圆中出现的范围。
  
  **图 8**  文本出现在圆上的范围修改<a name="fig1867192510273"></a>
  <img src="figures/文本出现在圆上的范围修改.png" width="700" alt="文本出现在圆上的范围修改">
- 选中画布内的ArcLabel组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101106.png" width="36">样式图标（Feature），在展开的Feature栏中修改ArcLabel组件的TextOrientation改变文本的朝向。
  
  **图 9**  文本朝向修改<a name="fig035742622712"></a>
  <img src="figures/文本朝向修改.png" width="700" alt="文本朝向修改">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - General页面下的MarginBottom属性和MarginRight属性无任何渲染效果。
> - ArcLabel组件不支持回调事件配置。

#### SwipeView组件<a name="ZH-CN_TOPIC_0000002399619881"></a>

本组件的共有属性不包含Border属性、Margin属性与Padding属性，其他共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中SwipeView组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941238.png" width="36">样式图标（Feature），在展开的Feature栏中修改SwipeView组件的Direction属性改变组件的滚动方向。子组件的位置是相对于父容器的左上角，当组件已经存在于界面上时，点击组件中心位置可以将组件拖入容器中。
  
  **图 1**  修改滚动条位置<a name="fig85221029131212"></a>
  <img src="figures/修改滚动条位置.png" width="700" alt="修改滚动条位置">
  
  <img src="figures/1.png" width="700">
  
  > ![](public_sys-resources/icon-note.gif) **说明：**
  > 滚动条已被隐藏，当容器内的子组件超出范围时可拖动，HORIZONTAL是水平滚动，VERTICAL是垂直滚动。
- 选中画布内的SwipeView组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621041.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  SwipeView组件支持的事件有：OnDrag、OnDragStart、OnDragEnd、OnSwipe。
  
  **图 2**  回调事件配置<a name="fig968711173355"></a>
  <img src="figures/回调事件配置-18.png" width="700" alt="回调事件配置-18">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - Feature页面下的Isloop属性、AlignMode属性、TickTime属性暂无渲染效果。
> - 在水平模式下上方工具栏的顶部对齐<img src="figures/zh-cn_image_0000002399740917.png" width="28">、底部对齐<img src="figures/zh-cn_image_0000002366101114.png" width="33">、上下居中对齐<img src="figures/zh-cn_image_0000002399621045.png" width="29">不可用。
> - 在垂直模式下上方工具栏的左对齐<img src="figures/zh-cn_image_0000002365941246.png" width="35">、右对齐<img src="figures/zh-cn_image_0000002399740925.png" width="24">、左右居中对齐<img src="figures/zh-cn_image_0000002366101122.png" width="27">不可用。
> - SwipeView的滚动条一直处于隐藏状态，但仍保持正常的拖拽方式。

#### TextureMapper组件<a name="ZH-CN_TOPIC_0000002365940070"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中TextureMapper组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621069.png" width="36">样式图标（Feature），在展开的Feature栏通过修改TextureMapper组件的ImagePath属性添加图片，通过将ImagePath属性恢复成空（手动删除），可以实现图片的删除。
  
  **图 1**  添加图片<a name="fig15600147102714"></a>
<img src="figures/添加图片-19.png" width="700" alt="添加图片-19">
  
  > ![](public_sys-resources/icon-note.gif) **说明：**
  > ImagePath属性不支持中文路径。
- 选中TextureMapper组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101150.png" width="36">样式图标（Feature），在展开的Feature栏通过修改TextureMapper组件的ImageOpacity属性修改图片的透明度。
  
  **图 2**  修改图片透明度<a name="fig1859224515129"></a>
  <img src="figures/修改图片透明度-20.png" width="700" alt="修改图片透明度-20">
- 选中画布内的TextureMapper组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399740985.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  TextureMapper组件支持的事件有：OnMapper。
  
  **图 3**  回调事件配置<a name="fig833815411471"></a>
  <img src="figures/回调事件配置-21.png" width="700" alt="回调事件配置-21">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - General页面下的MarginBottom属性和MarginRight属性均无渲染效果。
> - Feature页面下除ImagePath属性外其他属性均没有界面渲染效果，可在模拟器上查看效果。请在展示动画效果前先添加图片并且Feature页面的IsStart属性为true。
> - 组件的大小在添加图片后固定为图片的大小，且修改组件宽高不生效。

#### Slider组件<a name="ZH-CN_TOPIC_0000002399739753"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中Slider组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621121.png" width="36">样式图标（Feature），在展开的Feature栏中修改Slider组件的SliderValue属性改变滑块的位置。
  
  **图 1**  修改滑块位置<a name="fig520210134916"></a>
  <img src="figures/修改滑块位置.png" width="700" alt="修改滑块位置">
- 选中Slider组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741001.png" width="36">样式图标（Feature），在展开的Feature栏中修改Slider组件的SliderWidth属性改变滑槽的宽度。
  
  **图 2**  修改滑槽宽度<a name="fig19400118164911"></a>
  <img src="figures/修改滑槽宽度.png" width="700" alt="修改滑槽宽度">
- 选中Slider组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621137.png" width="36">样式图标（Feature），在展开的Feature栏中修改Slider组件的SliderHeight属性改变滑槽的高度。SliderHeight不能超出边框的高度。
  
  **图 3**  修改滑槽高度<a name="fig54931822183517"></a>
  <img src="figures/修改滑槽高度.png" width="700" alt="修改滑槽高度">
- 选中Slider组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741017.png" width="36">样式图标（Feature），在展开的Feature栏中修改Slider组件的Direction属性改变滑槽的方向。
  
  **图 4**  修改滑动方向<a name="fig1164616545365"></a>
  <img src="figures/修改滑方向.png" width="700" alt="修改滑方向">
- 选中Slider组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621145.png" width="36">样式图标（Feature），在展开的Feature栏中修改Slider组件的KnobWidth属性改变滑块的宽度。
  
  **图 5**  修改滑块大小<a name="fig2041221915013"></a>
  <img src="figures/修改滑块大小.png" width="700" alt="修改滑块大小">
- 选中Slider组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741033.png" width="36">样式图标（Feature），在展开的Feature栏中修改Slider组件的BackGroundImage属性改变滑槽的背景图片；修改Slider组件的BackGroundColor属性改变滑槽的背景色。
  
  > ![](public_sys-resources/icon-note.gif) **说明：**
  > 图片不支持中文路径，且路径中不能包含空格。
  
  **图 6**  添加图片<a name="fig20840845145018"></a>
  <img src="figures/添加图片-22.png" width="700" alt="添加图片-22">
  
  **图 7**  设置颜色<a name="fig1623652355120"></a>
  <img src="figures/设置颜色.png" width="700" alt="设置颜色">
- 选中Slider组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941362.png" width="36">样式图标（Feature），在展开的Feature栏的ForeStyle窗口中修改Slider组件的ForeImg属性改变滑槽的前景图片；修改ForeGroundColor改变滑槽的前景颜色。
  
  > ![](public_sys-resources/icon-note.gif) **说明：**
  > 图片不支持中文路径，且路径中不能包含空格。
  
  **图 8**  添加图片<a name="fig71361938195220"></a>
  <img src="figures/添加图片-23.png" width="700" alt="添加图片-23">
  
  **图 9**  修改背景颜色<a name="fig53181218398"></a>
  <img src="figures/修改背景颜色-24.png" width="700" alt="修改背景颜色-24">
- 选中Slider组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621173.png" width="36">样式图标（Feature），在展开的Feature栏KnobStyle窗口中修改Slider组件的KnobImage属性添加图片；修改KnobBackGroundColor属性改变滑块的颜色；修改KnobRadius属性改变滑块的边框弧度。
  
  **图 10**  添加图片<a name="fig4198162695412"></a>
<img src="figures/添加图片-25.png" width="700" alt="添加图片-25">
  
  > ![](public_sys-resources/icon-note.gif) **说明：**
  > 图片不支持中文路径，且路径中不能包含空格。
  
  **图 11**  修改背景颜色<a name="fig561313503547"></a>
  <img src="figures/修改背景颜色-26.png" width="700" alt="修改背景颜色-26">
  
  **图 12**  修改弧度<a name="fig620717512485"></a>
  <img src="figures/修改弧度.png" width="700" alt="修改弧度">
- 选中画布内的Slider组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941390.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  Slider组件支持的事件有：OnClick、OnLongPress、OnDrag、OnDragStart、OnDragEnd、OnPress、OnCancel、OnRelease、OnFocus、OnBlur。
  
  **图 13**  回调事件配置<a name="fig325561020"></a>
  <img src="figures/回调事件配置-27.png" width="700" alt="回调事件配置-27">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - General页面下的GaussianBlur属性、MarginBottom属性和MarginRight属性均无渲染效果。
> - 组件的大小在添加图片后固定为图片的大小，且修改组件宽高不生效。

#### Picker组件<a name="ZH-CN_TOPIC_0000002366099970"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中Picker组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101302.png" width="36">样式图标（Feature），在展开的Feature栏中修改Picker组件的Normal属性改变组件未选中字体颜色。
  
  **图 1**  修改未选中字体颜色<a name="fig19778846182613"></a>
  <img src="figures/修改未选中字体颜色.png" width="700" alt="修改未选中字体颜色">
- 选中Picker组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941414.png" width="36">样式图标（Feature），在展开的Feature栏中修改Picker组件的HighLight属性改变组件选中字体颜色。
  
  **图 2**  修改选中字体颜色<a name="fig13923214192715"></a>
  <img src="figures/修改选中字体颜色.png" width="700" alt="修改选中字体颜色">
- 选中Picker组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101314.png" width="36">样式图标（Feature），在展开的Feature栏中修改Picker组件的ItemHeight属性改变组件条目高度。
  
  **图 3**  修改条目高度<a name="fig1048834002714"></a>
  <img src="figures/修改条目高度.png" width="700" alt="修改条目高度">
- 选中Picker组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941430.png" width="36">样式图标（Feature），在展开的Feature栏中修改Picker组件的LoopState属性使组件内容循环。
  
  **图 4**  设置循环<a name="fig5493192817"></a>
  <img src="figures/设置循环.png" width="700" alt="设置循环">
- 选中Picker组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101326.png" width="36">样式图标（Feature），在展开的Feature栏中修改Picker组件的Values属性改变组件显示文本，数据格式为："x1", "x2", "x3"...（必须带""）
  
  **图 5**  修改文本<a name="fig1959462262820"></a>
  <img src="figures/修改文本.png" width="700" alt="修改文本">
- 选中Picker组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941446.png" width="36">样式图标（Feature），在展开的Feature栏中同步Count属性值（必须与输入数组子元素个数相等）。
  
  **图 6**  添加元素个数值<a name="fig9995037194619"></a>
  <img src="figures/添加元素个数值.png" width="700" alt="添加元素个数值">
- 选中Picker组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101334.png" width="36">样式图标（Feature），在展开的Feature栏中修改Picker组件的TextDirection属性值改变文本的书写方向。LTR表示文字方向为从左至右，RTL为从右至左。如图7所示，第一个值为“12345”，通过设置TextDirection属性值为RTL后，画布中显示的文字为“54321”。
  
  **图 7**  修改文本方向<a name="fig0726183665610"></a>
  <img src="figures/修改文本方向.png" width="700" alt="修改文本方向">
- 选中画布内的Picker组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941450.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  Picker组件支持的事件有：OnFocus和OnBlur。
  
  **图 8**  回调事件配置<a name="fig325561020"></a>
  <img src="figures/回调事件配置-28.png" width="700" alt="回调事件配置-28">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - General页面下的MarginBottom属性和MarginRight属性均无渲染效果。
> - Count属性值与数组元素个数保持一致。

#### TimePicker组件<a name="ZH-CN_TOPIC_0000002399619885"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中TimePicker组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101338.png" width="36">样式图标（Feature），在展开的Feature栏中修改TimePicker组件的ItemHeight属性改变组件条目高度。
  
  **图 1**  修改条目高度<a name="fig13492156123017"></a>
  <img src="figures/修改条目高度-29.png" width="700" alt="修改条目高度-29">
- 选中TimePicker组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941454.png" width="36">样式图标（Feature），在展开的Feature栏中修改TimePicker组件的Values属性改变组件显示文本。
  
  **图 2**  修改文本<a name="fig1536141613118"></a>
  <img src="figures/修改文本-30.png" width="700" alt="修改文本-30">
- 选中TimePicker组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101342.png" width="36">样式图标（Feature），在展开的Feature栏中修改TimePicker组件的HourLoop属性改变组件小时窗口是否循环。
  
  **图 3**  设置循环<a name="fig172573812312"></a>
  <img src="figures/设置循环-31.png" width="700" alt="设置循环-31">
- 选中TimePicker组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941458.png" width="36">样式图标（Feature），在展开的Feature栏中修改TimePicker组件的MinLoop属性改变组件分钟窗口是否循环。
  
  **图 4**  设置循环<a name="fig1741193616326"></a>
  <img src="figures/设置循环-32.png" width="700" alt="设置循环-32">
- 选中TimePicker组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101350.png" width="36">样式图标（Feature），在展开的Feature栏中修改TimePicker组件的SecLoop属性改变组件秒窗口是否循环。（在SecShows属性为true情况下）
  
  **图 5**  设置循环<a name="fig1031192215331"></a>
  <img src="figures/设置循环-33.png" width="700" alt="设置循环-33">
- 选中TimePicker组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941462.png" width="36">样式图标（Feature），在展开的Feature栏中修改TimePicker组件的NormalColor属性改变组件未选中字体颜色。
  
  **图 6**  修改字体颜色<a name="fig689444916334"></a>
  <img src="figures/修改字体颜色.png" width="700" alt="修改字体颜色">
- 选中TimePicker组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101354.png" width="36">样式图标（Feature），在展开的Feature栏中修改TimePicker组件的HighLightColor属性改变组件选中字体颜色。
  
  **图 7**  修改字体颜色<a name="fig1820476173413"></a>
  <img src="figures/修改字体颜色-34.png" width="700" alt="修改字体颜色-34">
- 选中TimePicker组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941466.png" width="36">样式图标（Feature），在展开的Feature栏中修改TimePicker组件的SecShow属性设置秒窗口是否显示。
  
  **图 8**  设置秒窗口显示<a name="fig732614223349"></a>
  <img src="figures/设置秒窗口显示.png" width="700" alt="设置秒窗口显示">
- 选中TimePicker组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101358.png" width="36">样式图标（Feature），在展开的Feature栏中修改TimePicker组件的BackgroundFontsize属性改变组件未选中字体大小。
  
  **图 9**  设置字体大小<a name="fig7257450143415"></a>
  <img src="figures/设置字体大小.png" width="700" alt="设置字体大小">
- 选中TimePicker组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941470.png" width="36">样式图标（Feature），在展开的Feature栏中修改TimePicker组件的HighlightFontsize属性改变组件选中字体大小。
  
  **图 10**  设置字体大小<a name="fig1568281294514"></a>
  <img src="figures/设置字体大小-35.png" width="700" alt="设置字体大小-35">
- 选中画布内的TimePicker组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101362.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  TimePicker组件支持的事件有：OnFocus和OnBlur。
  
  **图 11**  回调事件配置<a name="fig325561020"></a>
  <img src="figures/回调事件配置-36.png" width="700" alt="回调事件配置-36">

> ![](public_sys-resources/icon-note.gif) **说明：**
> General页面下的MarginBottom属性和MarginRight属性均无渲染效果。

#### Chart组件<a name="ZH-CN_TOPIC_0000002365940078"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中Chart组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941478.png" width="36">样式图标（Feature），在展开的Feature栏中修改XAxis栏中的属性改变横坐标的样式。
  
  - MarkNum属性：改变坐标轴上点的数量
  - RangMin和RangMax：改变坐标轴的取值范围
  - Color：改变坐标轴的颜色
  - Visible：控制坐标轴的可见与否
  
  **图 1**  修改横坐标属性<a name="fig8661439164210"></a>
  <img src="figures/修改横坐标属性.png" width="700" alt="修改横坐标属性">
- 选中Chart组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621289.png" width="36">样式图标（Feature），在展开的Feature栏中修改YAxis栏中的属性改变纵坐标样式，具体修改与横坐标相同，请参考图2。
  
  **图 2**  修改纵坐标样式<a name="fig16661839194210"></a>
  <img src="figures/修改纵坐标样式.png" width="700" alt="修改纵坐标样式">
- 选中Chart组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741189.png" width="36">样式图标（Feature），在展开的Feature栏中点击DataSerial属性右侧的加号添加数据，可以添加多组数据。
  
  **图 3**  添加数据<a name="fig10661113994214"></a>
  <img src="figures/添加数据.png" width="700" alt="添加数据">
- 选中Chart组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621297.png" width="36">样式图标（Feature），在展开的Feature栏中添加数据然后改变数据的样式，其中Data属性是要填入的数据，DataCount属性是数据的个数，FillColor是填充渐变色的颜色，LineColor是线的颜色，EnableGradient属性控制是否填充渐变色。
  
  **图 4**  添加数据效果展示<a name="fig96611939204217"></a>
  <img src="figures/添加数据效果展示.png" width="700" alt="添加数据效果展示">
- 选中Chart组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741197.png" width="36">样式图标（Feature），在展开的Feature栏中ColorGradient窗口中MinOpa和MaxOpa属性改变渐变色的范围，MaxOpa控制上面的渐变色透明度，MinOpa控制下面的渐变色透明度。
  
  **图 5**  修改透明度渐变色<a name="fig46611739184215"></a>
  <img src="figures/修改透明度渐变色.png" width="700" alt="修改透明度渐变色">
- 选中Chart组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621305.png" width="36">样式图标（Feature），在展开的Feature栏中修改Chart组件的Type属性改变图的类型。
  
  **图 6**  修改图的类型<a name="fig19661139164218"></a>
  <img src="figures/修改图的类型.png" width="700" alt="修改图的类型">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - General页面下的MarginBottom属性和MarginRight属性均无渲染效果。
> - 添加数据时的格式为： \{x1,y1\},\{x2,y2\};
>   数据中不能有空格，横坐标应从0开始依次递增，DataCount属性中的数字不能比数据个数小。当DataCount属性中的数字比数据个数大时，会自动补连\{0,0\}，柱状图时横坐标的MarkNum也不能比数据个数小。
> - ColorGradient属性只有在折线图时有用。
> - 目前Chart首次修改DataSerial属性时，属性界面可能无法发生变化，通过再次修改DataSerial属性中的其他属性可使界面发生变化。
> - Chart组件不支持回调事件的配置。

#### EditText组件<a name="ZH-CN_TOPIC_0000002399739757"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中EditText组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741209.png" width="36">样式图标（Feature），在展开的Feature栏中修改EditText组件的Placeholder属性以修改背景提示文字。
  
  **图 1**  修改背景提示文字<a name="fig688611508216"></a>
  <img src="figures/修改背景提示文字.png" width="700" alt="修改背景提示文字">
- 选中EditText组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621313.png" width="36">样式图标（Feature），在展开的Feature栏中修改EditText组件的Text属性以修改前景文字的内容，并且当前景文字存在时无法显示背景文字。
  
  **图 2**  修改前景文字<a name="fig18400183213561"></a>
  <img src="figures/修改前景文字.png" width="700" alt="修改前景文字">
- 选中EditText组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741217.png" width="36">样式图标（Feature），在展开的Feature栏中修改EditText组件的EditTextColor属性以修改前景文字的颜色。
  
  **图 3**  修改前景文字颜色<a name="fig581421214573"></a>
  <img src="figures/修改前景文字颜色.png" width="700" alt="修改前景文字颜色">
- 选中EditText组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621333.png" width="36">样式图标（Feature），在展开的Feature栏中修改EditText组件的MaxLength属性来限制前景文字输入长度。
  
  **图 4**  限制前景文字长度<a name="fig8651102555710"></a>
  <img src="figures/限制前景文字长度.png" width="700" alt="限制前景文字长度">
- 选中EditText组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741233.png" width="36">样式图标（Feature），在展开的Feature栏中修改EditText组件的InputType属性来改变输入模式。
  
  - text模式：输入的文本正常显示
  - password模式：输入的文本以密码模式显示（被“·”代替）
  
  **图 5**  改变输入模式<a name="fig13869958175715"></a>
  <img src="figures/改变输入模式.png" width="700" alt="改变输入模式">
- 选中EditText组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621357.png" width="36">样式图标（Feature），在展开的Feature栏中修改EditText组件的PlaceholderColor属性以修改背景提示文字的颜色。
  
  **图 6**  修改背景提示文字颜色<a name="fig1530813191564"></a>
  <img src="figures/修改背景提示文字颜色.png" width="700" alt="修改背景提示文字颜色">
- 选中EditText组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741261.png" width="36">样式图标（Feature），在展开的Feature栏中修改EditText组件的CursorColor属性来修改光标的颜色。
  
  **图 7**  光标颜色<a name="fig14857161318582"></a>
  <img src="figures/光标颜色.png" width="700" alt="光标颜色">
- 选中画布内的EditText组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621385.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  EditText组件支持的事件有：OnClick、OnLongPress、OnPress、OnCancel、OnRelease。
  
  **图 8**  回调事件配置<a name="fig325561020"></a>
  <img src="figures/回调事件配置-37.png" width="700" alt="回调事件配置-37">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - General页面下的MarginBottom属性和MarginRight属性均无渲染效果。
> - 文字修改只能通过右侧的特有属性进行，在界面上通过键盘删除文字时会出现将组件删除的问题，修改前景文本请通过右侧的Text文本进行。

#### DigitalClock组件<a name="ZH-CN_TOPIC_0000002366099978"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中DigitalClock组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741285.png" width="36">样式图标（Feature），在展开的Feature栏中修改DigitalClock组件的Color属性改变字体颜色。
  
  **图 1**  修改字体颜色<a name="fig28061923305"></a>
  <img src="figures/修改字体颜色-38.png" width="700" alt="修改字体颜色-38">
- 选中DigitalClock组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621393.png" width="36">样式图标（Feature），在展开的Feature栏中修改DigitalClock组件的DisplayMode属性改变组件显示状态。
  
  **图 2**  组件显示状态<a name="fig13710344441"></a>
  <img src="figures/组件显示状态.png" width="700" alt="组件显示状态">
- 选中DigitalClock组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741297.png" width="36">样式图标（Feature），在展开的Feature栏中修改DigitalClock组件的DisplayLeadigZero属性设置是否在小时前面加0。
  
  **图 3**  是否添加0<a name="fig9371123494418"></a>
  <img src="figures/是否添加0.png" width="700" alt="是否添加0">
- 选中DigitalClock组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621397.png" width="36">样式图标（Feature），在展开的Feature栏中修改DigitalClock组件的Hour属性设置小时。
  
  **图 4**  设置小时<a name="fig67661353475"></a>
  <img src="figures/设置小时.png" width="700" alt="设置小时">
- 选中DigitalClock组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741301.png" width="36">样式图标（Feature），在展开的Feature栏中修改DigitalClock组件的Minute属性设置分钟。
  
  **图 5**  设置分钟<a name="fig711219924715"></a>
  <img src="figures/设置分钟.png" width="700" alt="设置分钟">
- 选中DigitalClock组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621401.png" width="36">样式图标（Feature），在展开的Feature栏中修改DigitalClock组件的Second属性设置秒。
  
  **图 6**  设置秒<a name="fig177318106475"></a>
  <img src="figures/设置秒.png" width="700" alt="设置秒">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - General页面下的MarginBottom属性和MarginRight属性均无渲染效果。
> - DigitalClock不支持回调事件的配置。

#### ImageAnimator组件<a name="ZH-CN_TOPIC_0000002399619889"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中ImageAnimator组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741305.png" width="36">样式图标（Feature），在展开的Feature栏中添加图片路径。
  
  **图 1**  图片路径<a name="fig28061923305"></a>
  <img src="figures/图片路径.png" width="700" alt="图片路径">
- 选中ImageAnimator组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621409.png" width="36">样式图标（Feature），在展开的Feature栏中修改ImageAnimator组件的ImageNum属性保证显示图片的个数与输入图片路径的个数相等。
  
  **图 2**  图片的个数<a name="fig13710344441"></a>
  <img src="figures/图片的个数.png" width="700" alt="图片的个数">
- 选中ImageAnimator组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741313.png" width="36">样式图标（Feature），在展开的Feature栏中修改ImageAnimator组件的Times属性控制循环次数。该属性只有在Repeat为false情况下生效。
  
  **图 3**  循环次数<a name="fig9371123494418"></a>
  <img src="figures/循环次数.png" width="700" alt="循环次数">
- 选中ImageAnimator组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621413.png" width="36">样式图标（Feature），在展开的Feature栏中修改ImageAnimator组件的Fixed属性用来适应图片大小。
  
  **图 4**  是否适应图片大小<a name="fig67661353475"></a>
  <img src="figures/是否适应图片大小.png" width="700" alt="是否适应图片大小">
- 选中ImageAnimator组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741317.png" width="36">样式图标（Feature），在展开的Feature栏中修改ImageAnimator组件的Repeat属性控制图片是否循环。
  
  **图 5**  设置图片是否循环<a name="fig711219924715"></a>
  <img src="figures/设置图片是否循环.png" width="700" alt="设置图片是否循环">
- 选中ImageAnimator组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621417.png" width="36">样式图标（Feature），在展开的Feature栏中修改ImageAnimator组件的Reverse属性设置图片循环方向。
  
  **图 6**  设置图片循环方向<a name="fig137012812132"></a>
  <img src="figures/设置图片循环方向.png" width="700" alt="设置图片循环方向">
- 选中ImageAnimator组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741321.png" width="36">样式图标（Feature），在展开的Feature栏中修改ImageAnimator组件的FillMode属性设置动画最后一帧显示的图片，true为当前循环模式下动画的第一张图片，false为最后一张。其中，Reverse属性值影响循环动画的图片先后顺序。
  
  **图 7**  设置最后一帧<a name="fig142541859122517"></a>
  <img src="figures/设置最后一帧.png" width="700" alt="设置最后一帧">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - General页面下的MarginBottom属性和MarginRight属性均无渲染效果。
> - Feature页面下的TimeOfUpdate属性和TimeOfPause属性无渲染效果。
> - ImageAnimator组件不支持回调事件的配置。

#### Qrcode组件<a name="ZH-CN_TOPIC_0000002365940082"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中画布内的Qrcode组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621421.png" width="36">样式图标（Feature），在展开的Feature栏中修改Qrcode组件的ImageOpacity属性来修改二维码的透明度。
  
  **图 1**  修改二维码透明度<a name="fig19959125910167"></a>
  <img src="figures/修改二维码透明度.png" width="700" alt="修改二维码透明度">
- 选中画布内的Qrcode组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741325.png" width="36">样式图标（Feature），在展开的Feature栏中修改Qrcode组件的BackColor属性修改二维码的背景颜色。
  
  **图 2**  修改背景色<a name="fig6210165422115"></a>
  <img src="figures/修改背景色-39.png" width="700" alt="修改背景色-39">
- 选中画布内的Qrcode组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621425.png" width="36">样式图标（Feature），在展开的Feature栏中修改Qrcode组件的QrcodeColor属性修改二维码的颜色。
  
  **图 3**  修改二维码颜色<a name="fig10896149152219"></a>
  <img src="figures/修改二维码颜色.png" width="700" alt="修改二维码颜色">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - General页面下的BackgroungColor属性、MarginBottom属性和MarginRight属性均无渲染效果。
> - Feature页面下的Text属性无界面渲染效果，具体效果可在模拟器查看。
> - Qrcode组件不支持回调事件的配置。

#### AnalogClock组件<a name="ZH-CN_TOPIC_0000002399739761"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中AnalogClock组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741329.png" width="36">样式图标（Feature），在展开的Feature栏中修改Hour窗口中的PositionX属性和PositionY属性来修改时针的位置，同时，Minute窗口和Second窗口中的PositionX属性和PositionY属性可以分别修改分针和秒针的位置。
  
  **图 1**  修改指针的位置<a name="fig678011363619"></a>
  <img src="figures/修改指针的位置.png" width="700" alt="修改指针的位置">
- 选中AnalogClock组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621429.png" width="36">样式图标（Feature），在展开的Feature栏中修改Hour窗口中的CenterX属性和CenterY属性来修改时针的旋转中心（该坐标以PositionX和PositionY所在点为原点，相对于表针位置而非组件左上角）同时，Minute窗口和Second窗口中的CenterX属性和CenterY属性可以分别修改分针和秒针的旋转中心。
  
  **图 2**  修改指针的旋转中心<a name="fig821320214408"></a>
  <img src="figures/修改指针的旋转中心.png" width="700" alt="修改指针的旋转中心">
- 选中AnalogClock组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741333.png" width="36">样式图标（Feature），在展开的Feature栏中修改Hour窗口中的Color属性来修改时针的颜色，同时，Minute窗口和Second窗口中的Color属性可以分别修改分针和秒针的颜色。
  
  **图 3**  修改指针的颜色<a name="fig16383193311422"></a>
  <img src="figures/修改指针的颜色.png" width="700" alt="修改指针的颜色">
- 选中AnalogClock组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621433.png" width="36">样式图标（Feature），在展开的Feature栏中修改Hour窗口中的Width属性来修改时针的宽度，同时，Minute窗口和Second窗口中的Width属性可以分别修改分针和秒针的宽度。
  
  **图 4**  修改指针的宽度<a name="fig133511819144416"></a>
  <img src="figures/修改指针的宽度.png" width="700" alt="修改指针的宽度">
- 选中AnalogClock组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741337.png" width="36">样式图标（Feature），在展开的Feature栏中修改Hour窗口中的Length属性来修改时针的长度，同时，Minute窗口和Second窗口中的Length属性可以分别修改分针和秒针的长度。
  
  **图 5**  修改指针的长度<a name="fig189181575451"></a>
  <img src="figures/修改指针的长度.png" width="700" alt="修改指针的长度">
- 选中AnalogClock组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621437.png" width="36">样式图标（Feature），在展开的Feature栏中修改Hour窗口中的Opacity属性来修改时针的颜色透明度，同时，Minute窗口和Second窗口中的Opacity属性可以分别修改分针和秒针的颜色透明度。
  
  **图 6**  修改指针的颜色透明度<a name="fig1717814924613"></a>
  <img src="figures/修改指针的颜色透明度.png" width="700" alt="修改指针的颜色透明度">
- 选中AnalogClock组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741341.png" width="36">样式图标（Feature），在展开的Feature栏中修改Hour窗口中的LineImage属性来为时针添加图片，同时，Minute窗口和Second窗口中的LineImage属性可以分别为分针和秒针添加图片。
  
  **图 7**  为指针添加图片<a name="fig194842357484"></a>
<img src="figures/为指针添加图片.png" width="700" alt="为指针添加图片">
  
  > ![](public_sys-resources/icon-note.gif) **说明：**
  > LineImage属性不支持中文路径。
- 选中AnalogClock组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621445.png" width="36">样式图标（Feature），在展开的Feature栏中修改AnalogClock组件的NewMode属性来控制秒针的显示。
  
  **图 8**  控制秒针的显示<a name="fig7341154611512"></a>
  <img src="figures/控制秒针的显示.png" width="700" alt="控制秒针的显示">
- 选中画布内的AnalogClock组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741345.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  AnalogClock组件支持的事件有：OnClick、OnLongPress、OnPress、OnCancel和OnRelease。
  
  **图 9**  回调事件配置<a name="fig052871717486"></a>
  <img src="figures/回调事件配置-40.png" width="700" alt="回调事件配置-40">
- 选中组件栏中的其他组件，可以将其拖入AnalogClock组件中，并且对其他组件进行样式调整。子组件的位置是相对于父容器的左上角，当组件已经存在于界面上时，单击组件中心位置可以将组件拖入容器中。
  
  **图 10**  将其他组件拖入AnalogClock组件中<a name="fig192648563920"></a>
  <img src="figures/将其他组件拖入AnalogClock组件中.png" width="700" alt="将其他组件拖入AnalogClock组件中">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - General页面下的MarginBottom属性和MarginRight属性均无渲染效果。
> - Feature页面下的Am属性无界面渲染效果。

#### SweepClock组件<a name="ZH-CN_TOPIC_0000002366099982"></a>

本组件包含AnalogClock组件的所有属性，与其类似，因此相同属性不再赘述，请参见“AnalogClock组件”描述。

- 选中SweepClock组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941658.png" width="36">样式图标（Feature），在展开的Feature栏中修改Hour窗口中的CenterImage属性，为时针中心点添加图片，同时，Minute窗口和Second窗口中的CenterImage属性可以分别修改分针和秒针的中心点添加图片。
  
  **图 1**  为中心圆圈添加图片<a name="fig194842357484"></a>
<img src="figures/为中心圆圈添加图片.png" width="700" alt="为中心圆圈添加图片">
  
  > ![](public_sys-resources/icon-note.gif) **说明：**
  > CenterImage属性不支持中文路径。
- 选中SweepClock组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101542.png" width="36">样式图标（Feature），在展开的Feature栏中修改Hour窗口中的CenterImageX属性和CenterImageY属性，修改时针中心图片的位置。同时，Minute窗口和Second窗口中的CenterImage属性可以分别修改分针和秒针的中心图片的位置。
  
  **图 2**  控制秒针的显示<a name="fig7341154611512"></a>
  <img src="figures/控制秒针的显示-41.png" width="700" alt="控制秒针的显示-41">
- 选中组件栏中的其他组件，可以将其拖入SweepClock组件中，并且对其他组件进行样式调整。子组件的位置是相对于父容器的左上角，当组件已经存在于界面上时，单击组件中心位置可以将组件拖入容器中。
  
  **图 3**  将其他组件拖入SweepClock组件中<a name="fig192648563920"></a>
  <img src="figures/将其他组件拖入SweepClock组件中.png" width="700" alt="将其他组件拖入SweepClock组件中">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - General页面下的MarginBottom属性和MarginRight属性均无渲染效果。
> - Feature页面下的Am属性无界面渲染效果。
> - SweepClock不支持回调事件的配置。

#### Coverflow组件<a name="ZH-CN_TOPIC_0000002399619893"></a>

本组件的共有属性不包含Border属性、Margin属性与Padding属性，其他共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中Coverflow组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741357.png" width="36">样式图标（Feature），在展开的Feature栏导入一张或者多张图片（可以调整画布大小，使图片显示效果更好），第一张图片默认位于画布正中间。
  
  **图 1**  特有属性配置：ImagePath<a name="fig144651235202314"></a>
<img src="figures/特有属性配置-ImagePath-42.png" width="700" alt="特有属性配置-ImagePath-42">
  
  > ![](public_sys-resources/icon-note.gif) **说明：**
  > ImagePath不支持中文路径。
  > 导入的Bin文件必须是图片转的Bin，否则模拟器会异常。
- 选中画布内的Coverflow组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002399621461.png" width="36">样式图标（Feature），在展开的Feature栏中修改Coverflow组件中的ImgWidth和ImgHeight可以调整图片宽高。
  
  更改图片宽高后，所有导入Coverflow组件的图片宽高随之改变，且第一张图片始终位于组件的正中间。
  
  **图 2**  特有属性配置：ImgWidth和ImgHeight<a name="fig1549113615248"></a>
  <img src="figures/特有属性配置-ImgWidth和ImgHeight.png" width="700" alt="特有属性配置-ImgWidth和ImgHeight">
- 选中画布内的Coverflow组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002399741361.png" width="36">样式图标（Feature），在展开的Feature栏中修改Coverflow组件中的RotateAngle调整旋转角度。
  
  **图 3**  特有属性配置：RotateAngle<a name="fig1917017159243"></a>
  <img src="figures/特有属性配置-RotateAngle.png" width="700" alt="特有属性配置-RotateAngle">
- 选中画布内的Coverflow组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002399621465.png" width="36">样式图标（Feature），在展开的Feature栏中修改Coverflow组件中的Padding调整图片之间的间距。
  
  **图 4**  特有属性配置：Padding<a name="fig2142192232415"></a>
  <img src="figures/特有属性配置-Padding.png" width="700" alt="特有属性配置-Padding">
- 选中画布内的Coverflow组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002399741365.png" width="36">样式图标（Feature），在展开的Feature栏中修改Coverflow组件中的IsShowMirrorImg设置是否显示图片镜像。
  
  **图 5**  特有属性配置：IsShowMirrorImg<a name="fig25262913248"></a>
  <img src="figures/特有属性配置-IsShowMirrorImg.png" width="700" alt="特有属性配置-IsShowMirrorImg">
- 选中画布内的Coverflow组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002399621473.png" width="36">样式图标（Feature），在展开的Feature栏中修改Coverflow组件中的MirrorOpa设置图片镜像的透明度。
  
  **图 6**  特有属性配置：MirrorOpa<a name="fig96581036102413"></a>
  <img src="figures/特有属性配置-MirrorOpa.png" width="700" alt="特有属性配置-MirrorOpa">
- 选中画布内的Coverflow组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741373.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  Coverflow组件支持的事件有：OnDrag、OnDragStart、OnDragEnd和OnCoverflow。
  
  **图 7**  回调事件配置<a name="fig4163730162310"></a>
  <img src="figures/回调事件配置-43.png" width="700" alt="回调事件配置-43">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - Feature页面下的RotateAngle、IsShowMirrorImg和MirrorOpa暂无渲染效果，具体效果可以在模拟器查看。
> - Coverflow的回调事件在生成的XXXPresenter.cpp文件中的回调函数名称为“OnScroll（）”。

#### Barcode组件<a name="ZH-CN_TOPIC_0000002365940086"></a>

共有属性使用方法请参见“11.1.2.2 组件的共有属性”章节内容。

选中画布内的Barcode组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941686.png" width="36">样式图标（Feature），在展开的Feature栏中有一个BarcodeInfo属性，提供Barcode的信息，且不能设置为空，如果设置为空，生成代码后，在模拟器中会显示条形码失败。

**图 1**  特有属性配置：BarcodeInfo<a name="fig191081212257"></a>
<img src="figures/特有属性配置-BarcodeInfo.png" width="700" alt="特有属性配置-BarcodeInfo">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - Feature页面下的BarcodeInfo属性无页面渲染效果。
> - Barcode组件不支持回调事件的配置。
> - BarcodeInfo不能设置为空，且不支持中文，如果设置为空或者为中文，模拟器中显示条形码失败。
> - BarcodeInfo设置完成后请点击其他位置后，再生成代码。

#### CardPage组件<a name="ZH-CN_TOPIC_0000002399739765"></a>

本组件的General属性请参见“11.1.2.2 组件的共有属性”描述。

- 选中画布内的CardPage组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621509.png" width="36">  样式图标（Feature），在展开的Feature栏中有一个IsCoverable属性，默认为false，它的作用是设置拖动其他CardPage组件展示时是否覆盖上一个。
  
  **图 1**  特有属性配置：IsCoverable<a name="fig195722386343"></a>
  <img src="figures/特有属性配置-IsCoverable.png" width="700" alt="特有属性配置-IsCoverable">
- 选中画布内的CardPage组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101610.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  CardPage组件支持的事件类型有：OnClick、OnLongPress、OnDrag、OnDragStart、OnDragEnd、OnPress、OnCancel、OnRelease、OnFocus、OnBlur、OnRotate、OnRotateStart、OnRotateEnd。
  
  **图 2**  回调事件配置<a name="fig1790072115352"></a>
  <img src="figures/回调事件配置-44.png" width="700" alt="回调事件配置-44">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - Feature页面下的属性无界面渲染效果。
> - CardPage是一个容器组件，主要作为CrossView的子组件使用。

#### CrossView组件<a name="ZH-CN_TOPIC_0000002366099986"></a>

本组件的General属性请参见“11.1.2.2 组件的共有属性”描述。

- 选中CrossView组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741433.png" width="36">样式图标（Feature），在展开的Feature栏中可以看到EnableScreenCap属性，会弹出一个设定bool变量值的下拉框，它决定切换卡片时是否使能截图模式。
  
  **图 1**  特有属性配置：EnableScreenCap<a name="fig1358134319354"></a>
  
  <img src="figures/zh-cn_image_0000002366101638.png" width="700">
- 选中CrossView组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941766.png" width="36">样式图标（Feature），在展开的Feature栏中可以看到SetAnimatorTime属性，它决定模拟器中松手动画时间，其单位为毫秒（ms）。
  
  **图 2**  特有属性配置：SetAnimatorTime<a name="fig2959881495"></a>
  
  <img src="figures/zh-cn_image_0000002366101662.png" width="700">
- 选中CrossView组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621581.png" width="36">样式图标（Feature），在展开的Feature栏中可以看到DragDirection属性，它决定子组件拖拽来的方向。
  
  **图 3**  特有属性配置：DragDirection<a name="fig1772818954716"></a>
  
  <img src="figures/zh-cn_image_0000002365941782.png" width="700">
  
  当拖拽来的组件不是CardPage类型时，右下角会有“Only supports cardpage component”或“仅支持拖拽CardPage组件”提示，并且拖入的子组件不会在画布上展示。
  
  **图 4**  特有属性配置：DragDirection（仅支持拖拽CardPage类型的子组件）<a name="fig634965194814"></a>
  <img src="figures/特有属性配置-DragDirection（仅支持拖拽CardPage类型的子组件）.png" width="700" alt="特有属性配置-DragDirection（仅支持拖拽CardPage类型的子组件）">
  
  CrossView组件前端渲染界面仅会展示其某一个CardPage子组件：
  
  - 当新增CardPage组件时，默认会展示最新拖拽进来的CardPage组件，隐藏掉之前的；
  - 当删除CardPage组件时，默认会展示上个被拖入的CardPage组件，并隐藏掉其他的。
  
  如果想实现切换，可以点击“GUI拖拽界面介绍”中所介绍的Panel面板的<img src="figures/zh-cn_image_0000002366101686.png" width="49">按钮，该按钮使用说明如下：
  
  1. 左右切换会分别显示拖拽来的上一个和下一个CardPage子组件。
  2. 是否支持组件间的循环切换和模拟器的设定规则保持一致，可以在Feature栏的Isloop中选择True或False。当置为False时，第一个组件单击左按钮和最后一个组件单击右按钮时不会有切换效果；当置为True时，第一个组件单击左按钮和最后一个组件单击右按钮时会实现彼此切换。
  3. 切换按钮仅在选中父组件为CrossView的CardPage子组件时生效，选中操作可以通过单击左侧组件树中的CardPage，或在渲染界面单击CardPage后单击左右按钮，从而展示被选中组件的前一个和后一个，如图5所示。
  4. 切换按钮如果选的不是父组件为CrossView的CardPage子组件时，会弹窗提示，如图6所示。
  
  **图 5**  特有属性配置：DragDirection（CardPage切换展示逻辑）<a name="fig10608162418201"></a>
  <img src="figures/特有属性配置-DragDirection（CardPage切换展示逻辑）.png" width="700" alt="特有属性配置-DragDirection（CardPage切换展示逻辑）">
  
  **图 6**  特有属性配置：DragDirection（非CrossView子组件切换的弹窗提醒）<a name="fig560918241207"></a>
  <img src="figures/特有属性配置-DragDirection（非CrossView子组件切换的弹窗提醒）.png" width="700" alt="特有属性配置-DragDirection（非CrossView子组件切换的弹窗提醒）">
  
  当选择DragDirection为Horizontal方向时，拖拽CardPage组件，CardPage个数不做限制，每次拖拽入新的都会隐藏掉其他的CardPage子组件。
  
  **图 7**  特有属性配置：DragDirection（水平方向拖拽来的CardPage类型子组件）<a name="fig12609112419209"></a>
  <img src="figures/特有属性配置-DragDirection（水平方向拖拽来的CardPage类型子组件）.png" width="700" alt="特有属性配置-DragDirection（水平方向拖拽来的CardPage类型子组件）">
  
  当选择DragDirection为Vertical-Up方向时，拖拽CardPage组件，拖拽新的组件时也会隐藏掉其他的CardPage子组件，但仅支持拖拽一个，当再次拖拽该方向的CardPage子组件时右下角会有“UpPage direction only supports one component”或“竖直上方向的CardPage组件仅支持拖拽一个”提示。
  
  **图 8**  特有属性配置：DragDirectio（竖直上方向仅支持拖拽一个子组件）<a name="fig1260952419208"></a>
  <img src="figures/特有属性配置-DragDirectio（竖直上方向仅支持拖拽一个子组件）.png" width="700" alt="特有属性配置-DragDirectio（竖直上方向仅支持拖拽一个子组件）">
  
  当选择DragDirection为Vertical-Down方向时，拖拽CardPage组件，拖拽新的组件时也会隐藏掉其他的CardPage子组件，且仅支持拖拽的个数为一个，当再次拖拽该方向的CardPage子组件时右下角会有“DownPage direction only supports one component”或“竖直下方向的CardPage组件仅支持拖拽一个”提示。
  
  **图 9**  特有属性配置：DragDirection（竖直下方向仅支持拖拽一个子组件）<a name="fig16609142412208"></a>
  <img src="figures/特有属性配置-DragDirection（竖直下方向仅支持拖拽一个子组件）.png" width="700" alt="特有属性配置-DragDirection（竖直下方向仅支持拖拽一个子组件）">
- 选中CrossView组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941822.png" width="36">样式图标（Feature），在展开的Feature栏中点击Isloop属性，会弹出一个设定bool变量值的下拉框，它决定是否开启水平卡片循环模式。
  
  **图 10**  特有属性配置：Isloop<a name="fig18540511191815"></a>
  
  <img src="figures/zh-cn_image_0000002399741541.png" width="700">
- 选中CrossView组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101718.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  CrossView组件支持的事件有：OnClick、OnLongPress、OnDrag、OnDragStart、OnDragEnd、OnPress、OnCancel、OnRelease、OnFocus、OnBlur、OnRotate、OnRotateStart、OnRotateEnd、OnSwipe。其中OnSwipe默认为True。
  
  **图 11**  回调事件配置<a name="fig5540511151817"></a>
  
  <img src="figures/zh-cn_image_0000002399621633.png" width="700">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - Feature界面除DragDirection的特性和Isloop与GUI界面前端渲染关联外，其他属性无界面渲染效果，可在模拟器中查看。
> - CrossView组件仅支持CardPage类型的子组件拖拽。
> - CrossView组件的所有弹窗均支持中英文适配。

#### MapView组件<a name="ZH-CN_TOPIC_0000002399619897"></a>

本组件的General属性请参见“11.1.2.2 组件的共有属性”描述。

> ![](public_sys-resources/icon-notice.gif) **须知：**
> 使用MapView前，需要将随SDK携带的map\_convert\_tool复制到“tools\\simulator”目录下。

- 选中MapView组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941846.png" width="36">样式图标（Feature），在展开的Feature栏中单击SvgPath右侧的图标，会弹出一个选择Svg文件的界面，解压地图对应的Svg文件到任意目录，单击选择后，再次打开该目录，刷新，可以看到会生成一个与Svg文件同名的bin文件，该文件用于模拟器读取并展示。同时该地图文件会绘制到画布中。
  
  **图 1**  特有属性配置：SvgPath-选择Svg文件<a name="fig1556381810504"></a>
  <img src="figures/特有属性配置-SvgPath-选择Svg文件.png" width="700" alt="特有属性配置-SvgPath-选择Svg文件">
  
  **图 2**  特有属性配置：SvgPath-绘制Svg图片<a name="fig761210910508"></a>
  <img src="figures/特有属性配置-SvgPath-绘制Svg图片.png" width="700" alt="特有属性配置-SvgPath-绘制Svg图片">
- 选中MapView组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621645.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  MapView组件支持的事件有：OnClick、OnLongPress、OnDrag、OnDragStart、OnDragEnd、OnPress、OnCancel、OnRelease、OnFocus、OnBlur、OnRotate、OnRotateStart、OnRotateEnd和OnPOIClick。
  
  **图 3**  回调事件配置<a name="fig982152854217"></a>
  <img src="figures/回调事件配置-45.png" width="700" alt="回调事件配置-45">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - MapView是地图组件，应将地图文件展示到画布和模拟器中。
> - MapView的General属性仅包含ID、Position两个选项可供用户设置，工具和模拟器会默认展示该Svg图片的原始大小，用户可以根据模拟器实际效果调整Position参数。

#### RollerView组件<a name="ZH-CN_TOPIC_0000002365940090"></a>

本组件的共有属性不包含Border属性、Margin属性与Padding属性，其他共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中RollerView组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741561.png" width="36">样式图标（Feature），在展开的Feature栏导入一张或者多张图片（可以调整画布大小，使图片显示效果更好），第一张图片默认位于画布正中间。
  
  **图 1**  特有属性配置：ImagePath<a name="fig144651235202314"></a>
<img src="figures/特有属性配置-ImagePath-46.png" width="700" alt="特有属性配置-ImagePath-46">
  
  > ![](public_sys-resources/icon-note.gif) **说明：**
  > ImagePath不支持中文路径。
  > 导入的Bin文件必须是图片转的Bin，否则模拟器会异常。
- 选中画布内的RollerView组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002399621649.png" width="36">样式图标（Feature），在展开的Feature栏中修改RollerView组件中的ImageWidth和ImageHeight可以调整图片宽高。
  
  更改图片宽高后，所有导入RollerView组件的图片宽高随之改变，且第一张图片始终位于组件的正中间。
  
  **图 2**  特有属性配置：ImageWidth和ImageHeight<a name="fig1549113615248"></a>
  <img src="figures/特有属性配置-ImageWidth和ImageHeight.png" width="700" alt="特有属性配置-ImageWidth和ImageHeight">
- 选中画布内的RollerView组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002474130925.png" width="36">样式图标（Feature），在展开的Feature栏中修改RollerView组件中的MirrorOpacity调整滚筒元素镜像图的透明度。
  
  **图 3**  特有属性配置：MirrorOpacity<a name="fig514874910116"></a>
  <img src="figures/特有属性配置-MirrorOpacity.png" width="700" alt="特有属性配置-MirrorOpacity">
- 选中画布内的RollerView组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002440730820.png" width="36">样式图标（Feature），在展开的Feature栏中修改RollerView组件中的SensitivityFactor调整滚筒灵敏度。
  
  **图 4**  特有属性配置：SensitivityFactor<a name="fig2991158101319"></a>
  <img src="figures/特有属性配置-SensitivityFactor.png" width="700" alt="特有属性配置-SensitivityFactor">
- 选中画布内的RollerView组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002399741565.png" width="36">样式图标（Feature），在展开的Feature栏中修改RollerView组件中的Padding调整图片之间的间距。
  
  **图 5**  特有属性配置：Padding<a name="fig2142192232415"></a>
  <img src="figures/特有属性配置-Padding-47.png" width="700" alt="特有属性配置-Padding-47">
- 选中画布内的RollerView组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002399621653.png" width="36">样式图标（Feature），在展开的Feature栏中修改RollerView组件中的CameraDistance设置模拟器中相机的距离。
  
  **图 6**  特有属性配置：CameraDistance<a name="fig25262913248"></a>
  <img src="figures/特有属性配置-CameraDistance.png" width="700" alt="特有属性配置-CameraDistance">
- 选中画布内的RollerView组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002399741569.png" width="36">样式图标（Feature），在展开的Feature栏中修改RollerView组件中的CamY设置模拟器中相机的Y坐标。
  
  **图 7**  特有属性配置：CamY<a name="fig96581036102413"></a>
  <img src="figures/特有属性配置-CamY.png" width="700" alt="特有属性配置-CamY">
- 选中画布内的RollerView组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621657.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  RollerView组件支持的事件有：OnClick、OnLongPress、OnDrag、OnDragStart、OnDragEnd、OnPress、OnCancel、OnRelease、OnFocus、OnBlur、OnScroll、OnModeChange。
  
  **图 8**  回调事件配置<a name="fig17698142911119"></a>
  <img src="figures/回调事件配置-48.png" width="700" alt="回调事件配置-48">

> ![](public_sys-resources/icon-note.gif) **说明：**
> Feature页面下的CameraDistance和CamY暂无渲染效果，具体效果可以在模拟器查看。
> RollerView组件在模拟器中可以实现水平方向上将图片按照滚筒的形式排布并支持滚动交互。

#### HexagonsList组件<a name="ZH-CN_TOPIC_0000002399739769"></a>

本组件的共有属性不包含Border属性、Margin属性与Padding属性，其他共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中HexagonsList组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741573.png" width="36">样式图标（Feature），在展开的Feature栏导入一张或者多张图片（可以调整画布大小，使图片显示效果更好），第一张图片默认位于画布正中间。
  
  **图 1**  特有属性配置：ImagePath<a name="fig144651235202314"></a>
<img src="figures/特有属性配置-ImagePath-49.png" width="700" alt="特有属性配置-ImagePath-49">
  
  > ![](public_sys-resources/icon-note.gif) **说明：**
  > ImagePath不支持中文路径。
  > 导入的Bin文件必须是图片转的Bin，否则模拟器会异常。
- 选中画布内的HexagonsList组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002399621661.png" width="36">样式图标（Feature），在展开的Feature栏中修改HexagonsList组件中的ImageWidth和ImageHeight可以调整图片宽高。
  
  更改图片宽高后，所有导入HexagonsList组件的图片宽高随之改变，且第一张图片始终位于组件的正中间。
  
  **图 2**  特有属性配置：ImageWidth和ImageHeight<a name="fig1549113615248"></a>
  <img src="figures/特有属性配置-ImageWidth和ImageHeight-50.png" width="700" alt="特有属性配置-ImageWidth和ImageHeight-50">
- 选中画布内的HexagonsList组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002399741577.png" width="36">样式图标（Feature），在展开的Feature栏中修改HexagonsList组件中的Padding调整图片之间的间距。
  
  **图 3**  特有属性配置：Padding<a name="fig2142192232415"></a>
  <img src="figures/特有属性配置-Padding-51.png" width="700" alt="特有属性配置-Padding-51">
- 选中画布内的HexagonsList组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002399621673.png" width="36">样式图标（Feature），在展开的Feature栏中修改HexagonsList组件中的OriImgDistance属性来设置模拟器中两个相邻图像中心之间的原始图像距离。
  
  **图 4**  特有属性配置：OriImgDistance<a name="fig25262913248"></a>
  <img src="figures/特有属性配置-OriImgDistance.png" width="700" alt="特有属性配置-OriImgDistance">
- 选中画布内的HexagonsList组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002399741585.png" width="36">样式图标（Feature），在展开的Feature栏中修改HexagonsList组件中的ImgSizeInCenter属性来设置模拟器中图像位于中心位置的大小。
  
  **图 5**  特有属性配置：ImgSizeInCenter<a name="fig9701121017913"></a>
  <img src="figures/特有属性配置-ImgSizeInCenter.png" width="700" alt="特有属性配置-ImgSizeInCenter">
- 选中画布内的HexagonsList组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002399621681.png" width="36">样式图标（Feature），在展开的Feature栏中修改HexagonsList组件中的ReboundedSize属性来设置回弹大小，即拖拽到列表边缘之外再释放后回弹至列表边缘移动的距离。
  
  **图 6**  特有属性配置：ReboundedSize<a name="fig17864205617814"></a>
  <img src="figures/特有属性配置-ReboundedSize.png" width="700" alt="特有属性配置-ReboundedSize">
- 选中画布内的HexagonsList组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002399741589.png" width="36">样式图标（Feature），在展开的Feature栏中修改HexagonsList组件中的ScrollBlankSize属性来设置模拟器中组件滚动视图时的空白大小。
  
  **图 7**  特有属性配置：ScrollBlankSizec<a name="fig1256315491283"></a>
  <img src="figures/特有属性配置-ScrollBlankSizec.png" width="700" alt="特有属性配置-ScrollBlankSizec">
- 选中画布内的HexagonsList组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621689.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  HexagonsList组件支持的事件有：OnClick、OnLongPress、OnDrag、OnDragStart、OnDragEnd、OnPress、OnCancel、OnRelease、OnFocus、OnBlur、OnRotate、OnRotateStart、OnRotateEnd。
  
  **图 8**  回调事件配置<a name="fig17698142911119"></a>
  <img src="figures/回调事件配置-52.png" width="700" alt="回调事件配置-52">

> ![](public_sys-resources/icon-note.gif) **说明：**
> Feature页面下的OriImgDistance、ImgSizeInCenter、ReboundedSize和ScrollBlankSize暂无渲染效果，具体效果可以在模拟器查看。
> ImageWidth、Padding，ImageHeight可以用于用户在界面展示多个组件时设置图片大小。
> Rebounded和ScrollBlankSize的初始值为227，是默认组件的Position为\(0, 0, 454, 454\)，即整个表盘。具体的取值大小可以根据实际模拟器展示效果进行调整，通常这个取值为组件宽度的一半。
> OriImgDistance和ImgSizeInCenter属性与组件宽度的乘积，决定了模拟器中图片的摆放位置关系。

#### IcosahedronView组件<a name="ZH-CN_TOPIC_0000002366099990"></a>

本组件的共有属性不包含Position属性、Border属性、Margin属性与Padding属性，其他共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中IcosahedronView组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741597.png" width="36">样式图标（Feature），在展开的Feature栏导入一张图片（可以调整画布大小，使图片显示效果更好），图片会默认填满整个画布。
  
  **图 1**  特有属性配置：DefaultSrc<a name="fig1411916381693"></a>
  <img src="figures/特有属性配置-DefaultSrc.png" width="700" alt="特有属性配置-DefaultSrc">
- 选中IcosahedronView组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621693.png" width="36">样式图标（Feature），在展开的Feature栏中修改IcosahedronView组件中的LuminanceFactor属性来设置模拟器中亮度补偿因子，调整亮度。
  
  **图 2**  特有属性配置：LuminanceFactor<a name="fig51391045174012"></a>
  <img src="figures/特有属性配置-LuminanceFactor.png" width="700" alt="特有属性配置-LuminanceFactor">
- IcosahedronView组件仅支持Image组件导入。IcosahedronView组件的DefaultSrc属性设置了默认图片，如果导入的Image组件的图片个数少于20张，则使用该默认图片填充，达到类似足球样式的模拟器效果图。
  
  **图 3**  仅支持导入Image组件<a name="fig14703195575718"></a>
  <img src="figures/仅支持导入Image组件.png" width="700" alt="仅支持导入Image组件">
- 选中画布内的IcosahedronView组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101774.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  IcosahedronView组件支持的事件有：OnClick、OnLongPress、OnDrag、OnDragStart、OnDragEnd、OnPress、OnCancel、OnRelease、OnFocus、OnBlur、OnRotate、OnRotateStart、OnRotateEnd。
  
  **图 4**  回调事件配置<a name="fig1859035515919"></a>
  <img src="figures/回调事件配置-53.png" width="700" alt="回调事件配置-53">

> ![](public_sys-resources/icon-note.gif) **说明：**
> IcosahedronView组件仅支持width和height调节，其坐标位置固定。

#### CanvasExt组件<a name="ZH-CN_TOPIC_0000002399619901"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中画布内的CanvasExt组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941902.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。CanvasExt组件支持的事件有：OnClick、OnLongPress、OnPress、OnCancel和OnRelease。
  
  **图 1**  回调事件配置<a name="fig863319505301"></a>
  <img src="figures/回调事件配置-54.png" width="700" alt="回调事件配置-54">

#### TransformList组件<a name="ZH-CN_TOPIC_0000002365940094"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中TransformList组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101786.png" width="36">样式图标（Feature），在展开的Feature栏中修改itemHeight属性设置所有子项的高度；修改itemWidth属性设置所有子项的宽度；修改itemSpace属性设置子项之间的间隙；修改imageHeight属性设置所有子项图片的高度；修改FontSize属性设置所有子项的字体大小；点击Item栏右侧的加号添加子项，修改子项中的Text属性和ImagePath属性添加文本和图片。
  
  **图 1**  添加子项<a name="fig1424214304315"></a>
  <img src="figures/添加子项-55.png" width="700" alt="添加子项-55">
- 选中TransformList组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941910.png" width="36">样式图标（Feature），在展开的Feature栏中修改List组件的OffsetX属性设置所有子项的横向偏移，修改OffsetY属性设置所有子项的纵向偏移。
  
  **图 2**  修改子项的偏移<a name="fig8644443123216"></a>
  <img src="figures/修改子项的偏移-56.png" width="700" alt="修改子项的偏移-56">
- 选中TransformList组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002366101790.png" width="36">样式图标（Feature），在展开的Feature栏中修改TransformList组件的Direction属性设置列表的排列方向。
  
  **图 3**  修改列表的排列方向<a name="fig640685910323"></a>
  <img src="figures/修改列表的排列方向-57.png" width="700" alt="修改列表的排列方向-57">
- 选中画布内的TransformList组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365941914.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。

  TransformList组件支持的事件有：OnClick、OnLongPress、OnDrag、OnDragStart、OnDragEnd、OnPress、OnCancel、OnRelease、OnFocus和OnBlur。
  
  **图 4**  回调事件配置<a name="fig186332171075"></a>
  <img src="figures/回调事件配置-58.png" width="700" alt="回调事件配置-58">

#### SlipflowView组件<a name="ZH-CN_TOPIC_0000002399739773"></a>

本组件的共有属性不包含Position属性、Size属性、GaussOption属性、Border属性、Margin属性与Padding属性，其他共有属性使用方法请参见“11.1.2.2 组件的共有属性”描述。

- 选中画布内的SlipflowView组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002366101794.png" width="36">样式图标（Feature），在展开的Feature栏中修改SlipflowView组件中的ImgWidth和ImgHeight可以调整图片宽高。
  
  更改图片宽高后，所有导入SlipflowView组件的图片宽高随之改变，且第一张图片始终位于组件的正中间。
  
  **图 1**  特有属性配置：ImgWidth和ImgHeight<a name="fig186308234339"></a>
  <img src="figures/特有属性配置-ImgWidth和ImgHeight-59.png" width="700" alt="特有属性配置-ImgWidth和ImgHeight-59">
- 选中画布内的SlipflowView组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002399741625.png" width="36">样式图标（Feature），在展开的Feature栏中修改SlipflowView组件中的isEnableBackImage可以控制是否使能背景图片。
  
  **图 2**  特有属性配置：isEnableBackImage<a name="fig951023617338"></a>
  <img src="figures/特有属性配置-isEnableBackImage.png" width="700" alt="特有属性配置-isEnableBackImage">
- 选中SlipflowView组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621717.png" width="36">样式图标（Feature），在展开的Feature栏中修改SlipflowView组件中的backImagePath可以设置背景图片资源路径。
  
  **图 3**  特有属性配置：backImagePath<a name="fig1650517572334"></a>
  <img src="figures/特有属性配置-backImagePath.png" width="700" alt="特有属性配置-backImagePath">
- 选中SlipflowView组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741629.png" width="36">样式图标（Feature），在展开的Feature栏导入一张或者多张图片（可以调整画布大小，使图片显示效果更好），第一张图片默认位于画布正中间。
  
  **图 4**  特有属性配置：ImagePath<a name="fig144651235202314"></a>
  
  <img src="figures/zh-cn_image_0000002366101802.png" width="700">
  
  > ![](public_sys-resources/icon-note.gif) **说明：**
  > ImagePath不支持中文路径。
  > 导入的Bin文件必须是图片转的Bin，否则模拟器会异常。
- 选中画布内的SlipflowView组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399621721.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  SlipflowView组件支持的事件有：OnClick、OnLongPress、OnPress、OnCancel、OnRelease、OnScroll、OnScrollUpStart、OnScrollUpEnd、OnRemove和OnRefreshPageOpaScale。
  
  **图 5**  回调事件配置<a name="fig4163730162310"></a>
  <img src="figures/回调事件配置-60.png" width="700" alt="回调事件配置-60">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - SlipflowView组件坐标位置固定，大小固定。
> - Feature页面下的isEnableBackImage和backImagePath暂无渲染效果，具体效果可以在模拟器查看。

#### TransformGroup组件<a name="ZH-CN_TOPIC_0000002366099994"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中画布内的TransformGroup组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002399741665.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  TransformGroup组件支持的事件类型有：OnClick、OnLongPress、OnPress、OnCancel、OnRelease。
  
  **图 1**  回调事件配置<a name="fig1790072115352"></a>
  <img src="figures/回调事件配置-61.png" width="700" alt="回调事件配置-61">

> ![](public_sys-resources/icon-note.gif) **说明：**
> TransformGroup是一个容器组件，主要作为Coverflow2的子组件使用。

#### Coverflow2组件<a name="ZH-CN_TOPIC_0000002399619905"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中画布内的Coverflow2组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002399621761.png" width="36">样式图标（Feature），在展开的Feature栏中修改Coverflow2组件中的RotateAngle调整旋转角度。
  
  **图 1**  特有属性配置：RotateAngle<a name="fig1917017159243"></a>
  
  <img src="figures/zh-cn_image_0000002365941966.png" width="700">
- 选中画布内的Coverflow2组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002399741673.png" width="36">样式图标（Feature），在展开的Feature栏中修改Coverflow2组件中的Padding调整图片之间的间距。
  
  **图 2**  特有属性配置：Padding<a name="fig2142192232415"></a>
  
  <img src="figures/zh-cn_image_0000002366101846.png" width="700">
- 选中画布内的Coverflow2组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002399621765.png" width="36">样式图标（Feature），在展开的Feature栏中修改Coverflow2组件中的IsShowMirrorImg设置是否显示图片镜像。
  
  **图 3**  特有属性配置：IsShowMirrorImg<a name="fig25262913248"></a>
  
  <img src="figures/zh-cn_image_0000002365941970.png" width="700">
- 选中画布内的Coverflow2组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002399741677.png" width="36">样式图标（Feature），在展开的Feature栏中修改Coverflow2组件中的MirrorOpa设置图片镜像的透明度。
  
  **图 4**  特有属性配置：MirrorOpa<a name="fig96581036102413"></a>
  
  <img src="figures/zh-cn_image_0000002366101850.png" width="700">
  
  当拖拽来的组件不是TransformGroup类型时，右下角会有“Only supports transformgroup component”或“仅支持拖拽TransformGroup组件”提示，并且拖入的子组件不会在画布上展示。
  
  **图 5**  组件拖入Coverflow2组件中<a name="fig1535915143218"></a>
  <img src="figures/组件拖入Coverflow2组件中.png" width="700" alt="组件拖入Coverflow2组件中">

Coverflow2组件前端渲染界面仅会展示其某一个TransformGroup子组件：

- 当新增TransformGroup组件时，默认会展示最新拖拽进来的TransformGroup组件，隐藏掉之前的；
- 当删除TransformGroup组件时，默认会展示上个被拖入的TransformGroup组件，并隐藏掉其他的。

如果想实现切换，可以单击“GUI拖拽界面介绍”中的Panel面板的<img src="figures/zh-cn_image_0000002365941974.png" width="49">按钮，该按钮使用说明如下：

1. 左右切换会分别显示拖拽来的上一个和下一个TransformGroup子组件。
2. 切换按钮在选中父组件为Coverflow2的TransformGroup子组件时生效，选中操作可以通过单击左侧组件树中的TransformGroup，或在渲染界面单击TransformGroup后单击左右按钮，从而展示被选中组件的前一个和后一个，如图6所示。
3. 切换按钮如果选的不是父组件为Coverflow2的TransformGroup子组件时，会弹窗提示，如图7所示。

**图 6**  TransformGroup切换展示逻辑<a name="fig14330923143518"></a>
<img src="figures/TransformGroup切换展示逻辑.png" width="700" alt="TransformGroup切换展示逻辑">

**图 7**  非Coverflow2子组件切换的弹窗提醒<a name="fig1459413716407"></a>
<img src="figures/非Coverflow2子组件切换的弹窗提醒.png" width="700" alt="非Coverflow2子组件切换的弹窗提醒">

#### ParticleView组件<a name="ZH-CN_TOPIC_0000002414912630"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中画布内的ParticleView组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002415072518.png" width="36">样式图标（Feature），在展开的Feature栏中修改ParticleView组件中的birthRateRatio调整粒子生产率系数，该系数将与ParticleCell定义的粒子生产率相乘。
  
  **图 1**  特有属性配置：birthRateRatio<a name="fig910505382912"></a>
  <img src="figures/特有属性配置-birthRateRatio.png" width="700" alt="特有属性配置-birthRateRatio">
- 选中画布内的ParticleView组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002414912694.png" width="36">样式图标（Feature），在展开的Feature栏中修改ParticleView组件中的lifeTimeRatio调整粒子生命周期系数，该系数将与ParticleCell定义的粒子生命周期相乘。
  
  **图 2**  特有属性配置：lifeTimeRatio<a name="fig121968624213"></a>
  <img src="figures/特有属性配置-lifeTimeRatio.png" width="700" alt="特有属性配置-lifeTimeRatio">
- 选中画布内的ParticleView组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002415072510.png" width="36">样式图标（Feature），在展开的Feature栏中修改ParticleView组件中的scaleRatio调整粒子缩放系数，该系数将与ParticleCell定义的粒子缩放倍数相乘。
  
  **图 3**  特有属性配置：scaleRatio<a name="fig1273042764519"></a>
  <img src="figures/特有属性配置-scaleRatio.png" width="700" alt="特有属性配置-scaleRatio">
- 选中画布内的ParticleView组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002414912690.png" width="36">样式图标（Feature），在展开的Feature栏中修改ParticleView组件中的velocityRatio调整粒子速度系数，该系数将与ParticleCell定义的粒子速度相乘。
  
  **图 4**  特有属性配置：velocityRatio<a name="fig2389147184815"></a>
  <img src="figures/特有属性配置-velocityRatio.png" width="700" alt="特有属性配置-velocityRatio">
- 选中画布内的ParticleView组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002415072506.png" width="36">样式图标（Feature），在展开的Feature栏中修改ParticleView组件中的Repeat调整粒子动画是否重复运行。
  
  **图 5**  特有属性配置：Repeat<a name="fig174436186489"></a>
  <img src="figures/特有属性配置-Repeat.png" width="700" alt="特有属性配置-Repeat">
- 选中画布内的ParticleView组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002414912682.png" width="36">样式图标（Feature），在展开的Feature栏中修改ParticleView组件中的Time调整粒子动画运行周期。
  
  **图 6**  特有属性配置：Time<a name="fig18017328563"></a>
  <img src="figures/特有属性配置-Time.png" width="700" alt="特有属性配置-Time">
- 选中画布内的ParticleView组件，通过右侧属性样式中的<img src="figures/zh-cn_image_0000002415072502.png" width="36">样式图标（Feature），在展开的Feature栏中点击ImagePath右侧的加号，导入一张或多张图片，并设置各项参数。
  
  **图 7**  特有属性配置：ImagePath<a name="fig913100132315"></a>
  <img src="figures/特有属性配置-ImagePath-62.png" width="700" alt="特有属性配置-ImagePath-62">
  
  **表 1**  特有属性配置：ImagePath参数说明
  
  <a name="table17204037152519"></a>
  
  <table><thead align="left"><tr id="row22041537192514"><th class="cellrowborder" valign="top" width="37.230000000000004%" id="mcps1.2.3.1.1"><p id="p6204237152515"><a name="p6204237152515"></a><a name="p6204237152515"></a>属性</p>
  </th>
  <th class="cellrowborder" valign="top" width="62.77%" id="mcps1.2.3.1.2"><p id="p102046372256"><a name="p102046372256"></a><a name="p102046372256"></a>介绍</p>
  </th>
  </tr>
  </thead>
  <tbody><tr id="row3204143710258"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p72041371252"><a name="p72041371252"></a><a name="p72041371252"></a>Path</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p620413742511"><a name="p620413742511"></a><a name="p620413742511"></a>设置粒子图源。</p>
  </td>
  </tr>
  <tr id="row1020573782513"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p42051837172513"><a name="p42051837172513"></a><a name="p42051837172513"></a>CellWidth</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p891114132611"><a name="p891114132611"></a><a name="p891114132611"></a>设置粒子初始宽度。</p>
  </td>
  </tr>
  <tr id="row8205203717253"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p420516373255"><a name="p420516373255"></a><a name="p420516373255"></a>CellHeight</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p2205337152519"><a name="p2205337152519"></a><a name="p2205337152519"></a>设置粒子初始高度。</p>
  </td>
  </tr>
  <tr id="row4521651152719"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p9521105116271"><a name="p9521105116271"></a><a name="p9521105116271"></a>CellSizeRange</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p1152145142711"><a name="p1152145142711"></a><a name="p1152145142711"></a>设置粒子初始宽高上下浮动范围。</p>
  </td>
  </tr>
  <tr id="row779724562716"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p1379754572718"><a name="p1379754572718"></a><a name="p1379754572718"></a>PosX</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p279824552719"><a name="p279824552719"></a><a name="p279824552719"></a>设置粒子横向初始位置。</p>
  </td>
  </tr>
  <tr id="row420516372256"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p1720593716256"><a name="p1720593716256"></a><a name="p1720593716256"></a>PosY</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p226662423115"><a name="p226662423115"></a><a name="p226662423115"></a>设置粒子纵向初始位置。</p>
  </td>
  </tr>
  <tr id="row122051037172511"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p4205183718259"><a name="p4205183718259"></a><a name="p4205183718259"></a>PosXRange</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p82052037122517"><a name="p82052037122517"></a><a name="p82052037122517"></a>设置粒子横向初始位置上下浮动范围。</p>
  </td>
  </tr>
  <tr id="row720520373257"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p5205337122517"><a name="p5205337122517"></a><a name="p5205337122517"></a>PosYRange</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p911463613217"><a name="p911463613217"></a><a name="p911463613217"></a>设置粒子纵向初始位置上下浮动范围。</p>
  </td>
  </tr>
  <tr id="row19675194919321"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p467512494322"><a name="p467512494322"></a><a name="p467512494322"></a>Lifetime</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p10675134963217"><a name="p10675134963217"></a><a name="p10675134963217"></a>设置粒子生命周期。</p>
  </td>
  </tr>
  <tr id="row155488567324"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p854865615326"><a name="p854865615326"></a><a name="p854865615326"></a>LifetimeRange</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p2054825615324"><a name="p2054825615324"></a><a name="p2054825615324"></a>设置粒子生命周期上下浮动范围。</p>
  </td>
  </tr>
  <tr id="row396681173310"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p196615123319"><a name="p196615123319"></a><a name="p196615123319"></a>BirthRate</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p15966121133314"><a name="p15966121133314"></a><a name="p15966121133314"></a>设置粒子生产率：每个动画周期生产多少粒子。</p>
  </td>
  </tr>
  <tr id="row173016612331"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p1773016613317"><a name="p1773016613317"></a><a name="p1773016613317"></a>Velocity</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p12730126173314"><a name="p12730126173314"></a><a name="p12730126173314"></a>设置粒子速度。</p>
  </td>
  </tr>
  <tr id="row371125313218"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p117219532321"><a name="p117219532321"></a><a name="p117219532321"></a>VelocityRange</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p8722053163211"><a name="p8722053163211"></a><a name="p8722053163211"></a>设置粒子速度上下浮动范围。</p>
  </td>
  </tr>
  <tr id="row1855533544119"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p355653594111"><a name="p355653594111"></a><a name="p355653594111"></a>StartAlpha</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p1655613524111"><a name="p1655613524111"></a><a name="p1655613524111"></a>设置粒子起始透明度。</p>
  </td>
  </tr>
  <tr id="row58065391415"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p13807153918410"><a name="p13807153918410"></a><a name="p13807153918410"></a>FinalAlpha</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p1780733944116"><a name="p1780733944116"></a><a name="p1780733944116"></a>设置粒子最终透明度。</p>
  </td>
  </tr>
  <tr id="row812554594114"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p6125104519419"><a name="p6125104519419"></a><a name="p6125104519419"></a>AlphaRange</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p4125145184117"><a name="p4125145184117"></a><a name="p4125145184117"></a>设置粒子起始和最终透明度上下浮动范围。</p>
  </td>
  </tr>
  <tr id="row1168020714411"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p176806714413"><a name="p176806714413"></a><a name="p176806714413"></a>ScaleX</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p18681197204417"><a name="p18681197204417"></a><a name="p18681197204417"></a>设置粒子横向最终缩放倍数。</p>
  </td>
  </tr>
  <tr id="row17731911194415"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p12731711144417"><a name="p12731711144417"></a><a name="p12731711144417"></a>ScaleY</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p1073191113448"><a name="p1073191113448"></a><a name="p1073191113448"></a>设置粒子纵向最终缩放倍数。</p>
  </td>
  </tr>
  <tr id="row175261522134418"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p5526182213448"><a name="p5526182213448"></a><a name="p5526182213448"></a>ScaleRange</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p1152619221441"><a name="p1152619221441"></a><a name="p1152619221441"></a>设置粒子最终缩放倍数上下浮动范围。</p>
  </td>
  </tr>
  <tr id="row98292018114415"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p188291188445"><a name="p188291188445"></a><a name="p188291188445"></a>AccelerationX</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p8681252194818"><a name="p8681252194818"></a><a name="p8681252194818"></a>设置粒子横向加速度。</p>
  </td>
  </tr>
  <tr id="row13179101524415"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p3179161515447"><a name="p3179161515447"></a><a name="p3179161515447"></a>AccelerationY</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p73541316134911"><a name="p73541316134911"></a><a name="p73541316134911"></a>设置粒子纵向加速度。</p>
  </td>
  </tr>
  <tr id="row1330413394413"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p0304123344415"><a name="p0304123344415"></a><a name="p0304123344415"></a>EmissionAngle</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p1304113312447"><a name="p1304113312447"></a><a name="p1304113312447"></a>设置粒子发射角度。</p>
  </td>
  </tr>
  <tr id="row1052213317498"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p1152203112496"><a name="p1152203112496"></a><a name="p1152203112496"></a>EmissionRange</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p9522133134910"><a name="p9522133134910"></a><a name="p9522133134910"></a>设置粒子发射角度上下浮动范围。</p>
  </td>
  </tr>
  <tr id="row17855163519497"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p18551635154910"><a name="p18551635154910"></a><a name="p18551635154910"></a>TotalPeriod</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p16855183518497"><a name="p16855183518497"></a><a name="p16855183518497"></a>设置ParticleCell活跃周期。仅在活跃周期内的ParticleCell可以生产粒子。</p>
  </td>
  </tr>
  <tr id="row1667124013494"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p567154010498"><a name="p567154010498"></a><a name="p567154010498"></a>Spin</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p196711440194912"><a name="p196711440194912"></a><a name="p196711440194912"></a>设置粒子周期性旋转度数。</p>
  </td>
  </tr>
  <tr id="row6951194210499"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p795184214919"><a name="p795184214919"></a><a name="p795184214919"></a>SpinMode</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p095110422498"><a name="p095110422498"></a><a name="p095110422498"></a>设置粒子周期性旋转模式。</p>
  </td>
  </tr>
  <tr id="row15625194512496"><td class="cellrowborder" valign="top" width="37.230000000000004%" headers="mcps1.2.3.1.1 "><p id="p562514584911"><a name="p562514584911"></a><a name="p562514584911"></a>SpinRange</p>
  </td>
  <td class="cellrowborder" valign="top" width="62.77%" headers="mcps1.2.3.1.2 "><p id="p5625174594917"><a name="p5625174594917"></a><a name="p5625174594917"></a>设置粒子周期性旋转度数上下浮动范围。</p>
  </td>
  </tr>
  </tbody>
  </table>

#### LabelExt组件<a name="ZH-CN_TOPIC_0000002484375920"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中画布内的LabelExt组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002484376794.png" width="36">样式图标（Feature），在展开的Feature栏中修改LabelExt组件的Text属性来编辑文本内容（新添加组件默认文本内容为“default”）。
  
  **图 1**  特有属性配置：Text文本内容<a name="fig455274919252"></a>
  <img src="figures/特有属性配置-Text文本内容-63.png" width="700" alt="特有属性配置-Text文本内容-63">
- 选中画布内的LabelExt组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002516736717.png" width="36">样式图标（Feature），在展开的Feature栏中设置LabelExt组件的Font属性：
  
  - Color：字体颜色
  - Size：字体大小
  - LetterSpace：字母间距
  - LineSpace：行间距
  - LineHeight：行高（行高小于字体大小时将以字体大小为准，最终行高＝行高＋行间距）
  - TextDirection：文本方向
  
  **图 2**  通用属性配置：文本相关属性<a name="fig15826115915219"></a>
  <img src="figures/通用属性配置-文本相关属性-64.png" width="700" alt="通用属性配置-文本相关属性-64">
- 选中画布内的LabelExt组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002484376796.png" width="36">样式图标（Feature），在展开的Feature栏中修改LabelExt组件的TextAlign属性改变文字的横向与纵向排布。
  
  **图 3**  改变文字排布<a name="fig195425918268"></a>
  <img src="figures/改变文字排布-65.png" width="700" alt="改变文字排布-65">
- 选中画布内的LabelExt组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002516736719.png" width="36">样式图标（Feature），在展开的Feature栏中修改LabelExt组件的LineBreakMode属性。LineBreakMode对应6种换行模式，详细介绍如下：
  
  - Adapt：组件大小自适应文本，且不会对文本做自动换行。
    
    > ![](public_sys-resources/icon-note.gif) **说明：**
    > 组件宽高不受通用属性Width/Height值影响，因此不可通过拖拽组件或直接修改Width/Height值的方式改变组件宽高。
    
    **图 4**  LineBreakMode - Adapt<a name="fig1679161320461"></a>
    <img src="figures/LineBreakMode---Adapt-66.png" width="700" alt="LineBreakMode---Adapt-66">
  - Stretch：组件高度按照设置的Height值显示，宽度由文本中最长的行决定，文本不会自动换行。
    
    > ![](public_sys-resources/icon-note.gif) **说明：**
    > 组件宽度不受通用属性Width值影响，因此不可通过拖拽组件或直接修改Width值的方式改变组件宽度。
    
    **图 5**  LineBreakMode - Stretch<a name="fig8791181319462"></a>
    <img src="figures/LineBreakMode---Stretch-67.png" width="700" alt="LineBreakMode---Stretch-67">
  - Wrap：组件宽度按照设置的Width值显示，文本自动换行，组件高度由文本行数决定。
    
    > ![](public_sys-resources/icon-note.gif) **说明：**
    > 组件高度不受通用属性Height值影响，因此不可通过拖拽组件或直接修改Height值的方式改变组件高度。
    
    **图 6**  LineBreakMode - Wrap<a name="fig177911513124619"></a>
    <img src="figures/LineBreakMode---Wrap-68.png" width="700" alt="LineBreakMode---Wrap-68">
  - Ellipsis：组件大小按照设置的Height和Width值显示，文本自动换行，超出组件的文本将在末尾以省略号的形式显示。
    
    **图 7**  LineBreakMode - Ellipsis<a name="fig1179181314462"></a>
    <img src="figures/LineBreakMode---Ellipsis-69.png" width="700" alt="LineBreakMode---Ellipsis-69">
  - Clip：组件大小按照设置的Height和Width值显示，文本自动换行，超出组件的文本将在末尾自动隐藏。
    
    **图 8**  LineBreakMode - Clip<a name="fig14791191324610"></a>
    <img src="figures/LineBreakMode---Clip-70.png" width="700" alt="LineBreakMode---Clip-70">
  - Marquee：组件大小按照设置的Height和Width值显示，文本不自动换行，超出组件的文本将自动隐藏。
    
    > ![](public_sys-resources/icon-note.gif) **说明：**
    > 此模式下当单行文本长度不超出组件时，文本按照设置的TextAlign进行对齐。
    > 文本长度超出组件时，文本会自动向左循环滚动播放。
    
    **图 9**  LineBreakMode - Marquee<a name="fig530611174710"></a>
    <img src="figures/LineBreakMode---Marquee-71.png" width="700" alt="LineBreakMode---Marquee-71">
- 选中画布内的LabelExt组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002516616733.png" width="36">样式图标（Feature），在展开的Feature栏中修改LabelExt组件的RollAnimation属性。
  
  RollAnimation属性只在LineBreakMode为Marquee且文本长度超出时生效。RollAnimation包含滚动速度（Speed）和滚动起始位置（Pos）。
  
  **图 10**  特有属性配置：RollAnimation<a name="fig19196141194615"></a>
<img src="figures/特有属性配置-RollAnimation-72.png" width="700" alt="特有属性配置-RollAnimation-72">
  
  > ![](public_sys-resources/icon-note.gif) **说明：**
  > 
  > - General页面下的MarginBottom属性和MarginRight属性没有任何界面渲染效果，可在模拟器上查看效果。
  > - LabelExt组件不支持配置回调事件。
- 选中画布内的LabelExt组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002516744281.png" width="36">样式图标（Feature），在展开的Feature栏中修改LabelExt组件的TextId属性，设置TextId，该功能需用户手动注册多语言功能，否则界面无法正常显示。
  
  **图 11**  特有属性配置：TextId<a name="fig156815014613"></a>
  <img src="figures/特有属性配置-TextId.png" width="700" alt="特有属性配置-TextId">
- 选中画布内的LabelExt组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002484544338.png" width="36">样式图标（Feature），在展开的Feature栏中修改LabelExt组件的ForceResetText属性，强制根据控件内部的TextId设置文本内容，该功能需用户手动注册多语言功能，否则界面无法正常显示。
  
  **图 12**  特有属性配置：ForceResetText<a name="fig3903715204816"></a>
  <img src="figures/特有属性配置-ForceResetText.png" width="700" alt="特有属性配置-ForceResetText">

#### ChartPillarExt组件<a name="ZH-CN_TOPIC_0000002555963293"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中ChartPillarExt组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002555963639.png" width="36">样式图标（Feature），在展开的Feature栏中修改XAxis栏中的属性改变横坐标的样式。
  
  - MarkNum属性：改变坐标轴上点的数量
  - RangMin和RangMax：改变坐标轴的取值范围
  - Color：改变坐标轴的颜色
  - Visible：控制坐标轴的可见与否
  
  **图 1**  修改横坐标属性<a name="fig123829568359"></a>
  <img src="figures/修改横坐标属性-73.png" width="700" alt="修改横坐标属性-73">
- 选中ChartPillarExt组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002555964027.png" width="36">样式图标（Feature），在展开的Feature栏中修改YAxis栏中的属性改变纵坐标样式，具体设置与横坐标相同，请参考图2。
  
  **图 2**  修改纵坐标样式<a name="fig18433110133716"></a>
  <img src="figures/修改纵坐标样式-74.png" width="700" alt="修改纵坐标样式-74">
- 选中ChartPillarExt组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002555964189.png" width="36">样式图标（Feature），在展开的Feature栏中单击DataSerial属性右侧的加号添加数据，可以添加多组数据。
  
  **图 3**  添加数据<a name="fig1728417410386"></a>
  <img src="figures/添加数据-75.png" width="700" alt="添加数据-75">
- 选中ChartPillarExt组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002555964417.png" width="36">样式图标（Feature），在展开的Feature栏中添加数据然后改变数据的样式，其中Data属性是要填入的数据，DataCount属性是数据的个数，FillColor是填充颜色。
  
  **图 4**  添加数据效果展示<a name="fig187731248123814"></a>
  <img src="figures/添加数据效果展示-76.png" width="700" alt="添加数据效果展示-76">
- 选中ChartPillarExt组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002555805449.png" width="36">样式图标（Feature），在展开的Feature栏中修改ChartPillarExt组件的PillarType属性改变图的类型。
  
  **图 5**  修改图的类型<a name="fig1353005014615"></a>
  <img src="figures/修改图的类型-77.png" width="700" alt="修改图的类型-77">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - General页面下的MarginBottom属性和MarginRight属性均无渲染效果。
> - 添加数据时的格式为： \{x1,y1\},\{x2,y2\};
>   数据中不能有空格，横坐标应从0开始依次递增，DataCount属性中的数字不能比数据个数小。当DataCount属性中的数字比数据个数大时，会自动补连\{0,0\}，柱状图时横坐标的MarkNum也不能比数据个数小。
> - 目前ChartPillarExt会有第一次修改DataSerial属性里的属性界面无法发生变化的问题，通过再次修改DataSerial属性中的其他属性的方式可以使界面发生变化。
> - ChartPillarExt组件不支持回调事件的配置。

#### ListNested组件<a name="ZH-CN_TOPIC_0000002555803331"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中ListNested组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002555965917.png" width="36">样式图标（Feature），在展开的Feature栏中修改itemHeight属性设置所有子项的高度；修改itemWidth属性设置所有子项的宽度；修改imageWidth属性设置所有子项图片的宽度；修改imageHeight属性设置所有子项图片的高度；修改FontSize属性设置所有子项的字体大小；单击Item栏右侧的加号添加子项，修改子项中的Text属性和ImagePath属性添加文本和图片。
  
  **图 1**  添加子项<a name="fig12369193710485"></a>
  <img src="figures/添加子项-78.png" width="700" alt="添加子项-78">
- 选中ListNested组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002555805947.png" width="36">样式图标（Feature），在展开的Feature栏中修改ListNested组件的OffsetX属性设置所有子项的横向偏移，修改OffsetY属性设置所有子项的纵向偏移。
  
  **图 2**  修改子项的偏移<a name="fig18329101454916"></a>
  <img src="figures/修改子项的偏移-79.png" width="700" alt="修改子项的偏移-79">
- 选中ListNested组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002524885994.png" width="36">样式图标（Feature），在展开的Feature栏中修改ListNested组件的InterceptDireaction属性设置拦截方向，修改IsIntercept属性设置是否拦截上抛事件。
  
  **图 3**  修改拦截事件相关属性<a name="fig11609952204914"></a>
  <img src="figures/修改拦截事件相关属性.png" width="700" alt="修改拦截事件相关属性">
- 选中ListNested组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002524726044.png" width="36">样式图标（Feature），在展开的Feature栏中修改ListNested组件的Direction属性设置列表的排列方向。
  
  **图 4**  修改列表的排列方向<a name="fig115271937135017"></a>
  <img src="figures/修改列表的排列方向-80.png" width="700" alt="修改列表的排列方向-80">
- 选中画布内的ListNested组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002555965859.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  ListNested组件支持的事件有：OnClick、OnLongPress、OnDrag、OnDragStart、OnDragEnd、OnPress、OnCancel、OnRelease、OnFocus和OnBlur。
  
  **图 5**  回调事件配置<a name="fig112356202517"></a>
  <img src="figures/回调事件配置-81.png" width="700" alt="回调事件配置-81">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - General页面下的MarginBottom属性和MarginRight属性无渲染效果。
> - Feature页面下的Isloop属性、Autoalign属性、Aligntime属性、Startindex属性均没有界面渲染效果，在添加子元素后可在模拟器查看效果。

#### SwipeViewNested组件<a name="ZH-CN_TOPIC_0000002524723440"></a>

本组件的共有属性不包含Border属性、Margin属性与Padding属性，其他共有属性使用方法请参见“组件的共有属性”章节内容。

- 选中SwipeViewNested组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002524887776.png" width="36">样式图标（Feature），在展开的Feature栏中修改SwipeViewNested组件的Direction属性改变组件的滚动方向。子组件的位置是相对于父容器的左上角，当组件已经存在于界面上时，单击组件中心位置可以将组件拖入容器中。
  
  **图 1**  修改滚动条位置<a name="fig5584141412577"></a>
  <img src="figures/修改滚动条位置-82.png" width="700" alt="修改滚动条位置-82">
  
  <img src="figures/zh-cn_image_0000002555807703.png" width="700">
  
  > ![](public_sys-resources/icon-note.gif) **说明：**
  > 滚动条已被隐藏，当容器内的子组件超出范围时可拖动，HORIZONTAL是水平滚动，VERTICAL是垂直滚动。
- 选中SwipeViewNested组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002524887828.png" width="36">样式图标（Feature），在展开的Feature栏中修改SwipeViewNested组件的InterceptDireaction属性设置拦截方向，修改IsIntercept属性设置是否拦截上抛事件。
  
  **图 2**  修改拦截事件相关属性<a name="fig118081242145818"></a>
  <img src="figures/修改拦截时间相关属性.png" width="700" alt="修改拦截时间相关属性">
- 选中画布内的SwipeViewNested组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002524727874.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  SwipeViewNested组件支持的事件有：OnDrag、OnDragStart、OnDragEnd、OnSwipe。
  
  **图 3**  回调事件配置<a name="fig1473842595917"></a>
  <img src="figures/回调事件配置-83.png" width="700" alt="回调事件配置-83">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - Feature页面下的Isloop属性、AlignMode属性、TickTime属性暂无渲染效果。
> - 在水平模式下上方工具栏的顶部对齐<img src="figures/zh-cn_image_0000002555811581.png" width="28">、底部对齐<img src="figures/zh-cn_image_0000002555971545.png" width="33">、上下居中对齐<img src="figures/zh-cn_image_0000002555811585.png" width="29">不可用。
> - 在垂直模式下上方工具栏的左对齐<img src="figures/zh-cn_image_0000002555811583.png" width="35">、右对齐<img src="figures/zh-cn_image_0000002555971547.png" width="24">、左右居中对齐<img src="figures/zh-cn_image_0000002524731690.png" width="27">不可用。
> - SwipeViewNested的滚动条一直处于隐藏状态，但仍保持正常的拖拽方式。

#### LottieView组件<a name="ZH-CN_TOPIC_0000002524883390"></a>

共有属性使用方法请参见“组件的共有属性”章节内容。

> ![](public_sys-resources/icon-notice.gif) **须知：**
> 使用LottieView前，需要将随SDK携带的flatbuffertool复制到“tools\\simulator”目录下。

- 选中LottieView组件，单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002524891800.png" width="36">样式图标（Feature），在展开的Feature栏中单击LottiePath右侧的图标，会弹出一个选择Json文件的界面，选择Lottie对应的json文件，单击选择后，再次打开该目录、刷新，可以看到会生成一个与json文件同名的bin文件，该文件用于被模拟器读取并展示。同时画布中会显示Lottie图标。
  
  **图 1**  特有属性配置：LottiePath-选择Json文件<a name="fig1868010119219"></a>
  <img src="figures/特有属性配置-LottiePath-选择Json文件.png" width="700" alt="特有属性配置-LottiePath-选择Json文件">
  
  **图 2**  特有属性配置：LottiePath-绘制lottie图标<a name="fig1664912213319"></a>
  <img src="figures/特有属性配置-LottiePath-绘制lottie图标.png" width="700" alt="特有属性配置-LottiePath-绘制lottie图标">
- 选中LottieView组件，通过右侧属性样式栏中的<img src="figures/zh-cn_image_0000002555811967.png" width="23">样式图标（Events），在展开的Events栏中配置回调事件。
  
  LottieView组件支持的事件有：OnClick、OnLongPress、OnDrag、OnDragStart、OnDragEnd、OnPress、OnCancel、OnRelease、OnFocus、OnBlur、OnRotate、OnRotateStart、OnRotateEnd。
  
  **图 3**  回调事件配置<a name="fig1833365416315"></a>
  <img src="figures/回调事件配置-84.png" width="700" alt="回调事件配置-84">

#### Root组件<a name="ZH-CN_TOPIC_0000002365940098"></a>

本组件为中央区域的背景画布。

**图 1**  Root组件<a name="fig1548711534020"></a>
<img src="figures/Root组件.png" width="700" alt="Root组件">

选中Root组件，在右侧展开的General栏中修改BackgroundColor属性，可以修改Root组件的颜色。

**图 2**  修改背景色<a name="fig1728445112456"></a>
<img src="figures/修改背景色-85.png" width="700" alt="修改背景色-85">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - Root组件是所有组件的父组件。
> - Root组件是唯一的、自动创建的，不可进行除改变颜色外的其他操作。

#### 用户代码编辑<a name="ZH-CN_TOPIC_0000002399739777"></a>

用户单击画布右上角“Convert to C++ files”，会在工程中生成与界面样式对应的代码。

**图 1**  生成代码按钮<a name="fig36701735185010"></a>
<img src="figures/生成代码按钮.png" width="700" alt="生成代码按钮">

以工程名为aaaa的aaaa.gui工程和Button组件为例，生成的代码目录位于工程目录下“application/wearable/nativeapp/gui工程名”，具体结构如图2所示。用户可自行在xxxPresenter.cpp文件中按照模板添加自己的回调事件，模板代码如图3所示，其余部分都是自动生成的，每次点击生成代码按钮都会刷新。

**图 2**  生成代码目录结构<a name="fig084021814113"></a>
<img src="figures/生成代码目录结构.png" width="700" alt="生成代码目录结构">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - 从其他组件切成CrossView组件，或者从CrossView组件切成其他组件，而后去生成代码时，由于CrossView组件添加子组件的特殊性，需要先手动删除当前已有的“/src/$\{AppName\}View.cpp”后，再单击转换生成代码。
> - 其余“ui\_xxView.h”格式的文件不可修改，GUI每次单击生成代码都会覆盖生成。

**图 3**  回调事件模板代码<a name="fig113311395104"></a>
<img src="figures/回调事件模板代码.png" width="700" alt="回调事件模板代码">

以click的回调事件为例，如果当前画布中只有一个组件的click回调被置为true，用户可直接在OnClick的回调函数中添加回调事件，如果当前画布中有多个组件的click回调被置为true，则用户需要按照提示来区分组件实现各自的回调，如图4所示。

**图 4**  多个组件同一回调事件代码实现<a name="fig171118207109"></a>
<img src="figures/多个组件同一回调事件代码实现.png" width="700" alt="多个组件同一回调事件代码实现">

#### 组件对齐<a name="ZH-CN_TOPIC_0000002366099998"></a>

组件对齐通过选中组件再单击界面上方工具栏中的对齐按钮<img src="figures/zh-cn_image_0000002399741701.png" width="300">实现。

对齐效果分别为左对齐<img src="figures/zh-cn_image_0000002366101870.png" width="35">，左右居中对齐<img src="figures/zh-cn_image_0000002399621797.png" width="27">，右对齐<img src="figures/zh-cn_image_0000002365942006.png" width="24">，顶部对齐<img src="figures/zh-cn_image_0000002399741709.png" width="28">，上下居中对齐<img src="figures/zh-cn_image_0000002366101906.png" width="29">和底部对齐<img src="figures/zh-cn_image_0000002399621837.png" width="33">六种。

根据选中的组件个数不同，对齐操作分为两种：

- 选中一个组件
  
  只选中一个组件时，会将被选中的组件作为需要对齐的组件，以其父组件作为参照物进行对齐。可以通过组件树（Component Tree）查看组件的父子关系。
  
  > ![](public_sys-resources/icon-note.gif) **说明：**
  > 只选中Root组件时无效。
- 选中多个组件（通过按住Ctrl键单击多个组件）
  
  当被选中的组件不止一个时，将会以最后一个被选中的组件作为参照物，其他被选中的组件作为需要对齐的组件进行对齐。
  
  > ![](public_sys-resources/icon-note.gif) **说明：**
  > 被选中的组件只能为Root组件或其直接子组件。

#### 组件层级移动<a name="ZH-CN_TOPIC_0000002399619909"></a>

组件层级移动通过选中组件再单击界面上方工具栏中的层级移动按钮实现。

**图 1**  组件层级移动<a name="fig84841225105411"></a>
<img src="figures/组件层级移动.png" width="700" alt="组件层级移动">

<img src="figures/zh-cn_image_0000002366101930.png" width="28">为前进一层，<img src="figures/zh-cn_image_0000002399621861.png" width="25">为后退一层。

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - 每次单击操作只能使组件向前/向后一层。
> - 同时选中多个组件时层级移动操作只对最后一个选中的组件生效。
> - 层级移动无法调整当前组件的父组件（如当前组件是ScrollView/SwipeView的子组件，层级移动操作无法将组件移出父组件）。
> - 组件树中越靠近顶端的组件层级越低，越靠近底部的组件层级越高。

#### 高斯模糊属性<a name="ZH-CN_TOPIC_0000002365940102"></a>

组件的高斯模糊属性通过单击右侧属性样式栏中的<img src="figures/zh-cn_image_0000002365942066.png" width="20">样式图标（General），在展开的General栏中修改组件的GaussianBlur属性以设置高斯模糊，如图1所示。

**图 1**  高斯模糊<a name="fig12812852164217"></a>
<img src="figures/高斯模糊-86.png" width="700" alt="高斯模糊-86">

组件设置高斯模糊之后，会将层级低于该组件的其他组件模糊处理。如图2所示。

**图 2**  高斯模糊效果范例<a name="fig17715201710478"></a>
<img src="figures/高斯模糊效果范例.png" width="700" alt="高斯模糊效果范例">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - 只有同时满足FullScreenState为true且GaussianBlur不为0时，高斯模糊才会生效。
> - 因渲染方式不同，GUI界面中组件渲染效果会与模拟器中实际效果存在一定差异。
> - 多个组件均设置高斯模糊效果时，仅最下层组件的高斯模糊生效。
> - 组件层级可通过组件树查看，层级移动方式和详细说明参考“组件层级移动”。
> - GaussianBlur属性决定不同程度的高斯模糊效果，合理的范围是\[0, 32\]。
> - 事件添加：当前版本在控件的Events属性中使能事件不会生成事件代码，需用户手动添加事件代码，具体添加方法是在应用代码（以应用名为gui6为例，在SDK根目录/application/wearable/nativeapp/nativeui/gui6/include/ui\_gui6View.h）的setupUi方法中，对指定控件添加所需事件，如需要给id名称为“button1”的控件添加OnLongPress事件，则在setupUi方法末尾增加“button1-\>SetOnLongPressListener\(gui6presenter\_\);”，其中“button1\_”为控件在代码中的变量名，“gui6presenter\_”为与应用名关联的在代码中自动生成的指针。

## 模拟器效果展示方法<a name="ZH-CN_TOPIC_0000002399739781"></a>

- **Demo运行**
- **调试运行**
- **经过GUI界面拖拽生成代码运行**
- **模拟器尺寸与形状设置**

### Demo运行<a name="ZH-CN_TOPIC_0000002366100002"></a>

单击图示中的“App Debug”，会启动编译调试并展示模拟器，运行默认Demo效果如图2所示。

运行过程中的日志信息记录在当前打开的工程根目录下的“simulator\_build/error.txt”文件中。

**图 1**  App Debug按钮<a name="fig599934320100"></a>
<img src="figures/App-Debug按钮.png" width="700" alt="App-Debug按钮">

**图 2**  模拟器效果展示<a name="fig771416101317"></a>
<img src="figures/模拟器效果展示.png" width="700" alt="模拟器效果展示">

### 调试运行<a name="ZH-CN_TOPIC_0000002399619913"></a>

单击“App Debug”按钮后，会自动启动编译和调试功能，用户可自行添加断点并进行调试。调试功能请参见“工程调试”章节。

**图 1**  调试功能展示<a name="fig28521758191013"></a>
<img src="figures/调试功能展示.png" width="700" alt="调试功能展示">

### 经过GUI界面拖拽生成代码运行<a name="ZH-CN_TOPIC_0000002365940106"></a>

用户在画布中添加组件、更改属性或使能回调后，启动模拟器，即可按照图1操作看到画布中添加的功能列表。

**图 1**  从模拟器查看添加的组件功能<a name="fig18197143416124"></a>
<img src="figures/从模拟器查看添加的组件功能.png" width="700" alt="从模拟器查看添加的组件功能">

> ![](public_sys-resources/icon-note.gif) **说明：**
> 当模拟器显示主界面时，单击右上角按钮，会自动跳到功能列表页面，列表最前面即为添加组件列表；当模拟器显示非主界面时，单击右上角按钮，会自动跳到主界面。

从图中可以看出，当前添加的功能名称默认为创建的GUI工程名称，图标默认为“示例”图标。如果想要修改模拟器中GUI工程的图标和名称，可以在对应GUI工程的生成代码目录下的“Register.cpp”文件中进行修改。

**图 2**  修改默认GUI工程名称和列表前的图标<a name="fig21411933131311"></a>
<img src="figures/修改默认GUI工程名称和列表前的图标.png" width="700" alt="修改默认GUI工程名称和列表前的图标">

在对应的cpp文件中，修改这四个参数可以修改模拟器中对应组件的图标或者名称。

①：桌面中选择图标模式（默认模式）对应的图标显示，格式为.bin的文件。

②：桌面中选择蜂窝模式对应的图标显示，格式为.bin的文件。

③：桌面中对应足球模式对应的图标显示，格式为.bin的文件。

④：组件对应的名称。

对于默认选择的图标模式，只需要更改①和④中的参数，即可修改模拟器中组件对应的图标和组件名。

### 模拟器尺寸与形状设置<a name="ZH-CN_TOPIC_0000002399739785"></a>

用户创建GUI工程后，若需重新改变画布或模拟器的尺寸与形状，可以在工程中搜索“graphic\_config.h”文件，该文件位于工程目录下的“application/brandy/simulator/libui”目录下，更改HORIZONTAL\_RESOLUTION、VERTICAL\_RESOLUTION以及DEVICE\_SHAPE这三个参数，分别对应模拟器的直径（长宽）以及模拟器的形状。

**图 1**  更改模拟器的尺寸与形状<a name="fig5357185214137"></a>
<img src="figures/更改模拟器的尺寸与形状.png" width="700" alt="更改模拟器的尺寸与形状">

## 图形工具介绍<a name="ZH-CN_TOPIC_0000002366100006"></a>

单击插件页面或者状态栏中的“Graphics Tools”图标。

**图 1**  插件页面或者状态栏中的“Graphics Tools”图标<a name="fig135900300384"></a>

<img src="figures/zh-cn_image_0000002404301777.png" width="700">

弹出功能页签，当前包含“图片解压缩”和“视频首帧提取”两个功能。

**图 2**  “GUI图形工具”页签<a name="fig5647114212381"></a>

<img src="figures/zh-cn_image_0000002488087200.png" width="700">

- **环境配置**
- **图片解压缩工具**
- **视频首帧提取工具**
- **3D建模工具**
- **蒙版工具**

### 环境配置<a name="ZH-CN_TOPIC_0000002399619917"></a>

在使用“Graphics Tools”图形工具功能之前，需要配置Python环境，请参考“工具链配置”。

Python安装成功后需要安装pip的依赖模块。

- opencv模块
- numpy模块
- Pillow模块
- ffmpeg-python

Python环境可以通过在选择的工具链下载目录中tools目录下的python目录如“D:\\toolchain\\tools\\python”路径下打开cmd（命令提示符）窗口中进行验证。执行“.\\python.exe --version”输出结果为‘Python 3.11.4’，以及执行“python.exe ..\\..\\downloads\\pip.pyz list”命令输出结果显示有opencv-python、numpy以及pillow这些pip.pyz的依赖及其对应版本，则说明Python环境配置正确。

**图 1**  Python环境验证<a name="fig17189163271318"></a>
<img src="figures/python环境验证-87.png" width="700" alt="python环境验证-87">

### 图片解压缩工具<a name="ZH-CN_TOPIC_0000002365940110"></a>

图片解压缩工具的功能：依据xml文件配置的压缩算法以及像素格式信息，将指定目录下的每一个子目录都打包成一个资源文件并且生成资源索引文件“ImageResources.cpp”；或者将指定目录下的每一个图片文件都打包成一个资源文件。解压模式下会将.bin文件的图片解压为.png格式的图片；压缩模式下会将.png、.jpg或者.bmp文件格式的图片压缩为.bin文件格式。

**图 1**  图片解压缩界面<a name="fig2012249183513"></a>
<img src="figures/图片解压缩界面.png" width="700" alt="图片解压缩界面">

- 脚本路径：需选择“images\_convert.py”脚本所在的文件夹路径，一般为“graphic\_tools\\image\_converter\_tool”。
- 解/压缩模式：指定解压、压缩模式，默认选择压缩模式。
- 打包方式：分为文件打包方式和文件夹打包方式，默认选择文件打包方式。
  
  - 文件打包方式：以每个图片为单位进行打包。
  - 文件夹打包方式：将图片进行打包后，再以子文件夹为单位将其包含的已打包图片一并打包。
- 压缩模式：默认为手动压缩模式。
- alpha压缩模式：0：非压缩；1：压缩。默认为1：压缩模式。
- rgb压缩模式：0：非压缩；1：压缩。默认为1：压缩模式。
- 压缩单元宽度：4--4x4；6--6x4；8--8x4；16--16x4，也叫压缩倍率，在ARGB8888格式时分别对应4倍压缩、6倍压缩、8倍压缩、16倍压缩；在RGB888时分别对应3倍压缩，4.5倍压缩，6倍压缩，12倍压缩；在RGB565时分别对应2倍压缩，3倍压缩，4倍压缩，8倍压缩。默认为6倍压缩。
- 图片输入路径：指定待处理图片的根目录，图片目录下的图片可以为“.jpg、.png、.bmp”等格式。
- 图片输出路径：指定输出的文件夹。

> ![](public_sys-resources/icon-notice.gif) **须知：**
> 
> - 图片压缩仅支持.png、.bmp、.jpg文件的压缩，且图片分辨率大小不能超过1280\*800（宽\*高）。
> - 子文件夹名以及文件名必须为数字以及字母的组合。
> - 解/压缩模式选择为解压模式时，压缩模式、alpha压缩模式、rgb压缩模式和压缩单元宽度选项隐藏。
> - alpha和rgb同时为0是非压缩模式请注意适配。
> - 压缩倍率是与裸数据大小进行对比，而非与源文件大小对比，例如466×466的RGBA图片，无压缩大小为848KB，6倍压缩时为142KB，16倍压缩时大约为54KB。
> - 压缩操作为有损压缩，压缩倍率越大，图损越大。
> - 图片输出路径建议与图片输入路径保持一致，否则执行功能时会删除“图片输出路径”的文件夹，再自动创建一个同名文件夹保存输出的图片。

**压缩模式样例<a name="section19664105194515"></a>**

**图 2**  图片压缩具体选择样例<a name="fig611581334117"></a>
<img src="figures/图片压缩具体选择样例.png" width="700" alt="图片压缩具体选择样例">

各选项均选择后，单击完成，执行压缩功能，终端界面输出显示内容如下：

**图 3**  图片压缩执行时终端界面输出<a name="fig753304214242"></a>
<img src="figures/图片压缩执行时终端界面输出.png" width="700" alt="图片压缩执行时终端界面输出">

“Conversion Success!”说明压缩成功。图片所在文件夹前后对比如下，成功将两张png格式图片转换成bin文件格式：

**图 4**  图片压缩前后效果展示<a name="fig954913529412"></a>
<img src="figures/图片压缩前后效果展示.png" width="700" alt="图片压缩前后效果展示">

<img src="figures/zh-cn_image_0000002366102030.png" width="500">

**解压模式样例<a name="section15836552114418"></a>**

将解/压缩模式选择为解压模式，压缩模式、alpha压缩模式、rgb压缩模式和压缩单元宽度选项隐藏。

**图 5**  图片解压具体选择样例<a name="fig7235124264318"></a>
<img src="figures/图片解压具体选择样例.png" width="700" alt="图片解压具体选择样例">

各选项均选择后，单击完成，执行解压功能，终端界面输出显示内容如下：

**图 6**  图片解压执行时终端界面输出<a name="fig188091156104319"></a>
<img src="figures/图片解压执行时终端界面输出.png" width="700" alt="图片解压执行时终端界面输出">

“Decmpress Success”说明解压成功。图片所在文件夹前后对比如下，成功将两张bin格式图片解压为png文件格式：

**图 7**  图片解压前后对比<a name="fig5433744191910"></a>
<img src="figures/图片解压前后对比.png" width="700" alt="图片解压前后对比">

<img src="figures/zh-cn_image_0000002366102034.png" width="500">

### 视频首帧提取工具<a name="ZH-CN_TOPIC_0000002399739789"></a>

视频在未加载前，首帧为黑帧，需要用户手动设置一张起始预览图，该功能提供提取视频首帧并将其转换为BMP格式图片的能力。

**图 1**  视频首帧提取工具页面<a name="fig111561422134515"></a>
<img src="figures/视频首帧提取工具页面.png" width="700" alt="视频首帧提取工具页面">

- 脚本路径：需选择“convert\_2\_bmp.py”脚本所在的文件夹路径，一般为“graphic\_tools\\convert\_2\_bmp”。
- 视频路径：需要提取的视频文件路径。
- 视频帧宽/视频帧高：指定视频帧的宽高，需要与视频资源一致，且需要用户手动输入。
- 图片生成路径：图片生成路径。

> ![](public_sys-resources/icon-caution.gif) **注意：**
> 
> - 视频格式当前仅支持YUV444和YUV420这两种。
> - 视频仅支持MJPEG编码及.mp4封装，调整视频格式及大小可借助ffmpeg工具， 例："ffmpeg -i video.mp4 -vf crop=454:454 -b:v 4228k -vcodec mjpeg out.mp4"。
> - 视频帧宽/高可以在视频的属性--\>详细信息中查看。如果视频帧宽高与实际不一致，生成的图片会产生花屏。

视频首帧提取样例：

**图 2**  视频首帧提取具体选项样例<a name="fig28171353184518"></a>
<img src="figures/视频首帧提取具体选项样例.png" width="700" alt="视频首帧提取具体选项样例">

各选项均选择后，单击完成，执行视频首帧提取功能，终端界面输出显示内容如下：

**图 3**  视频首帧提取执行时终端界面输出<a name="fig1474664164615"></a>
<img src="figures/视频首帧提取执行时终端界面输出.png" width="700" alt="视频首帧提取执行时终端界面输出">

“success”说明提取成功。视频所在文件夹前后对比如下，成功将mp4格式视频的首帧提取出来并保存为bmp格式的图片：

**图 4**  适配转换前后对比<a name="fig7243352114711"></a>
<img src="figures/适配转换前后对比.png" width="700" alt="适配转换前后对比">

<img src="figures/zh-cn_image_0000002399621957.png" width="500">

如果执行功能时终端中提示“未检测到 FFmpeg”，如图5所示。

**图 5**  未安装ffmepg提示信息<a name="fig1170385182419"></a>
<img src="figures/未安装ffmepg提示信息.png" width="700" alt="未安装ffmepg提示信息">

需要手动下载安装[ffmepg工具](https://www.gyan.dev/ffmpeg/builds/)并添加至环境变量中，然后关闭所有的VS Code工具，再次打开执行视频首帧提取功能。

**图 6**  ffmpeg下载<a name="fig184311426162816"></a>
<img src="figures/ffmpeg下载.png" width="700" alt="ffmpeg下载">

配置环境变量：编辑系统环境变量-\>环境变量-\>Path-\>将解压后的绝对路径“\\ffmpeg-7.1.1-full\_build-shared\\bin”添加到环境变量中。

### 3D建模工具<a name="ZH-CN_TOPIC_0000002366100010"></a>

- **使用方法**
- **参数表**

#### 使用方法<a name="ZH-CN_TOPIC_0000002399619921"></a>

**运行<a name="section1992610916599"></a>**

单击COMMANDS里面的Modeling Tools，在上方选择“3D Modeling Tool”。

<img src="figures/zh-cn_image_0000002456945685.png" width="700">

**界面介绍<a name="section114256515596"></a>**

- **工具启动主界面**
  
  启动后工具主界面如图所示：
  
  **图 1**  3D建模工具启动主界面<a name="fig14212923556"></a>
  <img src="figures/3D建模工具启动主界面.png" width="700" alt="3D建模工具启动主界面">
  
  按钮功能介绍如下：
  
  - 模型类型：选择当前要使用的模型（球面：sphere；网格：mesh；平面：cylinder），会自动加载默认参数并运行。
  - 纹理导入：选择纹理素材文件。
  - 模型导入：选择模型文件。
  - 模型创建：打开子界面，进行模型参数配置并生成模型。
  - 模型导出：将工具中对应模式下生成的模型文件导出到本地。
  - 运行：运行对应模式下的用例并显示运行效果。
  - 默认参数：一键恢复所有参数到其默认值。
  - 参数导出：根据当前界面配置的模式，调整参数，获得理想效果后，可将该模式下的该组参数保存到一个文件中。
  - 显示日志/隐藏日志：打开/关闭下方的日志框。
- **模型创建界面**
  
  ①mesh界面：在模型选择中选择网格后，单击模型创建，弹出如图所示界面：
  
  **图 2**  mesh界面<a name="fig179479235556"></a>
  <img src="figures/mesh界面.png" width="700" alt="mesh界面">
  
  ②cylinder界面：在模型选择中选择平面后，单击模型创建，弹出如图所示界面：
  
  **图 3**  cylinder界面<a name="fig1460543113556"></a>
  <img src="figures/cylinder界面.png" width="700" alt="cylinder界面">
  
  ③sphere界面：在模型选择中选择球面后，单击模型创建，弹出如图所示界面：
  
  **图 4**  sphere界面<a name="fig18331542125514"></a>
  <img src="figures/sphere界面.png" width="700" alt="sphere界面">
  
  单击保存按钮保存配置值，当单击完保存后，子窗体自动关闭。

**操作步骤<a name="section1371818221405"></a>**

- **mesh模式：**
  
  mesh分为7种子模式，其中type0和type6属于强交互模式，模型在线生成，无需离线生成和载入；type1～type5属于弱交互模式，运行用例之前需要离线生成模型或者载入模型，二者选一即可。
  
  1. 单击主界面网格单选按钮，选中网格模式。
  2. 单击主界面模型创建按钮，进行模型参数设置。
  3. 单击type输入框进行子模式选择，并进行相关参数配置：
     
     type0：thr建议配置为1.0，len建议配置为5，cx、cy建议配置在非中心点位置，其他参数建议保持默认值
     
     type1：len建议配置为0.0，cx、cy只支持配置在中心点，其他参数建议保持默认值
     
     type2：len建议配置为4.52，cx、cy只支持配置在中心点，其他参数建议保持默认值
     
     type3：len建议配置为1，cx、cy只支持配置在中心点，其他参数建议保持默认值
     
     type4：len建议配置为3，cx、cy只支持配置在中心点，其他参数建议保持默认值
     
     type5：len建议配置为9.33，cx、cy只支持配置在中心点，其他参数建议保持默认值
     
     type6：thr建议配置为0.1，其他参数建议保持默认值
  4. 单击保存按钮保存，并返回主界面。
  5. 单击主界面纹理导入按钮选择纹理素材。
  6. 单击运行按钮。
  7. 拖动右侧mesh\_ctrl滑块，观察效果。
- **cylinder模式：**
  
  1. 单击主界面平面单选按钮，选中cylinder模式。
  2. 单击主界面模型创建按钮，进行模型参数设置，建议使用默认值。
  3. 单击保存按钮保存，并返回主界面。
  4. 单击纹理导入按钮选择纹理素材。
  5. 单击运行按钮。
  6. 拖动右侧cylinder\_ctrl滑块（主要滑动的是cylinder\_ctl），观察效果。
- **sphere模式：**
  
  1. 单击主界面球面单选按钮，选中sphere模式。
  2. 单击主界面模型创建按钮，进行模型参数设置，建议使用默认值。
  3. 单击保存按钮保存，并返回主界面。
  4. 单击纹理导入按钮选择纹理素材。
  5. 单击运行按钮。
  6. 拖动右侧angel\_x、angel\_y、angel\_z三个滑块，观察效果。

**素材默认路径<a name="section88057444012"></a>**

> ![](public_sys-resources/icon-notice.gif) **须知：**
> 以下路径不能删除，否则会导致程序运行错误。

- 默认纹理素材路径：C:/Users/$\{用户名\}/.vscode/extensions/hispark.hisparkstudio-$\{版本号\}/dist/source\_file/texture/
- 默认模型素材路径：C:/Users/$\{用户名\}/.vscode/extensions/hispark.hisparkstudio-$\{版本号\}/dist/source\_file/model/，即模型创建完成之后的默认保存路径
- 默认蒙版素材路径：C:/Users/$\{用户名\}/.vscode/extensions/hispark.hisparkstudio-$\{版本号\}/dist/source\_file/mask/，此路径仅供sphere模式使用，目的是使纹理与模型叠加时在连接处能够保持平滑过渡；生成的球蒙版宽高要和dst\_width/dst\_height一致，且球蒙版为ARGB8888。
- 其中$\{用户名\}为当前Windows登录账号用户名，$\{版本号\}为本插件版本号。

#### 参数表<a name="ZH-CN_TOPIC_0000002365940114"></a>

**滚筒日历<a name="section19555544673"></a>**

**表 1**  滚筒日历模型参数

<a name="table9440155420336"></a>

<table><thead align="left"><tr id="row2587354183315"><th class="cellrowborder" valign="top" width="12.98%" id="mcps1.2.5.1.1"><p id="p165871154173319"><a name="p165871154173319"></a><a name="p165871154173319"></a>序号</p>
</th>
<th class="cellrowborder" valign="top" width="24.2%" id="mcps1.2.5.1.2"><p id="p18587205433319"><a name="p18587205433319"></a><a name="p18587205433319"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="26.3%" id="mcps1.2.5.1.3"><p id="p558755412337"><a name="p558755412337"></a><a name="p558755412337"></a>参数含义</p>
</th>
<th class="cellrowborder" valign="top" width="36.52%" id="mcps1.2.5.1.4"><p id="p6587155473316"><a name="p6587155473316"></a><a name="p6587155473316"></a>范围&demo参数</p>
</th>
</tr>
</thead>
<tbody><tr id="row12587205417331"><td class="cellrowborder" valign="top" width="12.98%" headers="mcps1.2.5.1.1 "><p id="p135878543337"><a name="p135878543337"></a><a name="p135878543337"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="24.2%" headers="mcps1.2.5.1.2 "><p id="p15587654143310"><a name="p15587654143310"></a><a name="p15587654143310"></a>dst_width</p>
</td>
<td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.2.5.1.3 "><p id="p4587145443312"><a name="p4587145443312"></a><a name="p4587145443312"></a>屏幕的宽</p>
</td>
<td class="cellrowborder" valign="top" width="36.52%" headers="mcps1.2.5.1.4 "><p id="p11587195473319"><a name="p11587195473319"></a><a name="p11587195473319"></a>-</p>
</td>
</tr>
<tr id="row1758785413339"><td class="cellrowborder" valign="top" width="12.98%" headers="mcps1.2.5.1.1 "><p id="p1558714540336"><a name="p1558714540336"></a><a name="p1558714540336"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="24.2%" headers="mcps1.2.5.1.2 "><p id="p2587185415330"><a name="p2587185415330"></a><a name="p2587185415330"></a>dst_height</p>
</td>
<td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.2.5.1.3 "><p id="p18587125412333"><a name="p18587125412333"></a><a name="p18587125412333"></a>屏幕的高</p>
</td>
<td class="cellrowborder" valign="top" width="36.52%" headers="mcps1.2.5.1.4 "><p id="p55871054173311"><a name="p55871054173311"></a><a name="p55871054173311"></a>-</p>
</td>
</tr>
<tr id="row3587105412330"><td class="cellrowborder" valign="top" width="12.98%" headers="mcps1.2.5.1.1 "><p id="p1058712545336"><a name="p1058712545336"></a><a name="p1058712545336"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="24.2%" headers="mcps1.2.5.1.2 "><p id="p1587115423310"><a name="p1587115423310"></a><a name="p1587115423310"></a>cylinder_radius</p>
</td>
<td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.2.5.1.3 "><p id="p758725417337"><a name="p758725417337"></a><a name="p758725417337"></a>圆柱体半径</p>
</td>
<td class="cellrowborder" valign="top" width="36.52%" headers="mcps1.2.5.1.4 "><p id="p1588195410337"><a name="p1588195410337"></a><a name="p1588195410337"></a>(0,cylinder_dst_width]; 230</p>
</td>
</tr>
<tr id="row1858865413338"><td class="cellrowborder" valign="top" width="12.98%" headers="mcps1.2.5.1.1 "><p id="p25882544336"><a name="p25882544336"></a><a name="p25882544336"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="24.2%" headers="mcps1.2.5.1.2 "><p id="p155881854133320"><a name="p155881854133320"></a><a name="p155881854133320"></a>cylinder_width</p>
</td>
<td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.2.5.1.3 "><p id="p658815544331"><a name="p658815544331"></a><a name="p658815544331"></a>圆柱体宽</p>
</td>
<td class="cellrowborder" valign="top" width="36.52%" headers="mcps1.2.5.1.4 "><p id="p1588145410335"><a name="p1588145410335"></a><a name="p1588145410335"></a>(0,cylinder_dst_height]; 230</p>
</td>
</tr>
<tr id="row8588654143319"><td class="cellrowborder" valign="top" width="12.98%" headers="mcps1.2.5.1.1 "><p id="p16588754123314"><a name="p16588754123314"></a><a name="p16588754123314"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="24.2%" headers="mcps1.2.5.1.2 "><p id="p1958819542339"><a name="p1958819542339"></a><a name="p1958819542339"></a>cylinder_height</p>
</td>
<td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.2.5.1.3 "><p id="p1458845493318"><a name="p1458845493318"></a><a name="p1458845493318"></a>圆柱体高</p>
</td>
<td class="cellrowborder" valign="top" width="36.52%" headers="mcps1.2.5.1.4 "><p id="p1558855403315"><a name="p1558855403315"></a><a name="p1558855403315"></a>(0,cylinder_dst_height]; 230</p>
</td>
</tr>
<tr id="row65881054173318"><td class="cellrowborder" valign="top" width="12.98%" headers="mcps1.2.5.1.1 "><p id="p458895415338"><a name="p458895415338"></a><a name="p458895415338"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="24.2%" headers="mcps1.2.5.1.2 "><p id="p1258865418336"><a name="p1258865418336"></a><a name="p1258865418336"></a>cylinder_x_step</p>
</td>
<td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.2.5.1.3 "><p id="p105881954173311"><a name="p105881954173311"></a><a name="p105881954173311"></a>模型x方向采样步长</p>
</td>
<td class="cellrowborder" valign="top" width="36.52%" headers="mcps1.2.5.1.4 "><p id="p058818544336"><a name="p058818544336"></a><a name="p058818544336"></a>[4 ,8];4</p>
</td>
</tr>
<tr id="row8588135415332"><td class="cellrowborder" valign="top" width="12.98%" headers="mcps1.2.5.1.1 "><p id="p3588754193314"><a name="p3588754193314"></a><a name="p3588754193314"></a>7</p>
</td>
<td class="cellrowborder" valign="top" width="24.2%" headers="mcps1.2.5.1.2 "><p id="p158845483312"><a name="p158845483312"></a><a name="p158845483312"></a>cylinder_y_step</p>
</td>
<td class="cellrowborder" valign="top" width="26.3%" headers="mcps1.2.5.1.3 "><p id="p958814542335"><a name="p958814542335"></a><a name="p958814542335"></a>模型y方向采样步长</p>
</td>
<td class="cellrowborder" valign="top" width="36.52%" headers="mcps1.2.5.1.4 "><p id="p6588254103318"><a name="p6588254103318"></a><a name="p6588254103318"></a>[4 , 8];4</p>
</td>
</tr>
</tbody>
</table>

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - cylinder\_x\_step：逻辑约束cylinder\_dst\_width / cylinder\_x\_step
> - cylinder\_y\_step：逻辑约束cylinder\_dst\_height /cylinder\_y\_step
>   注意：对于cylinder模式的素材，建议宽为屏幕的1/2，高至少为2048。

**表 2**  滚筒日历demo参数

<a name="table13448145419338"></a>

<table><thead align="left"><tr id="row185881054183311"><th class="cellrowborder" valign="top" width="14.41%" id="mcps1.2.5.1.1"><p id="p35881654133318"><a name="p35881654133318"></a><a name="p35881654133318"></a>序号</p>
</th>
<th class="cellrowborder" valign="top" width="23.16%" id="mcps1.2.5.1.2"><p id="p18588115433315"><a name="p18588115433315"></a><a name="p18588115433315"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="26.040000000000003%" id="mcps1.2.5.1.3"><p id="p135881654163320"><a name="p135881654163320"></a><a name="p135881654163320"></a>参数含义</p>
</th>
<th class="cellrowborder" valign="top" width="36.39%" id="mcps1.2.5.1.4"><p id="p175881154133315"><a name="p175881154133315"></a><a name="p175881154133315"></a>范围&demo参数</p>
</th>
</tr>
</thead>
<tbody><tr id="row16588205417333"><td class="cellrowborder" valign="top" width="14.41%" headers="mcps1.2.5.1.1 "><p id="p1158825411336"><a name="p1158825411336"></a><a name="p1158825411336"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="23.16%" headers="mcps1.2.5.1.2 "><p id="p15588175415335"><a name="p15588175415335"></a><a name="p15588175415335"></a>angle</p>
</td>
<td class="cellrowborder" valign="top" width="26.040000000000003%" headers="mcps1.2.5.1.3 "><p id="p13588175415334"><a name="p13588175415334"></a><a name="p13588175415334"></a>旋转角度</p>
</td>
<td class="cellrowborder" valign="top" width="36.39%" headers="mcps1.2.5.1.4 "><p id="p1658865413318"><a name="p1658865413318"></a><a name="p1658865413318"></a>[-180,180]； 0</p>
</td>
</tr>
<tr id="row8588654173319"><td class="cellrowborder" valign="top" width="14.41%" headers="mcps1.2.5.1.1 "><p id="p3588105415334"><a name="p3588105415334"></a><a name="p3588105415334"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="23.16%" headers="mcps1.2.5.1.2 "><p id="p758811546331"><a name="p758811546331"></a><a name="p758811546331"></a>v_fov</p>
</td>
<td class="cellrowborder" valign="top" width="26.040000000000003%" headers="mcps1.2.5.1.3 "><p id="p115881554113315"><a name="p115881554113315"></a><a name="p115881554113315"></a>视口</p>
</td>
<td class="cellrowborder" valign="top" width="36.39%" headers="mcps1.2.5.1.4 "><p id="p10588454163311"><a name="p10588454163311"></a><a name="p10588454163311"></a>[1,200]； 102.674</p>
</td>
</tr>
<tr id="row15588125419334"><td class="cellrowborder" valign="top" width="14.41%" headers="mcps1.2.5.1.1 "><p id="p195891054143319"><a name="p195891054143319"></a><a name="p195891054143319"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="23.16%" headers="mcps1.2.5.1.2 "><p id="p758985453318"><a name="p758985453318"></a><a name="p758985453318"></a>aspect_ratio</p>
</td>
<td class="cellrowborder" valign="top" width="26.040000000000003%" headers="mcps1.2.5.1.3 "><p id="p1958925453315"><a name="p1958925453315"></a><a name="p1958925453315"></a>屏幕长宽比</p>
</td>
<td class="cellrowborder" valign="top" width="36.39%" headers="mcps1.2.5.1.4 "><p id="p1758915548334"><a name="p1758915548334"></a><a name="p1758915548334"></a>(0,2]； 0.46</p>
</td>
</tr>
<tr id="row12589854183313"><td class="cellrowborder" valign="top" width="14.41%" headers="mcps1.2.5.1.1 "><p id="p185890543339"><a name="p185890543339"></a><a name="p185890543339"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="23.16%" headers="mcps1.2.5.1.2 "><p id="p1058920549333"><a name="p1058920549333"></a><a name="p1058920549333"></a>z_near</p>
</td>
<td class="cellrowborder" valign="top" width="26.040000000000003%" headers="mcps1.2.5.1.3 "><p id="p16589195414334"><a name="p16589195414334"></a><a name="p16589195414334"></a>近平面</p>
</td>
<td class="cellrowborder" valign="top" width="36.39%" headers="mcps1.2.5.1.4 "><p id="p8589175433317"><a name="p8589175433317"></a><a name="p8589175433317"></a>[-400,400]； -10</p>
</td>
</tr>
<tr id="row75891854203313"><td class="cellrowborder" valign="top" width="14.41%" headers="mcps1.2.5.1.1 "><p id="p1758914549336"><a name="p1758914549336"></a><a name="p1758914549336"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="23.16%" headers="mcps1.2.5.1.2 "><p id="p10589155423314"><a name="p10589155423314"></a><a name="p10589155423314"></a>z_far</p>
</td>
<td class="cellrowborder" valign="top" width="26.040000000000003%" headers="mcps1.2.5.1.3 "><p id="p1858925411336"><a name="p1858925411336"></a><a name="p1858925411336"></a>远平面</p>
</td>
<td class="cellrowborder" valign="top" width="36.39%" headers="mcps1.2.5.1.4 "><p id="p16589754193318"><a name="p16589754193318"></a><a name="p16589754193318"></a>[-400,400]； -70</p>
</td>
</tr>
<tr id="row14589205453312"><td class="cellrowborder" valign="top" width="14.41%" headers="mcps1.2.5.1.1 "><p id="p75891054113312"><a name="p75891054113312"></a><a name="p75891054113312"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="23.16%" headers="mcps1.2.5.1.2 "><p id="p7589135403316"><a name="p7589135403316"></a><a name="p7589135403316"></a>axis_x</p>
</td>
<td class="cellrowborder" valign="top" width="26.040000000000003%" headers="mcps1.2.5.1.3 "><p id="p5589155423311"><a name="p5589155423311"></a><a name="p5589155423311"></a>三维空间的旋转轴x</p>
</td>
<td class="cellrowborder" valign="top" width="36.39%" headers="mcps1.2.5.1.4 "><p id="p115891754113319"><a name="p115891754113319"></a><a name="p115891754113319"></a>[-1,1]； 1</p>
</td>
</tr>
<tr id="row2589135433312"><td class="cellrowborder" valign="top" width="14.41%" headers="mcps1.2.5.1.1 "><p id="p35895542331"><a name="p35895542331"></a><a name="p35895542331"></a>7</p>
</td>
<td class="cellrowborder" valign="top" width="23.16%" headers="mcps1.2.5.1.2 "><p id="p1058985414334"><a name="p1058985414334"></a><a name="p1058985414334"></a>axis_y</p>
</td>
<td class="cellrowborder" valign="top" width="26.040000000000003%" headers="mcps1.2.5.1.3 "><p id="p4589175418339"><a name="p4589175418339"></a><a name="p4589175418339"></a>三维空间的旋转轴y</p>
</td>
<td class="cellrowborder" valign="top" width="36.39%" headers="mcps1.2.5.1.4 "><p id="p19589854203319"><a name="p19589854203319"></a><a name="p19589854203319"></a>[-1,1]； 0</p>
</td>
</tr>
<tr id="row1158985493315"><td class="cellrowborder" valign="top" width="14.41%" headers="mcps1.2.5.1.1 "><p id="p11589115493317"><a name="p11589115493317"></a><a name="p11589115493317"></a>8</p>
</td>
<td class="cellrowborder" valign="top" width="23.16%" headers="mcps1.2.5.1.2 "><p id="p1458917549335"><a name="p1458917549335"></a><a name="p1458917549335"></a>axis_z</p>
</td>
<td class="cellrowborder" valign="top" width="26.040000000000003%" headers="mcps1.2.5.1.3 "><p id="p13589125413331"><a name="p13589125413331"></a><a name="p13589125413331"></a>三维空间的旋转轴z</p>
</td>
<td class="cellrowborder" valign="top" width="36.39%" headers="mcps1.2.5.1.4 "><p id="p658910548333"><a name="p658910548333"></a><a name="p658910548333"></a>[-1,1]； 0</p>
</td>
</tr>
<tr id="row165892544337"><td class="cellrowborder" valign="top" width="14.41%" headers="mcps1.2.5.1.1 "><p id="p1458985414333"><a name="p1458985414333"></a><a name="p1458985414333"></a>9</p>
</td>
<td class="cellrowborder" valign="top" width="23.16%" headers="mcps1.2.5.1.2 "><p id="p125892545332"><a name="p125892545332"></a><a name="p125892545332"></a>scale_x</p>
</td>
<td class="cellrowborder" valign="top" width="26.040000000000003%" headers="mcps1.2.5.1.3 "><p id="p18589155411334"><a name="p18589155411334"></a><a name="p18589155411334"></a>沿x轴缩放系数</p>
</td>
<td class="cellrowborder" valign="top" width="36.39%" headers="mcps1.2.5.1.4 "><p id="p155891754193314"><a name="p155891754193314"></a><a name="p155891754193314"></a>(0,10]；1</p>
</td>
</tr>
<tr id="row1358917549338"><td class="cellrowborder" valign="top" width="14.41%" headers="mcps1.2.5.1.1 "><p id="p13589125493313"><a name="p13589125493313"></a><a name="p13589125493313"></a>10</p>
</td>
<td class="cellrowborder" valign="top" width="23.16%" headers="mcps1.2.5.1.2 "><p id="p1558913549335"><a name="p1558913549335"></a><a name="p1558913549335"></a>scale_y</p>
</td>
<td class="cellrowborder" valign="top" width="26.040000000000003%" headers="mcps1.2.5.1.3 "><p id="p8589654153310"><a name="p8589654153310"></a><a name="p8589654153310"></a>沿y轴缩放系数</p>
</td>
<td class="cellrowborder" valign="top" width="36.39%" headers="mcps1.2.5.1.4 "><p id="p958965423314"><a name="p958965423314"></a><a name="p958965423314"></a>(0,10]；1</p>
</td>
</tr>
<tr id="row558995412336"><td class="cellrowborder" valign="top" width="14.41%" headers="mcps1.2.5.1.1 "><p id="p8589105423315"><a name="p8589105423315"></a><a name="p8589105423315"></a>11</p>
</td>
<td class="cellrowborder" valign="top" width="23.16%" headers="mcps1.2.5.1.2 "><p id="p1758925412330"><a name="p1758925412330"></a><a name="p1758925412330"></a>scale_z</p>
</td>
<td class="cellrowborder" valign="top" width="26.040000000000003%" headers="mcps1.2.5.1.3 "><p id="p155901654143315"><a name="p155901654143315"></a><a name="p155901654143315"></a>沿z轴缩放系数</p>
</td>
<td class="cellrowborder" valign="top" width="36.39%" headers="mcps1.2.5.1.4 "><p id="p19590145463313"><a name="p19590145463313"></a><a name="p19590145463313"></a>(0,10]；1</p>
</td>
</tr>
<tr id="row0590054193316"><td class="cellrowborder" valign="top" width="14.41%" headers="mcps1.2.5.1.1 "><p id="p85901854113310"><a name="p85901854113310"></a><a name="p85901854113310"></a>12</p>
</td>
<td class="cellrowborder" valign="top" width="23.16%" headers="mcps1.2.5.1.2 "><p id="p175901542333"><a name="p175901542333"></a><a name="p175901542333"></a>translation_x</p>
</td>
<td class="cellrowborder" valign="top" width="26.040000000000003%" headers="mcps1.2.5.1.3 "><p id="p9590454133312"><a name="p9590454133312"></a><a name="p9590454133312"></a>沿x轴平移系数</p>
</td>
<td class="cellrowborder" valign="top" width="36.39%" headers="mcps1.2.5.1.4 "><p id="p459035415339"><a name="p459035415339"></a><a name="p459035415339"></a>[-400, 400]; 0</p>
</td>
</tr>
<tr id="row19590115412339"><td class="cellrowborder" valign="top" width="14.41%" headers="mcps1.2.5.1.1 "><p id="p1559016549337"><a name="p1559016549337"></a><a name="p1559016549337"></a>13</p>
</td>
<td class="cellrowborder" valign="top" width="23.16%" headers="mcps1.2.5.1.2 "><p id="p2590135473316"><a name="p2590135473316"></a><a name="p2590135473316"></a>translation_y</p>
</td>
<td class="cellrowborder" valign="top" width="26.040000000000003%" headers="mcps1.2.5.1.3 "><p id="p12590155418336"><a name="p12590155418336"></a><a name="p12590155418336"></a>沿y轴平移系数</p>
</td>
<td class="cellrowborder" valign="top" width="36.39%" headers="mcps1.2.5.1.4 "><p id="p1159085413333"><a name="p1159085413333"></a><a name="p1159085413333"></a>[-400, 400]; 0</p>
</td>
</tr>
<tr id="row0590135493314"><td class="cellrowborder" valign="top" width="14.41%" headers="mcps1.2.5.1.1 "><p id="p1759005417330"><a name="p1759005417330"></a><a name="p1759005417330"></a>14</p>
</td>
<td class="cellrowborder" valign="top" width="23.16%" headers="mcps1.2.5.1.2 "><p id="p1059095483312"><a name="p1059095483312"></a><a name="p1059095483312"></a>translation_z</p>
</td>
<td class="cellrowborder" valign="top" width="26.040000000000003%" headers="mcps1.2.5.1.3 "><p id="p17590854103310"><a name="p17590854103310"></a><a name="p17590854103310"></a>沿z轴平移系数</p>
</td>
<td class="cellrowborder" valign="top" width="36.39%" headers="mcps1.2.5.1.4 "><p id="p35901954173320"><a name="p35901954173320"></a><a name="p35901954173320"></a>[-400, 400]; 0</p>
</td>
</tr>
<tr id="row20590185463311"><td class="cellrowborder" valign="top" width="14.41%" headers="mcps1.2.5.1.1 "><p id="p6590115416332"><a name="p6590115416332"></a><a name="p6590115416332"></a>15</p>
</td>
<td class="cellrowborder" valign="top" width="23.16%" headers="mcps1.2.5.1.2 "><p id="p959020544338"><a name="p959020544338"></a><a name="p959020544338"></a>look_from_x</p>
</td>
<td class="cellrowborder" valign="top" width="26.040000000000003%" headers="mcps1.2.5.1.3 "><p id="p459095418332"><a name="p459095418332"></a><a name="p459095418332"></a>camera位置x轴的坐标</p>
</td>
<td class="cellrowborder" valign="top" width="36.39%" headers="mcps1.2.5.1.4 "><p id="p8590145416339"><a name="p8590145416339"></a><a name="p8590145416339"></a>[-400, 400]; 0</p>
</td>
</tr>
<tr id="row12590115473311"><td class="cellrowborder" valign="top" width="14.41%" headers="mcps1.2.5.1.1 "><p id="p7590205413334"><a name="p7590205413334"></a><a name="p7590205413334"></a>16</p>
</td>
<td class="cellrowborder" valign="top" width="23.16%" headers="mcps1.2.5.1.2 "><p id="p1359045443317"><a name="p1359045443317"></a><a name="p1359045443317"></a>look_from_y</p>
</td>
<td class="cellrowborder" valign="top" width="26.040000000000003%" headers="mcps1.2.5.1.3 "><p id="p11590554133312"><a name="p11590554133312"></a><a name="p11590554133312"></a>camera位置y轴的坐标</p>
</td>
<td class="cellrowborder" valign="top" width="36.39%" headers="mcps1.2.5.1.4 "><p id="p15590125463312"><a name="p15590125463312"></a><a name="p15590125463312"></a>[-400, 400]; 0</p>
</td>
</tr>
<tr id="row115901954113311"><td class="cellrowborder" valign="top" width="14.41%" headers="mcps1.2.5.1.1 "><p id="p959055414331"><a name="p959055414331"></a><a name="p959055414331"></a>17</p>
</td>
<td class="cellrowborder" valign="top" width="23.16%" headers="mcps1.2.5.1.2 "><p id="p10590454163312"><a name="p10590454163312"></a><a name="p10590454163312"></a>look_from_z</p>
</td>
<td class="cellrowborder" valign="top" width="26.040000000000003%" headers="mcps1.2.5.1.3 "><p id="p1159016541335"><a name="p1159016541335"></a><a name="p1159016541335"></a>camera位置z轴坐标</p>
</td>
<td class="cellrowborder" valign="top" width="36.39%" headers="mcps1.2.5.1.4 "><p id="p185903542332"><a name="p185903542332"></a><a name="p185903542332"></a>[-400, 400]; 0</p>
</td>
</tr>
</tbody>
</table>

**月相星球表盘<a name="section1102111018816"></a>**

**表 3**  月相星球表盘模型参数

<a name="table20462115423315"></a>

<table><thead align="left"><tr id="row115901354193313"><th class="cellrowborder" valign="top" width="14.149999999999999%" id="mcps1.2.5.1.1"><p id="p145901544335"><a name="p145901544335"></a><a name="p145901544335"></a>序号</p>
</th>
<th class="cellrowborder" valign="top" width="22.64%" id="mcps1.2.5.1.2"><p id="p1759045415333"><a name="p1759045415333"></a><a name="p1759045415333"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="26.56%" id="mcps1.2.5.1.3"><p id="p195901354133313"><a name="p195901354133313"></a><a name="p195901354133313"></a>参数含义</p>
</th>
<th class="cellrowborder" valign="top" width="36.65%" id="mcps1.2.5.1.4"><p id="p5590185473317"><a name="p5590185473317"></a><a name="p5590185473317"></a>范围& demo参数</p>
</th>
</tr>
</thead>
<tbody><tr id="row559015413316"><td class="cellrowborder" valign="top" width="14.149999999999999%" headers="mcps1.2.5.1.1 "><p id="p135901354143316"><a name="p135901354143316"></a><a name="p135901354143316"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="22.64%" headers="mcps1.2.5.1.2 "><p id="p1590135415333"><a name="p1590135415333"></a><a name="p1590135415333"></a>dst_width</p>
</td>
<td class="cellrowborder" valign="top" width="26.56%" headers="mcps1.2.5.1.3 "><p id="p7591185418339"><a name="p7591185418339"></a><a name="p7591185418339"></a>屏幕的宽</p>
</td>
<td class="cellrowborder" valign="top" width="36.65%" headers="mcps1.2.5.1.4 "><p id="p2059135473315"><a name="p2059135473315"></a><a name="p2059135473315"></a>-</p>
</td>
</tr>
<tr id="row1759114541331"><td class="cellrowborder" valign="top" width="14.149999999999999%" headers="mcps1.2.5.1.1 "><p id="p45911054103312"><a name="p45911054103312"></a><a name="p45911054103312"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="22.64%" headers="mcps1.2.5.1.2 "><p id="p105916548338"><a name="p105916548338"></a><a name="p105916548338"></a>dst_height</p>
</td>
<td class="cellrowborder" valign="top" width="26.56%" headers="mcps1.2.5.1.3 "><p id="p2591254113316"><a name="p2591254113316"></a><a name="p2591254113316"></a>屏幕的高</p>
</td>
<td class="cellrowborder" valign="top" width="36.65%" headers="mcps1.2.5.1.4 "><p id="p8591175420334"><a name="p8591175420334"></a><a name="p8591175420334"></a>-</p>
</td>
</tr>
<tr id="row759115413320"><td class="cellrowborder" valign="top" width="14.149999999999999%" headers="mcps1.2.5.1.1 "><p id="p5591205420334"><a name="p5591205420334"></a><a name="p5591205420334"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="22.64%" headers="mcps1.2.5.1.2 "><p id="p18591175415335"><a name="p18591175415335"></a><a name="p18591175415335"></a>sphere_radius</p>
</td>
<td class="cellrowborder" valign="top" width="26.56%" headers="mcps1.2.5.1.3 "><p id="p125914547331"><a name="p125914547331"></a><a name="p125914547331"></a>球半径</p>
</td>
<td class="cellrowborder" valign="top" width="36.65%" headers="mcps1.2.5.1.4 "><p id="p1259118541334"><a name="p1259118541334"></a><a name="p1259118541334"></a>(0, +∞) ；150</p>
</td>
</tr>
<tr id="row1591254163317"><td class="cellrowborder" valign="top" width="14.149999999999999%" headers="mcps1.2.5.1.1 "><p id="p2059185413332"><a name="p2059185413332"></a><a name="p2059185413332"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="22.64%" headers="mcps1.2.5.1.2 "><p id="p1159155493310"><a name="p1159155493310"></a><a name="p1159155493310"></a>sphere_cx</p>
</td>
<td class="cellrowborder" valign="top" width="26.56%" headers="mcps1.2.5.1.3 "><p id="p15591205483317"><a name="p15591205483317"></a><a name="p15591205483317"></a>球心x坐标</p>
</td>
<td class="cellrowborder" valign="top" width="36.65%" headers="mcps1.2.5.1.4 "><p id="p105911754133319"><a name="p105911754133319"></a><a name="p105911754133319"></a>(0, +∞) ；dst_surface中心</p>
</td>
</tr>
<tr id="row155914542335"><td class="cellrowborder" valign="top" width="14.149999999999999%" headers="mcps1.2.5.1.1 "><p id="p1359115413338"><a name="p1359115413338"></a><a name="p1359115413338"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="22.64%" headers="mcps1.2.5.1.2 "><p id="p959105453310"><a name="p959105453310"></a><a name="p959105453310"></a>sphere_cy</p>
</td>
<td class="cellrowborder" valign="top" width="26.56%" headers="mcps1.2.5.1.3 "><p id="p85911754123316"><a name="p85911754123316"></a><a name="p85911754123316"></a>球心y坐标</p>
</td>
<td class="cellrowborder" valign="top" width="36.65%" headers="mcps1.2.5.1.4 "><p id="p1591185443315"><a name="p1591185443315"></a><a name="p1591185443315"></a>(0, +∞) ；dst_surface中心</p>
</td>
</tr>
<tr id="row1259113549337"><td class="cellrowborder" valign="top" width="14.149999999999999%" headers="mcps1.2.5.1.1 "><p id="p195914548331"><a name="p195914548331"></a><a name="p195914548331"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="22.64%" headers="mcps1.2.5.1.2 "><p id="p1459120548337"><a name="p1459120548337"></a><a name="p1459120548337"></a>sphere_step_x</p>
</td>
<td class="cellrowborder" valign="top" width="26.56%" headers="mcps1.2.5.1.3 "><p id="p8591115416339"><a name="p8591115416339"></a><a name="p8591115416339"></a>模型x方向采样步长</p>
</td>
<td class="cellrowborder" valign="top" width="36.65%" headers="mcps1.2.5.1.4 "><p id="p1591175411337"><a name="p1591175411337"></a><a name="p1591175411337"></a>[4, 8];4；</p>
</td>
</tr>
<tr id="row19591354173315"><td class="cellrowborder" valign="top" width="14.149999999999999%" headers="mcps1.2.5.1.1 "><p id="p1591195410337"><a name="p1591195410337"></a><a name="p1591195410337"></a>7</p>
</td>
<td class="cellrowborder" valign="top" width="22.64%" headers="mcps1.2.5.1.2 "><p id="p7591054193314"><a name="p7591054193314"></a><a name="p7591054193314"></a>sphrere_step_y</p>
</td>
<td class="cellrowborder" valign="top" width="26.56%" headers="mcps1.2.5.1.3 "><p id="p1659116547335"><a name="p1659116547335"></a><a name="p1659116547335"></a>模型y方向采样步长</p>
</td>
<td class="cellrowborder" valign="top" width="36.65%" headers="mcps1.2.5.1.4 "><p id="p4591175423311"><a name="p4591175423311"></a><a name="p4591175423311"></a>[4, 8];4；</p>
</td>
</tr>
</tbody>
</table>

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - sphere\_step\_x：逻辑约束dst\_height / sphere\_step\_x< 128
> - sphere\_step\_y：逻辑约束dst\_width / sphere\_step\_y < 128

**表 4**  月相星球表盘demo参数

<a name="table4468175418337"></a>

<table><thead align="left"><tr id="row4591454193316"><th class="cellrowborder" valign="top" width="14.799999999999999%" id="mcps1.2.5.1.1"><p id="p19591115473310"><a name="p19591115473310"></a><a name="p19591115473310"></a>序号</p>
</th>
<th class="cellrowborder" valign="top" width="24.34%" id="mcps1.2.5.1.2"><p id="p4591105413316"><a name="p4591105413316"></a><a name="p4591105413316"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="26.169999999999998%" id="mcps1.2.5.1.3"><p id="p9591115433319"><a name="p9591115433319"></a><a name="p9591115433319"></a>参数含义</p>
</th>
<th class="cellrowborder" valign="top" width="34.69%" id="mcps1.2.5.1.4"><p id="p259111541331"><a name="p259111541331"></a><a name="p259111541331"></a>范围&demo值</p>
</th>
</tr>
</thead>
<tbody><tr id="row11591554153319"><td class="cellrowborder" valign="top" width="14.799999999999999%" headers="mcps1.2.5.1.1 "><p id="p1959113544330"><a name="p1959113544330"></a><a name="p1959113544330"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="24.34%" headers="mcps1.2.5.1.2 "><p id="p175924548336"><a name="p175924548336"></a><a name="p175924548336"></a>sphere_angle_x</p>
</td>
<td class="cellrowborder" valign="top" width="26.169999999999998%" headers="mcps1.2.5.1.3 "><p id="p8592175433314"><a name="p8592175433314"></a><a name="p8592175433314"></a>沿x旋转角度</p>
</td>
<td class="cellrowborder" valign="top" width="34.69%" headers="mcps1.2.5.1.4 "><p id="p4592145423319"><a name="p4592145423319"></a><a name="p4592145423319"></a>[0,360];0</p>
</td>
</tr>
<tr id="row15592854163311"><td class="cellrowborder" valign="top" width="14.799999999999999%" headers="mcps1.2.5.1.1 "><p id="p3592165418335"><a name="p3592165418335"></a><a name="p3592165418335"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="24.34%" headers="mcps1.2.5.1.2 "><p id="p14592185473314"><a name="p14592185473314"></a><a name="p14592185473314"></a>sphere_angle_y</p>
</td>
<td class="cellrowborder" valign="top" width="26.169999999999998%" headers="mcps1.2.5.1.3 "><p id="p659265433312"><a name="p659265433312"></a><a name="p659265433312"></a>沿y轴旋转角度</p>
</td>
<td class="cellrowborder" valign="top" width="34.69%" headers="mcps1.2.5.1.4 "><p id="p1959214546339"><a name="p1959214546339"></a><a name="p1959214546339"></a>[0,360];0</p>
</td>
</tr>
<tr id="row1659245411332"><td class="cellrowborder" valign="top" width="14.799999999999999%" headers="mcps1.2.5.1.1 "><p id="p205921454103319"><a name="p205921454103319"></a><a name="p205921454103319"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="24.34%" headers="mcps1.2.5.1.2 "><p id="p1959210549334"><a name="p1959210549334"></a><a name="p1959210549334"></a>sphere_angle_z</p>
</td>
<td class="cellrowborder" valign="top" width="26.169999999999998%" headers="mcps1.2.5.1.3 "><p id="p759235493317"><a name="p759235493317"></a><a name="p759235493317"></a>沿z轴旋转角度</p>
</td>
<td class="cellrowborder" valign="top" width="34.69%" headers="mcps1.2.5.1.4 "><p id="p115923542337"><a name="p115923542337"></a><a name="p115923542337"></a>[0,360];0</p>
</td>
</tr>
</tbody>
</table>

**MESH表盘互动<a name="section178548341816"></a>**

**表 5**  MESH表盘互动模型参数

<a name="table14471185473311"></a>

<table><thead align="left"><tr id="row105922547330"><th class="cellrowborder" valign="top" width="15.45%" id="mcps1.2.5.1.1"><p id="p14592125413316"><a name="p14592125413316"></a><a name="p14592125413316"></a>序号</p>
</th>
<th class="cellrowborder" valign="top" width="24.6%" id="mcps1.2.5.1.2"><p id="p65921549331"><a name="p65921549331"></a><a name="p65921549331"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="25.650000000000002%" id="mcps1.2.5.1.3"><p id="p13592155453318"><a name="p13592155453318"></a><a name="p13592155453318"></a>参数含义</p>
</th>
<th class="cellrowborder" valign="top" width="34.300000000000004%" id="mcps1.2.5.1.4"><p id="p4592105473314"><a name="p4592105473314"></a><a name="p4592105473314"></a>范围&demo值</p>
</th>
</tr>
</thead>
<tbody><tr id="row1459285463317"><td class="cellrowborder" valign="top" width="15.45%" headers="mcps1.2.5.1.1 "><p id="p659255410338"><a name="p659255410338"></a><a name="p659255410338"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="24.6%" headers="mcps1.2.5.1.2 "><p id="p8592054113314"><a name="p8592054113314"></a><a name="p8592054113314"></a>src_width</p>
</td>
<td class="cellrowborder" valign="top" width="25.650000000000002%" headers="mcps1.2.5.1.3 "><p id="p17592254163317"><a name="p17592254163317"></a><a name="p17592254163317"></a>素材的宽</p>
</td>
<td class="cellrowborder" valign="top" width="34.300000000000004%" headers="mcps1.2.5.1.4 "><p id="p19592175493313"><a name="p19592175493313"></a><a name="p19592175493313"></a>建议与屏幕的宽保持一致</p>
</td>
</tr>
<tr id="row95923541336"><td class="cellrowborder" valign="top" width="15.45%" headers="mcps1.2.5.1.1 "><p id="p155921454133314"><a name="p155921454133314"></a><a name="p155921454133314"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="24.6%" headers="mcps1.2.5.1.2 "><p id="p959215443320"><a name="p959215443320"></a><a name="p959215443320"></a>src_height</p>
</td>
<td class="cellrowborder" valign="top" width="25.650000000000002%" headers="mcps1.2.5.1.3 "><p id="p85921545334"><a name="p85921545334"></a><a name="p85921545334"></a>素材的高</p>
</td>
<td class="cellrowborder" valign="top" width="34.300000000000004%" headers="mcps1.2.5.1.4 "><p id="p259285413311"><a name="p259285413311"></a><a name="p259285413311"></a>建议与屏幕的高保持一致</p>
</td>
</tr>
<tr id="row16592115411335"><td class="cellrowborder" valign="top" width="15.45%" headers="mcps1.2.5.1.1 "><p id="p45929540334"><a name="p45929540334"></a><a name="p45929540334"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="24.6%" headers="mcps1.2.5.1.2 "><p id="p1459225413331"><a name="p1459225413331"></a><a name="p1459225413331"></a>dst_width</p>
</td>
<td class="cellrowborder" valign="top" width="25.650000000000002%" headers="mcps1.2.5.1.3 "><p id="p3592145418335"><a name="p3592145418335"></a><a name="p3592145418335"></a>屏幕的宽</p>
</td>
<td class="cellrowborder" valign="top" width="34.300000000000004%" headers="mcps1.2.5.1.4 "><p id="p1259255463313"><a name="p1259255463313"></a><a name="p1259255463313"></a>-</p>
</td>
</tr>
<tr id="row6593954203313"><td class="cellrowborder" valign="top" width="15.45%" headers="mcps1.2.5.1.1 "><p id="p75931254143316"><a name="p75931254143316"></a><a name="p75931254143316"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="24.6%" headers="mcps1.2.5.1.2 "><p id="p16593154113316"><a name="p16593154113316"></a><a name="p16593154113316"></a>dst_heigth</p>
</td>
<td class="cellrowborder" valign="top" width="25.650000000000002%" headers="mcps1.2.5.1.3 "><p id="p45931542336"><a name="p45931542336"></a><a name="p45931542336"></a>屏幕的高</p>
</td>
<td class="cellrowborder" valign="top" width="34.300000000000004%" headers="mcps1.2.5.1.4 "><p id="p17593145423318"><a name="p17593145423318"></a><a name="p17593145423318"></a>-</p>
</td>
</tr>
<tr id="row159355410334"><td class="cellrowborder" valign="top" width="15.45%" headers="mcps1.2.5.1.1 "><p id="p6593115413315"><a name="p6593115413315"></a><a name="p6593115413315"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="24.6%" headers="mcps1.2.5.1.2 "><p id="p1959395416337"><a name="p1959395416337"></a><a name="p1959395416337"></a>mesh_x_block_num</p>
</td>
<td class="cellrowborder" valign="top" width="25.650000000000002%" headers="mcps1.2.5.1.3 "><p id="p195931054123319"><a name="p195931054123319"></a><a name="p195931054123319"></a>水平方向划分块数量</p>
</td>
<td class="cellrowborder" valign="top" width="34.300000000000004%" headers="mcps1.2.5.1.4 "><p id="p20593105414335"><a name="p20593105414335"></a><a name="p20593105414335"></a>(0,32];32</p>
</td>
</tr>
<tr id="row19593115453312"><td class="cellrowborder" valign="top" width="15.45%" headers="mcps1.2.5.1.1 "><p id="p95931454123317"><a name="p95931454123317"></a><a name="p95931454123317"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="24.6%" headers="mcps1.2.5.1.2 "><p id="p185931954153319"><a name="p185931954153319"></a><a name="p185931954153319"></a>mesh_y_block_num</p>
</td>
<td class="cellrowborder" valign="top" width="25.650000000000002%" headers="mcps1.2.5.1.3 "><p id="p13593185418335"><a name="p13593185418335"></a><a name="p13593185418335"></a>垂直方向划分块数量</p>
</td>
<td class="cellrowborder" valign="top" width="34.300000000000004%" headers="mcps1.2.5.1.4 "><p id="p45931545332"><a name="p45931545332"></a><a name="p45931545332"></a>(0,32];32</p>
</td>
</tr>
<tr id="row1259314540335"><td class="cellrowborder" valign="top" width="15.45%" headers="mcps1.2.5.1.1 "><p id="p195931154143312"><a name="p195931154143312"></a><a name="p195931154143312"></a>7</p>
</td>
<td class="cellrowborder" valign="top" width="24.6%" headers="mcps1.2.5.1.2 "><p id="p1359345419339"><a name="p1359345419339"></a><a name="p1359345419339"></a>mesh_cx</p>
</td>
<td class="cellrowborder" valign="top" width="25.650000000000002%" headers="mcps1.2.5.1.3 "><p id="p559315413337"><a name="p559315413337"></a><a name="p559315413337"></a>形变中心点x坐标</p>
</td>
<td class="cellrowborder" valign="top" width="34.300000000000004%" headers="mcps1.2.5.1.4 "><p id="p3207730201312"><a name="p3207730201312"></a><a name="p3207730201312"></a>弱互动：mesh_dst_width / 2</p>
<p id="p115931454113310"><a name="p115931454113310"></a><a name="p115931454113310"></a>强互动建议在[0,mesh_dst_width)</p>
</td>
</tr>
<tr id="row1259335414335"><td class="cellrowborder" valign="top" width="15.45%" headers="mcps1.2.5.1.1 "><p id="p15593554133315"><a name="p15593554133315"></a><a name="p15593554133315"></a>8</p>
</td>
<td class="cellrowborder" valign="top" width="24.6%" headers="mcps1.2.5.1.2 "><p id="p359395418333"><a name="p359395418333"></a><a name="p359395418333"></a>mesh_cy</p>
</td>
<td class="cellrowborder" valign="top" width="25.650000000000002%" headers="mcps1.2.5.1.3 "><p id="p1593754173320"><a name="p1593754173320"></a><a name="p1593754173320"></a>形变中心点y坐标</p>
</td>
<td class="cellrowborder" valign="top" width="34.300000000000004%" headers="mcps1.2.5.1.4 "><p id="p986613280132"><a name="p986613280132"></a><a name="p986613280132"></a>弱互动：mesh_dst_height / 2</p>
<p id="p125931854203313"><a name="p125931854203313"></a><a name="p125931854203313"></a>强互动建议在[0,mesh_dst_height)</p>
</td>
</tr>
<tr id="row15593454133310"><td class="cellrowborder" valign="top" width="15.45%" headers="mcps1.2.5.1.1 "><p id="p175931954143320"><a name="p175931954143320"></a><a name="p175931954143320"></a>9</p>
</td>
<td class="cellrowborder" valign="top" width="24.6%" headers="mcps1.2.5.1.2 "><p id="p3593115416333"><a name="p3593115416333"></a><a name="p3593115416333"></a>mesh_len</p>
</td>
<td class="cellrowborder" valign="top" width="25.650000000000002%" headers="mcps1.2.5.1.3 "><p id="p1759316549330"><a name="p1759316549330"></a><a name="p1759316549330"></a>形变程度</p>
</td>
<td class="cellrowborder" valign="top" width="34.300000000000004%" headers="mcps1.2.5.1.4 "><p id="p759365443318"><a name="p759365443318"></a><a name="p759365443318"></a>[0,10]；</p>
</td>
</tr>
<tr id="row359315547335"><td class="cellrowborder" valign="top" width="15.45%" headers="mcps1.2.5.1.1 "><p id="p13593254183312"><a name="p13593254183312"></a><a name="p13593254183312"></a>10</p>
</td>
<td class="cellrowborder" valign="top" width="24.6%" headers="mcps1.2.5.1.2 "><p id="p559325417331"><a name="p559325417331"></a><a name="p559325417331"></a>mesh_thr</p>
</td>
<td class="cellrowborder" valign="top" width="25.650000000000002%" headers="mcps1.2.5.1.3 "><p id="p15593155463319"><a name="p15593155463319"></a><a name="p15593155463319"></a>形变阈值</p>
</td>
<td class="cellrowborder" valign="top" width="34.300000000000004%" headers="mcps1.2.5.1.4 "><p id="p75937548334"><a name="p75937548334"></a><a name="p75937548334"></a>[0,10]；</p>
</td>
</tr>
<tr id="row12593175413336"><td class="cellrowborder" valign="top" width="15.45%" headers="mcps1.2.5.1.1 "><p id="p2593125453317"><a name="p2593125453317"></a><a name="p2593125453317"></a>11</p>
</td>
<td class="cellrowborder" valign="top" width="24.6%" headers="mcps1.2.5.1.2 "><p id="p959413544336"><a name="p959413544336"></a><a name="p959413544336"></a>mesh_type</p>
</td>
<td class="cellrowborder" valign="top" width="25.650000000000002%" headers="mcps1.2.5.1.3 "><p id="p85942054163319"><a name="p85942054163319"></a><a name="p85942054163319"></a>mesh模型类别</p>
</td>
<td class="cellrowborder" valign="top" width="34.300000000000004%" headers="mcps1.2.5.1.4 "><p id="p1459417542333"><a name="p1459417542333"></a><a name="p1459417542333"></a>支持[0,6]共7种模型，其中type=0/6为强互动模型</p>
</td>
</tr>
<tr id="row2594165413312"><td class="cellrowborder" valign="top" width="15.45%" headers="mcps1.2.5.1.1 "><p id="p1159415548334"><a name="p1159415548334"></a><a name="p1159415548334"></a>12</p>
</td>
<td class="cellrowborder" valign="top" width="24.6%" headers="mcps1.2.5.1.2 "><p id="p259417540332"><a name="p259417540332"></a><a name="p259417540332"></a>region_w</p>
</td>
<td class="cellrowborder" valign="top" width="25.650000000000002%" headers="mcps1.2.5.1.3 "><p id="p15941254143320"><a name="p15941254143320"></a><a name="p15941254143320"></a>动画显示区域宽度</p>
</td>
<td class="cellrowborder" valign="top" width="34.300000000000004%" headers="mcps1.2.5.1.4 "><p id="p15594854143312"><a name="p15594854143312"></a><a name="p15594854143312"></a>[0,32]</p>
</td>
</tr>
<tr id="row9594105453317"><td class="cellrowborder" valign="top" width="15.45%" headers="mcps1.2.5.1.1 "><p id="p15594105483315"><a name="p15594105483315"></a><a name="p15594105483315"></a>13</p>
</td>
<td class="cellrowborder" valign="top" width="24.6%" headers="mcps1.2.5.1.2 "><p id="p359465417338"><a name="p359465417338"></a><a name="p359465417338"></a>region_h</p>
</td>
<td class="cellrowborder" valign="top" width="25.650000000000002%" headers="mcps1.2.5.1.3 "><p id="p1459495415334"><a name="p1459495415334"></a><a name="p1459495415334"></a>动画显示区域高度</p>
</td>
<td class="cellrowborder" valign="top" width="34.300000000000004%" headers="mcps1.2.5.1.4 "><p id="p25941054123312"><a name="p25941054123312"></a><a name="p25941054123312"></a>[0,32]</p>
</td>
</tr>
</tbody>
</table>

> ![](public_sys-resources/icon-note.gif) **说明：**
> 
> - mesh\_x\_block\_num：逻辑约束mesh\_dst\_width / x\_step < 128， 软算法性能约束不超过32。
> - mesh\_y\_block\_num：逻辑约束mesh\_dst\_height / y\_step < 128， 软算法性能约束不超过32。
>   弱互动：只转动表冠；需要载入模型。
>   强互动：支持触屏；不需要载入模型。

**表 6**  MESH表盘互动demo参数

<a name="table1148016549337"></a>

<table><thead align="left"><tr id="row659465413334"><th class="cellrowborder" valign="top" width="18.45%" id="mcps1.2.5.1.1"><p id="p1559495412335"><a name="p1559495412335"></a><a name="p1559495412335"></a>序号</p>
</th>
<th class="cellrowborder" valign="top" width="25.119999999999997%" id="mcps1.2.5.1.2"><p id="p7594654103317"><a name="p7594654103317"></a><a name="p7594654103317"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="31.430000000000003%" id="mcps1.2.5.1.3"><p id="p185945548335"><a name="p185945548335"></a><a name="p185945548335"></a>解释</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="p85941754143315"><a name="p85941754143315"></a><a name="p85941754143315"></a>范围&demo值</p>
</th>
</tr>
</thead>
<tbody><tr id="row759412543330"><td class="cellrowborder" valign="top" width="18.45%" headers="mcps1.2.5.1.1 "><p id="p059455410332"><a name="p059455410332"></a><a name="p059455410332"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="25.119999999999997%" headers="mcps1.2.5.1.2 "><p id="p959414548337"><a name="p959414548337"></a><a name="p959414548337"></a>mesh_ctl</p>
</td>
<td class="cellrowborder" valign="top" width="31.430000000000003%" headers="mcps1.2.5.1.3 "><p id="p1659414543335"><a name="p1659414543335"></a><a name="p1659414543335"></a>连续帧控制</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p3594154113313"><a name="p3594154113313"></a><a name="p3594154113313"></a>[0,100];</p>
</td>
</tr>
</tbody>
</table>

### 蒙版工具<a name="ZH-CN_TOPIC_0000002399739793"></a>

蒙版制作工具支持展示/生成圆形、高斯模糊、HSV三种蒙版文件。

- **使用方法**
- **控制参数**
- **注意事项**

#### 使用方法<a name="ZH-CN_TOPIC_0000002366100014"></a>

**运行<a name="section17318245565"></a>**

单击COMMANDS里面的Modeling Tools，在上方选择“Mask Tool”。

<img src="figures/zh-cn_image_0000002423227462.png" width="700">

**界面介绍<a name="section1021463835619"></a>**

启动后工具主界面如图所示：

**图 1**  蒙版工具启动界面<a name="fig39911508561"></a>
<img src="figures/蒙版工具启动界面.png" width="700" alt="蒙版工具启动界面">

其中按钮功能介绍如下：

- 模型类型：选择当前要使用的模型（高斯蒙版、圆形截图蒙版、HSV蒙版）
- 生成蒙版：在界面左侧区域生成蒙版图片
- 保存蒙版：保存蒙版文件。
- 显示日志/隐藏日志：打开/关闭下方的日志框

**操作步骤<a name="section17766850105615"></a>**

1. 选择模型类型
2. 修改右侧控制参数（可选）
3. 单击“生成蒙版”查看效果。
4. 单击“保存蒙版”保存蒙版文件

**素材默认路径<a name="section1982618594564"></a>**

HSV蒙版生成需要依赖素材文件。工具已提供默认素材文件，用户可自行替换。素材文件路径和文件名如下：

C:/Users/$\{用户名\}/.vscode/extensions/hispark.hisparkstudio-$\{版本号\}/dist/resources/mask/mask\_test/res/input/hsv\_img\_w454\_h454.data

其中$\{用户名\}为当前Windows登录账号用户名，$\{版本号\}为本插件版本号。

> ![](public_sys-resources/icon-notice.gif) **须知：**
> 该文件不能删除或修改文件名，否则会导致程序运行错误。

**高斯蒙版路径参数生成方法<a name="section710141005719"></a>**

1. 打开网址：[https://www.jyshare.com/more/svgeditor/](https://www.jyshare.com/more/svgeditor/)
2. 绘制图形：目前只支持以path开头的图形。
3. 注意：绘制图形时，需要在未选中图形时，在最右侧设置分辨率。
4. 单击菜单栏-\>视图-\>源代码。
5. 复制path中的“d=”之后的字符串到路径。

#### 控制参数<a name="ZH-CN_TOPIC_0000002399619925"></a>

路径：字符串（仅高斯蒙版模式需要）

宽度：素材width（最大640）

高度：素材height（最大600）

通道：素材channel（只支持3：RGB、4：ARGB）

#### 注意事项<a name="ZH-CN_TOPIC_0000002365940118"></a>

- 只有高斯蒙版需要路径，其他两个模式下路径默认为空。
- 圆形截图蒙版只支持正圆。
- HSV蒙版工具目前不支持RGB格式。

## GUI工程使用常见问题<a name="ZH-CN_TOPIC_0000002399739797"></a>

- **起模拟器日志记录位置**

### 起模拟器日志记录位置<a name="ZH-CN_TOPIC_0000002366100018"></a>

在启动模拟器运行过程中的日志信息记录在当前打开的工程根目录下的“simulator\_build/error.txt”文件中。可参考“Demo运行”。

# 高阶分析功能使用<a name="ZH-CN_TOPIC_0000002293224190"></a>

- **功能**
- **功能演示**

## 功能<a name="ZH-CN_TOPIC_0000002327223545"></a>

FBB系列工程支持高阶分析功能，具体如下：

- 支持内存统计功能，可显示每个目录每个文件的具体内存信息，且支持按内存区域以及符号类型筛选。
- 支持将当前内存统计保存为基线版本，支持将当前内存统计与基线版本做比较。
- 支持将内存统计导出为JSON/HTML文件，可在设置里配置“导出格式”以及“导出路径”。
- 支持解析CPU Trace日志。
- 支持解析CPU Trace二进制文件。
- 支持解析malloc trace日志（支持BS20、BS20C、BS21、BS21A、BS21E、BS22、BS25和BS26系列芯片）。
- 支持解析IRQ Trace日志。

## 功能演示<a name="ZH-CN_TOPIC_0000002327183805"></a>

- **内存统计功能**
- **基线对比功能**
- **CPU Trace 日志解析功能**
- **CPU Trace 二进制文件解析功能**
- **堆内存占用情况统计（BS2X和BS25系列芯片适用）**
- **锁中断时间统计以及中断函数执行时间统计维测**
- **Ko文件解析**
- **Bootimg文件解析**

### 内存统计功能<a name="ZH-CN_TOPIC_0000002293383866"></a>

FBB系列工程经过编译生成output目录后，单击右下角Analysis按钮，在弹出的菜单栏中选择内存统计（有时此窗口弹出较慢，请耐心等待），如图1所示。

**图 1**  打开内存统计功能<a name="fig10508109124015"></a>
<img src="figures/打开内存统计功能.png" width="700" alt="打开内存统计功能">

选择对应的target即可统计内存，内存统计界面如图2所示。

**图 2**  内存统计功能展示<a name="fig642651812595"></a>
<img src="figures/内存统计功能展示.png" width="700" alt="内存统计功能展示">

### 基线对比功能<a name="ZH-CN_TOPIC_0000002293224194"></a>

可快速对比更改代码前后的内存占用情况：

1. 单击保存为基线按钮，将当前内存保存为基线。
2. 修改代码或者切换target。
3. 刷新统计。
4. 单击对比按钮，如图1所示，对比结果示例如图2所示。
   
   **图 1**  保存基线版本和与基线版本对比按钮<a name="fig948185817020"></a>
   <img src="figures/保存基线版本和与基线版本对比按钮.png" width="700" alt="保存基线版本和与基线版本对比按钮">
   
   **图 2**  与基线版本对比结果示例<a name="fig68321937318"></a>
   <img src="figures/与基线版本对比结果示例.png" width="700" alt="与基线版本对比结果示例">

### CPU Trace 日志解析功能<a name="ZH-CN_TOPIC_0000002327223553"></a>

芯片死机时通常会通过串口打印出死机日志，其中包含cpu\_trace信息（即死机前一段时间的函数调用链）。

按照“内存统计功能”中的步骤，单击右下角cfbb按钮，在弹出的菜单栏中选择CPU Trace，选择对应的target，将死机时打印的cpu\_trace日志复制到文本框，再单击Trace按钮即可，如图1所示。

**图 1**  Cpu Trace功能<a name="fig2078817371822"></a>
<img src="figures/Cpu-Trace功能.png" width="700" alt="Cpu-Trace功能">

### CPU Trace 二进制文件解析功能<a name="ZH-CN_TOPIC_0000002379747024"></a>

此功能可以解析保存在芯片Flash中的CPU Trace二进制文件，主要用于定位芯片死机问题。

1. 单击右下角Analysis按钮。
2. 在弹出的菜单栏中选择CPU Trace bin，如图所示。
   
   <img src="figures/zh-cn_image_0000002413517473.png" width="700">
3. 在弹出的界面中按照提示选择需要解析的CPU Trace bin文件、编译工具链中的nm文件、编译生成的elf文件以及芯片的CPU频率，如图所示。
   
   <img src="figures/zh-cn_image_0000002379920812.png" width="700">
4. 设置完成后单击解析按钮进行二进制解析，解析完成后会在IDE界面显示解析结果，并自动将解析结果以txt格式保存至与CPU Trace二进制文件相同的目录中，如图所示。
   
   <img src="figures/zh-cn_image_0000002413404861.png" width="700">

### 堆内存占用情况统计（BS2X和BS25系列芯片适用）<a name="ZH-CN_TOPIC_0000002327183813"></a>

此功能可以解析堆内存维测日志，快速分析出堆内存的占用情况。

1. 单击右下角Analysis按钮。
2. 在弹出的菜单栏中选择Malloc Trace，选择对应的target。要打开堆内存维测，只需单击图示中的按钮，需要rebuild；使能后通过串口发送AT指令 AT+TASKMALLOC= \(指令后跟的参数是taskid\)或者调用print\_os\_all\_sys\_task\_heap即可打印出堆内存维测日志。
   
   **图 1**  Malloc Trace功能<a name="fig02211941443"></a>
   <img src="figures/Malloc-Trace功能.png" width="700" alt="Malloc-Trace功能">
3. 将堆内存维测日志复制到文本框中。
4. 单击Trace按钮即可显示出统计图，如图2所示。
   
   **图 2**  与基线版本对比结果示例<a name="fig1836521310443"></a>
   <img src="figures/与基线版本对比结果示例-88.png" width="700" alt="与基线版本对比结果示例-88">

### 锁中断时间统计以及中断函数执行时间统计维测<a name="ZH-CN_TOPIC_0000002293383870"></a>

**打开锁中断维测<a name="section6996192714500"></a>**

要打开锁中断维测，需要在SDK根目录的“kernel/osal\_adapter/CMakeLists.txt”文件的图示位置添加“set\(PUBLIC\_DEFINESOSAL\_IRQ\_RECORD\_INTTER OSAL\_IRQ\_RECORD\_DEBUG\) ”， 注意：需要rebuild。

**图 1**  CMakeLists.txt修改<a name="fig1883953718473"></a>
<img src="figures/CMakeLists-txt修改.png" width="700" alt="CMakeLists-txt修改">

**使用锁中断维测<a name="section470364455017"></a>**

- 调用“osal\_irq\_record\_flag\_set\(\)”接口即可使能/失能锁中断维测，其参数为0～3，参数说明如下：
  
  0：失能中断维测；
  
  1：使能锁中断时间统计维测；
  
  2：使能中断函数执行时间统计维测；
  
  3：使能锁中断和中断函数执行时间统计维测。
- 调用“osal\_print\_irq\_record\(\)”接口即可打印出记录的维测信息，其参数为空。

**解析维测信息<a name="section6885916125218"></a>**

1. 单击右下角cfbb按钮。
2. 在弹出的菜单栏中选择Irq Trace。
3. 选择对应的target。
4. 将维测日志复制到文本框，单击“Trace”按钮即可。

**图 2**  Irq Trace功能<a name="fig15230162419526"></a>
<img src="figures/Irq-Trace功能.png" width="700" alt="Irq-Trace功能">

### Ko文件解析<a name="ZH-CN_TOPIC_0000002591819330"></a>

1. 完成工具链toolchain配置后打开带有.ko后缀文件的文件夹。
   
   **图 1**  打开带有.ko后缀文件的文件夹<a name="fig58945020273"></a>
   <img src="figures/打开带有-ko后缀文件的文件夹.png" width="700" alt="打开带有-ko后缀文件的文件夹">
2. 单击ko文件解析按钮。
   
   **图 2**  点击.ko文件解析按钮<a name="fig12626195616277"></a>
   <img src="figures/点击-ko文件解析按钮.png" width="700" alt="点击-ko文件解析按钮">
3. 选择对应.json文件。
   
   **图 3**  选择json文件<a name="fig3844055142810"></a>
   <img src="figures/选择json文件.png" width="700" alt="选择json文件">
4. 得到解析结果。
   
   **图 4**  得到解析结果<a name="fig4685431162911"></a>
   <img src="figures/得到解析结果.png" width="700" alt="得到解析结果">

### Bootimg文件解析<a name="ZH-CN_TOPIC_0000002622338905"></a>

1. 完成工具链配置后打开带有vmlinux文件的文件夹。
   
   **图 1**  打开带有vmlinux文件的文件夹<a name="fig58945020273"></a>
   <img src="figures/打开带有vmlinux文件的文件夹.png" width="700" alt="打开带有vmlinux文件的文件夹">
2. 单击bootimg文件解析按钮。
   
   **图 2**  点击bootimg文件解析按钮<a name="fig12626195616277"></a>
   <img src="figures/点击bootimg文件解析按钮.png" width="700" alt="点击bootimg文件解析按钮">
3. 选择对应.json文件。
   
   **图 3**  选择json文件<a name="fig3844055142810"></a>
   <img src="figures/选择json文件-89.png" width="700" alt="选择json文件-89">
4. 得到解析结果。
   
   **图 4**  得到解析结果<a name="fig4685431162911"></a>
   <img src="figures/得到解析结果-90.png" width="700" alt="得到解析结果-90">

# 常见错误<a name="ZH-CN_TOPIC_0000002293224278"></a>

- **SDK根目录路径过长**
- **路径失效**
- **编译报错“Kconfig header saved to XXX”**
- **Kconfig Jump to弹框中搜索报错"NameError: name 're' is not defined"**
- **编译报错“Invalid argument”**
- **工程编译慢的问题**

## SDK根目录路径过长<a name="ZH-CN_TOPIC_0000002293383942"></a>

SDK根目录路径过长：

Windows 10和Windows 11下路径有260Byte的长度限制，过长的路径会导致编译时相关文件无法找到，或者编译时一直循环某些打印信息而不执行具体的编译内容，建议将SDK代码放到盘符的根目录或缩短SDK存放路径。

**图 1**  SDK根目录路径过长<a name="fig13895175443715"></a>
<img src="figures/SDK根目录路径过长.png" width="700" alt="SDK根目录路径过长">

## 路径失效<a name="ZH-CN_TOPIC_0000002293224282"></a>

导入工程路径问题导致的调试、栈分析、镜像分析等默认路径失效。

- 调试：修改默认的debug\_elf路径。
  
  **图 1**  修改默认debug\_elf路径<a name="fig1326371603814"></a>
  <img src="figures/修改默认debug_elf路径.png" width="700" alt="修改默认debug_elf路径">

## 编译报错“Kconfig header saved to XXX”<a name="ZH-CN_TOPIC_0000002327183901"></a>

如果编译报错“Kconfig header saved to XXX”，并且在SDK根目录下的build.log文件中搜索“error”有类似于“FAILED：xxx.c  ccache”的字段。

**图 1**  FAILED和ccache报错信息<a name="fig1964314202410"></a>
<img src="figures/FAILED和ccache报错信息.png" width="700" alt="FAILED和ccache报错信息">

可以尝试在下载工具链的目录下的tools/cfbb/thirdparty/ccache目录下执行“ccache.exe -s”指令清除缓存即可。

## Kconfig Jump to弹框中搜索报错"NameError: name 're' is not defined"<a name="ZH-CN_TOPIC_0000002293383962"></a>

1. 打开Kconfig后，单击“Jump to...”按钮，在弹框中搜索相关内容。
   
   **图 1**  从Jump to打开搜索框<a name="fig252132311619"></a>
   <img src="figures/从Jump-to打开搜索框.png" width="700" alt="从Jump-to打开搜索框">
2. 若出现如下异常打印：
   
   **图 2**  Kconfig异常打印<a name="fig17981081134"></a>
   <img src="figures/Kconfig异常打印.png" width="700" alt="Kconfig异常打印">
   
   需要修改“guiconfig.py”文件：在调用re模块前，添加import re，如图3所示。
   
   **图 3**  添加import re<a name="fig23065516141"></a>
   <img src="figures/添加import-re.png" width="700" alt="添加import-re">
3. 添加代码之后可正常搜索，如图4所示。
   
   **图 4**  正常搜索示意图<a name="fig1265846121315"></a>
   <img src="figures/正常搜索示意图.png" width="700" alt="正常搜索示意图">

## 编译报错“Invalid argument”<a name="ZH-CN_TOPIC_0000002304197682"></a>

编译过程中如果报错“Invalid argument”。

**图 1**  ws63编译报错“Invalid argument”<a name="fig1489361241813"></a>
<img src="figures/ws63编译报错-Invalid-argument.png" width="700" alt="ws63编译报错-Invalid-argument">

报错原因：解析elf时由于没有管理员权限导致失败。解决方法：用管理员权限打开VS Code再次进行编译。

## 工程编译慢的问题<a name="ZH-CN_TOPIC_0000002423558450"></a>

可能原因一：Microsoft PC Manager Service的CPU占用率过高导致，可以结束或者禁用这个进程，加快工程编译速度。

**图 1**  禁用Microsoft PC Manager Service服务<a name="fig8963151164517"></a>
<img src="figures/禁用Microsoft-PC-Manager-Service服务.png" width="700" alt="禁用Microsoft-PC-Manager-Service服务">

可能原因二：进程Antimalware Service Executable的CPU占用率较高，而且无法关闭。Antimalware Service Executable是一个Windows安全进程，它执行针对恶意软件的实时保护。其在后台运行，因此会不时地检测文件和程序。当它检测到病毒或其他恶意攻击时，它会删除或隔离它们。在工程编译时，其也会扫描整个工程目录，导致CPU占用率过高。因此，只要不让其扫描工程目录即可降低其CPU占用率，加快编译速度。解决方法如下：

1. 打开Windows安全中心，单击“威胁和病毒防护”**。**
   
   **图 2**  打开Windows安全中心<a name="fig34562361306"></a>
   <img src="figures/打开Windows安全中心.png" width="700" alt="打开Windows安全中心">
2. 打开“病毒威胁和防护设置”的“管理设置”，下滑找到“排除项”，单击“添加或删除排除项”。
   
   **图 3**  打开“病毒威胁和防护设置”的“管理设置”<a name="fig579017381819"></a>
   <img src="figures/打开-病毒威胁和防护设置-的-管理设置.png" width="700" alt="打开-病毒威胁和防护设置-的-管理设置">
   
   **图 4**  “排除项”， 点击 “添加或删除排除项**”**<a name="fig1779110251727"></a>
   <img src="figures/排除项-点击-添加或删除排除项.png" width="700" alt="排除项-点击-添加或删除排除项">
3. 在“排除项”中添加要编译的工程目录。
   
   **图 5**  在“排除项” 中添加要编译的工程目录<a name="fig553317481037"></a>
   <img src="figures/在-排除项-中添加要编译的工程目录.png" width="700" alt="在-排除项-中添加要编译的工程目录">

可能原因三：VS Code在效率模式下，可以关闭效率模式，提升编译速度。

1. 查看是否处于效率模式
   
   **图 6**  资源管理器中状态有“叶子”标志说明处于效率模式下<a name="fig1135213369810"></a>
   <img src="figures/资源管理器中状态有-叶子-标志说明处于效率模式下.png" width="700" alt="资源管理器中状态有-叶子-标志说明处于效率模式下">
2. 找到VS Code的快捷方式，右键进入属性，在“目标”栏后面加上一个英文空格，再添加“--disable-features=UseEcoQoSForBackgroundProcess”字段即可。
   
   **图 7**  添加字段解除效率模式<a name="fig17390172916149"></a>
   <img src="figures/添加字段解除效率模式.png" width="700" alt="添加字段解除效率模式">
3. 再次打开资源管理器，效率模式解除。
   
   **图 8**  效率模式解除<a name="fig7758184381415"></a>
   <img src="figures/效率模式解除.png" width="700" alt="效率模式解除">

