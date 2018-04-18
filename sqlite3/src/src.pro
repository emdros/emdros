TEMPLATE = lib

TARGET = sqlite3_emdros

CONFIG = makelocalamalgam staticlib copyheader

VERSION = 3.7.3

SOURCES = sqlite3_localamalgam.c


INCLUDEPATH = .


makelocalamalgam {
  win32 {
      system(copy /Y prefix.c /B +sqlite3.c /B +encryption.c /B sqlite3_localamalgam.c /B)
  }
  unix {
      system(cat prefix.c sqlite3.c encryption.c > sqlite3_localamalgam.c)
  }
  macx {
      system(cat prefix.c sqlite3.c encryption.c > sqlite3_localamalgam.c)
  }
}

copyheader {
  win32 {
      system(copy /Y sqlite3.h ..\\..\\include\\sqlite3_emdros.h)
  }
  unix {
      system(cp -f sqlite3.h ../../include/sqlite3_emdros.h)
  }
  macx {
      system(cp -f sqlite3.h ../../include/sqlite3_emdros.h)
  }
}


#macx:QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk
macx:QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6
macx:CONFIG += x86_64



