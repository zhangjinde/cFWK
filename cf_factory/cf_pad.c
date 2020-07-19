#include "cf_std.h"
#include "cf_pad.h"
#include "cf_allocator/cf_allocator_simple.h"
typedef struct cf_pad{
    cf_element* elem;
}cf_pad;
cf_pad* cf_pad_create(void){
    return cf_allocator_simple_alloc(sizeof(cf_pad));
}
bool cf_pad_link_element(cf_pad* pad,cf_element* elem){
    cf_assert(pad != NULL);
    pad->elem = elem;
    return true;
}
cf_element* cf_pad_get_linked_element(cf_pad* pad){
    cf_assert(pad != NULL);
    return pad->elem;
}

