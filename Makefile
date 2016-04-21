#DEBUG=1
VBYTE_CFLAGS=-mavx 	# undefine this to compile on non-intel platforms.

# -------------------------------------------------------

.SUFFIXES: .cpp .o .c .h
OBJECTS = vbyte.o varintdecode.o

ifeq ($(DEBUG),1)
    CFLAGS = -g -pedantic -DDEBUG=1 -D_GLIBCXX_DEBUG -Wall -Wextra 
else
    CFLAGS = -g -pedantic -Wall -Wextra -O3
endif #debug


HEADERS= $(shell ls *h)

all: test libvbyte.a
	echo "please run unit tests by running ./test"

vbyte.o: vbyte.h vbyte.cc
	$(CXX) $(CFLAGS) $(VBYTE_CFLAGS) -c vbyte.cc

varintdecode.o: vbyte.h varintdecode.c
	$(CXX) $(CFLAGS) -mavx -c varintdecode.c

vbyte: $(HEADERS) $(OBJECTS)
	ar rvs libvbyte.a $(OBJECTS)

test: vbyte $(HEADERS) test.cc
	$(CXX) $(CFLAGS) -std=c++11 -o test test.cc libvbyte.a \
			-lboost_chrono -lboost_system

clean: 
	rm -f *.o test 

.PHONY: all clean test
