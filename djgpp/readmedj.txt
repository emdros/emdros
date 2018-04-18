README for Emdros on DJGPP
Created: 7/11-2004 (July 11, 2004)
Last update: 11/9-2005 (November 9, 2005)


What is this?
=============

This is a version of the "Emdros on DJGPP" distribution.  Here you
will find:

- the executable files,
- the libraries,
- header files,
- the documentation.

The executables and libraries are prebuilt for DJGPP.


How do I use it?
================

I want to build programs
------------------------

If you want to build programs using Emdros, you will need DJGPP.  It
is available from:

http://www.delorie.com

Use the ZIP picker to download.  Be sure to read the files that are
recommended reading when you download.


I only want to use the example programs
---------------------------------------

If you only want to run the example programs, you need

ftp://ftp.fh-koblenz.de/pub/DJGPP/current/v2misc/csdpmi5b.zip

If this file has moved, go to:

http://www.delorie.com/

and use the ZIP picker to pick which files you need to run DJGPP
programs.



	

Version
=======

To see what version this is, run the following command:

bin\mql.exe -V



Building from source
====================

To build these files from source, you need to download the Emdros
sources from

http://emdros.org/download.html

Then untar the file.  Be sure to do so either on Linux or on Windows,
or at least use a tar program which knows how to handle long
filenames, thus creating not only the XXXXXX~Y.ZZZ 8+3 filenames, but
also the underlying long filenames.

Then 

cd djgpp\

and

make -f djgpp.mak

you can also do

make -f djgpp.mak clean

or 

make -f djgpp.mak dist

The latter builds a binary distribution.  You may need to change the
ZIP program in djgpp\djgpp.mak and/or its invocation in the dist
target.  I use Info-ZIP's DOS version, available from:

http://www.info-zip.org



Enjoy!

Ulrik Petersen
Emdros maintainer

