TEMPLATE = lib

TARGET = util_emdros

CONFIG = staticlib

VERSION = 3.2.1

INCLUDEPATH = . ../include

SOURCES = mql_exporter.cpp agxml.cpp opt.cpp \
          conf.cpp schema.cpp unboundbibleimporter.cpp \
          negraimporter.cpp tigerxmlimporter.cpp \
          plaintextimporter.cpp slashedtextimporter.cpp \
          sfm.cpp sfmimporter.cpp messages.cpp \
          pennparser.cpp pennlexer.cpp penn_yylex.cpp \
          pennimporter.cpp md5_emdros.cpp bptdumper.cpp



#macx:QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk
macx:QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6
macx:CONFIG += x86_64



