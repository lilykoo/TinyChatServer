#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H

#include "user.hpp"
#include "db.h"
#include <vector>

class FriendModel
{
public:
  //添加好友关系（具体业务C++用其他语言更合适）
  void insert(int userid, int friendid);
  //返回用户好友列表（本地存储方法）
  vector<User> query(int userid);
};

#endif