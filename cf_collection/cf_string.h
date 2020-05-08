#ifndef CF_STRING_H
#define CF_STRING_H

#ifdef __cplusplus
extern "C"{
#endif

typedef struct cf_string cf_string;
cf_string* cf_string_create(void);
cf_string* cf_string_create_frome_c_str(const char* c_str);
const char* cf_string_c_str(cf_string* str);
void cf_string_destroy(cf_string* str);

#ifdef __cplusplus
}
#endif

#endif //CF_STRING_H