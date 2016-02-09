#DEBUG=1

.SUFFIXES: .cpp .o .c .h
OBJECTS = vbyte-comp.o vbyte-plain.o vbyte-scalar1.o vbyte-scalar2.o \
		  vbyte-scalar3.o vbyte-scalar4.o vbyte-scalar5.o varintdecode.o

ifeq ($(DEBUG),1)
    CFLAGS = -g -pedantic -DDEBUG=1 -D_GLIBCXX_DEBUG -Wall -Wextra 
else
    CFLAGS = -g -pedantic -Wall -Wextra -O3
endif #debug


HEADERS= $(shell ls *h)

all: test libvbyte.a
	echo "please run unit tests by running ./test"

vbyte-comp.o: vbyte.h vbyte-comp.cc
	$(CXX) $(CFLAGS) -c vbyte-comp.cc

vbyte-plain.o: vbyte.h vbyte-plain.cc
	$(CXX) $(CFLAGS) -c vbyte-plain.cc

vbyte-scalar1.o: vbyte.h vbyte-scalar1.cc
	$(CXX) $(CFLAGS) -c vbyte-scalar1.cc

vbyte-scalar2.o: vbyte.h vbyte-scalar2.cc
	$(CXX) $(CFLAGS) -c vbyte-scalar2.cc

vbyte-scalar3.o: vbyte.h vbyte-scalar3.cc
	$(CXX) $(CFLAGS) -msse3 -c vbyte-scalar3.cc

vbyte-scalar4.o: vbyte.h vbyte-scalar4.cc
	$(CXX) $(CFLAGS) -msse3 -mssse3 -c vbyte-scalar4.cc

vbyte-scalar5.o: vbyte.h vbyte-scalar5.cc
	$(CXX) $(CFLAGS) -msse4 -c vbyte-scalar5.cc

varintdecode.o: vbyte.h varintdecode.c
	$(CXX) $(CFLAGS) -mavx -c varintdecode.c

vbyte: $(HEADERS) $(OBJECTS)
	ar rvs libvbyte.a $(OBJECTS)

gen: gen.pl
	perl gen.pl > vbyte-gen.c

test: vbyte $(HEADERS) test.cc
	$(CXX) $(CFLAGS) -o test test.cc libvbyte.a \
			-lboost_chrono -lboost_system

clean: 
	rm -f *.o test 

.PHONY: all clean test
