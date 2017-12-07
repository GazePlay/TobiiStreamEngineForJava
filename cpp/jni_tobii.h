#pragma once

#include <jni.h>

#ifdef __cplusplus
extern "C"
{
#endif

    JNIEXPORT jint JNICALL Java_jni_tobii_init(JNIEnv*, jclass);

    JNIEXPORT jstring JNICALL Java_jni_tobii_gazePosition(JNIEnv*, jclass);

#ifdef __cplusplus
}
#endif
