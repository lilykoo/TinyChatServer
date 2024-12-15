#include "httpcontext.hpp"

  HttpContext::HttpContext(): _state(kExpectRequestLine)
  {}

  //解析请求行
  bool HttpContext::processRequestLine(const char* begin, const char* end)
  {
    bool succeed=false;
    const char*start=begin;
    const char*space = std::find(start,end,' ');
    //设置请求方法//method_
    if(space!=end&&_request.setMethod(start,space))
    {
        start=space+1;
        space = std::find(start,end,' ');
        if(space !=end)
        {
            //解析URI
            const char* question = std::find(start, space, '?');
            if (question != space)
            {
                _request.setPath(start, question);
                _request.setQuery(question, space);
            }
            else
            {
                _request.setPath(start, space);
            }
            //解析HTTP版本号
            start = space+1;
            succeed = end-start == 8 && std::equal(start, end-1, "HTTP/1.");
            if (succeed)
            {
                if (*(end-1) == '1')
                {
                    _request.setVersion(HttpRequest::HTTP11);
                }
                else if (*(end-1) == '0')
                {
                    _request.setVersion(HttpRequest::HTTP10);
                }
                else
                {
                    succeed = false;
                }
            }//endif
        }//endif
    }//endif
    return succeed;
  }

  //解析请求头:包含请求行 
  bool HttpContext::parseRequest(muduo::net::Buffer *buf, Timestamp time)
  {
    bool ok= true;
    bool hasMore=true;
    while(hasMore)
    {
        //解析请求行
        if(_state==kExpectRequestLine)
        {
            //
            const char*crlf=buf->findCRLF();
            if(crlf)
            {
                //开始解析请求行
                ok=processRequestLine(buf->peek(),crlf);
                if(ok)
                {
                    //解析成功
                    _request.setTime(time);
                    //回收请求行buffer
                    buf->retrieveUntil(crlf+2);
                    _state=kExpectHeaders;
                }
                else
                {
                    hasMore=false;
                }
            }
            else
            {
                hasMore=false;
            }
        }
        //解析请求头
        else if(_state==kExpectHeaders)
        {
            const char* crlf = buf->findCRLF();
            if (crlf)
            {
                //冒号
                const char* colon = std::find(buf->peek(), crlf, ':');
                if (colon != crlf)
                {
                    _request.addHeader(buf->peek(), colon, crlf);
                }
                else
                {
                // empty line, end of header
                // FIXME:
                    _state = kGotAll;
                    hasMore = false;
                }
                buf->retrieveUntil(crlf + 2);//回收
            }
            else
            {
                hasMore = false;
            }
        }
        else if(_state==kExpectBody)
        {
            // FIXME:
        }
    }//endwhile
    return ok;
  }

  bool HttpContext::gotAll() const
  {
    return _state == kGotAll; 
  }

  void HttpContext::reset()
  {
    _state = kExpectRequestLine;
    HttpRequest dummy;
    _request.swap(dummy);
  }

  const HttpRequest & HttpContext::getRequest() const
  {
    return _request;
  }

HttpRequest& HttpContext::getRequest()
  {
    return _request;
  }