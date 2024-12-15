#include "httprequest.hpp"

HttpRequest::HttpRequest(): _method(INVALID), _version(UNKNOWN)
{

}

void HttpRequest::setVersion(HttpRequest::version v)
{
  _version =v;
}

//传入的参数仅仅是请求方法get,post等的字符串地址
bool HttpRequest::setMethod(const char *start, const char *end)
{
  //传入参数为1继续，不为1abort退出程序
  assert(_method == INVALID);
  string m(start, end);
  if (m == "GET")
  {
    _method = GET;
  }
  else if (m == "POST")
  {
    _method = POST;
  }
  else if (m == "HEAD")
  {
    _method = HEAD;
  }
  else if (m == "PUT")
  {
    _method = PUT;
  }
  else if (m == "DELETE")
  {
    _method = DELETE;
  }
  else
  {
    _method = INVALID;
  }
  return _method != INVALID;
}


void HttpRequest::setPath(const char *start, const char *end)
{
  //assign函数将一系列同类型值赋值（替换）给容器（高效？）
  //与直接使用string逻辑一样，效率不清楚
  _path.assign(start, end);
}

void HttpRequest::setQuery(const char *start, const char *end)
{
  _query.assign(start, end);
}

void HttpRequest::setTime(Timestamp t)
{
  _time = t;
}

void HttpRequest::addHeader(const char *start, const char *colon, const char *end)
{
  string field(start, colon);
  ++colon;
  while (colon < end && isspace(*colon))
  {
      ++colon;
  }
  string value(colon, end);
  //while循环用来去掉后导的空格
  while (!value.empty() && isspace(value[value.size()-1]))
  {
      value.resize(value.size()-1);
  }
  _headers[field] = value;//将解析出来的头信息放入map中
}

HttpRequest::version HttpRequest::getVersion() const
{
  return _version;
}

string HttpRequest::getHeader(const string &field) const
{
  string res;
  std::map<string, string>::const_iterator it = _headers.find(field);
  if (it != _headers.end())
  {
    //类数组映射方式只能放在左值，右值使用迭代器
    res = it->second;
  }
  return res;
}

HttpRequest::method HttpRequest::getMethod() const
{
  return _method;
}

const string& HttpRequest::getpath() const
{
  return _path;
}

const string & HttpRequest::getquery() const
{
  return _query;
}

Timestamp HttpRequest::getTime() const
{
  return _time;
}

const std::map<string, string>& HttpRequest::headers() const
{
  return _headers;
}

void HttpRequest::swap(HttpRequest& that)
{
  std::swap(_method, that._method);
  _path.swap(that._path);
  _query.swap(that._query);
  _time.swap(that._time);
  _headers.swap(that._headers);
}

//getmethod的第二种方法：返回的是字符串
const char* HttpRequest::methodString() const
{
  const char* result = "INVALID";
  switch(_method)
  {
  case GET:
      result = "GET";
      break;
  case POST:
      result = "POST";
      break;
  case HEAD:
      result = "HEAD";
      break;
  case PUT:
      result = "PUT";
      break;
  case DELETE:
      result = "DELETE";
      break;
  default:
      break;
  }
  return result;
}