#ifndef CF_LOCK_H
typedef struct cf_lock cf_lock;
cf_lock* cf_lock_create(void);
void cf_lock_lock(cf_lock*);
void cf_lock_unlock(cf_lock*);
void cf_lock_destroy(cf_lock*);

#define CF_LOCK_H
#endif//CF_LOCK_H

