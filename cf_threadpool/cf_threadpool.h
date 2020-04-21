#ifndef CF_THREADPOOL_H
#define CF_THREADPOOL_H
int cf_threadpool_run(void (*run)(void*),void* d);
#endif//CF_THREADPOOL_H