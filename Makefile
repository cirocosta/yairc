# 		--		user config

CC ?= clang
LIBS = 
DEFS = -D_GNU_SOURCE 
INCLUDES = -I/usr/include -I./include

PROGRAM_CLIENT = yairc-client
PROGRAM_SERVER = yairc-server

LIB = src/libyairc.a
SOURCE_CLIENT = src/yairc-client.c
SOURCE_SERVER = src/yairc-server.c
TESTS_DIR = tests/

BUILD := debug


#			--		generation

cflags.debug := -Wall -g -DDEBUG
cflags.release := -03 -DNDEBUG
CFLAGS := -std=gnu99 ${cflags.${BUILD}}

SRCS = $(shell find src/ -name '*.c')
LIB_OBJS := $(patsubst %.c, %.o, $(filter-out $(SOURCE), $(SRCS)))
TESTS:= $(patsubst %.c, %.out, $(shell find $(TESTS_DIR) -name '*.c'))

all: $(PROGRAM_CLIENT) $(PROGRAM_SERVER) test depend

$(PROGRAM_CLIENT): $(LIB) $(SOURCE_CLIENT)
	$(CC) $(CFLAGS) $(SOURCE_CLIENT) $(DEFS) $(INCLUDES) $(LIBS) -o $@ $<

$(PROGRAM_SERVER): $(LIB) $(SOURCE_SERVER)
	$(CC) $(CFLAGS) $(SOURCE_SERVER) $(DEFS) $(INCLUDES) $(LIBS) -o $@ $<

$(LIB): $(LIB_OBJS)
	$(AR) rvs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -c -o $@ $<


.PHONY: clean test depend

depend: .depend

.depend: $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) -MM $^ -MF ./.depend

include .depend

test: $(LIB) $(TESTS)
	@$(foreach test_exec,$(TESTS),./$(test_exec);)

%.out: %.c
	$(CC) $(CFLAGS) $< $(DEFS) $(INCLUDES) $(LIBS) -o $@ $(LIB) 

print-%:
	@echo '$*=$($*)'

clean:
	find . \( -name "*.o" -o -name "*.a" -o -name "*.out" \) -type f -delete &
	find . \( -name $(PROGRAM_CLIENT) -o -name $(PROGRAM_SERVER) \) -type f -delete &
	find . -name "callgrind.*" -type f -delete
	

