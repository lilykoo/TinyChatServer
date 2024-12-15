#ifndef HTTPCONTEXT_H
#define HTTPCONTEXT_H

#include <muduo/net/Buffer.h>
#include "httprequest.hpp"
using namespace std;
using namespace muduo;
using namespace muduo::net;

class Buffer;
class HttpContext
{
public:
  enum HttpRequestParseState
  {
    kExpectRequestLine,
    kExpectHeaders,
    kExpectBody,
    kGotAll
  };
  HttpContext();

  //解析请求头
  bool parseRequest(muduo::net::Buffer *buf, Timestamp time);

  bool gotAll() const;

  void reset();

  const HttpRequest & getRequest() const;

  HttpRequest& getRequest();

private:
  //解析请求行
  bool processRequestLine(const char* begin, const char* end);

  HttpRequestParseState _state;
  //解析的结果将保存在request_成员中
  HttpRequest _request;
};


#endif