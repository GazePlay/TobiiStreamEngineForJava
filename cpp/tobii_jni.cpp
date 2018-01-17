#include "tobii_jni.h"
extern "C"
{
#include "tobii_headers.h"
}
#include "tobii_api.h"
#include "tobii_device.h"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <memory>

using namespace std;
using namespace std::chrono;

namespace
{
    float latest_gaze_point[2];

    unique_ptr<TobiiAPI> tobii_api;

    unique_ptr<TobiiDevice> tobii_device;

    ofstream fout("C:\\Users\\Coyl\\out.txt");

    void gaze_callback(const tobii_gaze_point_t* gaze_point, void*)
    {
        //fout << gaze_point->position_xy[0] << " " << gaze_point->position_xy[1] << endl;
        latest_gaze_point[0] = gaze_point->position_xy[0];
        latest_gaze_point[1] = gaze_point->position_xy[1];
    }
}


JNIEXPORT jint JNICALL Java_tobii_Tobii_jniInit(JNIEnv*, jclass)
{
    tobii_api = make_unique<TobiiAPI>();

    if (tobii_api->api == nullptr)
    {
        return -1;
    }

    tobii_device = make_unique<TobiiDevice>(*tobii_api);

    if (tobii_device->device == nullptr)
    {
        return -2;
    }

    latest_gaze_point[0] = 0.5f;
    latest_gaze_point[1] = 0.5f;
    int error = tobii_gaze_point_subscribe(tobii_device->device, gaze_callback, NULL);
    if (error != TOBII_ERROR_NO_ERROR)
    {
        return -3;
    }

    thread([]
    {
        while (true)
        {
            tobii_device_process_callbacks(tobii_device->device);
            this_thread::sleep_for(milliseconds(16));
        }
    }).detach();

    return 0;
}

JNIEXPORT jfloatArray JNICALL Java_tobii_Tobii_jniGazePosition(JNIEnv* env, jclass)
{
    jfloatArray return_jfloat_array = env->NewFloatArray(2);
    jfloat return_jfloat_array_elements[2];
    return_jfloat_array_elements[0] = latest_gaze_point[0];
    return_jfloat_array_elements[1] = latest_gaze_point[1];
    env->SetFloatArrayRegion(return_jfloat_array, 0, 2, return_jfloat_array_elements);
    return return_jfloat_array;
}

