PERL ?= /usr/bin/perl
CFLAGS ?= -std=c99 -Wall -Wextra -Werror -pedantic

LIBNAME := lib3gpp23038
OBJECTS := lib.o
SHARED_OBJECTS := $(patsubst %.o,%_so.o,$(OBJECTS))
LIBS := $(LIBNAME).a $(LIBNAME).so

all : $(LIBS)

$(LIBNAME).a : $(OBJECTS)
	$(AR) rcs $@ $^

$(LIBNAME).so : $(SHARED_OBJECTS)
	$(CC) $(CFLAGS) -shared -o $@ $^

%_so.o : %.c
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

LIB_DEPS := lib3gpp23038.h tables.c

lib.o : $(LIB_DEPS)
lib_so.o : $(LIB_DEPS)

tables.c : gentables.pl
	$(PERL) gentables.pl > tables.c

test : test.o $(LIBNAME).a
	$(CC) $(CFLAGS) $(shell pkg-config --cflags check) -o $@ $^ $(shell pkg-config --libs check)

clean :
	$(RM) $(LIBS) $(OBJECTS) $(SHARED_OBJECTS) tables.c test test.o

.PHONY : clean all
