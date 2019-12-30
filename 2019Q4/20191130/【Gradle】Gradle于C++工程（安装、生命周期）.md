在`C++`项目工程的搭建中，一般很少使用`Gradle`，或者你接手的C++项目中很不幸地使用了。因此，你会不断地搜索相关的使用方法，但网络上基本都是在`Android Studio`中使用，很少涉及到`C++`项目。从本文开始，会有几篇博文从Gradle项目工程的搭建、到常用语法、到最终实战案例，带你手把手完成。

Gradle是一个专注于构建自动化和支持多语言开发的构建工具。如果你在任何平台上构建、测试、发布和部署软件，Gradle提供了一个灵活的模型，可以支持从编译和打包代码到发布的整个开发周期。Gradle被设计成**支持跨多种语言和平台的构建自动化，包括Java、Scala、Android、C/C++和Groovy**，并与开发工具和持续集成服务紧密地集成，包括`Eclipse`、`IntelliJ`和`Jenkins`。使用Gradle构建工具，我们可以方便又快速地实现自己的构建目标。

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2019Q4/20191130](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2019Q4/20191130)。

<br/>

## gradle的安装
官方安装教程(英文版)：[Gradle | Installation](https://gradle.org/install/)。

gradle的安装主要有两种方式：
### 手动安装
官方下载路径：[Gradle | Releases](https://gradle.org/releases/)。

选择合适的版本(可以直接选择**Binary-only二进制版本**)，将下载后的版本压缩包，直接解压放置到相应的目录。

接下来需要设置环境变量，如果是Windows版本，则直接加入环境变量`PATH`即可。

如果是Linux版本，则需要：
```bash
vi ~/.bashrc
```
其中，在文本最后添加：
```bash
export PATH=$PATH:/home/yngzmiao/software/gradle-2.9/bin
```
最后，直接`source`一下即可：
```bash
source ~/.bashrc
```

最终检验，则是在命令行/终端中输入命令`gradle tasks`：
```bash
yngzmiao@yngzmiao-virtual-machine:~/github/gradle-template-c++$ gradle tasks
:tasks

------------------------------------------------------------
All tasks runnable from root project
------------------------------------------------------------

Build Setup tasks
-----------------
init - Initializes a new Gradle build. [incubating]
wrapper - Generates Gradle wrapper files. [incubating]

Help tasks
----------
components - Displays the components produced by root project 'gradle-template-c++'. [incubating]
dependencies - Displays all dependencies declared in root project 'gradle-template-c++'.
dependencyInsight - Displays the insight into a specific dependency in root project 'gradle-template-c++'.
help - Displays a help message.
model - Displays the configuration model of root project 'gradle-template-c++'. [incubating]
projects - Displays the sub-projects of root project 'gradle-template-c++'.
properties - Displays the properties of root project 'gradle-template-c++'.
tasks - Displays the tasks runnable from root project 'gradle-template-c++'.

To see all tasks and more detail, run gradle tasks --all

To see more detail about a task, run gradle help --task <task>

BUILD SUCCESSFUL

Total time: 1.792 secs

This build could be faster, please consider using the Gradle Daemon: https://docs.gradle.org/2.9/userguide/gradle_daemon.html
```
这个命令是将所有的`task`全部都列举出来。至于什么是task，如何定义并使用task，博主会在后面的文章中讲解。

### 使用Gradle Wrapper进行安装
当把本地一个项目放入到远程版本库的时候，如果这个项目是以gradle构建的，那么其他人从远程仓库拉取代码之后如果本地没有安装过gradle会无法编译运行，如果对gradle不熟悉，会使得无法很好的去快速构建项目代码。所以，gradle可以自动生成一键运行的脚本，把这些一起上传远程仓库，使得即使没有安装gradle也可以自动去安装并且编译项目代码。

也就是说，**Gradle Wrapper是一个Gradle包装器，它使得你没有必要安装Gradle。当运行Gradlew或gradlew.bat的时候，将自动下载并缓存指定版本的Gradle**。

`Gradle Wrapper`的目录结构如下所示：
```
yngzmiao@yngzmiao-virtual-machine:~/github/gradle-template-c++$ tree
.
├── gradle
│   └── wrapper
│       ├── gradle-wrapper.jar
│       └── gradle-wrapper.properties
├── gradlew
└── gradlew.bat

2 directories, 4 files
```

其中`gradlew`和`gradlew.bat`分别是Linux和Windows下的可执行文件，**gradle-wrapper.properties中记录了默认下载并缓存的gradle的版本**：
```bash
#Fri Nov 29 10:53:20 CST 2019
distributionBase=GRADLE_USER_HOME
distributionPath=wrapper/dists
zipStoreBase=GRADLE_USER_HOME
zipStorePath=wrapper/dists
distributionUrl=https\://services.gradle.org/distributions/gradle-2.9-bin.zip
```
当本地`GRADLE_USER_HOM`E(当前用户目录)中的`wrapper/dists`没有安装gradle时，将会自动从此地址`distributionUrl`中下载gradle，之后的执行将不会再次下载安装。

可以看出，配置的gradle版本的信息为`gradle-2.9-bin.zip`。

那么，此时如何运行呢？直接运行可执行程序`gradlew`即可。
```bash
yngzmiao@yngzmiao-virtual-machine:~/github/gradle-template-c++$ ./gradlew tasks
:tasks

------------------------------------------------------------
All tasks runnable from root project
------------------------------------------------------------

Build Setup tasks
-----------------
init - Initializes a new Gradle build. [incubating]
wrapper - Generates Gradle wrapper files. [incubating]

Help tasks
----------
components - Displays the components produced by root project 'gradle-template-c++'. [incubating]
dependencies - Displays all dependencies declared in root project 'gradle-template-c++'.
dependencyInsight - Displays the insight into a specific dependency in root project 'gradle-template-c++'.
help - Displays a help message.
model - Displays the configuration model of root project 'gradle-template-c++'. [incubating]
projects - Displays the sub-projects of root project 'gradle-template-c++'.
properties - Displays the properties of root project 'gradle-template-c++'.
tasks - Displays the tasks runnable from root project 'gradle-template-c++'.

To see all tasks and more detail, run gradlew tasks --all

To see more detail about a task, run gradlew help --task <task>

BUILD SUCCESSFUL

Total time: 1.742 secs

This build could be faster, please consider using the Gradle Daemon: https://docs.gradle.org/2.9/userguide/gradle_daemon.html
```
可以看出，在运行`./gradlew tasks`其实和运行`gradle tasks`没有什么区别。

那么，Gradle Wrapper怎么获得呢？

一般来说，Gradle Wrapper是需要通过命令来获得的：
```bash
gradle wrapper
```

也就是说，如果想要生成Gradle Wrapper，那么你需要提前安装Gradle。

### 安装总结
**如果是仅自己开发，可以使用手动安装Gradle的方式；如果是很多人开发，可以使用Gradle Wrapper，那么其他人都可以不需要安装Gradle即可运行相应的程序**。

<br/>

## Gradle的生命周期
在讲解Gradle的生命周期前，先简单说一下Gradle里的任何东西都基于的两个基础概念:
* projects(项目)
* tasks(任务)

每个`project`代表什么取决于你想用Gradle做什么，而每个project又是由一个或多个`tasks`构成的. 一个task代表一些更加细化的构建。例如，一个project可以代表一个C++项目应用。那么对应的tasks可能是复制一些文件, 创建一个运行某些子程序, 或者对某个目录进行压缩等等。

Gradle的生命周期可以分为三个阶段：
* initlalization：**初始化阶段，解析所有的project**
* configuration：**配置阶段，解析project中的task，形成拓扑图**
* execution：**执行阶段，执行具体的task及其依赖task**

但是需要注意的是：**Task的代码在配置阶段也会执行，除非写在doLast{}结构体内的内容，才会在执行阶段执行。同时，只有Task的内容，才有机会在执行阶段执行，其余的代码都不会**。

如果暂时不能理解也没有关系，这些内容在后面讲解`Task`的部分会重点讲解。

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">