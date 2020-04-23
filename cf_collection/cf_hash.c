#include<stdint.h>
#include<string.h>
#include "cf_collection/cf_hash.h"
#include "cf_allocator/cf_allocator_simple.h"
#include "cf_collection/cf_list.h"

#define CF_DEFAULT_HASH_WIDTH 20

struct _hash_item
{
    void* key;
    void* value;
};

struct cf_hash
{
    size_t (*m_hash_func)(void*);
    bool (*m_key_equal_func)(void*,void*);
    void (*m_key_free_func)(void*);
    void (*m_value_free_func)(void*);
    struct _hash_item m_items[CF_DEFAULT_HASH_WIDTH];

    //每一个item对应一bit，0表示空闲，1表示占用，key相等未必不冲突。
    uint8_t m_used_flag[CF_DEFAULT_HASH_WIDTH/8 + (CF_DEFAULT_HASH_WIDTH%8 > 0)]; 
    //每一个item对应一bit，0表示未冲突，1表示冲突，如果冲突，则对应value实际为冲突list指针
    uint8_t m_conflict_flag[CF_DEFAULT_HASH_WIDTH/8 + (CF_DEFAULT_HASH_WIDTH%8 > 0)];   
};

size_t cf_hash_str_hash(void* key){
    uint32_t sum = 0;
    uint8_t* s = (uint8_t*)key;
    while(*s) sum+=*s++;
    return sum%CF_DEFAULT_HASH_WIDTH;
}

bool cf_hash_str_equal(void* key1,void* key2){
    return strcmp((const char*)key1,(const char*)key2) == 0 ? true : false;
}

struct cf_hash* cf_hash_create(size_t (*hash_func)(void*),bool (*key_equal_func)(void*,void*),void (*key_free_func)(void*),void (*value_free_func)(void*)){
    struct cf_hash* hash = (struct cf_hash*)cf_allocator_simple_alloc(sizeof(struct cf_hash));
    memset(hash,0,sizeof(struct cf_hash));
    hash->m_hash_func = hash_func;
    hash->m_key_equal_func = key_equal_func;
    hash->m_key_free_func = key_free_func;
    hash->m_value_free_func = value_free_func;
    return hash;
}

void cf_hash_delete(struct cf_hash* hash){
    if(hash){
        for(size_t inx = 0;inx < CF_DEFAULT_HASH_WIDTH; inx++)
        {
            if(hash->m_used_flag[inx/8] & (1 << (inx%8)))   //key对应hash索引被占用
            {
                if(hash->m_conflict_flag[inx/8] & (1 << (inx%8))) //key对应hash索引冲突
                {
                    while(cf_list_length((struct cf_list*)hash->m_items[inx].value) > 0){
                        struct _hash_item* item = (struct _hash_item*)cf_list_take_front((struct cf_list*)hash->m_items[inx].value);
                        if(hash->m_key_free_func)
                            hash->m_key_free_func(item->key);
                        if(hash->m_value_free_func)    
                            hash->m_value_free_func(item->value);
                        cf_allocator_simple_free(item);
                    }
                    cf_list_delete((struct cf_list*)hash->m_items[inx].value);
                }
                else
                {
                    if(hash->m_key_free_func)
                        hash->m_key_free_func(hash->m_items[inx].key);
                    if(hash->m_value_free_func)
                        hash->m_value_free_func(hash->m_items[inx].value);
                }
            }
        }
        cf_allocator_simple_free(hash);
    }
}

void cf_hash_insert(struct cf_hash* hash,void* key,void* value){
    size_t inx = hash->m_hash_func(key);
    if(hash->m_used_flag[inx/8] & (1 << (inx%8)) )  //key对应hash索引被占用
    {
        if(hash->m_conflict_flag[inx/8] & (1 << (inx%8))) //key对应hash索引冲突
        {
            struct cf_iterator iter = cf_list_begin((struct cf_list*)hash->m_items[inx].value);
            for(;!cf_iterator_is_end(&iter);cf_iterator_next(&iter))
            {
                struct _hash_item* item = (struct _hash_item*)cf_iterator_get(&iter);
                if(hash->m_key_equal_func(item->key,key))
                {
                    item->value = value;    //在冲突list找到对应key，替换value
                    break;
                }
            }
            if(cf_iterator_is_end(&iter)){ //冲突list未找到对应key,则追加新_hash_item
                struct _hash_item* item = (struct _hash_item*)cf_allocator_simple_alloc(sizeof(struct _hash_item));
                item->key = key;
                item->value = value;
                cf_list_push((struct cf_list*)hash->m_items[inx].value,item);
            }

        }
        else if(hash->m_key_equal_func(hash->m_items[inx].key,key)) //未冲突，key相等则直接替换
        {
            hash->m_items[inx].value = value;
        }
        else {  //未冲突，key不相等则作冲突化处理，创建冲突list，同时把旧的key，value及新的key，value同时加入冲突list
            hash->m_conflict_flag[inx/8] |= (1 << (inx%8));
            struct cf_list* list = cf_list_create(NULL); //list内的struct _hash_item需要在链表外部精确删除，因为需要调用相应的key/value free函数
            struct _hash_item* item = (struct _hash_item*)cf_allocator_simple_alloc(sizeof(struct _hash_item));
            item->key = hash->m_items[inx].key;
            item->value = hash->m_items[inx].value;
            cf_list_push(list,item);

            item = (struct _hash_item*)cf_allocator_simple_alloc(sizeof(struct _hash_item));
            item->key = key;
            item->value = value;
            cf_list_push(list,item);
            hash->m_items[inx].value = list;
        }
    }
    else{   //key对应hash索引未被占用,则设置占用标记，并填充
        hash->m_used_flag[inx/8] |= (1 << (inx%8));
        hash->m_items[inx].key = key;
        hash->m_items[inx].value = value;
    }
}

void* cf_hash_get(struct cf_hash* hash, void* key,int* error)
{
    size_t inx = hash->m_hash_func(key);
    if(error)
        *error = 0;
    if(hash->m_used_flag[inx/8] & (1 << (inx%8)) )  //key对应hash索引被占用
    {
        if(hash->m_conflict_flag[inx/8] & (1 << (inx%8))) //key对应hash索引冲突
        {
            struct cf_iterator iter = cf_list_begin((struct cf_list*)hash->m_items[inx].value);
            for(;!cf_iterator_is_end(&iter);cf_iterator_next(&iter))
            {
                struct _hash_item* item = (struct _hash_item*)cf_iterator_get(&iter);
                if(hash->m_key_equal_func(item->key,key))
                {
                    return item->value;
                }
            }
            if(cf_iterator_is_end(&iter)){ //冲突list未找到对应key,则追加新_hash_item
                if(error)
                    *error = -1;    //未找到对应key
                return NULL;
            }
        }
        else if(hash->m_key_equal_func(hash->m_items[inx].key,key)) //未冲突，key相等
        {
            return hash->m_items[inx].value;
        }
        else    //未冲突，key不相等,相当于未找到key
        {
            if(error)
                *error = -1;    //未找到对应key
            return NULL;
        }
    }
    else // key对应hash索引未被占用
    {
        if(error)
            *error = -1;    //未找到对应key
        return NULL;
    }
    return NULL;
}

#ifdef CF_HASH_TEST
/****************************************
 * gcc -g -DCF_HASH_TEST -I../ cf_hash.c cf_list.c cf_iterator.c ../cf_allocator/cf_allocator_simple.c -o cf_hash_test
 * *************************************/
#include<stdio.h>
int main()
{
    struct cf_hash* hash = cf_hash_create(cf_hash_str_hash,cf_hash_str_equal,NULL,NULL);
     cf_hash_insert(hash,"abc",(void*)1234);
     cf_hash_insert(hash,"def",(void*)5555);
     cf_hash_insert(hash,"cba",(void*)445566);
     cf_hash_insert(hash,"efd",(void*)987);
     printf("abc=%ld\n",(int64_t)cf_hash_get(hash,"abc",NULL));
     printf("def=%ld\n",(int64_t)cf_hash_get(hash,"def",NULL));
     printf("def=%ld\n",(int64_t)cf_hash_get(hash,"cba",NULL));
     printf("def=%ld\n",(int64_t)cf_hash_get(hash,"efd",NULL));
    printf("defor delete hash alloc_size=%ld\n",cf_allocator_alloc_size());
    cf_hash_delete(hash);
    printf("after delete hash alloc_size=%ld\n",cf_allocator_alloc_size());
    return 0;
}
#endif