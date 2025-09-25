# freeswitch安装包使用说明
备注：此安装包已编译成功，直接拷贝下来即可使用（无需再进行freeswith的源码编译）

## 1、freeswtich安装下载地址：
链接: https://pan.baidu.com/s/1S1O2j-4YavbDNt5kTmPX7g 提取码: 5kpp

## 2、使用方法：

（1）、将压缩文件，拷贝到 /usr/local/目录下解压

（2）、进入 /usr/local/freeswitch/bin/这个目录

（3）、执行命令： ./freeswitch -nonat

   运行成功。

## 3、错误提示：

在执行命令： ./freeswitch -nonat后，可能会出现报错，那是因为您的操作系统没

有运行freeswitch所依赖的库文件，

您可根据报错信息的提示，自行到网上搜索下载安装所需依赖的库文件。

## 4、freeswitch参数设置

  ### （1）、端口、密码设置：
  
   第一步：修改sip_port的端口
    
   查看 sip_port端口，执行命令： vi /usr/local/freeswitch/conf/vars.xml
   
   sip_port的端口默认设置为4473， 这个4473端口设置可以不需要改动。
   
   第二步：修改ESL端口和密码
   
执行命令：vi  /usr/local/freeswitch/conf/autoload_configs/event_socket.conf.xml

   基本修改配置
    
   Nat-map =    true
    
   Listen-port=   4521
    
   Password =    CherringSoft1258
   
   ESL的默认端口为4521， 密码默认为：CherringSoft1258
   
   这里的端口和密码可以自行修改
   
  ### （2）、分机设置：
	
   执行命令：vi /usr/local/freeswitch/conf/directory/default/users.xml
          
   这个users.xml中可以修改分机和密码：
          
   例如下图为分机号为6001，分机密码为6001的设置：
   
  ![fs分机密码修改.png](https://raw.gitcode.com/user-images/assets/7630596/fb9665fd-9a51-4769-8d0b-fab72830e0ef/fs分机密码修改.png 'fs分机密码修改.png')
  
## 5、软电话测试

 ### （1）、关闭防火墙
 在安装的freeswitch的服务器上关闭防火墙
 
 systemctl stop firewalld
 
 systemctl disable firewalld
 
 ### （2）、软电话验证注册freeswitch
 
 以下为软电话注册，也可以使用IP话机注册。
 
 ![image.png](https://raw.gitcode.com/user-images/assets/7630596/1d0662c8-2c92-4feb-81ee-2eb6523bfe6b/image.png 'image.png')
 
 ![image.png](https://raw.gitcode.com/user-images/assets/7630596/f458913e-6886-4a95-bd0d-1342b355444b/image.png 'image.png')
 
 如上图所示，表示注册成功，即可拨打电话

# 技术咨询与支持

![二维码](https://github.com/proudwolf/wixin_erweima/blob/main/%E5%BE%AE%E4%BF%A1%E4%BA%8C%E7%BB%B4%E7%A0%81.png)
