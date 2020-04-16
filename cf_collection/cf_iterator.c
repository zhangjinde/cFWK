#include <stddef.h>
#include "cf_iterator.h"



bool cf_iterator_has_next(struct cf_iterator* iter){
    if(iter && iter->m_vt && iter->m_vt->has_next)
        return iter->m_vt->has_next(iter);
    else
    {
        return false;
    }
}
bool cf_iterator_is_end(struct cf_iterator* iter){
    if(iter && iter->m_vt && iter->m_vt->has_next)
        return iter->m_vt->has_next(iter) == false;
    else
    {
        return true;
    }
}
void cf_iterator_next(struct cf_iterator* iter){
    if(iter && iter->m_vt && iter->m_vt->next)
        iter->m_vt->next(iter);

}
void* cf_iterator_get(struct cf_iterator* iter){
    if(iter && iter->m_vt && iter->m_vt->get)
        return iter->m_vt->get(iter);
    else
        return NULL;
}