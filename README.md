# CTI简介

本篇的CTI程序我们取名为TsapiServer，顾名思义我们通过标准化协议TSAPI连接上篇中的AES服务，AES服务提供接口起到TsapiServer与freeswitch的中间接口对接转化的功能。

他们三者之间的关系如下：

![image.png](https://raw.gitcode.com/user-images/assets/7630596/ce9262b0-0efe-4819-93f1-f6f888f069f1/image.png 'image.png')


## 1、CTI_installer

已经编译成功的CTI安装包，直接下载后，按照后面的配置使用说明即可使用。

## 2、CTI_source_code

CTI的源代码：读者可以下载源代码自行编译或进行二次开发

# CTI配置使用说明

## 1、CTI程序的安装
第一步：解压CTI安装程序

将TsapiServer_Release_V1.0.1.tar(根据实际情况安装最新版本)拷贝到以下目录：

/usr/local/

解压如下：

1、进入/usr/local

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/fc0331568f624038852274c3543713fa.png)

2、解压命令tar xvf  TsapiServer_Release_V1.0.1.tar

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/ddd4574a79af490eb8948c7cebbc6ea4.png)


第二步：链接动态库路径

设置TsapiServer_Release_V1.0.1所使用的动态库的路径（进入根目录，与root同一级别的etc目录）

（1）、执行命令：cd  /etc/ld.so.conf.d/

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/d84db7f0227c495da4bfde2257d5e24b.png)


（2）、新建TsapiServer.conf

输入命令 vi TsapiServer.conf：

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/d3d5a5411aca41f8998787ad0e198bd4.png)

（3）、
进入TsapiServer.conf文件，添加这一行按i（英文），输

入/usr/local/TsapiServer_ReleaseV1.0.1


![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/2e95a2b383b844179c01f65cb96ede2b.png)

保存并退出TsapiServer.conf（在键盘上按Esc键，然后输入:wq），如下

图所表示：

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/2ad1cbd7dd274539b6adafd6bf7487c9.png)

（4）、关联动态链接库：ldconfig

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/4efeb16828bd4d63903e038453e1284c.png)
## 2、CTI程序的配置

第一步：TsapiServer.ini参数配置

进入cd /usr/local/TsapiServer_Release_V1.0.1

打开TsapiServer.ini   命令vi TsapiServer.ini

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/d0010da8319546b4abcaa2d0394d4889.png)
 

填写如下信息：

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/59e314988b114ce0b204da03a87bf02b.png)
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/ffd33aaff5e640638bdd5386e051d317.png)

SeriNo=序列号

序列号的生成

如下图所示，运行./RegisterCode

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/feeb4299b7a14451b5f149c9ad27f7b4.png)
如上图，取出register_code：后面的注册码，然后生成序列号。

序列号的如何生成，参考上一篇介绍，点击如下连接即可：

[呼叫中心源代码开源社区专栏第二篇](https://blog.csdn.net/weixin_38921966/article/details/152166582?spm=1001.2014.3001.5502)



第二步：Csta32.ini参数配置

打开Csta32.ini配置文件

命令 vi Csta32.ini

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/09b74fdaa9a64597ba886faf6e53afc5.png)

打开Csta32.ini，进行如下参数配置：

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/9138989883a14c55ba378eb15a5f0b8c.png)


## 3、CTI的启动

第一步：

进入cd /usr/local/TsapiServer_Release_V1.0.1

输入命令：./TsapiServer

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/cbf1ed7158004dcfba5bb79ed6e81c20.png)

第二步：

在第一步输入命令后敲回车键出现如下图所示表示CTI启动成功

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/74982b2e5bc2432a95945e720ad464f0.png)

# 总结

通过以上选择直接使用TsapiServer安装包或TsapiServer源码编译两种方

式，读者按照我们第三篇的讲解和操作，经过自己的努力，应该就把

CTI(TsapiServer)服务搭建起来了。

# 技术咨询与支持

若您有任何问题，需要咨询或技术支持，请微信扫描加V：

![image.png](https://raw.gitcode.com/user-images/assets/7630596/1e866312-78bf-48f4-bf9a-1c256f67b6ff/image.png 'image.png')

