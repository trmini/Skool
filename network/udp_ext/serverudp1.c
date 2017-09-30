/*******************************************************************************
*
* Chau Nguyen
* 
* CS5523 Operating System
*
* A UDP Port Server
* Part I: UDP warmup
*
*******************************************************************************/


#include <errno.h> /* for perror */ 
#include <netdb.h> /* for sockaddr_in, INADDR_ANY */ 
#include <stdlib.h> /* for atoi, exit */ 
#include <stdio.h> /* for fprintf, perror */ 
#include <sys/socket.h> /* for socket, bind, socklen_t, recvfrom */ 
                        /* AF_INET, SOCK_DGRAM */ 
#include <sys/types.h> /* for socket, bind, uint16_t, ssize_t */ 
#include <unistd.h> /* for read, write */

#define BLKSIZE 1024 


/*******************************************************************************
*
*   main()
*
*   This is a server test of UDP.
*   It receives datagrams from a well-known port
*   and outputs them to standard output.
*
*******************************************************************************/

void main(int argc, char *argv[]) 
{ 
   uint16_t port; 
   int sock; 
   struct sockaddr_in server; 
   struct sockaddr_in client; 
   ssize_t bytesread; 
   ssize_t byteswritten; 
   socklen_t clientlen; 
   char buf[BLKSIZE]; 
   
   if (argc != 2) 
   { 
      fprintf(stderr, "Usage: %s port\n", argv[0]); 
      exit(1);    
   } 
   
   /* set up the address of the server */ 
   server.sin_family = AF_INET; 
   port = (uint16_t)atoi(argv[1]); 
   server.sin_port = htons(port); server.sin_addr.s_addr = htonl(INADDR_ANY); 
   
   /* create the socket */ 
   if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
   { 
      perror("Unable to create socket");
      exit(1); 
   } 
   
   /* bind the socket to a well-known port */ 
   if ( bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) 
   { 
      perror("Unable to bind well-known port to socket"); 
      exit(1); 
   } 
   
   /* receive from clients */ 
   while( (bytesread = recvfrom(sock, buf, (size_t)BLKSIZE, 0, 
                                   (struct sockaddr*)&client, &clientlen) ) > 0)
   { 
      byteswritten = write(STDOUT_FILENO, buf, (size_t)bytesread); 
      if (bytesread != byteswritten) 
      { 
         fprintf(stderr, "Error writing %ld bytes, %ld bytes written\n", 
                                           (long)bytesread, (long)byteswritten);
         break;
      } 
      
      sleep(1);

      /* send back to client */
      byteswritten = sendto(sock, buf, bytesread, 0, (struct sockaddr*)&client, 
      								     clientlen);
      if (bytesread != byteswritten) 
      { 
         fprintf(stderr, "Error writing %ld bytes, %ld bytes written\n", 
                                           (long)bytesread, (long)byteswritten);
         break;
      }
   } 
   
   close(sock); 
   exit(0); 
} 
