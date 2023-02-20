/*
    https://github.com/hit9/C-dict
*/

#include "dict.h"
#include <stdlib.h>
#include <string.h>

/*
 * prime array
 */
static int prime_array[] = {
    7,         17,        37,        79,         163,       331,
    673,       1361,      2729,      5471,       10949,     21911,
    43853,     87719,     175447,    350899,     701819,    1403641,
    2807303,   5614657,   11229331,  22458671,   44917381,  89834777,
    179669557, 359339171, 718678369, 1437356741, 2147483647};

/* the max of size_pos is 28 */

#define SIZE_POS_MAX 28

/*
 * bkdr hash function
 */
static unsigned int BKDRHash(char *str) {
  unsigned int seed = 131;  // 31 131 1313 13131 131313 etc..
  unsigned int hash = 0;

  while (*str) {
    hash = hash * seed + (*str++);
  }

  return (hash & 0x7FFFFFFF);
}

/*
 * get index(position) by key
 */
static int get_index(dict_t *dict, char *key) {
  return BKDRHash(key) % (prime_array[dict->size_pos]);
}

/*
 * init dict's pointer
 */
int dict_init(dict_t *dict) {
  dict->size_pos = 0;
  dict->ele_num = 0;
  dict->bucket = (bucket_t **)calloc(
      prime_array[0],
      sizeof(bucket_t *));  // each list's head node pointer set to 0

  if (dict->bucket == NULL) return 0;
  return 1;
}

/*
 * get dict's size
 */
int dict_size(dict_t *dict) { return dict->ele_num; }

/*
 * resize the dict if ele_num == dict's size.we will reinsert all nodes
 * again.but we don't need to check the keys
 */

static int dict_resize(dict_t *dict) {
  if (dict->size_pos - SIZE_POS_MAX >= 0) return 0;  // top the max, can't
                                                     // resize

  // record olds
  int old_size = prime_array[dict->size_pos];  // record old size
  bucket_t **old_bucket = dict->bucket;  // record the old bucket

  // resize
  dict->size_pos += 1;
  dict->bucket = (bucket_t **)calloc(
      prime_array[dict->size_pos],
      sizeof(bucket_t *));  // new bigger 0 space for dict->bucket

  /* --- begin reinsert .. --- */
  int i, index;  // iterator for old_bucket[]
  bucket_t *t, *p, *q, *node;

  for (i = 0; i < old_size; i++) {
    for (t = old_bucket[i]; t;
         q = t, t = t->next, free(q)) {  // free the old node the same time
      // new a node
      node = (bucket_t *)malloc(sizeof(bucket_t));
      node->key = t->key;
      node->value = t->value;
      node->next = 0;

      index = get_index(dict, node->key);
      p = (dict->bucket)[index];  // get new dict->bucket[index] first node
                                  // pointer

      // append node to the list
      if (!p)
        (dict->bucket[index]) = node;
      else {
        for (; p->next; p = p->next)
          ;
        p->next = node;
      }
    }
  }
  free(old_bucket);  // free old bucket
  return 1;  // success
}

/*
 * if key exists, set the new value.else add a pair of 'key => value' to dict
 */
void dict_set(dict_t *dict, char *key, void *value) {
  // if need resize
  if (prime_array[dict->size_pos] - dict->ele_num <= 1) dict_resize(dict);
  // get index by hash function
  int index = get_index(dict, key), flag = 0;  // flag for if find the key

  // get its bucket's head node's pointer
  bucket_t *first = (dict->bucket)[index], *temp = first;

  for (; temp; temp = temp->next)
    if (strcmp(temp->key, key) == 0) {
      temp->value = value;
      flag = 1;
    }

  if (!flag) {  // not find key, new a node , and append it to the list

    bucket_t *node = (bucket_t *)malloc(sizeof(bucket_t));
    node->key = key;
    node->value = value;
    node->next = NULL;

    if (!first)
      (dict->bucket)[index] = node;
    else {
      for (; first->next; first = first->next)
        ;
      first->next = node;
    }
    dict->ele_num += 1;
  }
}

int dict_get(dict_t *dict, char *key, void **value) {
  int index = get_index(dict, key);
  bucket_t *first = (dict->bucket)[index];

  for (; first; first = first->next) {  // first as a iterator pointer, scan the
                                        // list begin from dict->bucket[index]
    if (strcmp(key, first->key) == 0) {
      *value = first->value;
      return 1;  // success
    }
  }
  return 0;  // failed
}

int dict_del(dict_t *dict, char *key) {
  int index = get_index(dict, key);
  bucket_t *p = (dict->bucket)[index];
  if (!p) return 0;  // failed.
  if (strcmp(p->key, key) == 0) {  // if the first is the del node
    (dict->bucket)[index] = p->next;
    free(p);
    dict->ele_num -= 1;
  } else {
    for (; p->next && !strcmp(p->next->key, key); p = p->next)
      ;  // get that node's pre node
    p->next = p->next->next;
    free(p->next);
    dict->ele_num -= 1;
  }
  return 1;
}

void dict_keys(dict_t *dict, char **key_arr) {
  int size = prime_array[dict->size_pos], i = 0, j = 0;
  bucket_t *t = NULL;

  for (i = 0; i < size; i++)
    for (t = dict->bucket[i]; t; t = t->next) key_arr[j++] = t->key;
}

void dict_reset(dict_t *dict) {
  int size = prime_array[dict->size_pos], i = 0;
  bucket_t *t, *p;

  for (i = 0; i < size; i++) {  // free all nodes
    for (t = (dict->bucket)[i]; t; p = t, t = t->next, free(p))
      ;
    (dict->bucket)[i] = NULL;
  }

  // reset
  dict->size_pos = 0;
  dict->ele_num = 0;
  free(dict->bucket);
  dict->bucket =
      (bucket_t **)calloc(prime_array[dict->size_pos], sizeof(bucket_t *));
}

void dict_destory(dict_t *dict) {
  if(!dict) return;
  dict_reset(dict);
  free(dict->bucket);
  dict->bucket = NULL;
}

int dict_iter(dict_t *dict, char **key_p, void **value_p) {
  int size = prime_array[dict->size_pos];

  static int i = 0;
  static bucket_t *p = NULL;

  for (; !p; p = (dict->bucket)[i++])
    if (i == size) {
      i = 0;  // reset i
      return 0;
    }
  // assign
  *key_p = p->key;
  *value_p = p->value;
  // p goto the next
  p = p->next;
  return 1;
}

int dict_get_ulval(dict_t *dict, char *name, unsigned long *value) {
  char *var;

  if (!dict_get(dict, name, (void **)&var)) {
    return 0;
  }
  *value = strtoul(var, NULL, 16);
  return 1;
}

int dict_get_str(dict_t *dict, char *name, char *value, int len) {
  char *var;

  if (!dict_get(dict, name, (void **)&var)) {
    return 0;
  }
  strncpy(value, var, len);
  return 1;
}
