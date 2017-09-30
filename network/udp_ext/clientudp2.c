/*******************************************************************************
*
* Chau Nguyen
* 
* CS5523 Operating System
*
* A UDP Port Client
* Part II: Accessing Host Services (Client)
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
#define NAMESIZE 256

struct service
{
   int sequence;
   int port;
   char names[NAMESIZE];
};


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
*   extract(buffer, host, portno, serv)
*
*   This function extracts hostname, portnumber, protocol and service. 
*
*******************************************************************************/

void extract(char buffer[BLKSIZE], char *host, int *portno, char *serv)
{
   int i, counter, portnum;
   char chr;

   counter = 0;
   chr = buffer[counter]; 
   while ( (chr == ' ') || (chr == '\t') || (chr == '\n') )
   { /* skip space */
      counter++;
      chr = buffer[counter];
   }

   /* should be host name */
   i = 0;
   while ( !( (chr == ' ') || (chr == '\t') || (chr == '\n') ) )
   {
      host[i] = chr;
      counter++;
      i++;
      chr = buffer[counter];
   }
   host[i] = '\0';

   while ( (chr == ' ') || (chr == '\t') || (chr == '\n') )
   { /* skip space */
      counter++;
      chr = buffer[counter];
   }

   /* should be port number */
   portnum = 0;
   while ( (chr >= '0') && (chr <= '9') )
   {
      portnum = 10*portnum + (chr - '0');
      counter++;
      chr = buffer[counter];
   }

   while ( (chr == ' ') || (chr == '\t') || (chr == '\n') )
   { /* skip space */
      counter++;
      chr = buffer[counter];
   }

   /* should be protocol */
   i = 0;
   while ( !( (chr == ' ') || (chr == '\t') || (chr == '\n') ) )
   {
      serv[i] = chr;
      counter++;
      i++;
      chr = buffer[counter];
   }
   serv[i] = '\0';

   while ( (chr == ' ') || (chr == '\t') || (chr == '\n') )
   { /* skip space */
      counter++;
      chr = buffer[counter];
   }

   /* should be service */
   i++;
   while ( !( (chr == ' ') || (chr == '\t') || (chr == '\n') ) )
   {
      serv[i] = chr;
      counter++;
      i++;
      chr = buffer[counter];
   }
   serv[i] = '\0';

   *portno = portnum;

   return;
} 


/*******************************************************************************
*
*   printaliases(buffer)
*
*   This function prints out the aliases.
*
*******************************************************************************/

void printaliases(char buffer[NAMESIZE])
{
   char * chr;

   chr = buffer;
   while (*chr != '\0')
   {
      fprintf(stderr,"%s ", chr);
      chr += strlen(buffer) + 1;
   }
   fprintf(stderr, "\n\n");
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
   int port; 
   int sock, timeout; 
   struct sockaddr_in server; 
   struct hostent *hp; 
   ssize_t bytesread; 
   char buf[BLKSIZE], hostname[NAMESIZE]; 
   struct service hostsev, replysev;
   
   if (argc != 2) 
   { 
      fprintf(stderr, "Usage: %s timeout\n", argv[0]); 
      exit(1); 
   } 
   
   /* set up the address to the server to be included in datagram */ 
   server.sin_family = AF_INET; 

   /* get time out */
   timeout = atoi(argv[1]);

   fprintf(stderr, "hostname portnumber protocol service\n");

   /* create a socket for sending datagrams */
   if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
   {
      perror("Unable to create socket");
      exit(1);
   }

   
   for ( ; ; ) 
   { 
      bytesread = read(STDIN_FILENO, buf, BLKSIZE); 
      if ( (bytesread == -1) && (errno == EINTR) ) 
      {
         fprintf(stderr, "Client restarting read\n"); 
         continue;
      }
      else if (bytesread <= 0) 
      {
         fprintf(stderr, " Error reading\n");
         continue; 
      }
      else 
      { 
         /* extract hostname, port, protocol and service */
         extract(buf, hostname, &port, hostsev.names);

         server.sin_port = htons(port);

         if (!(hp = gethostbyname(hostname)))
         {
            perror("Unable to get host byname");
            exit(1);
         }

         memcpy((char *)&server.sin_addr, hp->h_addr_list[0], hp->h_length);

         /* Input sequence and port number of hostsev */
         hostsev.sequence = random();
         hostsev.port = 0;

         /* send message to server */
         if ( sendto(sock, (struct service *)&hostsev, sizeof(hostsev), 0,
                         (struct sockaddr *)&server, sizeof(server)) < 0)
            fprintf(stderr, "Error sending to server\n"); 
      } 
      
      /* check time out */
      if (readabletimeo(sock, timeout) == 0)
      {
         fprintf(stderr, "socket timeout\n");
         continue;
      }
      else
      {
         /* receive message from server */
         if ( (bytesread = recvfrom(sock, (struct service*)&replysev,
                               (size_t)sizeof(replysev), 0, NULL, NULL)) <= 0 )
            fprintf(stderr, "Error receiving from server\n");

         fprintf(stderr, "port: %d, aliases: ", 
                                                 replysev.port);
         printaliases(replysev.names);

         /* compare the sequence that received from server */
         if (hostsev.sequence != replysev.sequence)
            fprintf(stderr, "The sequence of reply doesn't match\n");
         else
            fprintf(stderr, "The sequence of reply matches\n");
      }
      fprintf(stderr, "\n");

   } 
   close(sock); 
   exit(0); 
} 
