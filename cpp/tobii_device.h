#pragma once

#include "non_copyable.h"

class TobiiAPI;
class tobii_device_t;

class TobiiDevice : public NonCopyable
{
    public :

        TobiiDevice(TobiiAPI& api);

        ~TobiiDevice();

        void subscribe_gaze_point_listener();

        static void url_receiver(char const* url, void* user_data);

    public :

        tobii_device_t* device;
};
