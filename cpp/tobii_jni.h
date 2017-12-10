#pragma once

#include <jni.h>

#ifdef __cplusplus
extern "C"
{
#endif

    JNIEXPORT jint JNICALL Java_tobii_Tobii_jniInit(JNIEnv*, jclass);

    JNIEXPORT jfloatArray JNICALL Java_tobii_Tobii_jniGazePosition(JNIEnv*, jclass);

#ifdef __cplusplus
}
#endif
