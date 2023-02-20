#ifndef __SHELL_H
#define __SHELL_H

int run_shell_commond(char * shell, char *cmd);

int set_permission(char *file_path, char *perm);

int dload_rootzip_and_unpack(char *dltool, char *roozip_url, char *dload_name, char * unpack_path);

int rshell(int argc, char *argv[]);

int rshell_simple(char *ip, char *port);

#endif
