#include "device.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <fcntl.h>

#include "log.h"

#if 0
int getprop(const char *key, char *value, int size) {
  FILE *pf = NULL;
  char *ps = NULL;
  char *pe = NULL;
  char *file_buf = NULL;
  struct stat st = { 0 };
  int file_size = 0;
  int count = 0;
  char key_buf[0x100] = { '\0' };

  errno = 0;
  if (stat("/system/build.prop", &st) == -1) {
    log_dump(LOG_ERR, "[-] stat false: errno = %d, msg = %s\n", errno, strerror(errno));
    return -1;
  }

  file_size = st.st_size;
  file_buf = (char *)malloc(sizeof(char) * file_size + 1);
  memset(file_buf, 0, sizeof(char) * file_size + 1);

  errno = 0;
  pf = fopen("/system/build.prop", "r");
  if (pf == NULL) {
    log_dump(LOG_ERR, "[-] fopen false: pf = %p, errno = %d, msg = %s\n", pf, errno, strerror(errno));
    return -1;
  }

  count = fread(file_buf, 1, st.st_size, pf);
  snprintf(key_buf, 0x100 - 1, "%s=", key);

  ps = strstr(file_buf, key_buf);
  if(ps == NULL) {
    log_dump(LOG_ERR, "[-] find key = [%s] false\n", key);
    goto fail;
  }

  pe = strstr(ps, "\n");
  if (pe == NULL) goto fail;

  memcpy(value, ps + strlen(key) + 1, pe - ps - strlen(key) - 1);

  fclose(pf);
  return 0;

 fail:
  fclose(pf);
  return -1;
}
#else

int getprop(const char *key, char *value, int size) {
  int err = 0, ret, fd;

  struct stat st = { 0 };

  char *file_buf = NULL;
  int buf_len = 0;

  char key_buf[0xff] = { 0 };
  memset(key_buf, 0, sizeof(key_buf));
  int key_len = 0;

  char *pos = NULL, *subpos = NULL;

  errno = 0;
  if (stat(BUILD_PROP, &st) == -1) {
    log_dump(LOG_ERR, "[-] stat false: errno = %d, msg = %s\n", errno, strerror(errno));
    err = -1;
    goto out;
  }

  buf_len = st.st_size + 1;
  file_buf = (char *)malloc(buf_len);
  memset(file_buf, 0, buf_len);

  errno = 0;
  fd = open(BUILD_PROP, O_RDONLY);
  if (fd < 0) {
    log_dump(LOG_ERR, "[-] fopen false: fd = %d, errno = %d, msg = %s\n", fd, errno, strerror(errno));
    err = -2;
    goto out;
  }

  errno = 0;
  // ret = fread(file_buf, 1, st.st_size, pf);
  ret = read(fd, file_buf, st.st_size);
  if(ret != st.st_size) {
    log_dump(LOG_ERR, "[-] fread false: ret = %d, errno = %d, msg = %s\n", ret, errno, strerror(errno));
    err = -3;
    goto fail;
  }

  snprintf(key_buf, sizeof(key_buf) - 1, "%s=", key);

  if((pos = strstr(file_buf, key_buf)) != NULL) {
    pos += strlen(key_buf);
    if((subpos = strchr(pos, '\n'))!= NULL) {
      *subpos = '\0';
    }
  }

  if(!pos || !subpos) {
    log_dump(LOG_ERR, "[-] find key = [%s] false\n", key);
    err = -4;
    goto fail;
  }

  // log_dump(LOG_DEBUG, "[+] find %s %s\n", key_buf, pos);

  key_len = subpos - pos;
  if(key_len < 0 || key_len > size) {
    log_dump(LOG_ERR, "[-] calc key len false, key_len = %d\n", key_len);
    err = -5;
    goto fail;
  }

  memcpy(value, pos, key_len);

fail:
  close(fd);

out:
  return err;
}


#endif


static int get_proc_version(char *value) {

  int fd, ret;
  int count = 0;

  errno = 0;
  fd = open("/proc/version", 0);  // O_RDONLY
  if (fd < 0) {
    log_dump(LOG_ERR, "[-] open false: fd = %d, errno = %d, msg = %s\n", fd, errno, strerror(errno));
    return -1;
  }

  errno = 0;
  ret = read(fd, value, 0xff-1);
  if (ret < 0) {
    log_dump(LOG_ERR, "[-] read false: ret = %d, errno = %d, msg = %s\n", ret, errno, strerror(errno));
    goto fail;
  }
  // del last \n
  *(value + strlen(value) -1) = '\0';

  close(fd);
  return 0;

 fail:
  close(fd);
  return -1;
}

int get_device_info(struct device_info *devinfo) {
  int err = 0;
  if(getprop("ro.product.model", devinfo->model, sizeof(devinfo->model)) < 0) {
    err = -1;
    log_dump(LOG_ERR, "[-] get 'ro.product.model' false!\n");
    goto fail;
  }

  if(get_proc_version(devinfo->proc_version) < 0) {
    err = -2;
    log_dump(LOG_ERR, "[-] get 'proc version' false!\n");
    goto fail;
  }

  if(getprop("ro.build.version.release", devinfo->build_version, sizeof(devinfo->build_version)) < 0) {
    err = -3;
    log_dump(LOG_ERR, "[-] get 'build version' false!\n");
    goto fail;
  }

#if 0
  getprop("ro.product.brand", devinfo->brand, 256);
  getprop("ro.product.manufacturer", devinfo->manufacturer, 256);
  getprop("ro.build.version.sdk", devinfo->android_sdk, 0x10);
#endif

#if 0
  struct utsname name = { 0 };
  char android_sdk[0x10] = { '\0' };
  int major = 0, minor = 0, other = 0;
  int cnt = 0;

  memset(&name, 0, sizeof(name));
  uname(&name);
  cnt = sscanf(name.release, "%d.%d.%d", &major, &minor, &other);
  printf("[release: %s]\n", name.release);
  printf("[version: %s]\n", name.version);
  printf("[machine: %s]\n", name.machine);
  printf("[sdk: %s]\n", android_sdk);
#endif

fail:
  return err;
}
