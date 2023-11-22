EXECBIN = banhammer

SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:%.c=%.o)

CC = clang
CFLAGS = -Wall -Werror -Wextra -Wpedantic

.PHONY: all clean spotless format

all: $(EXECBIN)

$(EXECBIN): $(OBJECTS)
	$(CC) -o $@ $^ 
	 
%.o : %.c
	$(CC) $(CFLAGS) -c $<
	
clean:
	rm -f $(OBJECTS)
	
spotless: clean
	rm -f $(EXECBIN)

format:
	clang-format -i -style=file {banhammer,bf,bv,ht,ll,node,parser}.c
