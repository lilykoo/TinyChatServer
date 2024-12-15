#include "groupmodel.hpp"

//创建群组，先查后插（更多IO不妥），插入时判断插入是否合法
int GroupModel::createGroup(Group &group, int creator_id)
{
  char sql[1024] = {0};
  sprintf(sql, "insert into Allgroups(groupname,groupdesc) values('%s','%s')", group.getName().c_str(), group.getDesc().c_str());
  MySQL mysql;
  if (mysql.connect())
  {
    if (mysql.update(sql))
    {
      group.setId(mysql_insert_id(mysql.getConnection()));
      memset(sql, '\0', sizeof(sql));
      sprintf(sql, "insert into GroupUser(groupid,userid,grouprole) values(%d,%d,'creator')", group.getId(), creator_id);
      if(mysql.update(sql)) return 1;
    }
  }
  return 0;
}

//除了创建者加入群组
bool GroupModel::addGroup(int userid, int groupid)
{
  char sql[1024] = {0};
  sprintf(sql, "insert into GroupUser(groupid,userid,grouprole) values(%d,%d,'normal')", groupid,userid);
  MySQL mysql;
  if (mysql.connect())
  {
    if(mysql.update(sql))
    return true;
  }
  return false;
}

//查询用户所在群组信息
vector<Group> GroupModel::queryGroups(int userid)
{
  char sql[1024] = {0};
  sprintf(sql, "select a.id,a.groupname,a.groupdesc from Allgroups a inner join GroupUser b on b.groupid = a.id where b.userid = %d", userid);
  MySQL mysql;
  vector<Group> ans;
  if (mysql.connect())
  {
    MYSQL_RES *res = mysql.query(sql);
    if (res != nullptr)
    {
      MYSQL_ROW row;
      
      while ((row = mysql_fetch_row(res)) != nullptr)
      {
        Group group;
        group.setId(atoi(row[0]));
        group.setName(row[1]);
        group.setDesc(row[2]);
        ans.push_back(group);
      }
    }
    mysql_free_result(res);
  }

  //查询每个组用户信息
  for (Group &group : ans)
  { 
    memset(sql, '\0', sizeof(sql));
    sprintf(sql, "select a.id,a.name,a.state,b.grouprole from User a inner join GroupUser b on a.id = b.userid where b.groupid = %d", group.getId());
    MYSQL_RES *res = mysql.query(sql);
    if (res != nullptr)
    {
      MYSQL_ROW row;
      while((row = mysql_fetch_row(res)) != nullptr)
      { 
        GroupUser user;
        user.setId(atoi(row[0]));
        user.setName(row[1]);
        user.setState(row[2]);
        user.setRole(row[3]);
        group.getUsers().push_back(user);
      }
    }
    mysql_free_result(res);
  }
  return ans;
}

//查询特定用户所在特定组内的成员信息，主要用于群发消息
vector<int> GroupModel::queryGroupUsers(int userid,int groupid)
{
  char sql[1024] = {0};
  sprintf(sql, "select userid from GroupUser where groupid = %d and userid != %d", groupid, userid);
  
  vector<int> ans;
  MySQL mysql;
  if (mysql.connect())
  {
    MYSQL_RES * res = mysql.query(sql);
    if (res != nullptr)
    {
      MYSQL_ROW row;
      while((row = mysql_fetch_row(res)) != nullptr)
      {
        int userid;
        userid = atoi(row[0]);
        ans.push_back(userid);
      }
    }
    mysql_free_result(res);
  }
  return ans;
}