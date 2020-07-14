#include "cf_allocator/cf_allocator_simple.h"
#include <string.h>
#include "cf_list.h"
struct _node
{
    void* data;
    struct _node* next;
    struct _node* pre;
};


struct cf_list{
    struct _node* m_head;
    struct _node* m_tail;
    size_t m_length;
    void (*m_free_data)(void*);
};

static struct _node* _create_node(void* data){
    struct _node* node = (struct _node*)cf_allocator_simple_alloc(sizeof(struct _node));
    if(node)
    {
        node->data = data;
        node->next = NULL;
        node->pre = NULL;
    }
    return node;
}


struct cf_list* cf_list_create(void (*free_data)(void*)){
    struct cf_list* list = (struct cf_list*)cf_allocator_simple_alloc(sizeof(struct cf_list));
    if(list)
    {
        list->m_head = _create_node(NULL);
        list->m_tail = list->m_head;
        list->m_length = 0;
        list->m_free_data  = free_data;
    }
    return list;
}
void cf_list_clear(cf_list* list){
    if(list == NULL)    return;
    void* data = cf_list_take_front(list);    
    while(data)
    {
        if(list->m_free_data)
            list->m_free_data(data);
        data = cf_list_take_front(list);
    }
}
void cf_list_destroy(struct cf_list* list){
    if(list == NULL)    return;
    cf_list_clear(list);
    cf_allocator_simple_free(list->m_head);
    cf_allocator_simple_free(list);
}

bool cf_list_is_empty(const struct cf_list* list){
    return list->m_length == 0 ? true:false;
}

void cf_list_push(struct cf_list* list,void* data){
    struct _node* node = _create_node(data);
    if(node)
    {
        node->pre = list->m_tail;
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
        if(node->next)
            node->next->pre = list->m_head;

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

static void _remove(struct cf_iterator* iter)
{
    struct cf_list* list = (struct cf_list*)iter->m_container;
    
    struct _node* node = (struct _node*)iter->m_priv;
    iter->m_priv = node->pre;
    node->pre->next = node->next;
    if(node->next)
        node->next->pre =node->pre;
    if(list->m_free_data)
        list->m_free_data(node->data);

    if(list->m_tail == node)
        list->m_tail = (struct _node*)iter->m_priv;
    list->m_length--;
    if(list->m_length == 0)
        list->m_tail = list->m_head;
}

static const struct cf_iterator_vt cf_list_iterator_vt = 
{
    .is_end = is_end,
    .next = next,
    .get = get,
    .remove = _remove
    
};
void cf_list_remove_item(cf_list* list,void* d){
    for(cf_iterator iter = cf_list_begin(list);!cf_iterator_is_end(&iter);cf_iterator_next(&iter)){
        if(cf_iterator_get(&iter) == d){
            cf_iterator_remove(&iter);
            break;
        }
    }
}
struct cf_iterator cf_list_begin(struct cf_list* list){
    struct cf_iterator iter;
    memset(&iter,0,sizeof(iter));
    iter.m_priv = list->m_head->next;
    iter.m_vt = &cf_list_iterator_vt;
    iter.m_container = list;
    return iter;
}
