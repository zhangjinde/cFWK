#ifndef CF_THREADPOOL_H
#define CF_THREADPOOL_H

#ifdef __cplusplus
extern "C"{
#endif

int cf_threadpool_run(void (*run)(void*),void* d);

#ifdef __cplusplus
}
#endif
#endif//CF_THREADPOOL_H