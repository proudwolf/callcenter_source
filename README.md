# AES配置使用说明

## 1、AES的安装

将AESServer_ReleaseV1.0.1.tar安装包（或通过源代码把编译好的安装包）拷贝到以下目录

第一步：解压AES压缩文件

（1）、进入/usr/local

![image.png](https://raw.gitcode.com/user-images/assets/7630596/8b3954ef-06a1-404f-8863-bcdae7210983/image.png 'image.png')

（2）、输入解压命令tar xvf AESServer_ReleaseV1.0.1.tar

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/5facb1dd51bc4708a43549cee5550cae.png)

第二步：链接动态库路径

设置AESServer_ReleaseV1.0.1所使用的动态库的路径（进入根目录，与

root同一级别的etc目录）

（1）、执行命令：cd  /etc/ld.so.conf.d/
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/b1428e4228bd48aeb5709d17cd7e9c50.png)


（2）、新建AESServer.conf

输入命令：vi   AESServer.conf

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/40424a0123db4c93ba5fe6d23e905166.png)
（3）、
进入AESServer.conf文件，添加这一行按i（英文），输

入/usr/local/AESServer_ReleaseV1.0.1

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/18a337ef5ba34dbf8c583f9393078b3d.png)

保存并退出AESServer.conf（在键盘上按Esc键，然后输入:wq），如下图所表示：
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/3f7d4679ea7340f1bfd5f67f9cefb669.png)

（4）、关联动态链接库：

返回到/etc/ld.so.conf.d/目录，输入命令：ldconfig， 最后按回车键。

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/c9e0d915fa5d49128894ef587a78448e.png)

在ldconfig时，若出现报错：/libclang.so.15 不是符号链接（不用管，没影响）

## 2、AES的配置
第一步：

进入cd /usr/local/AESServer_ReleaseV1.0.1

打开AESServer.ini   命令vi AESServer.ini

进行如下配置：

（1）、配置freeswitch的连接

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/b174657a6a064786b5c7a4cbdf72629a.png)

备注说明：

ServerPort=21893      //此AES服务的端口默认为:21893

freeswtich连接的配置：（假设freeswtich的参数如下）

FSIPAddr=192.168.31.166    //安装freeswitch的IP地址

FSPasswd=cherringsoft        //freeswitch连接的密码

FSPort=4521                         //freeswtich连接的端口

查看freeswitch的具体的密码和端口设置如下：

在freeswitch服务器上查看以下event_socket.conf.xml: /usr/local/freeswitch/conf/autoload_configs/event_socket.conf.xml

密码和端口如下：

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/21a01089b03d42db9f6f12f21b4f8546.png)

（2）、设置坐席的分机号：

以下示例为：这是从6001到6010的10个坐席分机。

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/23fe663a33d04591a56b457006a8b795.png)

（3）、加密序列号

备注：若用户不需要加密序列号，可以编译源代码，在源代码中注销掉需要加密序列
号的地方，重新编译即可。

加密序列号的目的：为了限制项目中分机的数量。

具体操作步骤如下：

序列号申请：

在如下图所示：在/usr/local/AESServer_ReleaseV1.0.1的目录下，运行./RegisterCode
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/a8e6bba4bd214aacb3796d690cbab838.png)
运行后得到注册码，如下所示：
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/216a86f1c9b842be86fa93eb4650bbfd.png)
将此注册码拷贝下来，进行加密：
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/1e6a5aa20cb94ba38bd80382d21b3024.png)
第1步：将上述拷贝的注册码填入：日期加密序列号框中

第2步：点击解密

以下开始对注册码进行加密：

注册码说明，注册码包含：网卡,IP,创建时间,路数，限制时间

以下示例中，0C4938,0.103,003136,30,2000

网卡：0C4938

IP:0.103

创建时间：003136

路数：30   （指的是坐席分机数量和IVR分机数量的总和）

限制时间：2000 （单位为天， 目前此参数暂时不起作用，无效）

使用步骤：

第1步：更改路数license许可限制为：30

第2步：点击 加密按钮

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/841e4cb09814411dab85d769476b64eb.png)

填入序列号：

将以上生成好的序列号，填入以下：

/usr/local/AESServer_ReleaseV1.0.1中，打开AESServer.ini，

在SeriNo=  处填入序列号。

如下图：

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/7d9b9040747844e098b34212b9c5a931.png)

## 3、启动AES服务

第一步：

进入 /usr/local/AESServer_ReleaseV1.0.1

输入命令：./AESServer

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/a2eddc859ccd419ebb5f5f42c9351d46.png)

第二步：

在第一步输入命令后敲回车键，出现如下图所示表示AES启动成功

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/16b770955ccb4ff88b799c15b507bb81.png)



# 总结

通过以上选择直接使用AES安装包或AES源码编译两种方式，读者按照我们第二篇的
讲解和操作，经过自己的努力，应该就把AES服务搭建起来了。


# 技术咨询与支持

若您有任何问题，需要咨询或技术支持，请微信扫描加V：

![image.png](https://raw.gitcode.com/user-images/assets/7630596/e2355d6a-81d0-465f-80a2-fb5880631a6a/image.png 'image.png')



