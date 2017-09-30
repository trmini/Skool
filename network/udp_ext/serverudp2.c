/*******************************************************************************
*
* Chau Nguyen
* 
* CS5523 Operating System
*
* A UDP Port Server
* Part II: Accessing Host Services (Server) 
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
#include <string.h> /* for strcpy */

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
*   extract(buffer, serv, protocol)
*
*   This function extracts hostname, portnumber, protocol and service.
*
*******************************************************************************/

void extract(char buffer[NAMESIZE], char *serv, char *protocol)
{
   int i, counter;
   char chr;

   counter = 0;
   chr = buffer[counter];

   /* get protocol name */
   i = 0;
   while ( !((chr == '\0') || (chr == ' ') || (chr == '\t') || (chr == '\n')) ) 
   {
      protocol[i] = chr;
      counter++;
      i++;
      chr = buffer[counter];
   }
   protocol[i] = '\0';

   /* skip space and null */
   while ( (chr == '\0') || (chr == ' ') || (chr == '\t') || (chr == '\n') )
   { 
      counter++;
      chr = buffer[counter];
   }

   /* get service name */
   i = 0;
   while ( !((chr == '\0') || (chr == ' ') || (chr == '\t') || (chr == '\n') ) )
   {
      serv[i] = chr;
      counter++;
      i++;
      chr = buffer[counter];
   }
   serv[i] = '\0';

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


/******************************************************************************
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
   int sock, i, strptr; 
   struct sockaddr_in server; 
   struct sockaddr_in client; 
   ssize_t bytesread;
   ssize_t byteswritten;
   socklen_t clientlen; 
   char servname[20], protoname[20]; 
   struct service hostsev;
   struct servent *sp;
   
   if (argc != 2) 
   { 
      fprintf(stderr, "Usage: %s port\n", argv[0]); 
      exit(1);    
   } 
   
   /* set up the address of the server */ 
   server.sin_family = AF_INET; 
   port = (uint16_t)atoi(argv[1]); 
   server.sin_port = htons(port); 
   server.sin_addr.s_addr = htonl(INADDR_ANY); 
   
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
   while ( (recvfrom(sock, (struct service*)&hostsev, 
        (size_t)sizeof(hostsev), 0, (struct sockaddr*)&client, &clientlen)) > 0)
   { 
      /* Extract service name and protocol from hostsev */
      extract(hostsev.names, servname, protoname);

      fprintf(stderr, "service name: %s, protocol: %s\n", servname, protoname);

      if ( (sp = getservbyname(servname, protoname)) == NULL)
      {
         fprintf(stderr, "service not found or error %s\n", servname);
         hostsev.port = -1;
         strcpy(hostsev.names, servname);
      }
      else
      {
         hostsev.port = sp -> s_port;
      
         /* copy aliases to names, sometime there are more than 1 alias */
         i = 0;
         strcpy(hostsev.names, sp -> s_aliases[i]);
         strptr = strlen(sp -> s_aliases[i]) + 1;
         i++;
         while (sp -> s_aliases[i] != '\0')
         {
            strcpy(hostsev.names + strptr, sp->s_aliases[i]);
            strptr = strptr + strlen(sp -> s_aliases[i]) + 1;
            i++;
         }
         *(hostsev.names+strptr) = '\0'; 
      }

      fprintf(stderr, "port: %d, aliases of service: ", 
                                                   hostsev.port);
      printaliases(hostsev.names);

      /* send back response to client */
      if (sendto(sock, (struct service *)&hostsev, sizeof(hostsev), 0,
                                (struct sockaddr *)&client, clientlen) < 0)
         fprintf(stderr, "Error sending to client\n");
   } 
   
   close(sock); 
   exit(0); 
} 
