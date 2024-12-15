#include "json.hpp"
using json = nlohmann::json;

#include <iostream>
#include <vector>
#include <map>
#include <string>
using namespace std;

//json序列化示例:普通数据序列化
string func1() {
  json js;
  //一key一value,类似map
  js["msg_type"] = 2;
  js["from"] = "zhang";
  js["to"] = "li";
  js["msg"] = "what are you doing now?";
  cout << js << endl;
  //输出按key字典序排；链式哈希表底层
  
  string sendBuf = js.dump(); //转为字符串/字符流格式
  // cout << sendBuf.c_str() << endl;
  return sendBuf;
}

string func2() {
  json js;
  //双key一val即json对象里存放json对象
  js["id"] = {1, 2, 3, 4, 5};
  js["name"] = "zhang";
  js["msg"]["zhang"] = "hello li"; 
  js["msg"]["li"] = "hello zhang";
  js["msg"] = {{"zhang", "hello li"}, {"li", "hello zhang"}};
  // cout << js << endl;
  return js.dump();
}

//容器序列化
string func3() {
  json js;

  //vec容器序列化
  vector<int> vec;
  vec.push_back(1);
  vec.push_back(2);
  vec.push_back(3);
  js["list"] = vec;

  //map容器序列化
  map<int, string> m;
  m.insert({1, "li"});
  m.insert({2, "zhang"});
  m.insert({3, "zhao"});
  js["path"] = m;
  //数组套数组

  string sendBuf = js.dump();
  // cout << sendBuf[3] << endl;
  //从1开始存放的字符串
  return sendBuf;
} 



int main() {
  string recvBuf = func1();
  json jsbuf = json::parse(recvBuf);
  cout << jsbuf["msg_type"] << endl;
  cout << jsbuf["from"] << endl;
  cout << jsbuf["to"] << endl;
  cout << jsbuf["msg"] << endl;

  string recvBuf2 = func2();
  json jsbuf2 = json::parse(recvBuf2);
  auto arr = jsbuf2["id"]; //获取所需字段数据
  cout << arr[2] << endl;
  
  string recvBuf3 = func3();
  json jsbuf3 = json::parse(recvBuf3);
  //如果是容器直接序列化， 可以直接用对应容器接收数据，反序列化
  vector<int> vec = jsbuf3["list"];
  for (int &v : vec) {
    cout << v << " ";
  }
  cout << endl;

  map<int, string> mymap = jsbuf3["path"];
  for (auto &i : mymap) {
    cout << i.first << " " << i.second << endl;
  }
  cout << endl;
  return 0;
}