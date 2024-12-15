#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include "group.hpp"
#include "db.h"
#include <string>
using namespace std;

class GroupModel 
{
public:
  //创建群组
  int createGroup(Group &group, int creator_id);
  //加入群组
  bool addGroup(int userid, int groupid);
  //查询用户所在群组信息
  vector<Group> queryGroups(int userid);
  //查询特定用户所在特定组内的成员信息，主要用于群发消息
  vector<int> queryGroupUsers(int userid,int groupid); 
private:
};

#endif