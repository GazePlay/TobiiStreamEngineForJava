#include "tobii_device.h"
extern "C"
{
#include "tobii_headers.h"
}
#include "tobii_api.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <functional>

using namespace std;

namespace
{
    struct url_receiver_context_t
    {
        char** urls;
        int capacity;
        int count;
    };

    struct device_list_t
    {
        char** urls;
        int count;
    };
}

TobiiDevice::TobiiDevice(TobiiAPI& api) :
    device(nullptr)
{
    struct device_list_t list = { NULL, 0 };

    struct url_receiver_context_t url_receiver_context;
    url_receiver_context.count = 0;
    url_receiver_context.capacity = 16;
    url_receiver_context.urls = (char**)malloc(sizeof(char*) * (unsigned int)url_receiver_context.capacity);

    tobii_enumerate_local_device_urls(api.api, url_receiver, &url_receiver_context);

    list.urls = url_receiver_context.urls;
    list.count = url_receiver_context.count;
    int error = tobii_device_create(api.api, list.urls[0], &device);
    for (int i = 0; i < list.count; ++i)
        free(list.urls[i]);
    free(list.urls);
    if (error != TOBII_ERROR_NO_ERROR)
    {
        device = nullptr;
    }
}

TobiiDevice::~TobiiDevice()
{
    if (device != nullptr)
    {
        tobii_device_destroy(device);
    }
}

void TobiiDevice::subscribe_gaze_point_listener(function<void(const tobii_gaze_point_t*, void*)> gaze_callback)
{
    int error = tobii_gaze_point_subscribe(device, gaze_callback.target<void(*)(const tobii_gaze_point_t*, void*)>, NULL);
    if (error != TOBII_ERROR_NO_ERROR)
    {
     //   throw new
    }
}


void TobiiDevice::url_receiver(char const* url, void* user_data)
{
    struct url_receiver_context_t* context = (struct url_receiver_context_t*) user_data;

    if (context->count >= context->capacity)
    {
        context->capacity *= 2;
        char** urls = (char**)realloc(context->urls, sizeof(char*) * (unsigned int)context->capacity);
        if (!urls)
        {
            fprintf(stderr, "Allocation failed\n");
            return;
        }
        context->urls = urls;
    }

    size_t url_length = strlen(url) + 1;
    context->urls[context->count] = (char*)malloc(url_length);
    if (!context->urls[context->count])
    {
        fprintf(stderr, "Allocation failed\n");
        return;
    }
    memcpy(context->urls[context->count++], url, url_length);
}
