#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "uici.h"

#ifndef MAXNAMLEN
#define MAXNAMLEN 256
#endif

#define BUFFSIZE 256
#define DATASIZE 26
#define NUM_SIZE 20
#define SIG_MINE SIGALRM

#define UPDATE_SET 0
#define UPDATE_FILE 1
#define UPDATE_SEND 2
#define UPDATE_BYTE 3
#define UPDATE_TIME 4

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

struct fileinfo {
  char *f_name;
  char f_type;
  off_t f_size;
  time_t f_time;
  struct fileinfo *next;
} f_head;

static void install_signal();
static void block_signal(int signum);
static void un_block_signal(int signum);

static void free_info(struct fileinfo *f_curr);
static void free_transinfo(struct transinfo *f_curr);
static int write_to_net(int fdout, char *buff, unsigned int size);

static int tell_parent(int fdout, int type_of_mess);
static int update_section(int field_num, unsigned long extra_info);

static void print_last_info();
static void print_section_info();
static void get_child_info();

static int *permission; /* This should be a shared variable */
static int pipefd[2];

int main(int argc, char **argv) {
  DIR *dir_p;
  struct dirent *dir_ent_p;
  struct stat statbuf;
  struct fileinfo *f_curr;
  struct transinfo *info_curr;

  char *fullpath;
  int dir_len;

  int fdin, fdout;
  char buff[MAXNAMLEN+DATASIZE];
  char *buff_p;

  int i, n, cnt;
  time_t f_modtime;
  char remote_name[MAXNAMLEN];

  /* Network Stuff */
  u_port_t portnumber;
  int listenfd;
  pid_t parent;
  pid_t host_pid;

  /* Server is up and running steps */
  fprintf(stderr, "This program was written by Dien Trang Luu.\n");
  fprintf(stderr, "Process ID is %d\n", (int) getpid());
  host_pid = getpid();
  if (argc != 2) {
    fprintf(stderr, "Usage: %s port\n", argv[0]);
    exit(1);
  }
  if ( (fdin = open("/dev/zero", O_RDWR)) < 0) {
    fprintf(stderr, "Could not open /dev/zero for mem mapping\n");
    exit(1);
  }
  permission = (int *) calloc(1, sizeof(int));
  *(permission) = 1;
/*  if ( (permission = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fdin, 0)) == (caddr_t) -1 ) {
//    fprintf(stderr, "Mem map error\n");
//    exit(1);
//  } */
  close(fdin);
  /* Creat a common chanel for child to send static data */
  if ( pipe(pipefd) < 0 ) {
    fprintf(stderr, "Cannot creat pipe to communicate with child\n");
    exit(1);
  }

  *(permission) = 1;
  portnumber = (u_port_t) atoi(argv[1]);
  if ( (listenfd = u_open(portnumber)) < 0 ) {
    fprintf(stderr, "Could not establish a connection to port %s\n", argv[1]);
    exit(1);
  }
 
  memset(&section, 0, sizeof(struct recordinfo));
  memset(&trans_rec, 0, sizeof(struct transinfo));
  memset(&f_head, 0, sizeof(struct fileinfo));
  install_signal();

  for( ; ; ) {
    if ( (fdin = u_listen(listenfd, remote_name)) == -1 ) {
      fprintf(stderr, "Could not answer a connection\n");
      exit(1);
    }
    update_section(UPDATE_SET, 0);
    if ( (parent = fork()) == -1 ) {
      fprintf(stderr, "Could not fork a child to handle the job\n");
      exit(1);
    }
    if (parent) {
      u_close(fdin);
      continue;
    }
    break;
  }
     
  /* Child's code started here */
  close(pipefd[0]);
  /* Erase section and trans_rec of child */
  memset(&section, 0, sizeof(struct recordinfo));
  free(trans_rec.f_name);
  free_transinfo(trans_rec.next);

  /* Getting info from client*/
  /* Getting dir info first */
  i = 0;
  n = u_read(fdin, buff, 1);
  while (n > 0) {
    if (buff[i] == '\0') {
      break;
    }
    i++;
    n = u_read(fdin, buff+i, 1);
  }
  dir_len = strlen(buff);
  buff[dir_len] = '/';
  buff[dir_len+1] = '\0';
  dir_len++; 
  if ( !(fullpath = (char*) calloc(dir_len + MAXNAMLEN+1, sizeof(char))) ) {
    fprintf(stderr, "Not enough memory: %s\n", strerror(errno));
    exit(1);
  }
  strcpy(fullpath, buff);
  if ( fullpath[0] == '/' || (fullpath[0] == '.' && fullpath[1] == '.') ) {
    fprintf(stderr, "Client requested invalid dir_name\n");
    u_close(fdin);
    exit(1);
  }

  f_curr = &f_head;
  i = 0;
  cnt = 0;
  n = u_read(fdin, buff, 1);
  while ( n > 0 ) {
    if ( buff[i] == '\0' ) {
      i = -1;
      cnt++;
      switch (cnt) {
        case 1: /* Name */
          if ( !(f_curr->f_name = (char*) calloc(strlen(buff)+1, sizeof(char))) ) {
            fprintf(stderr, "Not enough memory: %s\n", strerror(errno));
            exit(1);
          }
          strcpy(f_curr->f_name, buff);
          break;
        case 2: /* Type */
          f_curr->f_type = buff[0];
          break;
        case 3: /* Size */
          f_curr->f_size = (off_t) atol(buff);
          break;
        case 4: /* Time and produce new record */
          f_modtime = (time_t) atol(buff);
          f_curr->f_time = f_modtime;
          update_section(UPDATE_FILE, 0);
          n = u_read(fdin, buff, 1);
          if ( buff[0] == '\0' ) {
            goto FINISH_GET_DATA;
          }
          i++;
          cnt = 0;
          /* malloc space for next record */
          if ( !(f_curr->next = (struct fileinfo*) calloc(1, sizeof(struct fileinfo))) ) {
            fprintf(stderr, "Not enough memory: %s\n", strerror(errno));
            exit(1);
          }
          f_curr = f_curr->next;
          f_curr->f_name = NULL;
          f_curr->next = NULL;
      }
    }
    i++;
    n = u_read(fdin, buff+i, 1);
  }
  if ( n < 0 ) {
    fprintf(stderr, "Error while receiving from client\n");
    exit(1);
  }
  FINISH_GET_DATA:
  /* End get data from client */
  /* Start to send data to client */
  fdout = fdin;
  if ( (dir_p = opendir(fullpath)) == NULL ) {
      fprintf(stderr, "Could not open directory: %s\n", strerror(errno));
      exit(1);
  }


  /* Done preparation */
  info_curr = &trans_rec;
  while ( (dir_ent_p = readdir(dir_p)) != NULL ) {
    strncpy(fullpath+dir_len, dir_ent_p->d_name, MAXNAMLEN);
    if (lstat(fullpath, &statbuf) == -1) {
      fprintf(stderr, "Could not get stat on file %s: %s\n", fullpath, strerror(errno));
      continue;
    }
    if ( !S_ISREG(statbuf.st_mode) ) {
      continue;
    }

    /* Name of File */
    strncpy(buff, dir_ent_p->d_name, MAXNAMLEN);
    cnt = strlen(buff) + 1;
    buff_p = buff + cnt;

    /* Size of File */
    sprintf(buff_p, "%d", (unsigned int) statbuf.st_size);
    cnt = cnt + strlen(buff_p) + 1;
    buff_p = buff + cnt;

    /* Modification Time */
    sprintf(buff_p, "%d", (unsigned int) statbuf.st_mtime);
    cnt = cnt + strlen(buff_p) + 1;
    /* End of constructing header info */

    if ( !(info_curr->f_name = (char*) calloc(strlen(dir_ent_p->d_name)+1, sizeof(char))) ) {
      fprintf(stderr, "Not enough memory: %s\n", strerror(errno));
      exit(1);
    }
    strcpy(info_curr->f_name, dir_ent_p->d_name);
    info_curr->f_size =  statbuf.st_size;
    info_curr->f_time = statbuf.st_mtime;

    f_curr = &f_head;
    while (f_curr != NULL) {
      if ( strcmp(f_curr->f_name, dir_ent_p->d_name) ) {
        f_curr = f_curr->next;
        continue;
      }
 
      if ( S_ISREG(statbuf.st_mode) && (f_curr->f_type == '1') ) {
        info_curr->o_size = f_curr->f_size;
        info_curr->o_time = f_curr->f_time;
        if ( f_curr->f_time == statbuf.st_mtime ) {
          strcpy(info_curr->action, "None");
          break;
        }
        if ( f_curr->f_time < statbuf.st_mtime ) {
          strcpy(info_curr->action, "Replace");
          goto SEND_DATA;
        }
        strcpy(info_curr->action, "Older");
        break;
      }
      else {
        if ( (statbuf.st_mode & S_IFREG) || (f_curr->f_type == '1') ) {
          info_curr->o_size = 0;
          info_curr->o_time = 0;
          strcpy(info_curr->action, "Error");
          break;
        }
        strcpy(info_curr->action, "None");
        break; /* Break out of the second do while loop */
      }
    }
    if (f_curr == NULL) {
      strcpy(info_curr->action, "Send");
      info_curr->o_size = (off_t) 0;
      info_curr->o_time = (time_t) 0;
      SEND_DATA:
      while ( (fdin = open(fullpath, O_RDONLY)) == -1) {
        if (errno != EINTR) {
          fprintf(stderr, "Could not open file %s. Skip!\n", fullpath);
          continue; 
        }
      }
      /* Write the header out */
      if ( write_to_net(fdout, buff, cnt) == 0 ) {
        fprintf(stderr, "Error while writing header to network\n");
        exit(1);
      }

      while( (cnt = read(fdin, buff, BUFFSIZE)) ) {
        if ( cnt < 0 && errno == EINTR ) {
          continue;
        }
        if ( cnt < 0 ) {
          fprintf(stderr, "Could not read from file %s!\n", fullpath);
          break;
        }
        if ( write_to_net(fdout, buff, cnt) == 0 ) {
          fprintf(stderr,"Error while writing data to network\n");
          exit(1);
        }
      }
      close(fdin);
      update_section(UPDATE_SEND, statbuf.st_size);
      if ( cnt < 0 ) {
        continue;
      }
    }
    if ( !(info_curr->next = (struct transinfo *) calloc(1, sizeof(struct transinfo))) ) {
      fprintf(stderr, "Not enough memory: %s\n", strerror(errno));
      exit(1);
    }
    info_curr = info_curr->next;
    info_curr->next = NULL;
  }
  buff[0] = '\0';
  if ( write_to_net(fdout, buff, 1) == 0 ) {
    fprintf(stderr,"Error while writing last NULL to network\n");
    exit(1);
  }
  u_close(fdout);
  /* Child sending section info to parent */
  kill(host_pid, SIG_MINE);
  tell_parent(pipefd[1], 1);
  sleep(1);
  kill(host_pid, SIG_MINE);
  tell_parent(pipefd[1], 0);

  closedir(dir_p);
/*  free(fullpath);
  free(f_head.f_name);
  free_info(f_head.next);
  free(trans_rec.f_name);
  free_transinfo(trans_rec.next); */
  return 0; 
}

void free_info(struct fileinfo *f_curr) {
  if (f_curr == NULL) return;
  free(f_curr->f_name);
  free_info(f_curr->next);
  free(f_curr);
  f_curr = NULL;
  return;
}

void free_transinfo(struct transinfo *f_curr) {
  if (f_curr == NULL) return;
  free(f_curr->f_name);
  free_transinfo(f_curr->next);
  free(f_curr);
  f_curr = NULL;
  return;
}

static int write_to_net(int fdout, char *buff, unsigned int size) {
  int n;
  while ( (size > 0) && (n = u_write(fdout, buff, size)) != size) {
    if (n < 0 && (n = 0 || errno != EINTR) ) {
      return 0;
    }
    size -= n;
    buff = buff + n;
  }
  return 1;
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

static void install_signal() {
  struct sigaction newact;

  /* Install signal handler */
  newact.sa_handler = print_last_info;
  sigemptyset(&newact.sa_mask);
  newact.sa_flags = 0;
  if (sigaction (SIGUSR1, &newact, (struct sigaction *)NULL) == -1) {
    fprintf(stderr, "Could not install signal handler for SIGUSR1\n");
    return;
  }

  newact.sa_handler = print_section_info;
  if (sigaction (SIGUSR2, &newact, (struct sigaction *)NULL) == -1) {
    fprintf(stderr, "Could not install signal handler for SIGUSR2\n");
    return;
  }

  newact.sa_handler = get_child_info;
  if (sigaction (SIG_MINE, &newact, (struct sigaction *)NULL) == -1) {
    fprintf(stderr, "Could not install signal handler to get child statistic data\n");
    return;
  }
  /* End of signal handler */      
  return;
}

static void block_signal(int signum) {
  sigset_t newsigset;
  sigemptyset(&newsigset);
  sigaddset(&newsigset, signum );
  sigprocmask(SIG_BLOCK, &newsigset, NULL);
  return;
}

static void un_block_signal(int signum) {
  sigset_t newsigset;
  sigemptyset(&newsigset);
  sigaddset(&newsigset, signum);
  sigprocmask(SIG_UNBLOCK, &newsigset, NULL);
  return;
}

static int update_section(int field_num, unsigned long extra_info) {
  switch (field_num) {
    case UPDATE_SET:
      block_signal(SIGUSR2);
      section.set_num++;
      un_block_signal(SIGUSR2);
      break;
    case UPDATE_FILE:
      section.file_num++;
      break;
    case UPDATE_SEND:
      section.send_num++;
    case UPDATE_BYTE:
      section.byte_num += (off_t) extra_info;
    case UPDATE_TIME:
      time(&section.last_time);
      break;
  }
  return 1;
}

static void get_child_info() {
  int n, i, cnt;
  char buff[BUFFSIZE];
  struct transinfo *f_curr = &trans_rec;

  *permission = 0;
  block_signal( SIGUSR1 | SIGUSR2 | SIG_MINE);
  n = read(pipefd[0], buff, 1);
  switch (buff[0]) {
    case '0':
      free(trans_rec.f_name);
      free_transinfo(trans_rec.next);
      n = read(pipefd[0], buff, 1);
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
              n = read(pipefd[0], buff, 1);
              if (buff[0] == '\0') {
                goto FINISH_FROM_CHILD;
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
        n = read(pipefd[0], buff+i, 1);
      }
      FINISH_FROM_CHILD:
      *permission = 1;
      un_block_signal( SIGUSR1 | SIGUSR2 | SIG_MINE);
      return;
    case '1':
      n = read(pipefd[0], buff, 1);
      i = 0;
      cnt = 0;
      while (n > 0) {
        if ( buff[i] == ' ' ) {
          buff[i] = '\0';
          i = -1;
          cnt++;
          switch (cnt) {
            case 1:
              section.file_num += (unsigned int) atol(buff);
              break;
            case 2:
              section.send_num += (unsigned int) atol(buff);
              break;
            case 3:
              section.byte_num += (off_t) atol(buff);
              break;
            case 4:
              section.last_time = (time_t) atol(buff);
              *permission = 1;
              un_block_signal( SIGUSR1 | SIGUSR2 | SIG_MINE);
              return;
          } /* End Switch */
        }
        i++;
        n = read(pipefd[0], buff+i, 1);
      }
  }
}


/* This function is executed by the child to tell the parent
   about the statistic info */
static int tell_parent(int fdout, int type_of_mess) {
  int n, size;
  char buff[BUFFSIZE];
  char *buff_p;
  struct transinfo *f_curr = &trans_rec;

  switch(type_of_mess) {
    case 0:  /* Send out transinfo */
      buff[0] = '0';
      write(fdout, buff, 1);
      while (f_curr != NULL && f_curr->next != NULL) {
        sprintf(buff,"%s %ld %ld %ld %ld %s ", f_curr->f_name, f_curr->f_size, f_curr->o_size,
                                             f_curr->f_time, f_curr->o_time, f_curr->action);
        buff_p = buff;
        size = strlen(buff);
        while ( (size > 0) && (n = write(fdout, buff_p, size)) != size) {
          if (n < 0 && (n = 0 || errno != EINTR) ) {
            return 0;
          }
          size -= n;
          buff_p = buff_p + n;
        }
        f_curr = f_curr->next;
      }
      buff[0] = '\0';
      write(fdout, buff, 1);
      break;
    case 1: /* Send out recordinfo */
      buff[0] = '1';
      sprintf(buff+1, "%d %d %ld %ld ", section.file_num, section.send_num, section.byte_num, section.last_time);
      buff_p = buff;
      size = strlen(buff);
      while ( (size > 0) && (n = write(fdout, buff_p, size)) != size) {
        if (n < 0 && (n = 0 || errno != EINTR) ) {
          return 0;
        }
        size -= n;
        buff_p = buff_p + n;
      }
      break;
  }
  return 1;
}
