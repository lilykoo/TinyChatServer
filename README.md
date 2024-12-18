# TinyChatServer
1.介绍：
    基于muduo库，使用nginx 进行tcp负载均衡的集群聊天服务器，并构建了简单的客户端用于测试；每个服务器支持百万的消息并发和两万左右的用户连接，在nignx负载均衡下支持更大的用户接入

2.环境搭建：
    2.1  ubuntu 22.04 
    
    2.2  muduo库 boost库 ：参考博客https://blog.csdn.net/QIANGWEIYUAN/article/details/89023980
    boost库安装：apt-get install libboost-all-dev
    
    2.3  (数据库必须的依赖库，根据提示安装)
    mysql 5.7.17 server:
    手动安装，apt-get install自动下载8.0版本 
    mysql 5.7.17 client 
    
    2.4   redis-server + hiredis:
    （1）安装redis-serever  （2）git clone https://github.com/redis/hiredis (3) make && make install (4) sudo ldconfig /usr/local/lib 添加到环境变量中
    
    2.5   nginx-1.14.2（官网下载）：
    （1）安装前：./configure --with-stream(支持TCP数据流)   （2）安装: make && make install  （3）安装后:修改conf/nginx.conf 添加{stream}块，在{upstream Myserver}中配置使用到的服务器的服务器IP：端口号，以及保持心跳的相关参数; 在server中配置负载均衡服务器的参数（主要是端口相关参数）  （4）最后启动nginx服务（可执行文件nginx）
  
    2.6   cmake版本 3.22.1   c++版本 11.04

3.模块解读：（暂无）



4.编译方式：
    运行脚本文件 ./autobuild.sh


5.运行
cd bin
./ChatServer :运行服务器
./ChatClient :运行客户端
