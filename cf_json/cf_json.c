#include <stdbool.h>
#include "cf_json.h"
#include "cJSON/cJSON.h"

typedef struct cf_json{
}cf_json;

struct cf_json* cf_json_load(const char* str)
{
    return (struct cf_json*)cJSON_Parse(str);
}
char* cf_json_print(const struct cf_json* item){
    if(item == NULL )
        return NULL;
    return cJSON_Print((cJSON*)item);
}
int cf_json_print_preallocated(const struct cf_json* item,char *buffer, const int length, const bool format){
    if(item == NULL)
        return -1;
    return cJSON_PrintPreallocated((cJSON*)item,buffer,length,format) == 0 ? -1 : 0;
}
struct cf_json* cf_json_create_object(){
    return (struct cf_json*)cJSON_CreateObject();
}
struct cf_json* cf_json_clone(struct cf_json* json)
{
    return (struct cf_json *) cJSON_Duplicate( (cJSON *)json, true);
}

void cf_json_destroy_object(struct cf_json* item){
    cJSON_Delete((cJSON*)item);
}

struct cf_json* cf_json_create_string(const char *string){
    return  (struct cf_json*)cJSON_CreateString(string);
}

struct  cf_json* cf_json_create_array(){
    return (struct  cf_json*)cJSON_CreateArray();
}

struct  cf_json* cf_json_create_string_array(const char *const *strings, int count){
    return (struct  cf_json*)cJSON_CreateStringArray(strings,count);
}

struct cf_json* cf_json_create_int_array(const int *numbers, int count){
    return  (struct cf_json*)cJSON_CreateIntArray(numbers,count);
}

int cf_json_add_item_to_array(struct  cf_json *array, struct  cf_json *item){
    return cJSON_AddItemToArray((cJSON*)array,(cJSON*)item) == true ? 0 : -1;
}
int cf_json_get_array_size(struct  cf_json *array){
    return cJSON_GetArraySize((cJSON*)array);
}
cf_json* cf_json_get_item_from_array(struct  cf_json *array, int which){
    return (cf_json*)cJSON_GetArrayItem((cJSON*)array,which);
}
int cf_json_add_item_to_object(struct  cf_json *object, const char *string, struct  cf_json *item){
    if(object == NULL || item == NULL)
        return -1;
    return cJSON_AddItemToObject((cJSON*)object,string,(cJSON*)item) == 1 ? 0 : -1;
}

struct  cf_json* cf_json_add_string_to_object(struct  cf_json *object, const char * const name, const char * const string){
    if(object == NULL )
        return NULL;
    return (struct  cf_json*)cJSON_AddStringToObject((cJSON*)object,name,string);
}

struct  cf_json* cf_json_add_number_to_object(struct cf_json *object, const char * const name, const double number){
    if(object == NULL )
        return NULL;
    return (struct  cf_json*)cJSON_AddNumberToObject((cJSON*)object,name,number);
}

struct  cf_json* cf_json_add_int_to_object(struct cf_json *object, const char * const name,  int number){
    return cf_json_add_number_to_object(object,name,number);
}

struct cf_json* cf_json_get_item(struct cf_json *object, const char * const name){
    return (struct cf_json*)cJSON_GetObjectItem((cJSON*)object, name);
}
int cf_json_to_int(struct cf_json *object){
    double num = cJSON_GetNumberValue((cJSON*)object);
    return (int)num;
}
char* cf_json_to_string(struct cf_json *object){
    char* str = cJSON_GetStringValue((cJSON*)object);
    return str;
}
struct cf_json* cf_json_detach_item(struct cf_json* parent,const char* name){
    return (struct cf_json*)cJSON_DetachItemFromObject((cJSON *)parent, name);
}

char* cf_json_get_string(struct cf_json *object, const char * const name,int* err){
    char* str = NULL;
    int _err = 0;
    struct cf_json * item = cf_json_get_item(object, name);
    if(item)
    {
        str = cf_json_to_string(item);
    }
    else
    {
        _err = -1;
    }
    if(err)
        *err = _err;
    return str;
}
int cf_json_get_int(struct cf_json *object, const char * const name,int* err){
    int val = 0;
    int _err = 0;
    struct cf_json * item = cf_json_get_item(object, name);
    if(item)
        val = cf_json_to_int(item);
    else
    {
        _err = -1;
    }   
    if(err)
        *err = _err;
    return val;
}


struct  cf_json* cf_json_add_false_to_object(struct cf_json *object, const char * const name){
    if(object == NULL )
        return NULL;
    return (struct  cf_json*)cJSON_AddFalseToObject((cJSON*)object,name);
}

bool cf_json_contains(struct cf_json *object,const char* key){
    return cJSON_HasObjectItem((struct cJSON*)object, key);
}
