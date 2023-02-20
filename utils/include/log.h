#ifndef __LOG_H
#define __LOG_H

#define LOG_CONSOLE  1 // 1, log will to stderr.

#define LOG_FILE "/data/local/tmp/utils.log"

#ifdef LOG_CONSOLE
  // #define LOGV(...) { printf("%s @ %s:%d with log: ", __FILE__, __FUNCTION__, __LINE__); printf(__VA_ARGS__); fflush(stdout); }
  #define LOGV(...) { printf(__VA_ARGS__); fflush(stdout); }
#else
  #define LOGV(...)
#endif

//根据日志级别写日志
#define LOG_ERR 0  							// 0=记录错误日志 -
#define LOG_NORMAL 1  						// 1=记录日常日志和错误日志 +
#define LOG_DEBUG 2  						// 2=记录错误日志日常日志调试日志 * !

#ifdef LDEBUG 								// LOCAL_CFLAGS += -DLDEBUG
	#define LOG_LEVEL LOG_DEBUG
#else
	#define LOG_LEVEL LOG_ERR
#endif

#ifndef _MSC_VER
#define _MSC_VER 1600
#endif

void set_logfile_path(char *pfile);
void log_dump(int loglev, const char *format, ...);
void loghex(int loglev, unsigned char *logmsg, int len);

#endif
