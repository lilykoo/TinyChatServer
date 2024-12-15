#ifndef PUBLIC_H
#define PUBLIC_H
/*
server和client的公共文件
*/
enum EnMsgType
{
  LOGIN_MSG = 1,    //登录消息
  LOGIN_MSG_ACK,
  LOGOUT_MSG,
  LOGOUT_MSG_ACK,
  REG_MSG,         //注册消息
  REG_MSG_ACK,
  ONE_CHAT_MSG,   //聊天消息{"msgid":5,"id":2,"from":"zhang san","to":3,"msg":"I HATE YOU!"}
  ADD_FRIEND_MSG, //添加好友

  CREATE_GROUP_MSG, //创建群组
  CREATE_GROUP_MSG_ACK,
  ADD_GROUP_MSG, //加入群组
  ADD_GROUP_MSG_ACK,
  GROUP_CHAT_MSG //群聊天
  
};


#endif