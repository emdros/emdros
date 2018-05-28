Compiling Emdros with Visual C++
================================

This file contains instructions for how to compile Emdros with Visual
Studio .Net 2005 or higher.  The instructions are split up into these
parts:

- Generic build instructions
- SWIG support
- wxWidgets
- Building Emdros only



Generic build instructions
==========================

1) If you wish to support MySQL, download the latest MySQL from
   www.mysql.com and install it.  Do remember to check the box in the
   installer that says you want the development environment.

2) If you wish to support PostgreSQL, download the latest PostgreSQL
   installer from www.postgresql.org and install it.  Do remember to
   check the box in the installer that says you want the development
   environment.

3) Edit win32\config.mak to suit your taste.  In particular, you
   should comment-out any of the following lines with the backend name
   for which you do not wish support for that backend.

   SQLITE2=1
   SQLITE3=1
   MYSQL=1
   PGSQL=1

   Also, you should set MYSQLTOP if you need support for MySQL, and
   PGTOP if you need support for PostgreSQL.
   
   Also, consider un-commenting-out the following lines:

   #WITH_WXWIN=yes
   #WITH_SWIG_PYTHON=yes
   #WITH_SWIG_JAVA=yes

   Please see the note above about wxWidgets and/or SWIG if you decide
   to uncomment any of these lines.

4) Build the package

   a) Start a console window.

   b) Make sure you have the VC++ environment variables in the
      environment.  On Windows NT/2000, these may be there already.
      If in doubt, run the BAT file VCVARS32.BAT found in the Visual
      C++ distribution (usually found in "C:\Program Files\Microsoft
      Visual Studio 9.0\VC\bin", or an equivalent).

   c) cd to win32 in the Emdros source directory.

   d) nmake /f win32.mak

   e) If you wish, you can do an

      nmake /f win32.mak dist

      This will create a directory, called emdros-<version>-win32,
      which contains all the files needed in the distribution.

5) If you have opted for support for MySQL:

   In order to use the binaries, you need to place the following files
   from the MySQL installation, either in your PATH, or in the same
   directory as the Emdros binaries.

   $(MYSQLTOP)\lib\opt\libmysql.dll

6) If you have opted for support for PostgreSQL:

   In order to use the binaries, you need to place the following files
   from the PostgreSQL installation either in your path or in the same
   directory as the Emdros binaries.

   - $(PGTOP)\bin\libpq.dll
   - $(PGTOP)\bin\comerr32.dll
   - $(PGTOP)\bin\ssleay32.dll
   - $(PGTOP)\bin\libeay32.dll
   - $(PGTOP)\bin\krb5_32.dll
   - $(PGTOP)\bin\libiconv2.dll
   - $(PGTOP)\bin\libintl3.dll
   - $(PGTOP)\bin\k5sprt32.dll
   - $(PGTOP)\bin\gssapi32.dll

   These are copied into the emdros-<version>-win32 directory by
   default by the "dist" target.


SWIG support
============

What is SWIG?
-------------

SWIG is a wrapper interface generator for wrapping C and C++ for use
with scripting languages.  Currently, SWIG bindings for Python and
Java are supported on Win32.

You do not need SWIG installed in order to use the SWIG bindings,
since they have been pre-computed before distributing the Emdros
sources.


Python support
--------------

Before you compile Emdros, do the following:

1) Edit the win32\config.mak file as described in
   ..\SWIG\python\README-Python.

2) Proceed with one of the sections below (i.e., with PostgreSQL or
   MySQL compilation), or with Java configuration

It is recommended that you use the latest version of ActiveState
Python (www.activestate.com).


Java support
------------

Before you compile Emdros, do the following:

1) Edit the win32\config.mak file as described in
   ..\SWIG\java\README-Java.

2) Proceed with one of the sections below (i.e., with PostgreSQL or
   MySQL compilation), or with C# configuration


C# support
----------

Before you compile Emdros, do the following:

1) Edit the win32\config.mak file by uncommenting the line that says
   "WITH_SWIG_CSHARP=yes" (as described in
   ..\SWIG\java\README-csharp).

2) Proceed with one of the sections below (i.e., with PostgreSQL or
   MySQL compilation).


wxWidgets
=========

The graphical programs in Emdros use the wxWidgets framework, which
you can download free of charge from <http://www.wxwidgets.org>.
Emdros has been built with wxWidgets 2.8.X, 3.0.x, and 3.1.x.

wxWidgets support is enabled by default.  

This discussion is separated into two methods, depending on the
wxWidgets version:

- 2.8.X
- >= 3.0.0


2.8.X
-----

With this version of wxWidgets, the following method works for the
author.

1. Download, install, and build wxWidgets with Visual C++ 2008 or
   greater.

                         *** PLEASE NOTE ***

   *** IF YOU WISH TO BUILD EMDROS OUT-OF-THE-BOX WITH wxWidgets ***
   *** THEN YOU *MUST* BUILD wxWidgets  WITH /MT rather than /MD ***
   
   The best way to do this is to edit the configuration file for
   wxWidgets, then compile with nmake.

   If you don't wish to build wxWidgets with /MT, you MUST change the
   two lines that start with DEBUG_SENSITIVE_CPPFLAGS in
   win32\body.mak, to say /MDd and /MD respectively.

   Here's how to edit the wxWidgets config file and build wxWidgets
   with nmake:

   a) Edit c:\wxWidgets-2.8.X\build\msw\config.vc in your favorite
      editor.

   b) Edit the following variables as follows:

      BUILD = release
      UNICODE = 1
      MONOLITHIC = 1
      RUNTIME_LIBS = static
      
   c) Save the file.

   d) Build from a command prompt with the following two commands:

      cd c:\wxWidgets-2.8.X\build\msw
      nmake /f makefile.vc

2. In the config.mak file (in the win32/ directory of the Emdros
   sources), uncomment the line that says:

   WITH_WXWIN=yes

3. In the same config.mak file, set the

   WXWIN_HOME 

   variable to the root directory of your wxWidgets installation.

Then you can proceed with the build as detailed below.


wxWidgets version >= 3.0.0
--------------------------

1. Download the wxWidgets .exe installer -- not the binaries, the
   source installer -- from the wxWidgets website.

2. Install the wxWidgets sources in C:\wxWidgets-3.X.Y

3. Open a command prompt from your MS Visual Studio, so that all
   environment variables are set for compiling.  This must be the same
   version of MS Visual Studio with which you want to compile Emdros.

4. Issue these commands:

   cd c:\wxWidgets-3.X.Y\build\msw
   nmake /f makefile.vc BUILD=release RUNTIME_LIBS=static SHARED=0

   That should build wxWidgets for inclusion in Emdros for 32-bit
   Emdros.  Wait until it is finished.

   In case you want to build a 64-bit wxWidgets library, you should do
   this instead of the above nmake command:

   nmake /f makefile.vc BUILD=release RUNTIME_LIBS=static SHARED=0 CPU=x64

   A couple of explanations are in order:

   Setting BUILD=release means that it is release versions, not debug
   versions, that are built.

   Setting RUNTIME_LIBS=static means that /MT is used when building
   wxWidgets.

   Setting SHARED=0 means that it is .lib files that are build, not
   .dlls.

   Setting CPU=x64 causes wxWidgets to build in 64-bit mode.

5. Edit the win32\config.mak file within the Emdros sources in your
   favorite text editor.

   a. Remove the # so that the following line is not commented out:

   WITH_WXWIN=yes

   b. Set the following variable to point to your wxWidgets root
   directory:

   WXWIN_HOME=C:\wxWidgets-3.X.Y

6. Then proceed with building Emdros as detailed below.





Building Emdros only
====================

If you wish to build Emdros only (no SWIG, no query tool), you can
define EMDROS_ONLY=1 in win32\config.mak.

