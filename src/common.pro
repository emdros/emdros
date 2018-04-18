INCLUDEPATH += ../include

CODECFORTR = "UTF-8"

unix:LIBS += \
    ../harvest/libharvest.a \
    ../util/libutil_emdros.a \
    ../MQL/libmql.a \
    ../EMdF/libemdf.a \
    ../pcre/libpcre_emdros.a \
    ../sqlite3/src/libsqlite3_emdros.a \
    -ldl
macx:LIBS += \
    ../harvest/libharvest.a \
    ../util/libutil_emdros.a \
    ../MQL/libmql.a \
    ../EMdF/libemdf.a \
    ../pcre/libpcre_emdros.a \
    ../sqlite3/src/libsqlite3_emdros.a \
    -ldl
win32:LIBS += -L..\\pcre \
    -L..\\EMdF \
    -L..\\MQL \
    -L..\\sqlite3\\src \
    -L..\\harvest \
    -L..\\util \
    -lutil_emdros \
    -lharvest \
    -lmql \
    -lemdf \
    -lpcre_emdros \
    -lsqlite3_emdros 



#macx:QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk
macx:QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6
macx:CONFIG += x86_64
macx:CONFIG -= app_bundle

CONFIG += console
