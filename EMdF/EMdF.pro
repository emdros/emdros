TEMPLATE = lib

TARGET = emdf

CONFIG = staticlib

VERSION = 3.1.1

SOURCES = arena.cpp \
       conn.cpp \
       bptemdfdb.cpp \
       emdfdb.cpp \
       pgemdfdb.cpp \
       mysqlemdfdb.cpp \
       sqlite3conn.cpp \
       sqlite3emdfdb.cpp \
       crc32.cpp \
       emdf_ffeatures.cpp \
       emdf_hash.cpp \
       emdf_output.cpp \
       emdf_value.cpp \
       encryption.cpp \
       enum_const_cache.cpp \
       infos.cpp \
       inst.cpp \
       inst_object.cpp \
       logging.cpp \
       minidom.cpp \
       monads.cpp \
       qdxml.cpp \
       string_func.cpp \
       string_list.cpp \
       string_set_cache.cpp \
       table.cpp \
       utils.cpp \
       json_classes.cpp \
       json_parser.cpp \
       json_lexer.cpp

INCLUDEPATH = ../include


unix:LIBS += -L/usr/lib/mysql -lmysqlclient -lpq -lz

DEFINES += USE_MYSQL=0 USE_POSTGRESQL=0 USE_SQLITE3=1

unix:DEFINES += LINUX=1 SUN=0
macx:DEFINES += LINUX=1 SUN=0
win32:DEFINES += LINUX=0 SUN=0
win32:contains(MAKEFILE_GENERATOR, MSVC)|contains(MAKEFILE_GENERATOR, MSVC.NET) {
    QMAKE_CXXFLAGS += -EHsc
}



#macx:QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk
macx:QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6
macx:CONFIG += x86_64



