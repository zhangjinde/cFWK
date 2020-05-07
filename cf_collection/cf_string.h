#ifndef CF_STRING_H
#define CF_STRING_H
typedef cf_string cf_string;
cf_string* cf_string_create(void);
cf_string* cf_string_create_frome_c_str(const char*);
void cf_string_destroy(cf_string* );
#endif //CF_STRING_H