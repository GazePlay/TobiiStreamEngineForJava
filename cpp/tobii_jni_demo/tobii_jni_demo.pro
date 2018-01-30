TEMPLATE = app


QT -= core gui

INCLUDEPATH += "C:\Program Files\Java\default\include"
INCLUDEPATH += "C:\Program Files\Java\default\include\win32"

INCLUDEPATH += "$$_PRO_FILE_PWD_$$/../tobii_jni"

LIBS += -L"$$_PRO_FILE_PWD_$$\..\..\src\main\resources\lib\tobii\x64"
LIBS += -ltobii_stream_engine -ltobii_jni_stream_engine

SOURCES += \
    tobii_jni_demo.cpp
