#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <iostream>
#include <string>
#include <muduo/base/copyable.h>
#include <iostream>
#include <string>
#include <map>
#include <muduo/net/Buffer.h>
using namespace std;
using namespace muduo;
using namespace muduo::net;

class HttpResponse
{
public:
  enum HttpStatusCode
  {
      CODE_UNKNOWN,
      CODE_200=200,
      CODE_301=301,
      CODE_400=400,
      CODE_404=404
  };
  explicit HttpResponse(bool close)
      : _statusCode(CODE_UNKNOWN),
      _closeConnection(close)
  {
  }
  void setStatusCode(HttpStatusCode code);
  void setStatusMessage(const string&message);
  void setCloseConnection(bool on);
  void setContentType(const string&contentType);
  void setBody(const string &body);

  bool getCloseConnection()const;
  void addHeader(const string&key,const string&value);
  void appendToBuffer(muduo::net::Buffer *output) const;

private:
  //响应头
  std::map<string,string> _headers;
  //响应码
  HttpStatusCode _statusCode;
  //状态信息
  string _statusMessage;
  //是否 keep-alive
  bool _closeConnection;
  //响应报文
  string _body;
};


#endif