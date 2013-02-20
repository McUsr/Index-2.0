# vim: nospell ft=make
# David A. Curry Revision 1.0
# Tommy Bollman	 Revision 2.0 Uses Gnu make and relies on that. HEAVILY!
# http://wiki.osdev.org/Makefile contains some explanations for this makefile
# I have also picked up some at Stack Overflow.
#
# READ THIS!
# ==========
#
# Although this is my "private" makefile until I have the autotools going
# you should be able to build index yourself by using this make file, after
# having unpacked the tarball and standing in the expanded directory when
# you issue commands like make all, or make debug
#
# CAUTION:
# --------
# read INSTALL first!
#
# You should really invoke make like this:
#
# save yourself if your command history doesn't work and:
# export PREFIX=place/to/install and thereafter:
# --> make debug or --> make install.
# I really recommend debug.
# 
# If you want to build static, then you have to download and install the
# ICU library from: http://icu.org, I have used version 49.1.2 when I made this.
#
# NOTE: you have to have a copy of libncurses to build it yourself. 
# I installed ncurses 5.9 from http://macports.org
#
# I have added the ncurses library, and the necessary header files here, so that
# you in most of the cases should be ble to build ncurses statically.

# ligger på root og blir pakket slik i tarball.
# Dynamic betyr Dynamically link med ICU lib.
# Vi linker inn hele curses, for å slippe å ha brukere til å installere programmet.
BUILD	 ?= DYNAMIC
ARCH 	 ?= i386
DESTDIR	 ?= ~/opt
BINDIR   ?= $(DESTDIR)/bin
MANDIR	 ?= $(DESTDIR)/man/man1
# PRJROOT  := $(shell echo ~/Documents/Prj/Index/Source)
PRJROOT  := $(shell pwd)
DOCDIR	 := Doc
AUXFILES := Makefile $(shell find $(DOCDIR) -type f -name "*")\
$(shell find samples -type f -name "*") $(shell find LICENSES -type f -name "*")\
man1/index.1
OBJDIR   =  objdir
SRCDIR   := src
INCDIR   := src
INCLPATH := $(PRJROOT)/$(INCDIR)
CURINCL  := $(INCLPATH)/curses
NCURSPTH := $(CURINCL)
ICUINCL  := $(INCLPATH)/ICU
PROJDIRS := $(SRCDIR) 
SRCFILES := $(shell find $(PROJDIRS) -type f -name "*.c")
HDRFILES := $(shell find $(INCDIR) -type f -name "*.h")
OBJFN    := $(patsubst %.c,%.o,$(SRCFILES)) 
OBJFILES =  $(subst $(SRCDIR),$(OBJDIR),$(OBJFN)) 
TSTFILES := $(patsubst %.c,%_t,$(SRCFILES)) 
DEPFILES := $(patsubst %,%.d,$(TSTFILES))
ALLFILES := $(SRCFILES) $(HDRFILES) $(AUXFILES) $(ICUINCL)
DBGOBJFILES := $(shell find dbg_objdir -depth 1 -name "*") 

.PHONY: debug all mkcheck clean dist check testdriver todolist install cscope ctags

CFLAGS =  -g 

WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
-Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
-Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
-Wuninitialized -Wconversion -Wstrict-prototypes

LDFLAGS  := -arch $(ARCH)  

ifeq ($(BUILD),DYNAMIC)
	ALL_CFLAGS   =   -arch $(ARCH) -std=c99 -O3  -DU_DISABLE_RENAMING=1 -I$(INCLPATH) -I$(ICUINCL)
	LIBS     :=  -lpthread -lm  -licucore -lm
	LIBPATH  :=  -L/usr/lib -L$(OBJDIR)
else
	ALL_CFLAGS   :=  -arch $(ARCH) -g -std=ansi -O2 $(WARNINGS) -I/opt/local/include -I$(INCLPATH) -I$(ICUINCL)
	LIBS     :=  -L/opt/local/lib -lncursesw -lpthread -lm -L/usr/lib -licui18n -licuuc \
-licuio -licudata -lpthread -lm
	LIBPATH  := -L/opt/local/lib -L$(OBJDIR)
endif 

-include $(DEPFILES)

# $(TSTDEPFILES)
$(OBJDIR)/%.o : $(SRCDIR)/%.c Makefile
	@$(CC) $(ALL_CFLAGS) $(CFLAGS) -MMD -MP -c $< -o $(OBJDIR)/$*.o 

all : debug

debug: OBJDIR  = dbg_objdir
debug: ALL_CFLAGS  = -arch $(ARCH) $(WARNINGS) -std=c99 -ggdb -O  -DU_DISABLE_RENAMING=1\
-I/opt/local/include -I$(INCLPATH) -I$(ICUINCL)
debug: index

index : $(OBJFILES)
	@$(CC) $(ALL_CFLAGS) $(CFLAGS) $(OBJFILES) $(NCURSPTH)/libncursesw.a $(LDFLAGS) $(LIBPATH) $(LIBS) -o index

mkcheck:
	echo $(PRJROOT)
	exit 1
	echo $(DBGOBJFILES) 
	exit 1
	echo $(ALLFILES)
	exit 1
	echo $(HDRFILES)
	echo $(SRCFILES)
	echo $(OBJFILES)
	echo $(CFLAGS)

install:
	@command cp -f index $(BINDIR)
	@ls -l $(BINDIR)/index
#	cp -f man1/index.1 $(MANDIR)
	
clean:
	-@$(RM) $(wildcard $(OBJFILES) $(DEPFILES) $(TSTFILES) index)
	-@$(RM) $(wildcard $(DBGOBJFILES) $(DEPFILES) $(TSTFILES) index)

dist:
	tar czf index.tgz $(ALLFILES)

check: testdrivers
	-@rc=0; count=0; \
		for file in $(TSTFILES); do \
			echo "TST	$$file"; ./$$file; \
			rc=`expr $$rc + $$?`; count=`expr $$count + 1` ; \
		done; \
		echo; echo "Tests executed: $$count Tests failed: $$rc"

cscope:
	dsymutil -arch $(ARCH) index
	cscope -b -k -q -s $(SRCDIR) -Findex.dSYM

ctags:
	ctags $(SRCFILES) 
	@mv tags tags.aside
	ctags $(HDRFILES) 
	@cat tags >> tags.aside
	ctags -R  /usr/include
	@cat tags >> tags.aside
	@mv tags.aside tags


todolist:
	-@for file in $(ALLFILES:Makefile=); do fgrep -H -e TODO -e FIXME $$file; done; true
