#include <stddef.h>
#include "cf_element.h"
#include "cf_element_class.h"
#include "cf_collection/cf_string.h"
#include "cf_allocator/cf_allocator_simple.h"
#include "cf_logger/cf_logger.h"
#include "cf_pad.h"
#include "cf_std.h"
typedef struct cf_element{
    cf_element_class* elem_class;
    cf_string* elem_name;
    cf_capability src_cap;
    cf_capability sink_cap;
    cf_pad* src_pad;
    //cf_pad* sink_pad;
    void* priv;
}cf_element;
void cf_element_start(cf_element* elem){
    cf_assert(elem != NULL);
    cf_element_start_method start = cf_element_class_get_start_method(elem->elem_class);
    if(start != NULL){
        start(elem);
    }
    if(elem->src_pad != NULL)
    {
        cf_element* sub_elem = cf_pad_get_linked_element(elem->src_pad);
        if(sub_elem != NULL){
            cf_element_start(sub_elem);
        }
    }
}
void cf_element_set_priv(cf_element* elem,void* priv){
    cf_assert(elem != NULL);
    elem->priv = priv;
}
void* cf_element_get_priv(cf_element* elem){
    return elem->priv;
}
const char* cf_element_get_name(cf_element* elem){
    if(elem == NULL ) return NULL;
    return cf_string_c_str(elem->elem_name);
}

const char* cf_element_get_classname(cf_element* elem){
    if(elem == NULL ) return NULL;
    return cf_element_class_get_name(elem->elem_class);
}
void cf_element_set_src_capability(cf_element* elem,cf_capability cap){
    cf_assert(elem != NULL);
    elem->src_cap = cap;
}
void cf_element_set_sink_capability(cf_element* elem,cf_capability cap){
    cf_assert(elem != NULL);
    elem->sink_cap = cap;
}
cf_element* cf_element_create(const char* elem_name,cf_element_class* e_class){
    cf_element* e = cf_allocator_simple_alloc(sizeof(cf_element));
    e->elem_name = cf_string_create_from_cstr(elem_name);
    e->elem_class = e_class;
    cf_element_constructor constructor = cf_element_class_get_constructor(e_class);
    if(constructor != NULL && constructor(e) != CF_OK){
        cf_string_destroy(e->elem_name);
        cf_allocator_simple_free(e);
        e = NULL;
    }
    return e;
}
bool cf_element_link(cf_element* elem1,cf_element* elem2){
    cf_assert(elem1 != NULL && elem2 != NULL);
    if(elem1->src_cap != elem2->sink_cap ) return false;
    if(elem1->src_pad == NULL)
        elem1->src_pad = cf_pad_create();
    cf_pad_link_element(elem1->src_pad,elem2);
    return true;
}
void cf_element_set_attr(cf_element* elem,const char* attr_name,void* val){
    cf_assert(elem != NULL );
    cf_element_attr_write_method write_func = cf_element_class_get_attr_write_method(elem->elem_class,attr_name);
    if(write_func)
        write_func(elem,val);
}

void cf_element_push(cf_element* elem,void* data){
    cf_element_processer processer = cf_element_class_get_processer(elem->elem_class);
    if(processer){
        void* next_data = processer(elem,data);//返回动态数据怎么处理合适
        if(elem->src_pad){
            cf_element* next_elem = cf_pad_get_linked_element(elem->src_pad);
            if(next_elem)
            {
                cf_element_push(next_elem,next_data);
            }
        }
        cf_data_release_method release = cf_element_class_get_data_release_method(elem->elem_class);
        if(release)
            release(next_data);
    }
        
}