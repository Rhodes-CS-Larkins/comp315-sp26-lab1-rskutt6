/*
 * ping.c - UDP ping/pong client code
 *          author: <your name>
 */
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "util.h"

#define PORTNO "1266"


int main(int argc, char **argv) {
  int ch, errors = 0;
  int nping = 1;                        // default packet count
  char *ponghost = strdup("localhost"); // default host
  char *pongport = strdup(PORTNO);      // default port
  int arraysize = 100;                  // default packet size

  while ((ch = getopt(argc, argv, "h:n:p:")) != -1) {
    switch (ch) {
    case 'h':
      ponghost = strdup(optarg);
      break;
    case 'n':
      nping = atoi(optarg);
      break;
    case 'p':
      pongport = strdup(optarg);
      break;
    case 's':
      arraysize = atoi(optarg);
      break;
    default:
      fprintf(stderr, "usage: ping [-h host] [-n #pings] [-p port] [-s size]\n");
    }
  }

  unsigned char *array = malloc(arraysize);
  if (array == NULL) {
    perror("malloc");
    exit(1);
  }

  for (int i = 0; i < arraysize; i++) {
    array[i] = 200;
  }

  struct addrinfo hints, *rv;
  int sockfd;
  int status;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;

  status = getaddrinfo(ponghost, pongport, &hints, &rv);
  if (status != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    exit(1);
  }

  sockfd = socket(rv->ai_family, rv->ai_socktype, rv->ai_protocol);
  if (sockfd < 0) {
    perror("socket");
    freeaddrinfo(rv);
    exit(1);
  }

  unsigned char *reply = malloc(arraysize);
  double total_time = 0.0;
  
  for (int k = 0; k < nping; k++) {
    double start = get_wctime();

    sendto(sockfd, array, arraysize, 0, rv->ai_addr, rv->ai_addrlen);
    recvfrom(sockfd, reply, arraysize, 0, NULL, NULL);

    double time = get_wctime() - start;
    total_time += time;
    
    // find errors
    for (int i = 0; i < arraysize; i++) {
      if (reply[i] != 201) {
        errors++;
        break;
      }
    }

    printf("ping[%d] : round-trip time: %.3f ms\n", k, time * 1000.0);
  }

  // find average
  double avg_time = total_time / nping;
  printf("time to send %d packets of %d bytle %.3f ms (%.3f avg per packet)\n",
      nping, arraysize, total_time * 1000.0, avg_time * 1000.0);


  // UDP ping implemenation goes here
  printf("nping: %d arraysize: %d errors: %d ponghost: %s pongport: %s\n",
      nping, arraysize, errors, ponghost, pongport);

  return 0;
}
