#include <arpa/inet.h>
#include <ddos2/message.h>
#include <ddos2/network.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "socket.h"

size_t my_fucking_min(size_t* x, size_t* y) {
  if (*x < *y) {
    return *x;
  } else {
    return *y;
  }
}

struct in_addr make_inaddr(char* host){
    struct in_addr x;
    x.s_addr=inet_addr(host);
    return x;
}

char* hostname2ip(const char* hostname) {
    if (!hostname) {
      error("Programming error: hostname is NULL!(%s:%d)", __FILE__, __LINE__);
    }
    char* ip = (char*)malloc(16 * sizeof(char));  // 255.255.255.255\0
    struct hostent* he;
    struct in_addr** addr_list;
    int i;

    if ((he = gethostbyname(hostname)) == NULL) {
        // get the host info
        error("Failed to find host %s\n", hostname);
        return NULL;
    }

    addr_list = (struct in_addr**)he->h_addr_list;

    for (i = 0; addr_list[i] != NULL; i++) {
        // Return the first one;
        strcpy(ip, inet_ntoa(*addr_list[i]));
        return ip;
    }

    return NULL;
}

int cconnect_tcp(char* target, int port) {
  int sock =  socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serv_addr;  // set the server adress
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  serv_addr.sin_addr = make_inaddr(hostname2ip(target));
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
    error("Faild to connect to %s (%d: %s)\n", target, errno, strerror(errno));
    close(sock);
    return -1;
  }
  return sock;
}


bool send_packet_tcp(int sock, void* payload, size_t sz, size_t chunk_sz) {

  while (sz > 0) {
    if (send(sock, payload, my_fucking_min(&chunk_sz, &sz), 0) == -1) {
      error("send: %s(%d)\n", errno, strerror(errno));
      close(sock);
      return false;
    }
    sz -= chunk_sz;
  }
  return true;
}

void* receice_packet_tcp(int sock, size_t sz, size_t chunk_sz) {
  void* buffer = (void*)malloc(sz);
  sszie_t value = 0;
  while (sz > 0) {
    value = recv(sock, buffer, my_fucking_min(&chunk_sz, &sz));
    if (value == 0) {
      error("YAY! Connection was closed\n");
      close(sock);
      return buffer;
    } else if (value == -1) {
      error("YOUR CODE FUCKED FUCK FUCK ANDREW ZAY: andrewerr@github.com\n");
      close(sock);
      return NULL;
    } else {
      continue;
    }
    sz -= chunk_sz;
  }
  return buffer;
}
