# Configuration
#CC = gcc
LD = $(CC)
CFLAGS = -IModules
LDFLAGS = -lm # only actually needed by accci and wordaccci
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

LIB = \
	Modules/accrpt.o \
	Modules/charclass.o \
	Modules/ci.o \
	Modules/dist.o \
	Modules/edorpt.o \
	Modules/list.o \
	Modules/sort.o \
	Modules/stopword.o \
	Modules/sync.o \
	Modules/table.o \
	Modules/text.o \
	Modules/unicode.o \
	Modules/util.o \
	Modules/wacrpt.o \
	Modules/word.o

SRC = \
	accci/accci.c \
	accdist/accdist.c \
	accsum/accsum.c \
	accuracy/accuracy.c \
	asc2uni/asc2uni.c \
	editop/editop.c \
	editopcost/editopcost.c \
	editopsum/editopsum.c \
	groupacc/groupacc.c \
	ngram/ngram.c \
	nonstopacc/nonstopacc.c \
	synctext/synctext.c \
	uni2asc/uni2asc.c \
	vote/vote.c \
	wordacc/wordacc.c \
	wordaccci/wordaccci.c \
	wordaccdist/wordaccdist.c \
	wordaccsum/wordaccsum.c \
	wordfreq/wordfreq.c

HDR = $(LIB:.o=.h)
OBJ = $(SRC:.c=.o) $(LIB)
BIN = $(SRC:.c=)
MAN = $(SRC:.c=.1)

all: $(BIN)

$(OBJ): $(HDR)
$(BIN): util.a

.o:
	echo LD $@
	$(LD) -o $@ $< util.a $(LDFLAGS)

.c.o:
	echo CC $<
	$(CC) -c -o $@ $< $(CFLAGS)

util.a: $(LIB) $(HDR)
	echo AR $@
	$(AR) -r -s -c $@ $(LIB)

install: all
	echo Installing binaries to $(DESTDIR)$(PREFIX)/bin
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	for f in $(BIN); do b=`basename "$$f"`; cp -f "$$f" "$(DESTDIR)$(PREFIX)/bin/$$b"; done
	echo Installing manual pages to $(DESTDIR)$(MANPREFIX)/man1
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	for f in $(MAN); do b=`basename "$$f"`; cp -f "$$f" "$(DESTDIR)$(MANPREFIX)/man1/$$b"; done

clean:
	echo Cleaning
	rm -f $(BIN) $(OBJ) $(LIB) util.a

.PHONY: all install clean
.SILENT:
