#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>

#ifndef MAXNAMLEN
#define MAXNAMLEN 256
#endif

#define BUFFSIZE 256
#define DATASIZE 26
#define NUM_SIZE 20

#define UPDATE_SET 0
#define UPDATE_FILE 1
#define UPDATE_SEND 2
#define UPDATE_BYTE 3
#define UPDATE_TIME 4

static struct recordinfo_str {
  char set_num[NUM_SIZE];
  char file_num[NUM_SIZE];
  char send_num[NUM_SIZE];
  char byte_num[NUM_SIZE];
  char last_time[DATASIZE];
} section; 

static struct recordinfo {
  unsigned int set_num;
  unsigned int file_num;
  unsigned int send_num;
  off_t byte_num;
} section_int; 

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
};

static void install_signal();
static void block_signal(int signum);
static void un_block_signal(int signum);

static void free_info(struct fileinfo *f_curr);
static void free_transinfo(struct transinfo *f_curr);
static int write_to_pipe(int fdout, char *buff, unsigned int size);

static int update_info();
static int update_section(int field_num, unsigned long extra_info);

static void print_last_info();
static void print_section_info();


int main(int argc, char **argv) {
  DIR *dir_p;
  struct dirent *dir_ent_p;
  struct stat statbuf;
  struct fileinfo f_head;
  struct fileinfo *f_curr;
  
  struct transinfo info_list;
  struct transinfo *info_curr;

  char *fullpath;
  int dir_len;

  int fdin, fdout;
  mode_t fifo_mode = S_IRUSR | S_IWUSR; 
  char buff[MAXNAMLEN+DATASIZE];
  char *buff_p;

  int i, n, cnt;
  time_t f_modtime;

  fprintf(stderr,"This program was written by Dien Trang Luu.\n");
  fprintf(stderr,"Process ID is %d\n", (int) getpid());
  if (argc != 3) {
    fprintf(stderr, "Usage: %s infopipe sendpipe\n", argv[0]);
    exit(1);
  }
 
  if ( (mkfifo(argv[1], fifo_mode) == -1) && (errno != EEXIST) ) {
    fprintf(stderr, "Could not creat the info named pipe: %s\n", argv[1]);
    exit(1);
  }

  if ( (mkfifo(argv[2], fifo_mode) == -1) && (errno != EEXIST) ) {
    fprintf(stderr, "Could not creat the send named pipe: %s\n", argv[2]);
    exit(1);
  }
  memset(&section_int, 0, sizeof(struct recordinfo));
  memset(&section, 0, sizeof(struct recordinfo_str));
  memset(&trans_rec, 0, sizeof(struct transinfo));
  install_signal();
  /* Big Loop needs to be started here */
  LOOP:
  while ( (fdin = open(argv[1], O_RDONLY)) == -1 ) {
    if ( errno != EINTR ) {
      fprintf(stderr, "Could not open named pipe: %s\n", argv[1]);
      exit(1);
    }
  }
  update_section(UPDATE_SET, 0);
  f_head.f_name = NULL;
  f_head.next = NULL;
  f_curr = &f_head;

  /* Getting info from infopipe*/
  i = 0;
  cnt = 0;
  n = read(fdin, buff, 1);
  while ( n > 0 ) {
    if ( buff[i] == '\0') {
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
          if ( !(f_curr->next = (struct fileinfo*) calloc(1, sizeof(struct fileinfo))) ) {
            fprintf(stderr, "Not enough memory: %s\n", strerror(errno));
            exit(1);
          }
          f_curr = f_curr->next;
          f_curr->f_name = NULL;
          f_curr->next = NULL;
          n = read(fdin, buff, 1);
          cnt = 0;
          break;
      }
    }
    i++;
    n = read(fdin, buff+i, 1);
    /* Tricky code above, need explaintation */
  }
  if ( n < 0 ) {
    fprintf(stderr, "Error while reading from infopipe\n");
    exit(1);
  }
  close(fdin);
  /* End get info from infopipe */

  if ( (dir_p = opendir("./")) == NULL ) {
      fprintf(stderr, "Could not open current directory: %s\n", strerror(errno));
      exit(1);
  }

  dir_len = strlen("./");
  if ( !(fullpath = (char*) calloc(dir_len + MAXNAMLEN+1, sizeof(char))) ) {
    fprintf(stderr, "Not enough memory: %s\n", strerror(errno));
    exit(1);
  }
  strcpy(fullpath, "./");


  while ( (fdout = open(argv[2], O_WRONLY)) == -1 ) {
    if (errno != EINTR) {
      fprintf(stderr, "Could not open named pipe: %s\n", argv[2]);
      exit(1);
    }
  }
  /* Done preparation */
  info_list.next = NULL;
  info_curr = &info_list;
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
    /* End of constructing file info */

    if ( !(info_curr->f_name = (char*) calloc(strlen(dir_ent_p->d_name)+1, sizeof(char))) ) {
      fprintf(stderr, "Not enough memory: %s\n", strerror(errno));
      exit(1);
    }
    strcpy(info_curr->f_name, dir_ent_p->d_name);
    info_curr->f_size =  statbuf.st_size;
    info_curr->f_time = statbuf.st_mtime;

    f_curr = &f_head;
    do {
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
    while (f_curr != NULL && f_curr->next != NULL);
    if (f_curr == NULL || f_curr->next == NULL) {
      strcpy(info_curr->action, "Send");
      info_curr->o_size = 0;
      info_curr->o_time = 0;
      SEND_DATA:
      while ( (fdin = open(fullpath, O_RDONLY)) == -1) {
        if (errno != EINTR) {
          fprintf(stderr, "Could not open file %s. Skip!\n", fullpath);
          continue; 
        }
      }
      /* Write the header out */
      if ( write_to_pipe(fdout, buff, cnt) == 0 ) {
        fprintf(stderr, "Error while writing header to pipe\n");
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
        if ( write_to_pipe(fdout, buff, cnt) == 0 ) {
          fprintf(stderr,"Error while writing data to pipe\n");
          exit(1);
        }
      }
      close(fdin);
      if ( cnt < 0 ) {
        continue;
      }
      buff[0] = '\0';
      if ( write_to_pipe(fdout, buff, 1) == 0 ) {
        fprintf(stderr,"Error while writing last NULL to pipe\n");
        exit(1);
      }
      update_section(UPDATE_SEND, statbuf.st_size);
    }
    if ( !(info_curr->next = (struct transinfo *) calloc(1, sizeof(struct transinfo))) ) {
      fprintf(stderr, "Not enough memory: %s\n", strerror(errno));
      exit(1);
    }
    info_curr->next->next = NULL;
    info_curr = info_curr->next;
  }
  close(fdout);
  update_info( &info_list);
  closedir(dir_p);
  free(fullpath);
  f_curr = f_head.next;
  info_curr = info_list.next;
  free_info(f_curr);
  free_transinfo(info_curr);
  goto LOOP;
  return 0;
}

void free_info(struct fileinfo *f_curr) {
  if (f_curr == NULL) return;
  if (f_curr->next != NULL) {
    free_info(f_curr->next);
  }
  free(f_curr->f_name);
  free(f_curr);
  f_curr = NULL;
  return;
}

void free_transinfo(struct transinfo *f_curr) {
  if (f_curr == NULL) return;
  if (f_curr->next != NULL) {
    free_transinfo(f_curr->next);
  }
  free(f_curr->f_name);
  free(f_curr);
  f_curr = NULL;
  return;
}

static int write_to_pipe(int fdout, char *buff, unsigned int size) {
  int n;
  while ( (size > 0) && (n = write(fdout, buff, size)) != size) {
    if (n < 0 && (n = 0 || errno != EINTR) ) {
      return 0;
    }
    size -= n;
    buff = buff + n;
  }
  return 1;
}

static void print_last_info() {
  struct transinfo *p_curr;
  block_signal(SIGUSR1 | SIGUSR2);
  p_curr = &trans_rec;
  printf("\nLast active information\n");
  printf("=======================\n");
    
  do {
    printf("%-25s\t%10ld %24.24s\t", p_curr->f_name, p_curr->f_size, ctime(&p_curr->f_time));
    if (p_curr->o_time || p_curr->o_size ) {
      printf("%10ld %24.24s  %s\n", p_curr->o_size, ctime(&p_curr->o_time), p_curr->action);
    }
    else
      printf("\t%33s\n", p_curr->action);
  p_curr = p_curr->next;
  } while ( p_curr->next != NULL);
  fflush(stdout);
  un_block_signal(SIGUSR1 | SIGUSR2);
}

static void print_section_info() {

  /* No signal can be caught during the process of this function */
  char m0[] = "\n\nSummary Information\n===================\n";
  char m1[] = "Number of data set: ";
  char m2[] = "Number of file info receive: ";
  char m3[] = "Number of file send: ";
  char m4[] = "Number of byte send: ";
  char m5[] = "Time of last action: ";

  block_signal(SIGUSR1 | SIGUSR2);
  write(STDOUT_FILENO, m0, strlen(m0));
  write(STDOUT_FILENO, m1, strlen(m1));
  write(STDOUT_FILENO, section.set_num, strlen(section.set_num));
   
  write(STDOUT_FILENO, m2, strlen(m2));
  write(STDOUT_FILENO, section.file_num, strlen(section.file_num));

  write(STDOUT_FILENO, m3, strlen(m3));
  write(STDOUT_FILENO, section.send_num, strlen(section.send_num));

  write(STDOUT_FILENO, m4, strlen(m4));
  write(STDOUT_FILENO, section.byte_num, strlen(section.byte_num));

  write(STDOUT_FILENO, m5, strlen(m5));
  write(STDOUT_FILENO, section.last_time, strlen(section.last_time));
  un_block_signal(SIGUSR1 | SIGUSR2);
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

static int update_info(struct transinfo *infolist) {
  struct transinfo *n_curr;
  struct transinfo *o_curr;
  block_signal(SIGUSR1);
  free_transinfo(trans_rec.next);
  trans_rec.next = NULL;
  n_curr = &trans_rec;
  o_curr = infolist;
  do {
    if ( !(n_curr->f_name = (char*) calloc(strlen(o_curr->f_name)+1, sizeof(char))) ) {
      fprintf(stderr, "Not enough memory\n!");
      exit(1);
    }
    strcpy(n_curr->f_name, o_curr->f_name);  
    n_curr->f_size = o_curr->f_size;
    n_curr->o_size = o_curr->o_size;
    n_curr->f_time = o_curr->f_time;
    n_curr->o_time = o_curr->o_time;
    strcpy(n_curr->action, o_curr->action);
    o_curr = o_curr -> next;
    if ( !(n_curr->next = (struct transinfo *) calloc(1, sizeof(struct transinfo))) ) {
      fprintf(stderr, "Not enough memory\n!");
      exit(1);
    }
    n_curr = n_curr->next;
    n_curr->next = NULL;
  } while ( o_curr->next != NULL );
  un_block_signal(SIGUSR1);
  return 1;
}

static int update_section(int field_num, unsigned long extra_info) {
  time_t send_time;
  block_signal(SIGUSR2);
  switch (field_num) {
    case UPDATE_SET:
      section_int.set_num++;
      sprintf(section.set_num,"%d", section_int.set_num);
      strcat(section.set_num,"\n");
      break;
    case UPDATE_FILE:
      section_int.file_num++;
      sprintf(section.file_num,"%d", section_int.file_num);
      strcat(section.file_num,"\n");
      break;
    case UPDATE_SEND:
      time(&send_time);
      section_int.send_num++;
      sprintf(section.send_num,"%d", section_int.send_num);
      strcat(section.send_num,"\n");
    case UPDATE_BYTE:
      section_int.byte_num += (off_t) extra_info;
      sprintf(section.byte_num,"%ld", section_int.byte_num);
      strcat(section.byte_num,"\n");
    case UPDATE_TIME:
      strcpy(section.last_time, ctime(&send_time));
      break;
  }
  un_block_signal(SIGUSR2);
  return 1;
}
