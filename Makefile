include Makefile.DEF

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
TARGET=build/lib$(LIB_NAME).a
SO_TARGET=build/lib$(LIB_NAME).so.$(LIB_VERSION)

# The Target Build
release: CFLAGS += -DNDEBUG
debug: CFLAGS += -DDEBUG

all: CFLAGS += -Isrc
all: $(TARGET) $(SO_TARGET)

dev: CFLAGS += -g -Isrc -Wall -Wextra -Werror -pedantic
dev: $(TARGET) $(SO_TARGET)
	cd lib; \
	ln -fs ../$(SO_TARGET) lib$(LIB_NAME).so.$(MAJOR); \
	ln -fs lib$(LIB_NAME).so.$(MAJOR) lib$(LIB_NAME).so

$(SO_TARGET): BUILD_DYNAMIC=1
$(SO_TARGET): $(TARGET) $(OBJECTS)
	$(CC) -shared -Wl,-soname,lib$(LIB_NAME).so.$(MAJOR) $(OBJECTS) -o $@ -lc

$(TARGET): CFLAGS += -fPIC
$(TARGET): build $(OBJECTS)
	ar rcs $@ $(OBJECTS)
	ranlib $@

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
