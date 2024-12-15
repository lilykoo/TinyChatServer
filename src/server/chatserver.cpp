#include "chatserver.hpp"
#include "chatservice.hpp"
#include "json.hpp"
#include <functional>
#include <string>
using namespace std;
using namespace placeholders;
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop *loop,
                      const InetAddress &listenAddr,
                      const string &nameArg)
  :_server(loop, listenAddr, nameArg)
{
  //注册链接回调
  _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

  //注册消息回调
  _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

  _server.setThreadNum(4);
}

void ChatServer::start()
{
  _server.start();
}

void ChatServer::onConnection(const TcpConnectionPtr &conn) 
{
  //用户断开连接
  if(!conn->connected()) 
  {
    ChatService::instance()->clientCloseException(conn);
    conn->shutdown(); //释放conn和socket
  }
}

void ChatServer::onMessage(const TcpConnectionPtr &conn,
                          Buffer *buffer,
                          Timestamp time) 
{
  string buf = buffer->retrieveAllAsString();
  //数据反序列化
  json js = json::parse(buf);
  //get方法将json的字符串或者其他类型转化成整型
  auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());
  msgHandler(conn, js, time);
}