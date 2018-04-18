TEMPLATE = subdirs

CONFIG = copyheader

SUBDIRS = pcre sqlite sqlite3 EMdF MQL harvest util src

CONFIG += ordered

copyheader {
  win32 {
      system(copy /Y win32\\config.h.win32 /B .\\include\\emdros-config.h /B)
  }
  unix {
  }
  macx {
  }
}
