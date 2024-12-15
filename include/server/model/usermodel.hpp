#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.hpp"

class UserModel 
{
public:
  // User表增加方法
  bool insert(User &user);
  User query(int id);
  bool update(User &user);
  void resetState();
  void updateState(User &user);
private:
};

#endif