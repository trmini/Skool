/* Program B.4 */
/* uici.c  sockets implementation */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include "uici.h"

#define MAXBACKLOG 5

/* return 1 if error, 0 if OK */
int u_ignore_sigpipe()
{
   struct sigaction act;

   if (sigaction(SIGPIPE, (struct sigaction *)NULL, &act) < 0)
      return 1;
   if (act.sa_handler == SIG_DFL) {
      act.sa_handler = SIG_IGN;
      if (sigaction(SIGPIPE, &act, (struct sigaction *)NULL) < 0)
         return 1;
   }   
   return 0;
}

/*
 *                           u_open
 * Return a file descriptor which is bound to the given port.
 *
 * parameter:
 *        s = number of port to bind to
 * returns:  file descriptor if successful and -1 on error
 */
int u_open(u_port_t port)
{
   int sock;
   struct sockaddr_in server;
   int true = 1;
 
   if ( (u_ignore_sigpipe() != 0) ||
        ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) )
      return -1;
       
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = INADDR_ANY;
   server.sin_port = htons((short)port);
 
   if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char *)&true,sizeof(true)) < 0) 
       fprintf(stderr,"setsocketopt failed\n");
   if ( (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) ||
        (listen(sock, MAXBACKLOG) < 0) )
      return -1;
   return sock;
}

/*
 *                           u_listen
 * Listen for a request from a particular host on a specified port.
 *
 * parameters:
 *      fd = file descriptor previously bound to listening port
 *      hostn = name of host to listen for
 * returns:  communication file descriptor or -1 on error
 *
 * comments: This function is used by the server to listen for
 * communication.  It blocks until a remote request is received
 * from the port bound to the given file descriptor.
 * hostn is filled with an ASCII string containing the remote
 * host name.  It must point to s string which is large enough to
 * hold this name.
 */
int u_listen(int fd, char *hostn)
{
   struct sockaddr_in net_client;
   int len = sizeof(struct sockaddr);
   int retval;
   struct hostent *hostptr;
 
   while ( ((retval =
           accept(fd, (struct sockaddr *)(&net_client), &len)) == -1) &&
          (errno == EINTR) )
      ;
   if (retval == -1)
      return retval;
   hostptr =
      gethostbyaddr((char *)&(net_client.sin_addr.s_addr), 4, AF_INET);
   if (hostptr == NULL)
      strcpy(hostn, "unknown");
   else
      strcpy(hostn, (*hostptr).h_name);
   return retval;
}

/*
 *                           u_connect
 * Initiate communication with a remote server.
 *
 * parameters:
 *     port  = well-known port on remote server
 *     inept = character string giving the Internet name of the
 *             remote machine
 * returns: the file descriptor used for communication or -1 if error
 */
int u_connect(u_port_t port, char *hostn)
{
   struct sockaddr_in server;
   struct hostent *hp;
   int sock;
   int retval;
 
   if ( (u_ignore_sigpipe() != 0) ||
        !(hp = gethostbyname(hostn)) ||
        ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) )
      return -1;
       
   memcpy((char *)&server.sin_addr, hp->h_addr_list[0], hp->h_length);
 
   server.sin_port = htons((short)port);
   server.sin_family = AF_INET;
 
   while ( ((retval =
       connect(sock, (struct sockaddr *)&server, sizeof(server))) == -1)
        && (errno == EINTR) )
      ;
   if (retval == -1) {
        close(sock);
        return -1;
   }
   return sock;
}
 
/*
 *                           u_close
 * Close communication for the given file descriptor.
 * parameter:
 *       fd = file descriptor of socket connection to be closed
 * returns:
 *       a negative value indicates an error occurred
 */
 
int u_close(int fd)
{
   return close(fd);
}

/*
 *                           u_read
 *
 * Retrieve information from a file descriptor opened by u_open.
 *
 * parameters:
 *       fd = file descriptor
 *       buf = buffer to be output
 *       nbyte = number of bytes to retrieve
 * returns:
 *      a negative value indicates an error occurred
 *      otherwise the number of bytes read is returned
 */
 
ssize_t u_read(int fd, void *buf, size_t size)
{
   ssize_t retval;
 
   while (retval = read(fd, buf, size), retval == -1 && errno == EINTR)
     ;
   return retval;
}    
 
/*
 *                           u_write
 *
 * Send information on a file descriptor opened by u_open.
 *
 * parameters:
 *       fd = file descriptor
 *       buf = buffer to be output
 *       nbyte = number of bytes to send
 * returns:
 *      a negative value indicates an error occurred
 *      otherwise the number of bytes written is returned
 */
 
ssize_t u_write(int fd, void *buf, size_t size)
{
   ssize_t retval;
 
   while (retval = write(fd, buf, size), retval == -1 && errno == EINTR)
      ;
   return retval;
}     
 
/*
 *                           u_error
 *  Display an error message in the manner of perror or t_error.
 *
 *  parameter:
 *          s =  string to be prepended to system error message
 *  returns:    0
 *
 *  algorithm:  Since the only type of error comes from a system call,
 *              we only need to call perror.  This implementation should
 *              be as MT-safe as the underlying sockets.
 */

void u_error(char *s)
{
   perror(s);
}

/*
 *                           u_sync
 *
 * This function must be called after an exec or a dup to attach an
 * opened file descriptor in simple UICI implementations.  It is not
 * needed for sockets.
 *
*/
/*ARGSUSED*/
int u_sync(int fd)
{
/* Not needed for sockets, retained for compatibility */
   return 0;
}
