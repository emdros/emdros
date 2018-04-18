TEMPLATE = lib

TARGET = harvest

CONFIG = staticlib 

VERSION = 3.1.1

INCLUDEPATH = ../include .

SOURCES = \
          templatelang_classes.cpp \
          templatelang_parser.cpp templatelang_lexer.cpp \
          renderxml.cpp \
          memobject.cpp \
          renderobjects.cpp \
          harvest_fts.cpp \
          harvest_fts2.cpp

win32:contains(MAKEFILE_GENERATOR, MSVC)|contains(MAKEFILE_GENERATOR, MSVC.NET) {
    QMAKE_CXXFLAGS += -EHsc
}


#macx:QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk
macx:QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6
macx:CONFIG += x86_64


