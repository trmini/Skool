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
#define NUM_SIZE 20

static struct recordinfo {
  unsigned int set_num;
  unsigned int file_num;
  unsigned int send_num;
  off_t byte_num;
  time_t last_time;
} section;

static struct transinfo {
  char *f_name;
  off_t f_size;
  off_t o_size;
  time_t f_time;
  time_t o_time;
  char action[NUM_SIZE];
  struct transinfo *next;
} trans_rec;

static void print_last_info();
static void print_section_info();

int main(int argc, char **argv) {

  int i, n, cnt;

  int fdout, fdin;

  char buff[MAXNAMLEN+DATA_SIZE] = "send status";
  char *buff_p = buff;

  u_port_t portnumber;
  struct transinfo *f_curr = &trans_rec;
  
  fprintf(stderr,"This program was written by Dien Trang Luu.\n");
  if (argc != 3) {
    fprintf(stderr, "Usage: %s  hostname  port\n", argv[0]);
    exit(1);
  }
  memset(&section, 0, sizeof(struct recordinfo));
  memset(&trans_rec, 0, sizeof(struct transinfo));

  portnumber = (u_port_t) atoi(argv[2]);
  if ( (fdout = u_connect(portnumber, argv[1])) < 0 ) {
    fprintf(stderr, "Unable to establish a connection to %s:%s\n", argv[1], argv[2]);
    exit(1);
  }
  /* Write data to server */
  cnt = strlen(buff) + 1;
  buff_p = buff;
  while ( (n = u_write(fdout, buff_p, cnt)) != cnt) {   
    if ( n < 0 ) {
      fprintf(stderr, "Could not write request to server\n");
      exit(1);
    }
    cnt -= n;
    buff_p += n;
  } 

  fdin = fdout;
  /* Start receiving data from server */
  n = u_read(fdin, buff, 1);
  /*Read in Record info*/
  i = 0;
  cnt = 0;
  n = u_read(fdin, buff, 1);
  while (n > 0) {
    if ( buff[i] == ' ' ) {
      buff[i] = '\0';
      i = -1;
      cnt++;
      switch (cnt) {
        case 1:
          section.set_num = (unsigned int) atol(buff);
          break;
        case 2:
          section.file_num = (unsigned int) atol(buff);
          break;
        case 3:
          section.send_num = (unsigned int) atol(buff);
          break;
        case 4:
          section.byte_num = (off_t) atol(buff);
          break;
        case 5:
          section.last_time = (time_t) atol(buff);
          goto END_GET_RECORD;
      } /* End Switch */
    }
    i++;
    n = u_read(fdin, buff+i, 1);
  }
  if ( n < 0 ) {
    fprintf(stderr,"Error while reading from network\n");
    exit(1);
  }
  END_GET_RECORD:
  n = u_read(fdin, buff, 1);
  n = u_read(fdin, buff, 1);
  i = 0;
  cnt = 0;
  while ( n > 0 ) {
    if ( buff[i] == ' ' ) {
      buff[i] = '\0';
      i = -1;
      cnt++;
      switch(cnt) {
        case 1: /* Name */
          if ( !(f_curr->f_name = (char*) calloc(strlen(buff)+1, sizeof(char))) ) {
            fprintf(stderr, "Not enough memory: %s\n", strerror(errno));
            exit(1);
          }
          strcpy(f_curr->f_name, buff);
          break;
        case 2: /* File size */
          f_curr->f_size = (off_t) atol(buff);
          break;
        case 3: /* Other Size */
          f_curr->o_size = (off_t) atol(buff);
          break;
        case 4: /* File Time */
          f_curr->f_time = (time_t) atol(buff);
          break;
        case 5: /* Other Time */
          f_curr->o_time = (time_t) atol(buff);
          break;
        case 6: /* Action and creat a new record */
          strcpy(f_curr->action, buff);
          n = u_read(fdin, buff, 1);
          if (buff[0] == '\0') {
          goto END_GET_TRANS;
          }
          i++;
          cnt = 0;
          if ( !(f_curr->next = (struct transinfo*) calloc(1, sizeof(struct transinfo))) ) {
            fprintf(stderr, "Not enough memory: %s\n", strerror(errno));
            exit(1);
          }
          /* malloc space for next record */
          f_curr = f_curr->next;
          f_curr->f_name = NULL;
          f_curr->next = NULL;
          break;
      }
    }
    i++;
    n = u_read(fdin, buff+i, 1);
  }
  END_GET_TRANS:
  u_close(fdin);
  print_section_info();
  print_last_info();
  return 0;
}



static void print_last_info() {
  struct transinfo *p_curr = &trans_rec;
  printf("\nLast active information\n");
  printf("=======================\n");
  if (p_curr->f_name == NULL) {
    printf("No record has been made\n");
  }
  else {
    while ( p_curr != NULL) {
      printf("%-25s\t%10ld %24.24s\t", p_curr->f_name, p_curr->f_size, ctime(&p_curr->f_time));
      if (p_curr->o_time || p_curr->o_size ) {
        printf("%10ld %24.24s  %s\n", p_curr->o_size, ctime(&p_curr->o_time), p_curr->action);
      }
      else
        printf("\t%33s\n", p_curr->action);
      p_curr = p_curr->next;
    }
  }
  fflush(stdout);
}

static void print_section_info() {

  /* No signal can be caught during the process of this function */

  printf("\n\nSummary Information\n===================\n");
  printf("Number of data set: %d\n", section.set_num);
  printf("Number of file info received: %d\n", section.file_num);
  printf("Number of file sent: %d\n", section.send_num);
  printf("Number of byte sent: %ld\n", section.byte_num);
  printf("Time of last action: %s", ctime(&section.last_time));

  return;
}

