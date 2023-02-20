// #include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdarg.h>

#include "log.h"

// get current date with format yyyyMMdd
int currentdate(char *currDate) {
  struct tm *ptm = NULL;
  time_t tme;
  tme = time(NULL);
  ptm = localtime(&tme);
  char szTime[256];
  memset(szTime, 0, 256);
  sprintf(szTime, "%d%02d%02d", (ptm->tm_year + 1900), ptm->tm_mon + 1,
          ptm->tm_mday);
  strcpy(currDate, szTime);
  return 0;
}

// get current date time with format yyyy-MM-dd hh:mm:ss
int currenttime(char *currTime) {
  struct tm *ptm = NULL;
  time_t tme;
  tme = time(NULL);
  ptm = localtime(&tme);
  char szTime[256];
  memset(szTime, 0, 256);
  sprintf(szTime, "[%d-%02d-%02d %02d:%02d:%02d] ", (ptm->tm_year + 1900),
          ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min,
          ptm->tm_sec);
  strcpy(currTime, szTime);
  return 0;
}

char * SET_LOG_FILE = NULL;
void set_logfile_path(char *pfile) {
  log_dump(LOG_NORMAL, "[!] set log file path from %s to %s\n", LOG_FILE, pfile);
  SET_LOG_FILE = pfile;
}

// write log
void log_dump(int loglev, const char *format, ...) {
  if (loglev > LOG_LEVEL) return;

  char temp[1024];
  memset(temp, 0, 1024);

  va_list args;
  va_start(args, format);
  vsprintf(temp, format, args);
  va_end(args);

  char currTime[256];
  memset(currTime, 0, 256);
  currenttime(currTime);

  char logs[0x400] = {0};
  memset(logs, 0, sizeof(logs));

  if(LOG_CONSOLE && !SET_LOG_FILE) {
    sprintf(logs, "%s", temp);
  } else {
    sprintf(logs, "%s%s", currTime, temp);
  }

  FILE *pf = NULL;
  if(LOG_CONSOLE && !SET_LOG_FILE) {
    LOGV("%s", logs);
    return;
  } else if (SET_LOG_FILE) {
    pf = fopen(SET_LOG_FILE, "aw");
  } else {
    pf = fopen(LOG_FILE, "aw");
  }

  if (pf) {
    /*
    fputs(currTime, pf);
    fputs(temp, pf);
    fputs("\n", pf);
    */
    fputs(logs, pf);
    fclose(pf);
  }
  return;
}

void loghex(int loglev, unsigned char *logmsg, int len) {
  if (loglev > LOG_LEVEL) return;
  char msg[1024];
  int ilen, i;
  ilen = len;
  if (ilen > 512) ilen = 512;
  for (i = 0; i < ilen; i++) {
    sprintf(msg + i * 2, "%02X", logmsg[i]);
  }
  log_dump(loglev, msg);
}
