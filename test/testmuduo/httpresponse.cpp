#include "httpresponse.hpp"

void HttpResponse::setStatusCode(HttpStatusCode code)
{
  _statusCode = code;
}

void HttpResponse::setStatusMessage(const string&message)
{
  _statusMessage = message;
}

void HttpResponse::setCloseConnection(bool on)
{
  _closeConnection = on;
}

void HttpResponse::setContentType(const string&contentType)
{
  addHeader("Content-Type", contentType);
}

void HttpResponse::setBody(const string &body)
{
  _body = body;
}

bool HttpResponse::getCloseConnection() const
{
  return _closeConnection;
}

void HttpResponse::addHeader(const string&key,const string&value)
{
  _headers[key] = value;
}

void HttpResponse::appendToBuffer(muduo::net::Buffer *output) const
{
  //构造响应行
  char buf[32];
  snprintf(buf, sizeof buf, "HTTP/1.1 %d", _statusCode);
  output->append(buf);
  output->append(_statusMessage);
  output->append("\r\n");

  if (_closeConnection)
  {
    output->append("Connection: close\r\n");
  }
  else
  {
    //Keep-Alive需要Content-Length
    snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", _body.size());
    output->append(buf);
    output->append("Connection: Keep-Alive\r\n");
  }

  for(map<string, string>::const_iterator it = _headers.begin();it != _headers.end(); ++it)
  {
    //迭代构造响应头
    output->append(it->first);
    output->append(": ");
    output->append(it->second);
    output->append("\r\n");
  }
  output->append("\r\n");
  //响应报文
  output->append(_body);
}
