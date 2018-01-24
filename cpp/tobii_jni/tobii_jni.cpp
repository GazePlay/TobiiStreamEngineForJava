#include "tobii_jni.h"
#include "tobii_headers.h"
#include "tobii_api.h"
#include "tobii_device.h"
//#include <iostream>
//#include <fstream>
#include <memory>

using namespace std;

namespace
{
    unique_ptr<TobiiAPI> tobii_api;

    unique_ptr<TobiiDevice> tobii_device;

//    ofstream fout("tobii_jni_out.txt");
}

JNIEXPORT jint JNICALL Java_tobii_Tobii_jniInit(JNIEnv*, jclass)
{
    try
    {
        tobii_api = make_unique<TobiiAPI>();
        tobii_device = make_unique<TobiiDevice>(*tobii_api);
    }
    catch (int error_code)
    {
        return error_code;
    }
    return 0;
}

JNIEXPORT jfloatArray JNICALL Java_tobii_Tobii_jniGazePosition(JNIEnv* env, jclass)
{
    jfloatArray return_jfloat_array = env->NewFloatArray(2);
    jfloat return_jfloat_array_elements[2];
    return_jfloat_array_elements[0] = tobii_device->get_latest_gaze_point()[0];
    return_jfloat_array_elements[1] = tobii_device->get_latest_gaze_point()[1];
    env->SetFloatArrayRegion(return_jfloat_array, 0, 2, return_jfloat_array_elements);
    return return_jfloat_array;
}

