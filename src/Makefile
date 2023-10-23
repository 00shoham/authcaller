BIN=\
  authcaller\

all: $(BIN)

UTILDIR=../../idanlib/src
UTILSLIB=$(UTILDIR)/utils.a

LUADIR := $(shell $(UTILDIR)/lua-dir.sh)
LUALIB := $(shell $(UTILDIR)/lua-ver.sh)

INCLUDES=$(LUADIR) -I$(UTILDIR)
LIBS=$(UTILSLIB) -lm -lpthread -luuid $(LUALIB) -lcurl

CC=gcc
CFLAGS=-Wall -O2 -g $(INCLUDES)
# CFLAGS=-Wall -g -rdynamic -fsanitize=address $(INCLUDES)

SHAREDOBJ=\
  config.o\
  pin.o\
  api.o\

ALLOBJ=\
  $(SHAREDOBJ)\

%.o: %.c $(wildcard *.h)
	$(CC) $(CFLAGS) -c $<

$(BIN): % : %.o $(ALLOBJ) $(UTILSLIB)
	$(CC) $(CFLAGS) -o $@ $< $(ALLOBJ) $(LIBS)

clean: NULL
	rm -f $(SHAREDOBJ) $(BIN)
	rm -f *.o

NULL:
