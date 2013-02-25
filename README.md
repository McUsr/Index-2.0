Index-2.0
=========

David A. Curry's index, now for Utf-8 and local collation order, for
Mac Os X, but portable. Uses ICU.

About Index-2.0
---------------

It is very effective in many respects.
--------------------------------------
This is a tremendously useful little tool, for creating a small
"databases" you needed yesterday.  It takes to secs, from the thought of
it would be cool to have a "database" before you are entering records,
records that you can later filter with awk and other tools. The duality of
the tool, working both as a "screen" program, and as a filter, makes it
very useful, in supporting note-taking in a diverging stage of whatever,
without a thought about structuring there and then.  But you can later
converge the semantics of your "data", by filtering the information you
have gathered through it.

It supports international encoding schemes.
-------------------------------------------

It uses utf-8 so accents aren't any trouble.  It also sorts things
accordingly to your locale settings, and uses Perl reg-exps should you
like to use them thanks to ICU both those, and the regexps described in
*man re_format(7)* works.

It uses a regular UTF8 text file as "data-storage".
---------------------------------------------------

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
(man1/index.1 for all the options). or [Index html man page] (https://dl.dropbox.com/u/6829111/index.html "Index.1")

The progress:
-------------

I have implemented the calculation of buffer sizes that should work at 
all times, there are however other limits that needs to be adjusted,
and maybe there are some reallocations to be added for max-memory
efficiency given the fact that I have been more concerned about the 
calulations to work at all times, than being memory efficient.

The tool as such is now finished, give or take any bug I may find in the
near future. I guess I won't but never say never.

Maybe there is something at miss when it comes to adhering to the GNU
coding standard, but not much.

How to build
------------

It is built by a makefile, and for the moment it is assumed that you are
using Mac OS X Leopard or later. To build it, you'll also need Ncurses,
as I uses cursesw.a when building it.

Ncurses 5.9 can be easily downloaded from http://www.macports.org.

Make configuration
------------------

I urge you to read the makefile carefully, it builds as both an X86_64
and i386 executable.

You can use a stand alone ICU library, but I have chosen to use the
ICU that ships with Mac Os X, hence the limitation to Mac Os X Leopard
and newer.

you must choose your destdirs for both Index and its man file, or 
choose to perfom the copy manually.

How to make
-----------

You'll have to stand in the root directory (the directory above "src"
and issue "make" after you have adjusted the parameters. Maybe adjusted
the path to ncursesw.a, and ICU if you use a stand alone library. You'll
need to have Apple's developer tools or similar in order to build it on
OS X.

Try make -n "all" first to assure that it finds what it needs before you
issue make "all", then eventually make "install".

**Enjoy**

Tommy
