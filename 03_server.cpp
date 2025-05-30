//Yair Sanchez Banos, May 15 2025

/*
  This will be a redis clone, this file is the process of making a TCP Server
*/

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h> 
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

const size_t k_max_msg = 4096;

static int32_t one_request(int);

static void msg(const char *msg);

static void die(const char *msg);

static int32_t read_full(int, char*, size_t);

static int32_t write_all(int, char*,  size_t);

//step 6 read and write
//static void do_something(int);

int main(){
  
  //Step 1 need to obtain the socket handle
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  
  //Step 2 need to set the socket options
  int val = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
  
  //Step 3 Bind to an address
  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(1234);        //port
  addr.sin_addr.s_addr = htonl(0);    // wildcard IP 0.0.0.0
  int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
  if(rv) {
    die("bind()"); 
  }
  
  //step 4 Listen, os will handle tcp handshakes and places established
  //connections in a queue, application can then retrieve them via accept()
  rv = listen(fd, SOMAXCONN);
  if (rv){
    die("listen()");
  }
  
  //step 5 Accept Connections
  //server will enter a loop that accepts and processes each client connection
  
  while(true) {
    //accept
    struct sockaddr_in client_addr = {};
    socklen_t addrlen = sizeof(client_addr);
    int connfd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);
    if (connfd < 0) {
      continue; //error
    }
    
    //only serves one client connection at once
    while(true){
      int32_t err = one_request(connfd);
      if(err){
        break;
      }
    }
    
    //do_something(connfd);
    close(connfd);
  }
  
  
  
  return 0;
}

static void msg(const char *msg){
  fprintf(stderr, "%s\n", msg);
}

static void die(const char *msg){
  int err = errno;
  fprintf(stderr, "[%d] %s\n", err, msg);
  abort();
  
}

static int32_t read_full(int fd, char *buf, size_t n){
  while(n > 0){
    ssize_t rv = read(fd, buf, n);
    if(rv <= 0){
      return -1; //error, or unexpected End of File EOF
    }
    assert((size_t)rv <= n);
    n -= (size_t)rv;
    buf += rv;
  }
  return 0;
}

static int32_t write_all(int fd, char *buf, size_t n){
  while(n > 0){
    ssize_t rv = write(fd, buf, n);
    if(rv <= 0){
      return -1; // error had occurred
    }
    assert((size_t)rv <= n);
    n-= (size_t)rv;
    buf += rv;
  }
  return 0;
}

static int32_t one_request(int connfd){
  // 4 bytes header
  char rbuf[4 + k_max_msg];
  errno = 0;
  
  int32_t err = read_full(connfd, rbuf, 4);
  if(err){
    msg(errno == 0 ? "EOF" : "read() error");
    return err;
  }
  
  uint32_t len = 0;
  memcpy(&len, rbuf, 4); // assume little endian
  if(len > k_max_msg){
    msg("too long");
    return -1;
  }
  
  //request body 
  err = read_full(connfd, &rbuf[4], len);
  if(err){
    msg("read() error");
    return err;
  }
  
  //do something
  printf("client says %.*s\n", len, &rbuf[4]);
  //reply using the same protocol
  const char reply[] = "world";
  char wbuf[4 + sizeof(reply)];
  len = (uint32_t)strlen(reply);
  memcpy(wbuf, &len, 4);
  mempcpy(&wbuf[4], reply, len);
  return write_all(connfd, wbuf, 4 + len);
  
}


/*
static void do_something(int connfd){

  char rbuf[64] = {};
  ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
  if (n < 0) {
    msg("read() error");
    return;
  }
  printf("client says: %s\n", rbuf);
  
  char wbuf[] = "world";
  write(connfd, wbuf, strlen(wbuf));
  */
  
  

