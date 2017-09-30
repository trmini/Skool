#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <utime.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "uici.h"

#ifndef MAXNAMLEN
#define MAXNAMLEN 256
#endif

#define DATA_SIZE 26
#define BUFFSIZE 256

static int transfer_file(int fdout, int fdin, off_t filesize);

int main(int argc, char **argv) {
  DIR *dir_p;
  struct dirent *dir_ent_p;
  struct stat statbuf;
  struct utimbuf filetime;
  off_t filesize;

  char *fullpath;
  int dir_len;

  int i, n, cnt;

  int fdout, fdin;

  char buff[MAXNAMLEN+DATA_SIZE];
  char *buff_p = buff;

  u_port_t portnumber;
  
  fprintf(stderr,"This program was written by Dien Trang Luu.\n");
  if (argc != 3) {
    fprintf(stderr, "Usage: %s  hostname  port\n", argv[0]);
    exit(1);
  }

  portnumber = (u_port_t) atoi(argv[2]);
  if ( (fdout = u_connect(portnumber, argv[1])) < 0 ) {
    fprintf(stderr, "Unable to establish a connection to %s:%s\n", argv[1], argv[2]);
    exit(1);
  }
  
  if ( (dir_p = opendir("./")) == NULL) {
      fprintf(stderr, "Could not open current directory: %s", strerror(errno));
      exit(1);
  }

  dir_len = strlen("./");
  if ( !(fullpath = (char*) calloc(dir_len+MAXNAMLEN+1, sizeof(char))) ) {
    fprintf(stderr, "Not enough memory: %s\n", strerror(errno));
    exit(1);
  }
  strcpy(fullpath, "./");

  while ( (dir_ent_p = readdir(dir_p)) != NULL ) {
    strncpy(fullpath+dir_len, dir_ent_p->d_name, MAXNAMLEN);
    if (lstat(fullpath, &statbuf) == -1) {
      fprintf(stderr, "Could not get stat on file %s: %s\n", fullpath, strerror(errno));
      continue;
    }
    
    /* Name of File */
    strncpy(buff, dir_ent_p->d_name, MAXNAMLEN);
    cnt = strlen(buff) + 1;
    buff_p = buff + cnt;
    
    /* Type of File */
    if ( S_ISREG(statbuf.st_mode)) {
      *(buff_p++) = '1';
    }
    else {
      *(buff_p++) = '0';
    }
    *(buff_p++) = '\0';
    cnt += 2;

    /* Size of File */
    sprintf(buff_p, "%d", (unsigned int) statbuf.st_size);
    cnt = cnt + strlen(buff_p) + 1;
    buff_p = buff + cnt;
    
    /* Modification Time */
    sprintf(buff_p, "%d", (unsigned int) statbuf.st_mtime);
    cnt = cnt + strlen(buff_p) + 1;
    buff_p = buff;

    /* Write data to network */
    while ( (n = u_write(fdout, buff_p, cnt)) != cnt) {   
      if ( n < 0 ) {
        fprintf(stderr, "Could not write data to network\n");
        exit(1);
      }
      cnt -= n;
      buff_p += n;
    } 

  }
  closedir(dir_p);
  buff[0] = '\0';
  /* Sending out a single ternimator character */
  if ( u_write(fdout, buff, 1) < 0 ) {
    fprintf(stderr, "Could not write last NULL to network\n");
    exit(1);
  }
  fdin = fdout;
  /* Start receiving data from server */
  i = 0;
  cnt = 0;
  n = u_read(fdin, buff, 1);
  while ( n > 0 ) {
    if ( buff[i] == '\0') {
      i = -1;
      cnt++;
      switch (cnt) {
        case 1: /* Print Name */
          strncpy(fullpath, buff, MAXNAMLEN);
          if ( (fdout = open(fullpath, O_WRONLY | O_CREAT | O_TRUNC, 0600) ) == -1 ) {
            fprintf(stderr, "Could not open file %s\n", fullpath);
          }
          break;
        case 2: /* Size */
          filesize = (off_t) atol(buff);
          break;
        case 3: /* Time */
          if ( (transfer_file(fdout, fdin, filesize)) == 0 ) {
            fprintf(stderr, "Error on transfering file %s\n", fullpath);
            exit(1); 
          }
          filetime.modtime = (time_t) atol(buff);
          time(&filetime.actime);
          close(fdout);
          if ( utime(fullpath, &filetime) ) {
            fprintf(stderr, "Error while setting time for %s: %s\n", fullpath, strerror(errno));
          }
          fprintf(stderr, "%-25s %10d %24.24s Updated\n", fullpath, (int) filesize, ctime(&filetime.modtime));
          n = u_read(fdin, buff, 1);
          if ( buff[0] == '\0' ) {
            goto EXIT;
          }
          i++;
          cnt = 0;
          break;
      }
    }
    i++;
    n = u_read(fdin, buff+i, 1);
  }
  if ( n < 0 ) {
    fprintf(stderr,"Error while reading from network\n");
    exit(1);
  }
  EXIT:
  u_close(fdin);
  free(fullpath);
  return 0;
}

static int transfer_file(int fdout, int fdin, off_t filesize) {

  char buff[BUFFSIZE];
  char *buff_p = buff;
  int n;
  int byte_num = filesize % BUFFSIZE;
  int cnt = 1 + filesize / BUFFSIZE;
  int byte_cnt;

  do {
    byte_cnt = byte_num;
    while ( (n = u_read(fdin, buff_p, byte_cnt)) < byte_cnt) {
      if ( n < 0 ) return 0;
      buff_p += n;
      byte_cnt -= n;
    }
    /* Make sure that it read in what suppposed to read */

    byte_cnt = byte_num;
    buff_p = buff;
    while ( (n = write(fdout, buff_p, byte_cnt)) != byte_cnt) {
      if ( n < 0 ) return 0;
      buff_p += n;
      byte_cnt -= n;
    }
  byte_num = BUFFSIZE;
  buff_p = buff;
  cnt--;
  } while ( cnt );
  return 1;
}
