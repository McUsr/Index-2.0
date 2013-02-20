#
# $Header: /u5/davy/progs/index/RCS/Makefile,v 1.1 89/08/09 11:06:00 davy Exp $
#
# Makefile for the "index" program.
#
# David A. Curry
# Research Institute for Advanced Computer Science
# Mail Stop 230-5
# NASA Ames Research Center
# Moffett Field, CA 94035
# davy@riacs.edu
#
# $Log:	Makefile,v $
# Revision 1.1  89/08/09  11:06:00  davy
# Initial revision
# 
#
CFLAGS=	-O
LIBS=	-lcurses -ltermcap

SRCS=	createdb.c dbfunc.c dbio.c main.c printdb.c screen.c \
	searchdb.c selectdb.c util.c
OBJS=	createdb.o dbfunc.o dbio.o main.o printdb.o screen.o \
	searchdb.o selectdb.o util.o

index: $(OBJS)
	$(CC) $(CFLAGS) -o index $(OBJS) $(LIBS)

clean:
	rm -f a.out core index $(OBJS) \#*

createdb.o:	createdb.c defs.h
dbfunc.o:	dbfunc.c defs.h
dbio.o:		dbio.c defs.h
main.o:		main.c defs.h
printdb.o:	printdb.c defs.h
screen.o:	screen.c defs.h
searchdb.o:	searchdb.c defs.h
selectdb.o:	selectdb.c defs.h
util.o:		util.c defs.h
