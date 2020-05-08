CC = g++
CFWK_DIR = ..
target =  test

C_FLAGS=
src=  $(CFWK_DIR)/cf_allocator/cf_allocator_simple.c \
	  $(CFWK_DIR)/cf_iostream/cf_iostream.c \
      $(CFWK_DIR)/cf_iostream/cf_iostream_file.c \
      $(CFWK_DIR)/cf_iostream/cf_iostream_tcp.c \
      $(CFWK_DIR)/cf_collection/cf_string.c \
      $(CFWK_DIR)/cf_collection/cf_iterator.c \
      $(CFWK_DIR)/cf_collection/cf_vector.c \
      $(CFWK_DIR)/cf_collection/cf_hash.c \
      $(CFWK_DIR)/cf_collection/cf_list.c \
      $(CFWK_DIR)/cf_util/cf_util.c \
      $(CFWK_DIR)/cf_async_queue/cf_async_queue.c \
      $(CFWK_DIR)/cf_threadpool/cf_threadpool.c \
      $(CFWK_DIR)/cf_logger/cf_logger.c \
      $(CFWK_DIR)/cf_json/cf_json.c \
      $(CFWK_DIR)/cf_json/cJSON/cJSON.c \
      $(CFWK_DIR)/cf_stp/cf_stp.c \
      main.c test_log.c test_stp.c
obj := $(src:c=o)

all:	$(obj)
	$(CC)	-g $(C_FLAGS) -I$(CFWK_DIR) $^ -lpthread -o $(target)
$(obj): %.o : %.c
	$(CC) -g $(C_FLAGS) -I$(CFWK_DIR) -o $@ -c $^
clean:
	rm $(obj) $(target)
