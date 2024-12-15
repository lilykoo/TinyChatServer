#include "usermodel.hpp"
#include "db.h"
#include <string>
#include <iostream>
using namespace std;

bool UserModel::insert(User &user)
{
  //1.组装sql语句
  char sql[1024] = {0};
  sprintf(sql, "insert into User(name,password,state) values('%s', '%s', '%s')", user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());

  MySQL mysql;
  if(mysql.connect())
  {
    if(mysql.update(sql)) {
      //获取插入成功数据生成的主键id
      user.setId(mysql_insert_id(mysql.getConnection()));
      return true;
    }
  }
  return false;
}

User UserModel::query(int id)
{
  //1.组装sql语句
  char sql[1024] = {0};
  sprintf(sql, "select * from User where id = %d", id);

  MySQL mysql;
  if(mysql.connect())
  { 
    MYSQL_RES* res = mysql.query(sql);
    if(res != nullptr) 
    {
      //成功读取数据，id存在否还需判断
      MYSQL_ROW row = mysql_fetch_row(res);
      if (row != nullptr)
      {
        User user;
        //字符串转整数atoi，数据库存储的内容是字符串
        user.setId(atoi(row[0]));
        user.setName(row[1]);
        user.setPassword(row[2]);
        user.setState(row[3]);
        return user;
      }
    }
    mysql_free_result(res);
  }
  return User();
}

bool UserModel::update(User &user)
{
  char sql[1024] = {0};
  sprintf(sql, "update User	set state = '%s' where id = %d", user.getState().c_str(), user.getId());
  
  MySQL mysql;
  if(mysql.connect())
  {
    if(mysql.update(sql)) 
    {
      return true;
    }
  }
  return false;
}

void UserModel::resetState()
{
  char sql[1024] = {0};
  sprintf(sql, "update User	set state = 'offline' where state = 'online'");
  
  MySQL mysql;
  if(mysql.connect())
  {
    mysql.update(sql);
  }
}

void UserModel::updateState(User &user)
{
  char sql[1024] = {0};
  sprintf(sql, "update User	set state = '%s' where id = '%d'", user.getState().c_str(), user.getId());
  
  MySQL mysql;
  if(mysql.connect())
  {
    mysql.update(sql);
  }
}