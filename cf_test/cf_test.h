#include <stdbool.h>
typedef struct cf_test cf_test;
extern cf_test* cf_test_create(const char* name,void(*test_func)(cf_test*));
void cf_test_destroy(cf_test* tc);
extern const char* cf_test_msg(cf_test* tc);
extern const char* cf_test_name(cf_test* tc);
extern void cf_test_run(cf_test* tc);

typedef struct cf_suite cf_suite;
cf_suite* cf_suite_create(void);
void cf_suite_add_test(cf_suite* suite,cf_test* tc);

extern void cf_assert(cf_test* tc,const char* file,int line,const char* msg,bool cond);
#define CF_ASSERT(tc,msg,cond)     cf_assert(tc,__FILE__,__LINE__,msg,(cond));
