Index-2.0
=========

David A. Curry's index, now for Utf-8 and local collation order, for
Mac Os X, but portable. Uses ICU for i8n and l10n.

About Index-2.0
---------------

It is very effective in many respects.
--------------------------------------

This is a tremendously useful little tool, for creating a small
"databases" you needed yesterday.  It takes two secs, from the thought
of it would be cool to have a "database" before you have defined the
database and are eagerly entering records, records that you can later
filter with awk and other tools. The duality of the tool, working both as
a "screen" program, and as a filter, makes it very useful, in supporting
note-taking in a diverging stage of whatever, without a thought about
structuring there and then.  But you can later converge the semantics
of your "data", by filtering the information you have gathered through it.

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

**Edit** Having implemented the autotools for creating cross platform
configure scripts and compilation, I am now revising stuff, and removing
the last (small) memory deallocation bugs.

I'll also have to implement the config.h, and specifying different
default text editors and such for different platforms when $EDITOR
isn't specified.

Maybe there is something at miss when it comes to adhering to the GNU
coding standard, but not much.

How to build
============

OS X Leopard or later:
----------------------

It is easier to build this on OS X than any other platform, since it
was developed here. But from now onwards, it is recommonded that you
use the configure script, that generates makefiles.

The configure script supports staged installs, so that you can create a
folder, and make the tool in there, not mingling with the source.
You can of course "make install" from this directory.

You'll need Ncurses 5.9 or later in addtion to what is delivered with
OS X in order to build.

Ncurses 5.9 can be easily downloaded from http://www.macports.org. for
OS X

Other platforms
---------------

I have no hopes about this being easy to install under cygwin or anything
but since GNU autotools are used, I suppose it wont't be impossible.

If this is your case, and you face problems, don't hesitate to contact me
(through "issues").

Thanks to GNU autotools I have high hopes that this package will just
install on other *nixes. If it don't, don't hesitate. ("issues"), and I'll
see what I can do, swiftly!

General prequisites
-------------------

You need to install and compile Ncurses 5.9 and ICU 4.9 or later
Having this in shape, and the paths to where it is installed, then
it is time to perform the installation.


The Configuration
====================

The short version
-----------------

1. configure
2. make all
3. sudo make install


The more elaborate (boring) version.
------------------------------------

Stand in the root of the project. (Same directory as you found this
document.) It is your call whether you want to create a folder to
build in or build directly in the source folder.

Preliminaries
-------------
I'll take this a little bit slow in case this is the first time you
are using a configure script. Go to **Configuring the project** if
I am boring you.

First of all: you can configure an "autoconfiscated" projects by two
means, environment variables that steers the compilation, and prefixes
to the configure script that controls the details of the compilation and
installation,in this case, configure can do so much more, but that is
all that is needed for this project.

The C-compiler environment uses a series of environment variables
for setting flags, and finding stuff, you can set those variables
with your particular options.

The variables are:

* CFLAGS 		: This variable should contain the compiler options
like **-g, -O2** and so on. Those are the settings you want to set in
additon to those set in the makefile. (**-g** is always set, if you really
want to strip out the debug information, then **"make install-strip"**
afterwards.)

* CPPFLAGS		: This variable contains flags and defines for the 
C-preprocessor. The most important ones coming to my mind is ther
**-Iincludepath**

* LDFLAGS			: This variable contains flags for the linking
stager of the compilation, or linker if a separate linker is used. The
most common usage is to specify the path to the libraries, you may have
to specify the paths to something like -L/opt/lib, or similar to make
configure find the **ICU-libs** and the **ncursesw** libraries.

* LIBS 			: This variable is for specifying libraries directly
you will have no usage of this variable during this configure.

Setting the variables:
----------------------

You specify the variable before you execute the **./configure** script.
Like this:
CFLAGS="-g -O3" (CFLAGS="-arch= X86_64" is another example.) 


**Configuring the project** 
--------------------------

Short version:
--------------

**--help**, **--libdir**, **--includedir**, **--prefix**, **--bindir**, **--mandir** and **--docdir** works.

Long version:
-------------

You should really try **./configure --help** to see all the options,
I'll go through the most important settings or those that will work
here prioritized by what I think is the most important ones to the lesser.

Options for compiling and making index
--------------------------------------

In the examples I use the path ../ in front of configure, imagining I am standing
in a "build" folder.

* **--libdir** 	:This option tells configure where to look for your libraries
Example:
../configure --libdir=/opt/lib --libdir=/usr/lib
Tells configure to first check /opt/lib, then /usr/lib for libraries.

* **--includedir**	:This option tells configure where to look for your include files.
Example:
../configure --includedir=/opt/include --includedir=/usr/include
Tells configure to first check /opt/include then /usr/include for header files.

Options that influences the installation.
----------------------------------------

* **--prefix** : This prefix option, is kind of a general option, to specify
the "root" of the installation. there should be a bin, and a  share directory
under this path, or they will be created, under the share directory, there should be
a man/man1..N tree, or at least man/man1 will be created to have a place for index.1
It will also be created a share/index directory for storing documents relating to 
index, and samples.

Example:
**../configure --prefix=/opt**

Tells configure to install index, man1/index.1, samples and
other docs into /opt/bin/index, /opt/share/man/man1/index.1
/opt/share/doc/index/samples and /opt/share/doc (the docs, including
this).

or **a=~/opt ../configure --prefix=$a**
Works the same way but uses another root: (~/opt).

Tells configure to install index and auxuillary files under ~/opt I
recommend to place the stuff here or in ~/usr, this is called "account
install" This makes it easy to take the software with you with an
operating system upgrade since it resides in your homefolder, it will
also never conflict with any packet-manager.

* **--bindir**
This is a more selective option it works like the  **--prefix** option,
but works only for specifying the path to put the binary index.
Handy to use if you want to specify stuff in different "trees".

Example:
**../configure --bindir=/opt/bin**

* **--mandir**
This is a more selective option it works like the  **--prefix** option,
but works only for specifying the path to put the index man file..
Example:

* **--docdir**
This is a more selective option it works like the  **--prefix** option,
but works only for specifying the path to put the docs and samples files.
Handy to use if you want to specify stuff in different "trees". You can of course
specify a place you'll later trash. :)
Example:
**../configure --docdir=/opt/share/doc/index**

Ok, that's all about the configure options, --help included, you don't
have much use for any of the others when configuring this package.


Make configuration
------------------

Make rules not war!

* all:				Compiles the program.

* install:			Installs the package.

* uninstall:		Uninstalls the package, save directories created during installation.

* install-strip:	Like install but strips the executable for debug information.

* clean:			Deletes all files in the current directory that where created during the build.

* distclean:		Deletes all files in the current directory that where made by building or configuring this program.

dist:				Create a distribution tar file of this package.

distcheck:			Checks if the dist rule behaves sanely.

If you want more information about those make rules, feel free to read the GNU Coding Standard chp. 7.

**Enjoy**

Tommy/McUsr
