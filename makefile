DEBUGGER=valgrind
CFLAGS=-g -std=iso9899:1999 -Wextra -Wall -pedantic -Werror -Wshadow \
	-Wpointer-arith -Wcast-qual -Wstrict-prototypes \
	-Wmissing-prototypes -Wconversion \
	-Isrc $(OPTFLAGS) 
LIBS=-lm $(OPTLIBS)

SOURCES:=$(wildcard src/*.c)
OBJECTS:=$(patsubst %.c,%.o,$(SOURCES))
DEPENDENCIES:=$(patsubst %.c,%.d,$(SOURCES))

TESTSOURCE:=$(wildcard tests/*_tests.c)
TESTOBJECT:=$(patsubst %.c,%.o,$(TESTSOURCE))
TESTS:=$(patsubst %.c,%,$(TESTSOURCE))
DEPENDENCIES+=$(patsubst %.c,%.d,$(TESTSOURCE))

TARGET=bin/ebs
TARGETMAIN=src/main.o
TESTSCRIPT=tests/runtests.sh

all: $(DEPENDENCIES) $(TARGET) tests

dev: CFLAGS=-g -Isrc $(OPTFLAGS)
dev: all

release: CFLAGS=-g -O2 -Isrc -DNDEBUG $(OPTFLAGS)
release: all

$(TARGET): build $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LIBS) 

build:
	@mkdir -p bin

src/%.d: src/%.c
	$(CC) $(CFLAGS) -MM $< -MF $@
	sed -e 's:$*.o:src/$*.o:g' $@ > tmp
	mv -f tmp $@


$(TESTS): % : %.o $(subst $(TARGETMAIN),,$(OBJECTS))
	$(CC) -o $@ $< $(subst $(TARGETMAIN),,$(OBJECTS)) $(LIBS) 

tests: $(TESTS)
	sh ./$(TESTSCRIPT)

tests/%.d: tests/%.c
	$(CC) $(CFLAGS) -MM $< -MF $@
	sed -e 's:$*.o:tests/$*.o:g' $@ > tmp
	mv -f tmp $@

valgrind:
	VALGRIND="valgrind --log-file=/tmp/valgrind-%p.log" $(MAKE)

tags:
	ctags -R src

clean:
	-rm $(OBJECTS)
	-rm $(TESTOBJECT)
	-rm $(DEPENDENCIES)

install: $(TARGET)
	@cp $(TARGET) ~/bin
	@mkdir ~/.ebs
	@touch ~/.ebs/task.tsv
	@touch ~/.ebs/time.tsv

.PHONY: dev release build tests trace clean tags install

ifeq (,$(filter $(MAKECMDGOALS),clean))
-include $(DEPENDENCIES)
endif
