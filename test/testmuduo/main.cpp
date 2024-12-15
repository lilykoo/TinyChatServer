#include "server.hpp"
#include "httprequest.hpp"
#include "httpresponse.hpp"
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>
#include "util.hpp"

#include <iostream>
#include <map>
#include <string>
using namespace std;


void onRequest(const HttpRequest& req, HttpResponse* resp)
{
    string path=Util::ConstructPath(req.getpath());
    string extent=Util::GetExtent(path);
    string contentType="";
    Util::GetContentType(extent,contentType);
    string content=Util::GetContent(path);
    if(content.empty())
    {
        resp->setStatusCode(HttpResponse::CODE_404);
        resp->setStatusMessage("Not Found");
    }
    else
    {
        resp->setBody(content);
        resp->setStatusCode(HttpResponse::CODE_200);
        resp->setStatusMessage("OK");
        resp->setContentType(contentType);
    } 
}

int main()
{
  muduo::net::EventLoop loop;
  Server server(&loop, muduo::net::InetAddress("127.0.0.1",8000),"Server");
  server.setHttpCallback(onRequest);
  server.start();
  loop.loop();
  return 0;
}
