#include "cf_allocator/cf_allocator_simple.h"
#include <string.h>
#include "cf_list.h"
struct _node
{
    void* data;
    struct _node* next;
};


struct cf_list{
    struct _node* m_head;
    struct _node* m_tail;
    
    size_t m_length;
};

static struct _node* _create_node(void* data){
    struct _node* node = (struct _node*)cf_allocator_simple_alloc(sizeof(struct _node));
    if(node)
    {
        node->data = data;
        node->next = NULL;
    }
    return node;
}


struct cf_list* cf_list_create(){
    struct cf_list* list = (struct cf_list*)cf_allocator_simple_alloc(sizeof(struct cf_list));
    if(list)
    {
        list->m_head = _create_node(NULL);
        list->m_tail = list->m_head;
        list->m_length = 0;
    }
    return list;
}

void cf_list_delete(struct cf_list* list,void (*free_data)(void* data)){
    if(list)
    {
        void* data = cf_list_take_front(list);
        
        while(data)
        {
            if(free_data)
                free_data(data);
            data = cf_list_take_front(list);
        }
        cf_allocator_simple_free(list->m_head);
        cf_allocator_simple_free(list);
    }
}

bool cf_list_is_empty(const struct cf_list* list){
    return list->m_length == 0 ? true:false;
}

void cf_list_push(struct cf_list* list,void* data){
    struct _node* node = _create_node(data);
    if(node)
    {
        list->m_tail->next = node;
        list->m_tail = node;
        list->m_length++;
    }
}

void* cf_list_take_front(struct cf_list* list){
    void* data = NULL;
    if(list->m_length > 0)
    {
        struct  _node *node = list->m_head->next;
        list->m_head->next = node->next;
        list->m_length--;
        data = node->data;
        cf_allocator_simple_free(node);
        if(list->m_length == 0)
            list->m_tail = list->m_head;
    }
    return data;
}

size_t cf_list_length(const struct cf_list* list){
    return list->m_length;
}

static bool is_end(struct cf_iterator* iter){
    struct _node* node = (struct _node*)iter->m_priv;
    return node == NULL;
}

static void next(struct cf_iterator* iter){
    struct _node* node = (struct _node*)iter->m_priv;
    iter->m_priv = node->next;
}

static void* get(struct cf_iterator* iter){
    return ((struct _node*)iter->m_priv)->data;
}

const struct cf_iterator_vt cf_list_iterator_vt = 
{
    .is_end = is_end,
    .next = next,
    .get = get
};

struct cf_iterator cf_list_begin(struct cf_list* list){
    struct cf_iterator iter;
    memset(&iter,0,sizeof(iter));
    iter.m_priv = list->m_head->next;
    iter.m_vt = &cf_list_iterator_vt;
    return iter;
}

#ifdef CF_LIST_TEST
/*************************
 * gcc -DCF_LIST_TEST -I../ cf_list.c cf_iterator.c ../cf_allocator/cf_allocator_simple.c -o cf_list_test
 * *********************/
static int f(){
    return 1;
}
static int x = f();
int main(){

printf("%d\n",x);
    struct cf_list* list = cf_list_create();
    cf_list_push(list,(void*)123);
    cf_list_push(list,(void*)456);
    cf_list_push(list,(void*)789);
    printf("for:\n");
    for(struct cf_iterator iter = cf_list_begin(list);!cf_iterator_is_end(&iter);cf_iterator_next(&iter))
    {
        long long x = (long long)cf_iterator_get(&iter);
        printf("x=%lld\n",x);
    }

    printf("take front:\n");
    while(cf_list_length(list)>0){
        long long x = (long long)cf_list_take_front(list);
        printf("x=%lld\n",x);
    }
    return 0;
}
#endif//CF_LIST_TEST