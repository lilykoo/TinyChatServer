#ifndef GROUP_H
#define GROUP_H

#include "groupuser.hpp"
#include <string>
#include <vector>

class Group
{
public:
  Group(int id = -1, string name = "", string desc = "")
  {
    _id = id;
    _name = name;
    _desc = desc;
  }
  int getId() {return _id;}
  string getName() {return _name;}
  string getDesc() {return _desc;}
  //容器能Get就能set
  vector<GroupUser> &getUsers() {return _users;}
  
  void setId(int id) {_id = id;}
  void setName(string name){_name = name;}
  void setDesc(string desc) {_desc = desc;}

private:
  int _id;
  string _name;
  string _desc;
  vector<GroupUser> _users;
};

#endif