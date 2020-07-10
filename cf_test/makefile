CC = gcc
CFWK_DIR = ..
target =  cf_test_sample

C_FLAGS=
src=  $(CFWK_DIR)/cf_test/cf_test.c \
	  $(CFWK_DIR)/cf_test/cf_test_sample.c

obj := $(src:c=o)

all:	$(obj)
	$(CC)	-g $(C_FLAGS) -I$(CFWK_DIR) $^ -o $(target)
$(obj): %.o : %.c
	$(CC) -g $(C_FLAGS) -I$(CFWK_DIR) -o $@ -c $^
clean:
	rm $(obj) $(target)
