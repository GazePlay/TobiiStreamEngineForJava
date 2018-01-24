TEMPLATE = app

QT -= core gui

win32:INCLUDEPATH += "C:\Program Files\Java\default\include"
win32:INCLUDEPATH += "C:\Program Files\Java\default\include\win32"

unix:INCLUDEPATH += "/usr/lib/jvm/default/include"
unix:INCLUDEPATH += "/usr/lib/jvm/default/include/linux"

INCLUDEPATH += "$$_PRO_FILE_PWD_$$/../tobii_jni"

win32:LIBS += -L"$$_PRO_FILE_PWD_$$\..\..\src\main\resources\lib\tobii\x64"

LIBS += -ltobii_stream_engine -ltobii_jni_stream_engine

SOURCES += \
    tobii_jni_demo.cpp
