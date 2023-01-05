Compiling Emdros with Visual C++
================================

This file contains instructions for how to compile Emdros with Visual
Studio 2015 or higher.  The instructions are split up into these
parts:

- Prerequisites
- Building Emdros
- SWIG support
- wxWidgets
- Special configurations


Prerequisites
=============

0) Install Visual Studio 2015 or later [required]

https://visualstudio.microsoft.com/

1) Install MySQL [optional].

If you want to use MySQL as a backend, you must install MySQL on the build machine.

Go to www.mysql.com to install it.

The development files must be installed with MySQL as part of the
installation process. Specifically, the C++ Connector must be installed.


2) Install PostgreSQL [optional]

If you want to use PostgreSQL as a backend, you must install PostgreSQL on the build machine.

https://www.postgresql.org/


3) Install wxWidgets [optional]

If you want to build the client desktop programs that come with Emdros, you must install wxWidgets.

See the section on wxWidgets below for how to do this.


4) Install the Java (Open)JDK [optional]

If you want to compile support for the SWIG Java bindings, you must install the Java JDK.

You can install the OpenJDK version:

https://openjdk.net/

You could also install the version from Oracle that requires you to
comply with the requirements of Oracle's license.

5) Install Visual Studio 2015 or later.

You can install any edition of Visual Studio, including the Community Edition.



Building Emdros
===============

1) First, ensure that you've installed all the required / wanted prerequisites (see above).

2) Open a Developer Command window from Visual Studio.

Currently, the versions of the Developer Command window using PowerShell are not supported.

See the documentation for Visual Studio for how to do this.

You should ensure that you open the Developer Command window which builds for the CPU target you want.

Currently, the following target CPUs are supported

- x64 (64 bit)
_ x86 (32 bit)

3) Unpack the Emdros sources somewhere on your file system. E.g.:

C:\Users\john\source\emdros\emdros-3.8.0

4) In the Developer Command window, navigate to the "win32" directory inside the root of the Emdros sources. E.g.:

> cd C:\Users\john\source\emdros\emdros-3.8.0\win32

5) Now it's time to build Emdros, using the nmake tool. nmake is a part of the Visual Studio installation.

You can set a number build variables on the command line, or you can set them by editing the config.mak.

Here is an example which sets some of the possible build variables:

> nmake /f win32.mak SQLITE=1 MYSQL=1 MYSQLTOP="C:\Program Files\MySQL\Connector C++ 8.0" PGSQL=0 BPT=0 WITH_WXWIN=yes WXWIN_HOME=C:\wxWidgets-3.2.1 TARGET_CPU=X64

You are encouraged to study the "config.mak" file in this directory. It shows you what variables can be set.

If the build fails, please try to debug it. If you need help, please
contact the author of Emdros. You can find the email address in the
AUTHORS file in the root of the sources.

6) You can build a directory with an Emdros distribution, where all files are placed in a nice directory structure.

> nmake /f win32.mak dist

The directory will be called something like
emdros-<version>-<target_cpu>, and will be located inside the root of
the emdros sources.

You can drag and drop this resource folder to whereever you want and use it from there.

7) Build an executable installer [optional]

You can build an executable installer for Emdros with the NSIS tool
(Nullsoft Installer System). Install NSIS first, before you do this.

> nmake /f win32.mak inst

You may need to tweak the win32\emdros.nsi file.

Have fun with using Emdros profitably!



SWIG support
============

What is SWIG?
-------------

SWIG is a wrapper interface generator for wrapping C and C++ for use
with scripting languages.  Currently, SWIG bindings for Java and C#
are supported on Windows.

You do not need SWIG installed in order to use the SWIG bindings,
since they have been pre-computed before distributing the Emdros
sources.


Python support
--------------

Python support is probably currently broken. Please skip the following
obsolete instructions.

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




Special configurations
======================


Building Emdros only
--------------------

If you wish to build Emdros only (no examples, no SWIG, no query
tool), you can define EMDROS_ONLY=1 in win32\config.mak.

Or you can pass

EMDROS_ONLY=1

to nmake.



Building the amalgamation library only
--------------------------------------

In win32\config.mak, there is a configuration variable called

BUILD_FOR_DEVEL

If this is 1, then all Emdros libraries are built (libemdf.lib,
libmql.lib, etc.) and used when linking programs against the Emdros
libraries.

This has been the case since the very first Windows version of Emdros,
so the default value of BUILD_FOR_DEVEL is 1.

If you set it to non-1, then only the
win32\Release\libemdros_amalgamation.lib library is built and used for
linking programs.

You can simply pass

BUILD_FOR_DEVEL=0

to nmake for this to take effect.

