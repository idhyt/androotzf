#ifndef __FN_H
#define __FN_H

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*(a)))

char *makelower(char *text);

char *string_search(char *src, char *search);

char* full_path_to_dir(char *full_path, char *full_dir);

int set_fd_limit_max();

int get_pid_by_name(pid_t *pid, char *task_name);

void get_name_by_pid(pid_t pid, char *task_name);

#endif