#include <stdbool.h>
typedef struct cf_test cf_test;
cf_test* cf_test_create(const char* name,void(*test_func)(cf_test*));
void cf_test_destroy(cf_test* tc);
const char* cf_test_msg(cf_test* tc);
const char* cf_test_name(cf_test* tc);
void cf_test_run(cf_test* tc);

typedef struct cf_suite cf_suite;
cf_suite* cf_suite_create(void);
void cf_suite_destroy(cf_suite* suite);
void cf_suite_add_test(cf_suite* suite,cf_test* tc);
void cf_suite_add_suite(cf_suite* suite,cf_suite* src);
void cf_suite_run(cf_suite* suite);
const char*  cf_suite_msg(cf_suite* suite);

void cf_assert(cf_test* tc,const char* file,int line,const char* msg,bool cond);
void cf_assert_str_equals(cf_test* tc, const char* file, int line, const char* message, 
	const char* expected, const char* actual);

#define CF_ASSERT(tc,msg,cond)     cf_assert(tc,__FILE__,__LINE__,(msg),(cond));
#define CF_ASSERT_STR_EQUALS(tc,msg,ex,ac )     cf_assert_str_equals((tc),__FILE__,__LINE__,(msg),(ex),(ac))


