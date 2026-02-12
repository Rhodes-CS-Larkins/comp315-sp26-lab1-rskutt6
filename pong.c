/*
 * pong.c - UDP ping/pong server code
 *          author: <your name>
 */
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "util.h"

#define PORTNO "1266"


int main(int argc, char **argv) {
  int ch;
  int nping = 1;                    // default packet count
  char *pongport = strdup(PORTNO);  // default port

  while ((ch = getopt(argc, argv, "h:n:p:")) != -1) {
    switch (ch) {
    case 'n':
      nping = atoi(optarg);
      break;
    case 'p':
      pongport = strdup(optarg);
      break;
    default:
      fprintf(stderr, "usage: pong [-n #pings] [-p port]\n");
    }
  }
  
  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;

  int r = getaddrinfo(NULL, pongport, &hints, &res);
  if (r != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(r));
    exit(1);
  }

  int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (sockfd < 0) {
    perror("socket");
    exit(1);

  } if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
    perror("bind");
    freeaddrinfo(res);
    exit(1);
  }

  unsigned char buf[50];

  for (int i = 0; i < nping; i++) {
    struct sockaddr from;
    socklen_t fromlen = sizeof(from);

    ssize_t n = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&from, &fromlen);
    if (n < 0) {
      perror("recvfrom");
      exit(1);
    }


    // add 1 to every byte
    ssize_t j = 0;
    while (j < n) {
      buf[j]++;
      j++;
    }

    if (sendto(sockfd, buf, (size_t)n, 0, (struct sockaddr *)&from, fromlen) < 0) {
      perror("sendto");
      exit(1);
    }
  }

  close(sockfd);
  return 0;

  // pong implementation goes here.
  printf("nping: %d pongport: %s\n", nping, pongport);

  return 0;
}


