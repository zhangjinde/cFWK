typedef struct cf_element cf_element;

void cf_element_start(cf_element* elem);
const char* cf_element_get_name(cf_element* elem);
void cf_element_set_priv(cf_element* elem,void* priv);
void* cf_element_get_priv(cf_element* elem);
const char* cf_element_get_classname(cf_element* elem);

