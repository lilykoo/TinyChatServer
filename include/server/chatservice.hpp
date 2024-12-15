#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <muduo/base/Logging.h>
#include <mutex>
#include <vector>
#include "json.hpp"
#include "public.hpp"
#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"
#include "groupmodel.hpp"
#include "redis.hpp"
using namespace std;
using namespace muduo;
using namespace muduo::net;
using json = nlohmann::json;

using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp time)>;

//聊天服务器业务模块类
class ChatService
{
public:
  //单例模式唯一接口
  static ChatService* instance();
  //处理登录业务
  void log(const TcpConnectionPtr &,
            json &,
            Timestamp);

  //处理登出业务
  void logout(const TcpConnectionPtr &,
          json &,
          Timestamp);
  //处理注册业务
  void reg(const TcpConnectionPtr &,
          json &,
          Timestamp);
  //获取消息对应的处理器
  MsgHandler getHandler(int msgid);

  //用户conn断开时的业务，数据处理
  void clientCloseException(const TcpConnectionPtr &conn);
  
  //一对一聊天
  void one2oneChat(const TcpConnectionPtr &,
                  json &,
                  Timestamp);

  //异常退出时的（收到ctr+c）reset函数:把online变成offline
  void reset();


  //添加好友
  void addFriend(const TcpConnectionPtr &,
                  json &,
                  Timestamp);

  //创建群聊    
  void createGroup(const TcpConnectionPtr &,
                  json &,
                  Timestamp);

  //加入群聊
  void addGroup(const TcpConnectionPtr &,
                  json &,
                  Timestamp);



  //群聊
  void groupChat(const TcpConnectionPtr &,
                json &,
                Timestamp);

  void handleRedisSubscribeMsg(int id, string msg);
private:
  //单例模式私有化构造函数
  ChatService();

  //存储消息id和其对应事件业务处理方法，即使访问也不会改变，不必考虑线程安全问题
  unordered_map<int, MsgHandler> _msgHandlerMap;

  //消息-handler,map初始化
  void map_ini();

  //频繁使用栈内存：局部变量，成员变量
  //数据库User表操作对象
  UserModel _usermodel;
  //数据库offlinemessage表操作对象
  OfflineMsgModel _offmsgmodel;
  //数据库Friend表操作对象
  FriendModel _friendmodel;
  //数据库Group操作对象
  GroupModel _groupmodel;

  //redis操作对象
  Redis _redis;

  //定义互斥锁保证_userConnMap线程安全，利用智能指针自动释放锁，锁力度太大，并行变串行 
  mutex _connMutex;

  //存储在线用户的通信连接：知道用户的登录状态，也知道用户的连接状态
  unordered_map<int, TcpConnectionPtr> _userConnMap;

};


#endif