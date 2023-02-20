#ifndef __DICT_H
#define __DICT_H

typedef struct _node {
    char *key; 
    void *value; 
    struct _node *next; 
} bucket_t; 

typedef struct {
    int size_pos; //size_pos in prime_array, the size of dict = prime_array[size_pos]
    int ele_num; //ele number in dict
    bucket_t **bucket; 
} dict_t; 

/* API */

int dict_init(dict_t *); 

int dict_size(dict_t *); 

void dict_set(dict_t *, char *, void *); 

int dict_get(dict_t *, char *, void **); 

int dict_del(dict_t *, char *); 

void dict_keys(dict_t *, char **); 

void dict_reset(dict_t *); 

void dict_destory(dict_t *); 

int dict_iter(dict_t *, char **, void **); 

int dict_get_ulval(dict_t *, char *name, unsigned long *value);

int dict_get_str(dict_t *, char *name, char *value, int len);

#endif 