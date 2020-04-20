#ifndef CF_JSON_H
#define CF_JSON_H
struct cf_json;
struct cf_json* cf_json_load(const char* str);
struct cf_json* cf_json_create_object();
char* cf_json_print(const struct cf_json* item);    //返回内存不用释放
int cf_json_add_item_to_object(struct  cf_json *object, const char *string, struct  cf_json *item);
struct  cf_json* cf_json_add_int_to_object(struct cf_json *object, const char * const name,  int number);
struct  cf_json* cf_json_add_string_to_object(struct  cf_json *object, const char * const name, const char * const string);
struct cf_json* cf_json_get_item(struct cf_json *object, const char * const name);
char* cf_json_to_string(struct cf_json *object);
int cf_json_get_int(struct cf_json *object, const char * const name,int* err);
#endif//CF_JSON_H