#ifndef _TCPSOCK_H_
#define _TCPSOCK_H_

#include <bits/stdc++.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <unistd.h>

#define middleware(f) if (f) return

namespace serv {
  extern int svfd, clfd; // server socket, client socket
  extern int opt;  // setsockopt
  extern struct sockaddr_in addr; // bind
  extern std::string static_file; // static file css, image, js

  void _socket();
  void _setsockopt();
  void _bind();
  void _listen();
  void _accept();
  void _close();
}

#endif // _TCPSOCK_H_ 