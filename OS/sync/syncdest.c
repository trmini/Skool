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

  char *fullpath;
  int dir_len;

  mode_t fifo_mode = S_IRUSR | S_IWUSR;
  int fdout, fdin;

  char buff[MAXNAMLEN+DATA_SIZE];
  char *buff_p = buff;

  int i, n, cnt;
  off_t filesize;
   
  fprintf(stderr,"This program was written by Dien Trang Luu.\n");
  if (argc != 3) {
    fprintf(stderr, "Usage: %s  infopipe sendpipe\n", argv[0]);
    exit(1);
  }
  
  if ( (mkfifo(argv[1], fifo_mode) == -1) && (errno != EEXIST) ) {
    fprintf(stderr, "Could not creat a named pipe: %s\n", argv[1]);
    exit(1);
  }

  if ( (fdout = open(argv[1], O_WRONLY)) == -1 ) {
    fprintf(stderr, "Could not open named pipe: %s\n", argv[1]);
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
    strncpy(buff,dir_ent_p->d_name, MAXNAMLEN);
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
    buff_p = buff + cnt;
    
    /* End of Output */
    *(buff_p) = '\0';
    cnt++;
    buff_p = buff;

    /* Write data into pipe */
    while ( (n = write(fdout,buff_p,cnt)) != cnt) {   
      if ( n < 0 ) {
        fprintf(stderr, "Could not write data to named pipe %s\n", argv[1]);
        exit(1);
      }
      cnt -= n;
      buff_p += n;
    } 

  }
  close(fdout);
  closedir(dir_p);

  /* Start receiving data from syncsource */
  if ( (mkfifo(argv[2], fifo_mode) == -1) && (errno != EEXIST) ) {
    fprintf(stderr, "Could not creat a named pipe: %s\n", argv[2]);
    exit(1);
  }

  if ( (fdin = open(argv[2], O_RDONLY)) == -1 ) {
    fprintf(stderr, "Could not open named pipe: %s\n", argv[2]);
    exit(1);
  } 
  /* Block until syncsource goes into send data stage */

  i = 0;
  cnt = 0;
  n = read(fdin, buff, 1);
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
            fprintf(stderr,"Error while set time for %s: %s\n", fullpath, strerror(errno));
          }
          fprintf(stderr,"%-25s %10d %24.24s Update\n",fullpath, (int) filesize, ctime(&filetime.modtime));
          n = read(fdin, buff, 1);
          cnt = 0;
          break;
      }
    }
    i++;
    n = read(fdin, buff+i, 1);
  }
  if ( n < 0 ) {
    fprintf(stderr,"Error while reading from pipe %s\n", argv[2]);
    exit(1);
  }
  close(fdin);
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
    while ( (n = read(fdin, buff_p, byte_cnt)) < byte_cnt) {
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
