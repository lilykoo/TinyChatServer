#include "offlinemessagemodel.hpp"
#include <iostream>
using namespace std;

void OfflineMsgModel::insert(int userid, string msg)
{
  char sql[1024] = {0};
  sprintf(sql, "insert into offlinemessage(userid,message) values(%d, '%s') ", userid, msg.c_str());
  MySQL mysql;
  if(mysql.connect())
  {
    mysql.update(sql);
  }
}

//删除
void OfflineMsgModel::remove(int userid)
{
  char sql[1024] = {0};
  sprintf(sql, "delete from offlinemessage where userid = %d ", userid);
  MySQL mysql;
  if(mysql.connect())
  {
    mysql.update(sql);
  }
}

//查询
vector<string> OfflineMsgModel::query(int userid)
{
  //1.组装sql语句
  char sql[1024] = {0};
  sprintf(sql, "select message from offlinemessage where userid = %d", userid);

  vector<string> ans;
  MySQL mysql;
  if(mysql.connect())
  { 
    MYSQL_RES* res = mysql.query(sql);
    if(res != nullptr) 
    {
      //多行msg的读取
      MYSQL_ROW row;
      while ((row = mysql_fetch_row(res)) != nullptr)
      {
        ans.push_back(row[0]);
      }
      mysql_free_result(res);
      return ans;
    }
    mysql_free_result(res);
  }
  return ans;
}