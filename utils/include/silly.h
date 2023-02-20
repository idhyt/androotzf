/*
 * by pass android kernel protect
 *
 * Copyright (C) 2017 by idhyt3r@gmail.com
 *
 */


#ifndef __SILLY_H
#define __SILLY_H

#include "dict.h"

#ifndef __SECBYPASS_H
#define __SECBYPASS_H

#define SAMSUNG "samsung"
#define VIVO "vivo"
#define HUAWEI "huawei"
#define HONOR "honor"
#endif

int vendor_protect_bypass(dict_t *transl_param);

int obtain_kaslr_kernel_base(dict_t *transl_param, void *kaslr_kernel_base);

int silly_everything_after_root(dict_t *transl_param);

int check_dm_verity_status();

#endif