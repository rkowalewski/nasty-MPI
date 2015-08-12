CC=mpicc
LIB_NAME=nasty_mpi
MAJOR=0
MINOR=1

CFLAGS=-O3 -std=gnu11  $(OPTFLAGS)
LIBS=$(OPTLIBS)
PREFIX?=/usr/local

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TEST_SRC=$(wildcard tests/**/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

SAMPLES_SRC=$(wildcard samples/**/*.c)
SAMPLES=$(patsubst samples/%.c,bin/%,$(SAMPLES_SRC))

LIB_VERSION=$(MAJOR).$(MINOR)
TARGET=build/lib$(LIB_NAME).so.$(LIB_VERSION)

UNAME := $(shell uname -s)

# The Target Build
all: lib samples

lib: CFLAGS += -DNDEBUG -Isrc
lib: $(TARGET)

dev: CFLAGS += -g -Isrc -Wall -Wextra -Werror -pedantic
dev: $(TARGET)
	cd lib; \
	ln -fs ../$(TARGET) lib$(LIB_NAME).so.$(MAJOR); \
	ln -fs lib$(LIB_NAME).so.$(MAJOR) lib$(LIB_NAME).so

$(TARGET): CFLAGS += -fPIC
$(TARGET): build $(OBJECTS)
ifeq ($(UNAME), Darwin)
	$(CC) -shared -Wl,-install_name,lib$(LIB_NAME).so.$(MAJOR) $(OBJECTS) -o $@ -lc
else
	$(CC) -shared -Wl,-soname,lib$(LIB_NAME).so.$(MAJOR) $(OBJECTS) -o $@ -lc
endif

build:
	@mkdir -p build
	@mkdir -p bin
	@mkdir -p lib

# The Unit Tests
.PHONY: tests
tests: LDLIBS = -l$(LIB_NAME)
tests: LDFLAGS = -Wl,-rpath,./lib/ -L./lib/
tests: CFLAGS += -g -Isrc -Wall -Wextra -Werror
tests: lib $(TESTS)
	sh ./tests/basic_tests.sh

samples: $(SAMPLES)

$(SAMPLES): bin/% : samples/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ samples/testbench.c $<

# The Cleaner
clean:
	rm -rf build $(OBJECTS) $(TESTS)
	rm -f tests/tests.log
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`
	rm -rf $(SAMPLES)
	rm -f samples/tests.log
	rm -rf bin/*

# The Install
install: lib
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/
	ldconfig -n  $(DESTDIR)/$(PREFIX)/lib/
	cd  $(DESTDIR)/$(PREFIX)/lib/; \
	ln -fs lib$(LIB_NAME).so.$(MAJOR) lib$(LIB_NAME).so
