Index-2.0
=========

David A. Curry's index, now for Utf-8 and local collation order, for
Mac Os X, but portable. Uses ICU.

About Index-2.0
---------------

This is a tremendously useful little tool, for creating a small
"databases" you needed yesterday.  It takes to secs, from the thought of
it would be cool to have a "database" before you are entering records,
that you can filter with awk and other tools. It uses utf-8 so accents
aren't any trouble.  It also sorts things accordingly to your locale
settings, and uses Perl reg-exps should you like to use them thanks to
ICU both those, and the regexps described in *man re_format(7)* works.

Database is really not the correct term:, the "database" consists of
lines, relating to fields but it serves as a database semantically,
and it can be a good format to collect various tidbits of information
in, superimposing a structure upon information gathering. It is also a
good prototyping tools, to help you hone the fields you want in real
databases. You also have all the *nix tools available to prototype
relations, reports, and what not. The main advantage of this tool,
given its stone-age user interface, is with what speed you can create a
database, and have a reasonably comfortable interface for putting data
into it. Gaining a structured information collection within mere seconds!
At the same time it is reasonably easy to filter the information. You can
have filters working on the database to filter on the criteria you'll
like. There are a great many commandline options to help you "massage"
your data, and you can also export your data as csv. See the man page
(man1/index.1 for all the options). or (https://github.com/McUsr/Index-2.0/blob/master/Doc/index.html)

The progress:
-------------

The tool as such is now finished, give or take any bug I may find in the
near future. I guess I won't but never say never.

What is up creating calculations for allocating buffers so it can be
usable cross-platform whatever width a widechar_t has.

Maybe there is something at miss when it comes to adhering to the GNU
coding standard, but not much.

How to build
------------

It is built by a makefile, and for the moment it is assumed that you are
using Mac OS X Leopard or later. To build it, you'll also need Ncurses,
as I uses cursesw.a when building it.

Ncurses 5.9 can be easily downloaded from http://www.macports.org.

I urge you to read the makefile carefully, it builds as both an X86_64
and i386 executable.

You can use a stand alone ICU library, but I have chosen to use the
ICU that ships with Mac Os X, hence the limitation to Mac Os X Leopard
and newer.


You'll have to stand in the root directory (the directory above "src"
and issue "make" after you have adjusted the parameters. You'll need to
have Apple's developer tools or similar in order to build it.

Enjoy.

