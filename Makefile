CC=mpicc
LIB_NAME=nasty_mpi
MAJOR=0
MINOR=1

CFLAGS=-g -O3 -Wall -std=c11  $(OPTFLAGS)
LIBS=$(OPTLIBS)
PREFIX?=/usr/local

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TEST_SRC=$(wildcard tests/**/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

SAMPLES_SRC=$(wildcard samples/*.c)
SAMPLES=$(patsubst samples/%.c,bin/%,$(SAMPLES_SRC))

LIB_VERSION=$(MAJOR).$(MINOR)
TARGET=build/lib$(LIB_NAME).so.$(LIB_VERSION)

UNAME := $(shell uname -s)

# The Target Build
all: CFLAGS += -DNDEBUG -Isrc
all: $(TARGET)

dev: CFLAGS += -Isrc -Wextra -Werror -pedantic
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
.PHONY: tests mpi_samples
tests: LDLIBS = -l$(LIB_NAME)
tests: LDFLAGS = -Wl,-rpath,./lib/ -L./lib/
tests: CFLAGS += -Isrc
tests: all $(TESTS)
	sh ./tests/basic_tests.sh

mpi_samples: $(SAMPLES)
	sh ./samples/runSamples.sh

$(SAMPLES): $(SAMPLES_SRC)
	$(CC) $(CFLAGS) -o $@ $< 

# The Cleaner
clean:
	rm -rf build $(OBJECTS) $(TESTS)
	rm -f tests/tests.log
	rm -rf bin/*
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`

# The Install
install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/
	ldconfig -n  $(DESTDIR)/$(PREFIX)/lib/
	cd  $(DESTDIR)/$(PREFIX)/lib/; \
	ln -fs lib$(LIB_NAME).so.$(MAJOR) lib$(LIB_NAME).so
