#ifndef CF_ELEMENT_H
#define CF_ELEMENT_H
#include <stdbool.h>
typedef struct cf_element cf_element;

typedef enum cf_capability{
    CF_RGBA
} cf_capability;

void cf_element_start(cf_element* elem);
const char* cf_element_get_name(cf_element* elem);
void cf_element_set_priv(cf_element* elem,void* priv);
void cf_element_set_src_capability(cf_element* elem,cf_capability cap);
void cf_element_set_sink_capability(cf_element* elem,cf_capability cap);
void* cf_element_get_priv(cf_element* elem);
const char* cf_element_get_classname(cf_element* elem);
bool cf_element_link(cf_element* elem1,cf_element* elem2);
void cf_element_set_attr(cf_element* elem,const char* attr_name,void* val);
void cf_element_update(cf_element* elem);
void cf_element_push(cf_element* elem,void* data);

#endif//CF_ELEMENT_H
