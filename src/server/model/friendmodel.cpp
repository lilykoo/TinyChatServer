#include "friendmodel.hpp"

//添加好友关系（具体业务C++用其他语言更合适）
void FriendModel::insert(int userid, int friendid)
{
  char sql[1024] = {0};
  sprintf(sql, "insert into Friend(userid,friendid) values(%d, %d) ", userid, friendid);
  MySQL mysql;
  if(mysql.connect())
  {
    mysql.update(sql);
  }
}


//返回用户好友列表，使用离线消息方法展示（本地存储方法）
vector<User> FriendModel::query(int userid)
{
  char sql[1024] = {0};
  //friendid的用户无法收到好友列表
  sprintf(sql, "select a.id,a.name,a.state from User a inner join Friend b on b.friendid = a.id where b.userid= %d union select c.id,c.name,c.state from User c inner join Friend d on d.userid = c.id where d.friendid= %d", userid, userid);

  vector<User> ans;
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
        User user;
        user.setId(atoi(row[0])); //自动转换
        user.setName(row[1]);
        user.setState(row[2]); 
        ans.push_back(user);
      }
      mysql_free_result(res);
      return ans;
    }
    mysql_free_result(res);
  }
  return ans;
}