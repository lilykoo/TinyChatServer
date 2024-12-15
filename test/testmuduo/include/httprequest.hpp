#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H
#include <iostream>
#include <muduo/base/Timestamp.h>
#include <string>
#include <map>
using namespace std;
using namespace muduo;

class HttpRequest
{
public:
  enum method
  {
    INVALID,
    GET,
    POST,
    HEAD,
    PUT,
    DELETE
  };
  enum version
  {
    UNKNOWN,
    HTTP10,
    HTTP11
  };
  HttpRequest();

  void setVersion(version v);
  bool setMethod(const char *start, const char *end);
  void setPath(const char *start, const char *end);
  void setQuery(const char *start, const char *end);
  void setTime(Timestamp t);
  void addHeader(const char *start, const char *colon, const char *end);

  version getVersion() const;
  method getMethod() const;
  const string& getpath() const;
  const string & getquery() const;
  Timestamp getTime() const;
  const std::map<string, string>& headers() const;
  void swap(HttpRequest& that);
  //按照请求头的映射关系提取
  string getHeader(const string &field) const;

  //没搞懂为什么
  const char* methodString() const;

  
  
private:
  method _method;
  version _version;
  string _path;
  string _query;
  Timestamp _time;
  map<string, string> _headers;
};


#endif