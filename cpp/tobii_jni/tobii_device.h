#pragma once

#include "non_copyable.h"

class TobiiAPI;
struct tobii_device_t;

class TobiiDevice : public NonCopyable
{
    public :

        TobiiDevice(TobiiAPI& api);

        ~TobiiDevice();

        tobii_device_t* get_device();

        float* get_latest_gaze_point();

    private :

        tobii_device_t* device;

};
