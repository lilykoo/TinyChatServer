#ifndef DB_H
#define DB_H

#include <mysql/mysql.h>
#include <string>
#include <muduo/base/Logging.h>
using namespace std;
// 数据库配置信息
static string server = "127.0.0.1";
static string user = "chat";
static string password = "010605";
static string dbname = "chat";
// 数据库操作类
class MySQL
{
public:
  // 初始化数据库连接
  MySQL();
  // 释放数据库连接资源
  ~MySQL();
// 连接数据库
  bool connect();
// 更新操作
  bool update(string sql);
// 查询操作
  MYSQL_RES* query(string sql);
//获取连接 
  MYSQL* getConnection();
private:
  MYSQL *_conn;
};

#endif