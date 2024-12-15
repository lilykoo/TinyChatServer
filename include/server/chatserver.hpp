#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>

using namespace muduo;
using namespace muduo::net;

class ChatServer
{
  public:
    //初始化聊天服务器对象
    ChatServer(EventLoop* loop,
        const InetAddress& listenAddr,
        const string& nameArg);
    
    //启动服务
    void start();
  private:
    //响应连接相关信息的回调函数
    void onConnection(const TcpConnectionPtr &);
    //响应（读写事件）消息相关信息的对调函数
    void onMessage(const TcpConnectionPtr &,
        Buffer *,
        Timestamp);
    TcpServer _server; //实现服务器底层功能接口
    EventLoop *_loop; //退出事件循环接口
};

#endif