gcc -std=c99  -g -D_GNU_SOURCE  -o cf_stp_test cf_stp.c -I../ ../cf_collection/cf_list.c ../cf_collection/cf_hash.c ../cf_collection/cf_vector.c \
  ../cf_collection/cf_iterator.c ../cf_allocator/cf_allocator_simple.c ../cf_json/cf_json.c ../cf_json/cJSON/cJSON.c ../cf_util/cf_util.c \
  ../cf_threadpool/cf_threadpool.c ../cf_async_queue/cf_async_queue.c cf_stp_test.c -lpthread
