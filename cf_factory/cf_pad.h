#include "cf_element.h"
typedef struct cf_pad cf_pad;
cf_pad* cf_pad_create(void);
bool cf_pad_link_element(cf_pad* pad,cf_element* elem);
cf_element* cf_pad_get_linked_element(cf_pad* pad);