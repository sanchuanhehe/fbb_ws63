# 贡献指南

我们欢迎对 WS63 文档项目做出贡献，如修复错误，添加文档等。我们通过 [GitCode Pull Requests](https://gitcode.com/HiSpark/fbb_ws63/pulls){ target=_blank} 接受贡献。

## 提交流程

这一节，是对 `新增问题` 和 `修改问题` 两个操作的流程简要介绍，流程中涉及的环节具体要求，请点击链接查看。

针对 `git` 相关操作不做具体的介绍，可以查看 [Git 相关教程](https://git-scm.com/book/zh/v2){ target=_blank}。

### 新增问题

1. 点击页面右下角的`意见反馈按钮`（邮件图标），进入问题列表页面；

2. 点击`新建 Issue`，选择缺陷模板，然后填写问题标题和描述，点击`提交 Issue`完成新增问题。

### 修改问题

1. 在本地新建分支
2. 在本地或者 web IDE (Integrated Development Environment) 找到与问题类型对应的 `*.md` 文件，修改期望修改的问题；
3. 遵循 [提交信息规范](#提交信息规范)，推送到 gitcode 后并提交 Pull Requests；
4. 若满足上述预期，则 [提交合并请求](#提交合并请求)；
5. 待文档所有讨论解决并成功提交 PR，即完成修改的流程。

## 问题编辑规范

请按照以下格式规范规则添加或更新 Q&A：

**通用规则：**

- 添加新的 Q&A 时，切记需在前项问题后添加分隔符“----------------”。

**问题格式：**

- 须简洁清晰地描述问题，如：
  - WS63 固件烧录时出现错误 “Com0 open fail, please check com is busy or not exist”？**（问题不清晰）**
  - WS63 固件烧录时出现错误 “Com0 open fail, please check com is busy or not exist”。如何解决？**（清晰）**
- 问题不宜过长。如描述太多，可精炼出主要的问题作为标题，并在回答的正文中详细描述问题背景及细节。

**答案格式：**

- 如正文中需引用代码，请使用 code 语法将其与文字隔开。
- 如某个问题的回答仅包含一句话，则使用正常段落书写即可，无需使用列表。
- 需要列举多个条目或排列顺序时，请使用列表：
  - 数字列表：有一定顺序（如操作步骤），或后文中需引用列表中的某个条目。
  - 项目符号列表：无特定顺序。
- 列表前需有介绍性文字，说明下述列表的含义或目的，且以冒号“：”结尾。
- 如两项条目是互为选择的关系，应使用项目符号列表罗列（非数字列表），并在段前介绍性文字中说明这二者的关系。
- 正文中（不论列表还是段落），每一行之前需空两格。
- 如某项条目后需跟注释或说明性文字，应缩进该注释，使其成为子条目。

### 问题模块示例

> ```
> --------------
> 
> 星闪WS63是否支持测距？
> -------------------------------------
> 
>   目前开源社区提供BS21e实现测距，提供了案例及文档。
> ```

### 问题图片示例

> ```
> --------------
> 
> curses.h: No such file or directory？
> -------------------------------------------
> 
> 问题截图：support bs21 chip
> 
> .. figure:: _static/application-solution/sample_application/sle_distanc_error.png
>     :align: center
>     :width: 800
>     :height: 100
> 
> 解决方法 ：sudo apt-get install libncurses5-dev
> ```

## 提交信息规范 {#提交信息规范}

在分支上添加提交信息，以说明添加/修改/删除问题功能。每个提交都有一条消息，例如：

```
artificial-intelligence: add bs21e support sle_measure_dis

1. bs21e support sle_measure_dis.
```

提交信息的第一行应类似于“问题类别：添加/修复/删除/更改内容”。第一行以提交要更改的文件名的名称开头。例如：

```
artificial-intelligence: bs21e support sle_measure_dis.
```

要添加有关该提交的更多详细信息，请将其放在第一行之后的提交消息中。

一个好的 git 提交消息讲述了一个为什么发生更改的故事，因此，阅读提交日志的人可以了解项目的开发。编写良好的提交信息现在看来似乎是在浪费时间，但是在将来尝试了解某些原因更改时，这对您和您的同事很有用（对我们的客户也有用）。

## 提交合并请求 {#提交合并请求}

一旦完成修改就可以对分支进行第一次提交，如果您需要进行更多的更改，请进行更多提交。完成您对该分支的所有提交后，提交合并请求。

我们使用 GitCode 合并请求功能将分支合并到主分支中，步骤：

1. 将您的分支推送到 GitCode 仓库；
2. 转到 [fbb_ws63](https://gitcode.com/HiSpark/fbb_ws63/pulls){ target=_blank}，然后单击 "新建Pull Request"；
3. 选择您刚创建准备合并的分支，然后填写"合并请求"详细信息。

### 提交合并请求相关规范

- Title 要求:

```
add: 简要描述
```

- Description 要求:

  分点描述该合并修改的信息。

- 示例：

Title:

```
artificial-intelligence: add support sle_measure_dis.
```

Description:

```
1. add bs21e support sle_measure_dis.
```
