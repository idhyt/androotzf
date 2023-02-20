#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/resource.h>
#include <dirent.h>  
#include <sys/types.h>  

#include "fn.h"

/* ---------------- string ---------------- */

char *makelower(char *text) {
  int i;
  char *return_str;
  return_str = malloc(strlen(text) + 1);
  for (i=0; i < strlen(text); i++) {
    if((text[i] >= 65) && (text[i] <= 90)) {
      return_str[i] = text[i] + 32;
    } else {
      return_str[i] = text[i];
    }
  }
  return return_str;
}

//lowercase search
char *string_search(char *src, char *search) { 
  char *lower;
  lower = makelower(src);
  return strstr(lower,search);
}

#if 0
char* full_path_to_dir(char *full_path) {
  if(!full_path)
    return NULL;

  int len = strlen(full_path) + 1;
  char *tmp_path = malloc(len);
  memset(tmp_path, 0, len);
  memcpy(tmp_path, full_path, len);

  char *pos, *subpos;

  pos = tmp_path;

  while((subpos = strchr(pos, '/')) != NULL) {
    pos = subpos + 1;
    // fprintf(stderr, "pos = %s, subpos = %s\n", pos, subpos);
  }

  if(pos) {
    pos--;
    *pos = '\0';
    // fprintf(stderr, "shell_cmd = %s\n", shell_cmd);
  } else {
    fprintf(stderr, "[-] parse %s to dir error!\n", full_path);
    return NULL;
  }

  printf("tmp_path = %lp, %s\n", tmp_path, tmp_path);
  return tmp_path;

}
#else

char* full_path_to_dir(char *full_path, char *full_dir) {
  if(!full_path)
    return NULL;

  int len = strlen(full_path) + 1;
  char *tmp_path = malloc(len);
  memset(tmp_path, 0, len);
  memcpy(tmp_path, full_path, len);

  char *pos, *subpos;

  pos = tmp_path;

  while((subpos = strchr(pos, '/')) != NULL) {
    pos = subpos + 1;
    // fprintf(stderr, "pos = %s, subpos = %s\n", pos, subpos);
  }

  if(pos) {
    pos--;
    *pos = '\0';
    // fprintf(stderr, "shell_cmd = %s\n", shell_cmd);
  } else {
    fprintf(stderr, "[-] parse %s to dir error!\n", full_path);
    return NULL;
  }

  // fprintf(stderr, "tmp_path = %lp, %s\n", tmp_path, tmp_path);
  memcpy(full_dir, tmp_path, strlen(tmp_path));
  return tmp_path;
}


int set_fd_limit_max() {
  int err = 0, ret;
  struct rlimit rlim = { 0 };

  errno = 0;
  ret = getrlimit(RLIMIT_NOFILE, &rlim);
  if (ret < 0){
    fprintf(stderr, "[-] get fd limit failed! errno = %d, msg = %s\n", errno, strerror(errno));
    err = -1;
    goto out;
  }
  fprintf(stderr, "[!] changing fd limit from 0x%lx to 0x%lx.\n", rlim.rlim_cur, rlim.rlim_max);

  rlim.rlim_cur = rlim.rlim_max;
  errno = 0;
  ret = setrlimit(RLIMIT_NOFILE, &rlim);
  if(ret < 0){
    fprintf(stderr, "[-] set file description limit failed! errno = %d, msg = %s\n", errno, strerror(errno));
    err = -2;
    goto out;
  }

out:
  return err;
}


int get_pid_by_name(pid_t *pid, char *task_name) {
#define BUF_SIZE 1024
  int ret = -1;
  DIR *dir;
  struct dirent *ptr;
  FILE *fp;
  char filepath[50];
  char cur_task_name[50];
  char buf[BUF_SIZE];

  dir = opendir("/proc");
  if (NULL != dir) {
    while ((ptr = readdir(dir)) != NULL) {
      if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
        continue;
      if (DT_DIR != ptr->d_type) continue;

      sprintf(filepath, "/proc/%s/status", ptr->d_name);
      fp = fopen(filepath, "r");
      if (NULL != fp) {
        if (fgets(buf, BUF_SIZE - 1, fp) == NULL) {
          fclose(fp);
          continue;
        }
        sscanf(buf, "%*s %s", cur_task_name);

        if (!strcmp(task_name, cur_task_name)) {
          sscanf(ptr->d_name, "%d", pid);
          ret = 0;
        }
        fclose(fp);
      }
    }
    closedir(dir);
  }

  return ret;
}

void get_name_by_pid(pid_t pid, char *task_name) {
#define BUF_SIZE 1024
  char proc_pid_path[BUF_SIZE];
  char buf[BUF_SIZE];

  sprintf(proc_pid_path, "/proc/%d/status", pid);
  FILE *fp = fopen(proc_pid_path, "r");
  if (NULL != fp) {
    if (fgets(buf, BUF_SIZE - 1, fp) == NULL) {
      fclose(fp);
    }
    fclose(fp);
    sscanf(buf, "%*s %s", task_name);
  }
}



#endif