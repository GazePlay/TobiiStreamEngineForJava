#pragma once

#include <jni.h>

extern "C"
{
    JNIEXPORT jint JNICALL Java_tobii_Tobii_jniInit(JNIEnv*, jclass);

    JNIEXPORT jfloatArray JNICALL Java_tobii_Tobii_jniGazePosition(JNIEnv*, jclass);
}
