TEMPLATE = lib

CONFIG += shared
QT -= core gui

win32:INCLUDEPATH += "C:\Program Files\Java\jdk1.8.0_111\include"
win32:INCLUDEPATH += "C:\Program Files\Java\jdk1.8.0_111\include\win32"

unix:INCLUDEPATH += "/usr/lib/jvm/default/include"
unix:INCLUDEPATH += "/usr/lib/jvm/default/include/linux"

LIBS += -L"C:\Users\schwab\Desktop\tobii\lib\x64"
LIBS += -ltobii_stream_engine

SOURCES += \
    cpp/jni_tobii.cpp

HEADERS += \
    cpp/jni_tobii.h
