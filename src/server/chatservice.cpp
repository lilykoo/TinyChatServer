#include "chatservice.hpp"

ChatService* ChatService::instance()
{
  static ChatService service;
  return &service;
}

/*（1）直接放入msgid和函数名：不可，函数名不可以作为变量，函数指针才可以，但是函数指针使用需要bind
（2）msgid+函数指针*/
void ChatService::map_ini()
{
  _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::log, this, _1, _2, _3)});
  _msgHandlerMap.insert({LOGOUT_MSG, std::bind(&ChatService::logout, this, _1, _2, _3)});
  _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
  _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::one2oneChat, this, _1, _2, _3)});
  _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3)});
  _msgHandlerMap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3)});
  _msgHandlerMap.insert({ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});
  _msgHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});
}

//获取msgid对应的Handler
MsgHandler ChatService::getHandler(int msgid)
{ 
  //记录错误日志，msgid没有对应事件处理回调
  auto it = _msgHandlerMap.find(msgid);
  if (it == _msgHandlerMap.end())
  { 
    //不用加endl
    //网络模块需要处理服务模块抛出的异常,但是返回默认的函数指针后，不会让程序出错
    return [=](const TcpConnectionPtr &conn, json &js, Timestamp time)
    {
      LOG_ERROR << "msgid:" << msgid << "cannot find handler!";
    };
  }
  else 
  {
    return _msgHandlerMap[msgid];
  }
}

//注册消息以及对应的Handler回调函数
ChatService::ChatService()
{
  map_ini();
  if (_redis.connect())
  {
    _redis.init_notify_handler(std::bind(&ChatService::handleRedisSubscribeMsg, this, _1, _2));
  }
}

//处理登录业务
void ChatService::log(const TcpConnectionPtr &conn,
                      json &js,
                      Timestamp time)
{ 
  int id = js["id"].get<int>();
  // string name = js["name"];
  string pwd = js["password"];
  User user = _usermodel.query(id);
  if(user.getId() != -1 && user.getPwd() == pwd)
  {
    if(user.getState() == "online") 
    {
      //无需重复登录
      json response;
      response["msgid"] = LOGIN_MSG_ACK;
      response["errno"] = 2;
      response["errmsg"] = "该账号已经登录，请重新输入新账号！";
      response["id"] = user.getId();
      response["name"] = user.getName();
      conn->send(response.dump());
    }
    else
    { 
      //登录成功，记录用户通信连接
      // _userConnMap[id] = conn;
      //后续这个map会被多个线程访问（并改变），要考虑线程安全问题
      {  
        lock_guard<mutex> lock(_connMutex);
        _userConnMap.insert({id, conn});
      }

      //登陆成功，向redis订阅channel(id)
      _redis.subscribe(id);

      //登录成功，更新用户状态信息 state offline->online，面向数据库的更新
      user.setState("online"); //集群
      bool state = _usermodel.update(user);

      if (state)
      {
        //局部变量，线程栈隔离不冲突
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        response["name"] = user.getName(); 
        //本地记录信息，不然数据库压力太大
        //此处不调用writecallback直接使用conn->send()

        //查询offlinemessage消息
        vector<string> msg = _offmsgmodel.query(id);
        if(!msg.empty())
        {
          response["offlinemsg"] = msg;
          //读取完后删除离线消息
          _offmsgmodel.remove(id);
        }

        //上线后查看好友列表
        vector<User> fri_list = _friendmodel.query(id); 
        if(!fri_list.empty())
        {
          vector<string> frikist_ackmsg;
          for(User &user: fri_list)
          {
            json js;//使用json.dump将对象obj转化成string
            js["id"] = user.getId();
            js["name"] = user.getName();
            js["state"] = user.getState();
            frikist_ackmsg.push_back(js.dump());
          }
          response["friends"] = frikist_ackmsg; 
        }

        //上线后查看群列表
        vector<Group> groups;
        groups = _groupmodel.queryGroups(id);
        if (!groups.empty())
        {
          vector<string> vec;
          for(Group &group: groups)
          {
            json temp;
            temp["id"] = group.getId();
            temp["groupname"] = group.getName();
            temp["groupdesc"] = group.getDesc();
            vector<GroupUser> groupusers = group.getUsers();
            vector<string> groupusers_str;
            if (!groupusers.empty())
            {
              for (auto &groupuser: groupusers)
              {
                json temp1;
                temp1["id"] = groupuser.getId();
                temp1["name"] = groupuser.getName();
                temp1["state"] = groupuser.getState();
                temp1["role"] = groupuser.getRole();
                groupusers_str.push_back(temp1.dump());
              }
              temp["groupusers"] = groupusers_str;
            }
            vec.push_back(temp.dump());
          }
          response["groups"] = vec;
        }
        conn->send(response.dump());
      }
    }
  }
  else
  {
    json response;
    response["msgid"] = LOGIN_MSG_ACK;
    response["errno"] = 1;
    response["errmsg"] = "登录密码出错，请重新输入新密码！";
    conn->send(response.dump());
  }
}

//登出
void ChatService::logout(const TcpConnectionPtr &conn,
                      json &js,
                      Timestamp time)
{
  int id = js["id"];
  lock_guard<mutex> lock(_connMutex);
  {
    auto it = _userConnMap.find(id);
    if (it != _userConnMap.end())
    {
      _userConnMap.erase(it);
    }
  }
  _redis.unsubscribe(id);
  // 更新用户的状态信息
  User user(id, "", "", "offline");
  _usermodel.updateState(user);
}


//处理注册业务
void ChatService::reg(const TcpConnectionPtr &conn,
                      json &js,
                      Timestamp time)
{ 
  string name = js["name"];
  string pwd = js["password"];
  User user;
  user.setName(name);
  user.setPassword(pwd);
  bool state = _usermodel.insert(user);
  if (state)
  {
    json response;
    response["msgid"] = REG_MSG_ACK;
    response["errno"] = 0;
    response["id"] = user.getId();
    //此处不调用writecallback直接使用conn->send()
    conn->send(response.dump());
  }
  else
  {
    json response;
    response["msgid"] = REG_MSG_ACK;
    response["errno"] = 1;
    conn->send(response.dump());
  }
}


//客户端连接断开时的异常处理
void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
  User user; 
  {
    lock_guard<mutex> lock(_connMutex);
    //使用迭代器进行删除erase(iter)
    for(auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
    {
      if(it->second == conn)
      {
        user.setId(it->first);
        _userConnMap.erase(it);
        break;
      }
    }
  }

  //连接和用户一一对应，客户端退出，也要退订
  _redis.unsubscribe(user.getId());

  //更新用户状态：如果不判断数据库日志报错，多进行数据库磁盘IO操作；找的话无需进行磁盘IO
  if(user.getId() != -1)
  {
    user.setState("offline");
    _usermodel.update(user);
  }

}


//一对一聊天服务
void ChatService::one2oneChat(const TcpConnectionPtr &conn,json &js,Timestamp time)
{ 
  //先判断用户是否在线，使用业务层的中间变量，减少数据库IO次数，同时减少锁的使用数量
  int to_id = js["toid"].get<int>();
  // string msg = js["msg"];
  {
    lock_guard<mutex> lcok(_connMutex);
    auto it = _userConnMap.find(to_id);
    if (it != _userConnMap.end())
    {
      //toid在线，转发消息，转发的消息格式内容还要改进
      it->second->send(js.dump());
    }
  }

  //toid不在线，存储消息：用户没在这个主机登录，可能在其他电脑登录（集群）
  //此时直接查数据库，查看是否有该用户是否在线
  User user = _usermodel.query(to_id);

  //集群服务器上在线，字符串使用双引号
  if (user.getState() == "online")
  {
    _redis.publish(to_id, js.dump());
    return ;
  }

  _offmsgmodel.insert(to_id, js.dump());
}


//重置用户信息
void ChatService::reset()
{
  //此处以偏概全，需要得到这个服务器上的conn信息
  //server回调service，操作_userConnMap，数据库，将用户登录在其他服务器，再开一个独立线程（要阻塞等待信号量）
  _usermodel.resetState();
}


//添加好友服务
void ChatService::addFriend(const TcpConnectionPtr &conn,
                  json &js,
                  Timestamp time)
{
  int userid = js["id"].get<int>();
  int friendid = js["friendid"].get<int>();
  _friendmodel.insert(userid,friendid);
}


//创建群聊服务
void ChatService::createGroup(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
  int id = js["id"].get<int>();
  string group_name = js["groupname"];
  string group_desc = js["groupdesc"];
  Group group(-1, group_name, group_desc);
  // group.setName(group_name);
  // group.setDesc(group_desc);
  int state = _groupmodel.createGroup(group, id);
  //响应
  json response;
  switch(state)
  {
    case 1: 
      response["msgid"] = CREATE_GROUP_MSG_ACK;
      response["errno"] = 0;
      response["ack"] = "you have already created the group!";
      //此处不调用writecallback直接使用conn->send()
      conn->send(response.dump());
      break;
    case 0:
      response["msgid"] = CREATE_GROUP_MSG_ACK;
      response["errno"] = 1;
      response["ack"] = "create group failed!";
      //此处不调用writecallback直接使用conn->send()
      conn->send(response.dump());
      break;
    default: break;
  }
}

//加入群聊
void ChatService::addGroup(const TcpConnectionPtr &conn,json &js,Timestamp time)
{ 

  int userid = js["id"].get<int>();
  int groupid = js["groupid"].get<int>();
  bool state = _groupmodel.addGroup(userid, groupid);
  json response;
  if (state)
  {
    response["msgid"] = ADD_GROUP_MSG_ACK;
    response["errno"] = 0;
    response["ack"] = "you have already participated the group!";
    conn->send(response.dump()); 
  }
  else
  {
    response["msgid"] = ADD_GROUP_MSG_ACK;
    response["errno"] = 1;
    response["ack"] = " participat the group failed!";
    conn->send(response.dump()); 
  }
}


//群聊
void ChatService::groupChat(const TcpConnectionPtr &conn,
                            json &js,
                            Timestamp time)
{
  int userid = js["id"].get<int>();
  int groupid = js["groupid"].get<int>();

  vector<int> groupuser_id = _groupmodel.queryGroupUsers(userid, groupid);

  lock_guard<mutex> lock(_connMutex);
  for (int id: groupuser_id)
  {
    //json格式不同修改起来比较麻烦并且在大量消息需要传输的情况下不合适
    // one2oneChat(conn, js, time);
    if (_userConnMap.find(id) != _userConnMap.end())
    {
      _userConnMap[id]->send(js.dump());
    }
    else
    { 
      User user = _usermodel.query(id);
      if (user.getState() == "online")
      {
        _redis.publish(id, js.dump());
      }

      _offmsgmodel.insert(id, js.dump());
    }
  } 
}

void ChatService::handleRedisSubscribeMsg(int id, string msg)
{
  json js = json::parse(msg.c_str());

  {
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(id);
    if (it != _userConnMap.end())
    {
      it->second->send(msg);
      return ;
    }
  }

  _offmsgmodel.insert(id, msg);
}