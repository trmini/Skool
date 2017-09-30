/*******************************************************************************
*
* Chau Nguyen
* 
* CS5523 Operating System
*
* A UDP Port Client
* Part I: UDP warmup
*
*******************************************************************************/

#include <errno.h> /* for perror */ 
#include <netdb.h> /* for gethostbyname */ 
/*#include   for sockaddr_in, INADDR_ANY */ 
#include <stdlib.h> /* for atoi, exit */ 
#include <stdio.h> /* for fprintf, perror */ 
#include <string.h> /* for memcpy */ 
#include <sys/socket.h> /* for socket, bind, socklen_t, recvfrom */ 
			/* AF_INET, SOCK_DGRAM */ 
#include <sys/types.h> /* for socket, bind, uint16_t, ssize_t */ 
#include <unistd.h> /* for read, write, close */ 
#include <sys/select.h>
#include <sys/time.h>

#define BLKSIZE 1024 


/*******************************************************************************
*
*   int readabletimeo(int fd, int sec)
*
*   This function waits for the descriptor to be ready for reading.
*
*******************************************************************************/

int readabletimeo(int fd, int sec)
{
   fd_set rset;
   struct timeval tv;
   
   FD_ZERO(&rset);
   FD_SET(fd, &rset);
   
   tv.tv_sec = sec;
   tv.tv_usec = 0;
   
   return (select(fd + 1, &rset, NULL, NULL, &tv)); 
   /* > 0 if descriptor is readable */
}


/*******************************************************************************
*
*   main()
*
*   This is a client test of UDP. 
*   It reads a file from stdin in blocks of size BLKSIZE
*   and outputs the blocks to the connection.
*
*******************************************************************************/

void main(int argc, char *argv[]) 
{ 
   uint16_t port; 
   int sock, timeout; 
   struct sockaddr_in server; 
   struct hostent *hp; 
   ssize_t bytesread; 
   ssize_t byteswritten; 
   char sendbuf[BLKSIZE], recvbuf[BLKSIZE]; 
   
   if (argc != 4) 
   { 
      fprintf(stderr, "Usage: %s host port timeout\n", argv[0]); 
      exit(1); 
   } 
   
   /* set up the address to the server to be included in datagram */ 
   server.sin_family = AF_INET; 
   port = (uint16_t)atoi(argv[2]); 
   server.sin_port = htons(port); 
   
   /* get time out */
   timeout = atoi(argv[3]);
   
   if (!(hp = gethostbyname(argv[1]))) 
   { 
      perror("Unable to get host byname"); 
      exit(1); 
   }

   memcpy((char *)&server.sin_addr, hp->h_addr_list[0], hp->h_length); 
   
   /* create a socket for sending datagrams */ 
   if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
   { 
      perror("Unable to create socket"); 
      exit(1); 
   } 
   
   for ( ; ; ) 
   { 
      bytesread = read(STDIN_FILENO, sendbuf, BLKSIZE); 
      if ( (bytesread == -1) && (errno == EINTR) ) 
      {
         fprintf(stderr, "Client restarting read\n"); 
         continue;
      }
      else if (bytesread <= 0) 
         break; 
      else 
      { 
         byteswritten = sendto(sock, sendbuf, bytesread, 0, 
                                    (struct sockaddr *)&server, sizeof(server));
         if (byteswritten != bytesread) 
         { 
            fprintf(stderr, "Error writing %ld bytes, %ld bytes written\n", 
                                           (long)bytesread, (long)byteswritten);
            break; 
         } 
      } 
      
      if (readabletimeo(sock, timeout) == 0)
      {
         fprintf(stderr, "socket timeout\n");
         continue;
      }
      else
      {
         if ( (bytesread = recvfrom(sock, recvbuf, 
      					(size_t)BLKSIZE, 0, NULL, NULL)) <= 0 )
         {   
            fprintf(stderr, "Error receiving from server\n");
            break;
         }  
      
         recvbuf[bytesread] = 0; /* null terminate */
      
         byteswritten = write(STDOUT_FILENO, recvbuf, (size_t)bytesread); 
         if (bytesread != byteswritten) 
         { 
            fprintf(stderr, "Error writing %ld bytes, %ld bytes written\n", 
                                           (long)bytesread, (long)byteswritten);
            break;
         }
      }
   } 
   close(sock); 
   exit(0); 
} 
