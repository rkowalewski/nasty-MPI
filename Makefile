CC=mpicc
LIB_NAME=nasty_mpi
MAJOR=0
MINOR=1

CFLAGS=-g -O2 -Wall -std=c99 -Wextra -Isrc $(OPTFLAGS) #-DNDEBUG
LIBS=$(OPTLIBS)
PREFIX?=/usr/local

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

LIB_VERSION=$(MAJOR).$(MINOR)
TARGET=build/lib$(LIB_NAME).so.$(LIB_VERSION)

UNAME := $(shell uname -s)

# The Target Build
all: CFLAGS += -DNDEBUG
all: $(TARGET)

dev: CFLAGS += -Werror
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
tests: dev $(TESTS)
	sh ./tests/runtests.sh

# The Cleaner
clean:
	rm -rf build $(OBJECTS) $(TESTS)
	rm -f tests/tests.log
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`

# The Install
install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/
	ldconfig -n  $(DESTDIR)/$(PREFIX)/lib/
	cd  $(DESTDIR)/$(PREFIX)/lib/; \
	ln -fs lib$(LIB_NAME).so.$(MAJOR) lib$(LIB_NAME).so
