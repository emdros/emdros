# ---------------------
# Set the target CPU:
# X86 for 32-bit, X64 for 64-bit
# ---------------------
!IF "$(TARGET_CPU)" == ""
TARGET_CPU = X64
!ENDIF


# ---------------------
# Comment out the BUILD_FOR_DEVEL line if you
# only wish to build (and use) the amalgamation.
# Otherwise, all individual Emdris libraries
# will be built, as usual.
# (libemdf.lib, libmql.lib, etc.).
#
# If you comment this out, or set it to 0,
# then the individual libraries are not built.
# Instead, the amalgamation is built and used.
# ---------------------
!IF "$(BUILD_FOR_DEVEL)" == ""
BUILD_FOR_DEVEL = 1
!ENDIF



# ---------------------
# Comment any of these out
# in order not to compile that backend
# ---------------------
!IF "$(SQLITE3)" == ""
SQLITE3=1
!ENDIF

!IF "$TARGET_CPU" == "X64"
# You may modify this...
!IF "$(MYSQL)" == ""
MYSQL=1
!ENDIF
!ELSE
# But not this...
MYSQL=0
!ENDIF

# PostgreSQL support is not provided in the official binaries,
# due to library conflicts between PostgreSQL and MySQL,
# and due to licensing restrictions posed on the libraries
# shipped with PostreSQL.
!IF "$(PGSQL)" == ""
PGSQL=0
!ENDIF

#
# The BPT backend is a database backend for Emdros developed
# by Ulrik Sandborg-Petersen, the author of Emdros.
#
# It is much faster than even SQLite, which otherwise is the fastest
# backend for Emdros, and the databases created for the BPT backend
# are much smaller than even the databases of SQLite 3, which already
# compresses data a lot.
#
# The BPT backend is proprietary, and only avilable by licensing from
# Ulrik Sandborg-Petersen.
#
# Please feel free to inquire about the BPT backend: ulrikp@emdros.org
#
# If you have obtained a license, and therefore the source code,
# you can set the following to 1.
#
!IF "$(BPT)" == ""
BPT=0
!ENDIF



# -----------------------
# Choose debug or release
# -----------------------

!IF "$(DEBUG)" == ""
DEBUG=0
#DEBUG=1
!ENDIF

# Don't do anything to this.
DEBUGINFO=1


# -----------------------------------------------
# Choose whether to build stuff other than Emdros
# Set this to 1 if only Emdros is to be built
# (including the programs in src\, but not in
#  src\qrytool).
# -----------------------------------------------

!IF "$(EMDROS_ONLY)" == ""
EMDROS_ONLY=0
#EMDROS_ONLY=1
!ENDIF



# ----------------------------------------------
# Set one of the following paths 
# to wherever you have your PostgreSQL
# or MySQL (and ignore whichever isn't relevant)
# ----------------------------------------------
!IF "$(PGTOP)" == ""
PGTOP=$(PROGRAMFILES)\PostgreSQL\8.4
!ENDIF

!IF "$(MYSQLTOP)" == ""
MYSQLTOP=C:\Program Files\MySQL\MySQL Connector C 6.1
!ENDIF


# ----------------------------------------------
# 
# SQLite variables
# 
# ----------------------------------------------

# If this is set to 1, then syncing is turned off on SQLite. 
# This means that changes are just handed to the operating system
# and then the SQLite library moves on, without calling the appropriate
# OS API to make sure changes are flushed to disk.  This means that 
# changes may be lost if the machine crashes while the SQLite library 
# has a database open. 
#
# On the other hand, things are significantly faster with this set to 1.

!IF "$(SQLITE3_NO_SYNC)" == ""
SQLITE3_NO_SYNC=1
#SQLITE3_NO_SYNC=0
!ENDIF


# ----------------------------------------------
# 
# wxWidgets variables
# 
# ----------------------------------------------

# Uncomment this line if you wish to compile the programs
# which depend on wxWidgets
!IF "$(WITH_WXWIN)" == ""
WITH_WXWIN=yes
!ENDIF


# Set this as appropriate
!IF "$(WXWIN_HOME)" == ""
WXWIN_HOME=c:\wxWidgets-3.1.2
!ENDIF

# ----------------------------------------------
# SWIG C# support
# ----------------------------------------------

# set to "yes" to enable C# SWIG support
!IF "$(WITH_SWIG_CSHARP)" == ""
WITH_SWIG_CSHARP=no
#WITH_SWIG_CSHARP=yes
!ENDIF


# ----------------------------------------------
# SWIG Python support
# ----------------------------------------------

# uncomment to enable Python
!IF "$(WITH_SWIG_PYTHON)" == ""
WITH_SWIG_PYTHON=no
#WITH_SWIG_PYTHON=yes
!ENDIF

#PYTHON_INCLUDE : Set this to the directory that contains python.h
!IF "$(PYTHON_INCLUDE)" == ""
PYTHON_INCLUDE=c:\python27\include
!ENDIF

# PYTHON_LIB : Set this to the python library including path for linking with
!IF "$(PYTHON_LIB)" == ""
PYTHON_LIB=c:\python27\libs\python27.lib
!ENDIF


# ----------------------------------------------
# SWIG Java upport
# ----------------------------------------------

# uncomment to enable Java
!IF "$(WITH_SWIG_JAVA)" == ""
WITH_SWIG_JAVA=no
#WITH_SWIG_JAVA=yes
!ENDIF

#SWIG_JDK_DIR : Set this to the top-level JDK directory
!IF "$(SWIG_JDK_DIR)" == ""
SWIG_JDK_DIR=c:\Program Files\java\jdk1.8.0_121
!ENDIF



# ----------------------------------------------
# 
# NSIS "Nullsoft install-system" support 
# (http://nsis.sourceforge.net)
# 
# ----------------------------------------------
!IF "$(NSISEXE)" == ""
NSISEXE=C:\Program Files (x86)\NSIS\makensis.exe
!ENDIF


# Set VERSION variable
VERSION=@VERSION@


# Don't touch this, or Emdros won't compile.
# Useful for re-purposing body.mak to projects outside of Emdros.
COMPILE_EMDROS = 1

#
# This is the basis for recursive nmake.
# In win32/win32.mak, this MAKEARGS macro is used
# whenever doing recursive nmake
#
MAKEARGS =  CC="$(CC)" CXX="$(CXX)" CFLAGS="$(CFLAGS)" CXXFLAGS="$(CXXFLAGS)" \
            CPPFLAGS="$(CPPFLAGS)" LDFLAGS="$(LDFLAGS)" CPP="$(CPP)" \
	    TARGET_CPU="$(TARGET_CPU)" \
	    BUILD_FOR_DEVEL="$(BUILD_FOR_DEVEL)" \
	    SQLITE3="$(SQLITE3)" \
	    MYSQL="$(MYSQL)" PGSQL="$(PGSQL)" \
	    BPT="$(BPT)" \
	    DEBUG="$(DEBUG)" \
	    EMDROS_ONLY="$(EMDROS_ONLY)" \
	    PGTOP="$(PGTOP)" MYSQLTOP="$(MYSQLTOP)" \
	    SQLITE3_NO_SYNC="$(SQLITE3_NO_SYNC)" \
	    WITH_WXWIN="$(WITH_WXWIN)" \
	    WXWIN_HOME="$(WXWIN_HOME)" \
	    WITH_SWIG_CSHARP="$(WITH_SWIG_CSHARP)" \
	    WITH_SWIG_PYTHON="$(WITH_SWIG_PYTHON)" \
	    PYTHON_INCLUDE="$(PYTHON_INCLUDE)" \
	    PYTHON_LIB="$(PYTHON_LIB)" \
	    WITH_SWIG_JAVA="$(WITH_SWIG_JAVA)" \
	    SWIG_JDK_DIR="$(SWIG_JDK_DIR)" \
	    NSISEXE="$(NSISEXE)" 
