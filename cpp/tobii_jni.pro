TEMPLATE = lib

CONFIG += shared
QT -= core gui

win32:INCLUDEPATH += "C:\Program Files\Java\default\include"
win32:INCLUDEPATH += "C:\Program Files\Java\default\include\win32"

win32:LIBS += -L"$$_PRO_FILE_PWD_$$\..\java\src\main\resources\lib\tobii\x64"
win32:DESTDIR = "$$_PRO_FILE_PWD_$$\..\java\src\main\resources\lib\tobii\x64"

unix:INCLUDEPATH += "/usr/lib/jvm/default/include"
unix:INCLUDEPATH += "/usr/lib/jvm/default/include/linux"

LIBS += -ltobii_stream_engine
TARGET = tobii_jni_stream_engine

SOURCES += \
    tobii_jni.cpp

HEADERS += \
    tobii_jni.h
