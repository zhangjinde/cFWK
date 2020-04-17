#ifndef CF_LIST_H
#define CF_LIST_H

#include <stdbool.h>
#include "cf_iterator.h"
struct cf_list;
struct cf_list* cf_list_create();
void cf_list_delete(struct cf_list* list,void (*free_data)(void* data));
bool cf_list_is_empty(const struct cf_list*);
void cf_list_push(struct cf_list*,void*);
void* cf_list_take_front(struct cf_list*);
size_t cf_list_length(const struct cf_list*);

struct cf_iterator cf_list_begin(struct cf_list* list);


#endif//CF_LIST_H