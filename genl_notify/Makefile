
include := -I "/usr/include/libnl3"
source := genl_client.c
target := genl_client
ccflags := -Wextra -Wall -Werror -Wno-unused-parameter 
other_opts := $(shell pkg-config --cflags --libs libnl-genl-3.0)

CC := gcc

all:
	$(CC) $(source) -o $(target) $(include) $(ccflags) $(other_opts)

clean:
	rm -f *.o target
