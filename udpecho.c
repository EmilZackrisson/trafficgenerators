/* UDP echo server program -- echo-server-udp.c */
#define _XOPEN_SOURCE 700


#include <stdio.h>      /* standard C i/o facilities */
#include <stdlib.h>     /* needed for atoi() */
#include <unistd.h>     /* defines STDIN_FILENO, system calls,etc */
#include <sys/types.h>  /* system data type definitions */
#include <sys/socket.h> /* socket specific definitions */
#include <netinet/in.h> /* INET constants and stuff */
#include <arpa/inet.h>  /* IP address conversion stuff */
#include <netdb.h>      /* gethostbyname */
#include <signal.h>
#include <errno.h>



/* this routine echos any messages (UDP datagrams) received */

#define MAXBUF 1024*1024

volatile sig_atomic_t stop_requested = 0;
int server_socket = -1;

void handle_stop_signal(int signo) {
  (void)signo;
  stop_requested = 1;
  if (server_socket >= 0) {
    close(server_socket);
    server_socket = -1;
  }
}

void echo( int sd ) {
  socklen_t len;
  int n, pktNum = 0;
  char bufin[MAXBUF];
  struct sockaddr_in remote;

  /* need to know how big address struct is, len must be set before the
     call to recvfrom!!! */

  len = sizeof(remote);

  while (!stop_requested) {
    /* read a datagram from the socket (put result in bufin) */
    n=recvfrom(sd,bufin,MAXBUF,0,(struct sockaddr *)&remote,&len);

    if (n<0) {
      if (stop_requested || errno == EINTR) {
        break;
      }

      perror("Error receiving data");
      continue;
    }

    // Only print every 10 packets
    if (pktNum % 10 == 0) {
      /* print out the address of the sender */
      printf("Got 10 datagrams.");
      printf("Got a datagram from %s port %d\n",inet_ntoa(remote.sin_addr), ntohs(remote.sin_port));
      
      printf("GOT %d BYTES\n",n);
    }

    /* Got something, just send it back */
    sendto(sd,bufin,n,0,(struct sockaddr *)&remote,len);
  }
}

/* server main routine */

int main(int argc, char *argv[]) {
  int ld;
  struct sockaddr_in skaddr;
  socklen_t length;
  struct sigaction sa;

  setvbuf(stdout, NULL, _IONBF, 0);

  if (argc<2)    {
    printf("Please provide a port number. \n");
    exit(EXIT_FAILURE);
    // LOCAL_SERVER_PORT=atoi(argv[1]);
  }

  sa.sa_handler = handle_stop_signal;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGTERM, &sa, NULL);
  sigaction(SIGINT, &sa, NULL);
  
  
  /* create a socket
     IP protocol family (PF_INET)
     UDP protocol (SOCK_DGRAM)
  */

  if ((ld = socket( PF_INET, SOCK_DGRAM, 0 )) < 0) {
    printf("Problem creating socket\n");
    exit(1);
  }

  server_socket = ld;

  /* establish our address
     address family is AF_INET
     our IP address is INADDR_ANY (any of our IP addresses)
     the port number is assigned by the kernel
  */

  skaddr.sin_family = AF_INET;
  skaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  skaddr.sin_port = htons(atoi(argv[1]));

  if (bind(ld, (struct sockaddr *) &skaddr, sizeof(skaddr))<0) {
    printf("Problem binding\n");
    exit(0);
  }

  /* find out what port we were assigned and print it out */

  length = sizeof( skaddr );
  if (getsockname(ld, (struct sockaddr *) &skaddr, &length)<0) {
    printf("Error getsockname\n");
    exit(1);
  }

  /* port number's are network byte order, we have to convert to
     host byte order before printing !
  */
  printf("The server UDP port number is %d\n",ntohs(skaddr.sin_port));

  /* Go echo every datagram we get */
  echo(ld);

  if (ld >= 0) {
    close(ld);
  }
  return(0);
}
