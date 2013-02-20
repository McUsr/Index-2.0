Index-2.0
=========

David A. Curry's index, now for Utf-8 and local collation order, for Mac Os X, but portable. Uses ICU.

It is a tremendously useful little tool, for creating a small "database" you needed yesterday.
It takes to secs, from the thought of it would be cool to have a "database" before you are entering
records, that you can filter with awk and other tools. It uses utf-8 so accents aren't any trouble.
It also sorts things accordingly to your locale settings, and uses Perl reg-exps should you like to
use them.

This is the rather premature "dump" of Index 2.0. 

My intentions are to first remove the "rough edges", and perform cross-platform acceptable caluculations
of buffers and so on, adhering more closely to the GNU coding standard, and implement autotools for 
building it.

It is built by a makefile, and for the moment it is assumed that you are using Mac OS X Leopard or
later.

You'll have to go through the makefile and adjust things, as well as download the ncurses library.
You can use a stand alone ICU library, but I have chosen to use the ICU that ships with Mac Os X,
hence the limitation to Mac Os X Leopard and newer.

It builds with both i386, and X86_64 Architecture. And it runs too.

What remains to make this cross platform should not be more than adjust the buffer calculations
as it is, 64-bit wide chars are assumed. And you'll need the Ncurses library. (See the makefile.)
You can get Ncurses version 5.9 at Macports.org.

You'll have to stand in the root directory and issue "make" after you have adjusted the parameters.

The installation, is totally manual at the moment. be sure to read the manual file that resides in the 
man1 folder.

Enjoy.

