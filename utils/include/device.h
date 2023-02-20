#ifndef __DEVICE_H
#define __DEVICE_H

#define BUILD_PROP "/system/build.prop"

struct device_info {
  char model[0x80];
  char proc_version[0x100];
  char build_version[0x10];
  // char brand[0xff];
  // char manufacturer[0xff];
  // char android_sdk[0xff];
  char root_parameters[0x400];
};

int getprop(const char *key, char *value, int size);

// int get_proc_version(char *value);

int get_device_info(struct device_info *devinfo);

#endif