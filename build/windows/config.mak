# ---------------------
# Set the target CPU:
# X86 for 32-bit, X64 for 64-bit
# ---------------------
!IFNDEF TARGET_CPU
TARGET_CPU = X64
!ENDIF

# ---------------------
# Comment any of these out, or set them to 0,
# in order not to build in support for that backend
# ---------------------

!IFNDEF SQLITE3
SQLITE3=1
!ENDIF


!IFNDEF MYSQL
MYSQL=1
!ENDIF


# PostgreSQL support is not provided in the official binaries,
# due to library conflicts between PostgreSQL and MySQL,
# and due to licensing restrictions posed on the libraries
# shipped with PostreSQL.
#
# Feel free to enable PostgreSQL support in your own binaries,
# by setting the following variable to 1
!IFNDEF PGSQL
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
!IFNDEF BPT
BPT=0
!ENDIF


#
# The BPT2 backend is a database backend similar to the BPT backend,
# developed by the author of Emdros.
#
# It compresses the data even more than BPT.
#
# BPT2 is proprietary, and only available by licensing from Ulrik
# Sandborg-Petersen.
#
# Feel free to inquire about the BPT2 backend: ulrikp@emdros.org.
#
# If you have obtained a license, and therefore the source code,
# you can set the following to 1.
#
!IFNDEF BPT2
BPT2=0
!ENDIF



# -----------------------
# Choose debug (1) or release (0)
# -----------------------

!IFNDEF DEBUG
DEBUG=0
!ENDIF

# Don't do anything to this.
DEBUGINFO=1


# -----------------------------------------------
# Choose whether to build stuff other than Emdros
# Set this to 1 if only Emdros is to be built
# (including the programs in src\, but not in
#  src\qrytool).
# -----------------------------------------------

!IFNDEF EMDROS_ONLY
EMDROS_ONLY=0
!ENDIF



# ----------------------------------------------
# Set one of the following paths 
# to wherever you have your PostgreSQL
# or MySQL (and ignore whichever isn't relevant)
# ----------------------------------------------
!IFNDEF PGTOP
PGTOP=$(PROGRAMFILES)\PostgreSQL\8.4
!ENDIF


!IFNDEF MYSQLTOP
MYSQLTOP=C:\Program Files\MySQL\MySQL Connector.C 6.1
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

!IFNDEF SQLITE3_NO_SYNC
SQLITE3_NO_SYNC=1
!ENDIF



# ----------------------------------------------
# 
# wxWidgets variables
# 
# ----------------------------------------------

!IFNDEF WITH_WXWIN
# Comment out, or set to no, to disable wxWidgets support
WITH_WXWIN=yes
!ENDIF


# Set this as appropriate
!IFNDEF WXWIN_HOME
WXWIN_HOME=c:\wxWidgets-3.0.4
!ENDIF


# ----------------------------------------------
# SWIG C# support
# ----------------------------------------------

!IFNDEF WITH_SWIG_CSHARP
# Comment out, or set to no, to disable C# support
WITH_SWIG_CSHARP=yes
!ENDIF



# ----------------------------------------------
# SWIG Python support
# ----------------------------------------------

!IFNDEF WITH_SWIG_PYTHON
# uncomment to enable Python
#WITH_SWIG_PYTHON=yes
!ENDIF


!IFNDEF PYTHON_INCLUDE
#PYTHON_INCLUDE : Set this to the directory that contains python.h
PYTHON_INCLUDE=c:\python27\include
!ENDIF


!IFNDEF PYTHON_LIB
# PYTHON_LIB : Set this to the python library including path for linking with
PYTHON_LIB=c:\python27\libs\python27.lib
!ENDIF



# ----------------------------------------------
# SWIG Java upport
# ----------------------------------------------

!IFNDEF WITH_SWIG_JAVA
# uncomment to enable Java
#WITH_SWIG_JAVA=yes
!ENDIF


!IFNDEF SWIG_JDK_DIR
#SWIG_JDK_DIR : Set this to the top-level JDK directory
SWIG_JDK_DIR=c:\Program Files\java\jdk1.8.0_121
!ENDIF




# ----------------------------------------------
# 
# NSIS "Nullsoft install-system" support 
# (http://nsis.sourceforge.net)
# 
# ----------------------------------------------
!IFNDEF NSISEXE
NSISEXE=C:\Program Files (x86)\NSIS\makensis.exe
!ENDIF





# Don't touch this, or Emdros won't compile.
# Useful for re-purposing body.mak to projects outside of Emdros.
COMPILE_EMDROS = 1