Notes on how to make and use Emdros on Android

Overview
--------

Emdros works great on Android.

The general overview of how to make and use Emdros on Android
includes:

- You need to set up the Emdros sources

- You need to create an Application.mk file (example provided below)

- You need to create an Android.mk file for (example providede below)

- You can optionally build Emdros at this point to see that everything
  is in order.

- You need to create a C++ wrapper around the parts of Emdros that you
  wish to expose.  This should be coded in C++, but should define some
  interface functions which are declared 'extern "C"'.

- You need to create a JNI wrapper around your C++ wrapper. This
  should be written in C, taking advantage of the interface exposed by
  the C++ wrapper.

- You need to build your shared library, including the Emdros static
  library.

- You need to set up your Android Java sources for using your shared
  library.


Setting up the Emdros sources on Linux or Mac OS X
--------------------------------------------------

- mkdir $PROJECT/jni
- cd $PROJECT/jni
- untar the Emdros sources
- Rename the emdros-X.Y.Z directory to simply "emdros"
- cd emdros
- ./configure --disable-shared --enable-local-pcre --disable-local-zlib --with-sqlite3=local --with-sqlite=no --with-mysql=no --with-postgresql=no --with-wx=no
- make (with the native Linux or Mac OS X tools, not the NDK)

Creating an Android.mk and an Application.mk
--------------------------------------------

The default emdros/Android.mk makefile generates a static library,
libemdros.a, which you can link against in your own shared JNI
library.

First, you should write an Application.mk file in $PROJECT/jni/ which
contains the following line at a minimum:

APP_STL := stlport_shared

You should not use the "system" C++ library, as it does not contain
the necessary STL support.

Second, you can add lines to Application.mk, such that it does more of
what you want, e.g.:

APP_STL := stlport_shared
APP_PLATFORM := android-8
APP_ABI := all

Third, you need to create your own $PROJECT/jni/Android.mk file, like
this:

------------ cut here ------------
include $(call all-subdir-makefiles)

include $(CLEAR_VARS)
LOCAL_MODULE := myemdros
# Add your own sources to the next variable!
LOCAL_SRC_FILES := 
LOCAL_STATIC_LIBRARIES := emdros
include $(BUILD_SHARED_LIBRARY)
------------ cut here ------------


Building Emdros and your shared library
---------------------------------------

Assuming you have set up your Application.mk and your Android.mk files
in $PROJECT/jni, you can now do:

- cd $PROJECT/jni
- $NDK/ndk-build

This should generate emdros/libemdros.a and your own shared library
linking to Emdros.


Creating a C++ wrapper around Emdros
------------------------------------
