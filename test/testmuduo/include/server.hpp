/*
muduo网络库给用户提供两个主要类
TcpServer:用于编写服务器程序
TcpClient:用于编写客户端程序

epoll + 线程池
好处：把网络I/O的代码和业务代码区分开
                可视（暴露）/关注事件：用户的连接和断开 用户的可读写事件（怎么进行的已经封装）
*/
#ifndef SERVER_H
#define SERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <string>
#include <functional>
#include "httpcontext.hpp"
#include "httprequest.hpp"
#include "httpresponse.hpp"
using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

/*基于muduo网络库开发服务器程序
  1.组合TcpServer对象
  2.创建EventLoop事件循环指针(用于关闭整个服务器程序，以及注册相关事件（同步异步）)
  3.明确TcpServer构造函数需要什么参数（默认构造以及重载），输出ChatServer的构造函数（大类构造函数参数为外传入参数）
  4.先注册处理连接的回调函数，和处理读写的回调函数（此时只有单线程）
  5.设置服务器的线程数量
*/
class Server 
{
  public:
    typedef function<void (const HttpRequest &, HttpResponse *)> httpCallback;

    Server(EventLoop* loop,
        const InetAddress& listenAddr,
        const string& nameArg)
      :_server(loop, listenAddr, nameArg),_loop()
    {
        //给服务器注册用户连接的创建和断开回调（发生时间和发生后业务分开（后者我们可以控制））
        _server.setConnectionCallback(std::bind(&Server::onConnection, this, _1));
        
        //给服务器注册用户读写事件回调
        _server.setMessageCallback(std::bind(&Server::onMessage, this, _1, _2, _3));

        //设置服务器线程数量4:1个I/O线程，3个work(I/O)线程
        _server.setThreadNum(4);
    }

    //开启事件循环
    void start() {
      _server.start();
    }

    void setHttpCallback(const httpCallback& cb)
    {
      _httpCallback = cb;
    }
    
  private:
    //I/O:处理用户连接创建和断开
    void onConnection(const TcpConnectionPtr &conn) 
    {
      if(conn->connected()) 
      {
        conn->setContext(HttpContext());
      }
      else
      {
        conn->shutdown(); //close(fd)
        //_loop->quit();
      }
      
    }

    void onMessage(const TcpConnectionPtr &conn, //连接对象
                  muduo::net::Buffer *buffer,                  //缓冲区
                  Timestamp time)                //接收到数据的时间戳
    {
      HttpContext*context=boost::any_cast<HttpContext>(conn->getMutableContext());
      //解析请求
      if(!context->parseRequest(buffer,time))
      {
          conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
          conn->shutdown();
      }
      if (context->gotAll())//state_==gotALL
      {
      //请求已经解析完毕
          onRequest(conn, context->getRequest());

          context->reset();//context reset
      }
    }

    void onRequest(const TcpConnectionPtr& conn,const HttpRequest& req)
    {
      const string& connection = req.getHeader("Connection");
      bool close = connection == "close" ||
      (req.getVersion() == HttpRequest::HTTP10 && connection != "Keep-Alive");
      HttpResponse response(close);//构造响应
      _httpCallback(req, &response);//用户回调
      muduo::net::Buffer buf;
      //此时response已经构造好，将向客户发送Response添加到buffer中
      response.appendToBuffer(&buf);
      conn->send(&buf);
      //如果非Keep-Alive则直接关掉
      if (response.getCloseConnection())
      {
          conn->shutdown();
      }
    }

    TcpServer _server;
    EventLoop *_loop;
    httpCallback _httpCallback;
};

#endif