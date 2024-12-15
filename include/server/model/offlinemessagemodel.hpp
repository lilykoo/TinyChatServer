#ifndef OFFLINEMESSAGEMODEL_H
#define OFFLINEMESSAGEMODEL_H
#include <string>
#include <vector>
#include "db.h"

using namespace std;

class OfflineMsgModel
{
public:
  //存储用户离线消息
  void insert(int userid, string msg);

  //删除
  void remove(int userid);

  //查询
  vector<string> query(int userid);
};

#endif