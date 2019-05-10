README Emdros support for MinGW compilation on Windows
Last udpate: 2019-05-09

Introduction
============

Emdros can usually be compiled with MinGW on Windows.  If that is not
the case, feel free to contact the author, or file a bug on GitHub.

https://github.com/emdros/emdros

Prerequisites
=============

1) A Windows machine. Cross-compilation from Linux or other OS's is
   currently not supported.

2) You need to install MinGW on your Windows machine.  This needs to
   be mingw32, without MSYS.  While MSYS may be installed, you should
   not use it to compile Emdros.

3) If you wish to compile the GUI programs, you need to install
   wxWidgets and compile it before compiling Emdros.  See below for
   how to do that.

4) If you wish to build the installers, you need to install Nullsoft
   Scriptable Install System (NSIS):

   https://nsis.sourceforge.io/


Compiling Emdros without wxWidgets
==================================

1) Unpack the Emdros sources.

2) Run a cmd.exe command prompt.

3) Ensure that the MinGW tools are in your path.  E.g.:

SET PATH=C:\MinGW\bin;%PATH%

4) cd into the Emdros sources's win32\ subdirectory:

cd emdros-x.y.z\win32

5) Run this:

mingw32-make -f mingw.mak -j8 all TARGET_CPU=X86 WITH_WXWIN=no 

See mingw-config.mak for which configuration variables you can add to
the command line.

6) Wait for this to finish.  If it fails, please file a bug report on GitHub:

https://github.com/emdros/emdros

7) Once it builds with success, you might want to build an installer.

mingw32-make -f mingw.mak inst TARGET_CPU=X86 WITH_WXWIN=no

This produces an installer .exe in the root of the Emdros sources,
provided you have installed NSIS (see above, under Prerequisites).



Compiling wxWidgets
===================

1) Download the latest wxWidgets sources:

https://wxwidgets.org/

We use the .exe installer.

Install the wxWidgets source code.

2) Open a cmd.exe command prompt.

3) Ensure that the MinGW tools are in your path.  E.g.:

SET PATH=C:\MinGW\bin;%PATH%

4) cd into the wxWidgets source code's build\msw directory, e.g.:

c:
cd \wxWidgets-3.1.2\build\msw
cd emdros-x.y.z\win32

5) Run this:

mingw32-make -f makefile.gcc -j8 all CPU=X86 RELEASE=1 RUNTIME_LIBS=static SHARED=0

Let it run to completion.

You can now proceed with compiling Emdros with wxWidgets.



Compiling Emdros with wxWidgets
===============================

Just do the same as specified above under "Compiling Emdros without wxWidgets", but remove

WITH_WXWIN=no

from the command lines.

That is, substitute the following in the above steps:


5) Run this:

mingw32-make -f mingw.mak -j8 all TARGET_CPU=X86

7) Once it builds with success, you might want to build an installer.

mingw32-make -f mingw.mak inst TARGET_CPU=X86


Enjoy!


Ulrik Sandborg-Petersen
