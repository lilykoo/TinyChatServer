#include "chatserver.hpp"
#include "chatservice.hpp"
#include <iostream>
#include <signal.h>
using namespace std;

void resetHandler(int) //信号驱动函数传实参int
{
  ChatService::instance()->reset();
  exit(0);
}

int main(int argc, char **argv)
{ 
  if (argc != 3)
  {
      cerr << "command invalid! example: ./ChatClient 127.0.0.1 6000" << endl;
      exit(-1);
  }
  // 解析通过命令行参数传递的ip和port
  char *ip = argv[1];
  uint16_t port = atoi(argv[2]);


  signal(SIGINT, resetHandler); //服务器收到ctr + C中断
  EventLoop loop;
  InetAddress addr(ip, port);
  ChatServer server(&loop, addr, "ChatServer");

  server.start();
  loop.loop();

  return 0;
}