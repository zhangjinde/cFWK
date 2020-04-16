#ifndef CF_ITERATOR_H
#define CF_ITERATOR_H

#include <stdbool.h>
struct cf_iterator;
struct cf_iterator_vt
{
    bool(*has_next)(struct cf_iterator*);
    void (*next)(struct cf_iterator*);
    void* (*get)(struct cf_iterator*);
};

struct cf_iterator {
    const struct cf_iterator_vt* m_vt;
    void* m_priv;
};

bool cf_iterator_has_next(struct cf_iterator*);
bool cf_iterator_is_end(struct cf_iterator* iter);
void cf_iterator_next(struct cf_iterator*);
void* cf_iterator_get(struct cf_iterator*);

#endif//CF_ITERATOR_H