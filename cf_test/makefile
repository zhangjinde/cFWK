CC = g++
CFWK_DIR = ..
target =  test

C_FLAGS=
src=  $(CFWK_DIR)/cf_allocator/cf_allocator_simple.c \
	  $(CFWK_DIR)/cf_iostream/cf_iostream.c \
      $(CFWK_DIR)/cf_iostream/cf_iostream_file.c \
	  $(CFWK_DIR)/cf_collection/cf_string.c \
      $(CFWK_DIR)/cf_collection/cf_iterator.c \
      $(CFWK_DIR)/cf_collection/cf_list.c \
      $(CFWK_DIR)/cf_logger/cf_logger.c \
      main.c
obj := $(src:c=o)

all:	$(obj)
	$(CC)	-g $(C_FLAGS) -I$(CFWK_DIR) $^ -o $(target)
$(obj): %.o : %.c
	$(CC) -g $(C_FLAGS) -I$(CFWK_DIR) -o $@ -c $^
clean:
	rm $(obj) $(target)
