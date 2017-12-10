#include "tobii_jni.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "tobii/tobii.h"
#include "tobii/tobii_streams.h"
#include "tobii/tobii_wearable.h"
#include "tobii/tobii_engine.h"

#ifdef __cplusplus
}
#endif

#include <Windows.h>

#include <string>
#include <sstream>

using namespace std;

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
/*
struct thread_context_t
{
    tobii_device_t* device;
    HANDLE reconnect_event;
    HANDLE timesync_event;
    HANDLE exit_event;
    volatile LONG is_reconnecting;
};
*/
//static tobii_device_t* device;

//static thread_context_t thread_context;

static tobii_gaze_point_t latest_gaze_point;

static void gaze_callback(const tobii_gaze_point_t* gaze_point, void*)
{
    latest_gaze_point = *gaze_point;
}

static void url_receiver(char const* url, void* user_data)
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

static struct device_list_t list_devices(tobii_api_t* api)
{
    struct device_list_t list = { NULL, 0 };

    struct url_receiver_context_t url_receiver_context;
    url_receiver_context.count = 0;
    url_receiver_context.capacity = 16;
    url_receiver_context.urls = (char**)malloc(sizeof(char*) * (unsigned int)url_receiver_context.capacity);
    if (!url_receiver_context.urls)
    {
        fprintf(stderr, "Allocation failed\n");
        return list;
    }

    tobii_error_t error = tobii_enumerate_local_device_urls(api, url_receiver, &url_receiver_context);
    if (error != TOBII_ERROR_NO_ERROR) fprintf(stderr, "Failed to enumerate devices.\n");

    list.urls = url_receiver_context.urls;
    list.count = url_receiver_context.count;
    return list;
}

static void free_device_list(device_list_t* list)
{
    for (int i = 0; i < list->count; ++i)
        free(list->urls[i]);

    free(list->urls);
}
/*
static DWORD WINAPI reconnect_and_timesync_thread(LPVOID param)
{
    struct thread_context_t* context = (struct thread_context_t*) param;

    HANDLE objects[3];
    objects[0] = context->reconnect_event;
    objects[1] = context->timesync_event;
    objects[2] = context->exit_event;

    for (; ; )
    {
        // Block here, waiting for one of the three events.
        DWORD result = WaitForMultipleObjects(3, objects, FALSE, INFINITE);
        if (result == WAIT_OBJECT_0) // Handle reconnect event
        {
            tobii_error_t error;
            // Run reconnect loop until connection succeeds or the exit event occurs
            while ((error = tobii_device_reconnect(context->device)) != TOBII_ERROR_NO_ERROR)
            {
                if (error != TOBII_ERROR_CONNECTION_FAILED)
                    fprintf(stderr, "Reconnection failed: %s.\n", tobii_error_message(error));
                // Blocking waiting for exit event, timeout after 250 ms and retry reconnect
                result = WaitForSingleObject(context->exit_event, 250); // Retry every quarter of a second
                if (result == WAIT_OBJECT_0)
                    return 0; // exit thread
            }
            InterlockedExchange(&context->is_reconnecting, 0L);
        }
        else if (result == WAIT_OBJECT_0 + 1) // Handle timesync event
        {
            tobii_error_t error = tobii_update_timesync(context->device);
            LARGE_INTEGER due_time;
            LONGLONG const timesync_update_interval = 300000000LL; // Time sync every 30 s
            LONGLONG const timesync_retry_interval = 1000000LL; // Retry time sync every 100 ms
            due_time.QuadPart = (error == TOBII_ERROR_NO_ERROR) ? -timesync_update_interval : -timesync_retry_interval;
            // Re-schedule timesync event
            SetWaitableTimer(context->timesync_event, &due_time, 0, NULL, NULL, FALSE);
        }
        else if (result == WAIT_OBJECT_0 + 2) // Handle exit event
        {
            // Exit requested, exiting the thread
            return 0;
        }
    }
}
*/
JNIEXPORT jint JNICALL Java_tobii_Tobii_jniInit(JNIEnv*, jclass)
{
    tobii_api_t* api;

    tobii_error_t error = tobii_api_create(&api, NULL, NULL);
    if (error != TOBII_ERROR_NO_ERROR)
    {
        return -1;
    }

    struct device_list_t devices = list_devices(api);
    if (devices.count == 0)
    {
        free_device_list(&devices);
        tobii_api_destroy(api);
        return -2;
    }
    const char* selected_device = devices.urls[0];
    tobii_device_t* device;

    error = tobii_device_create(api, selected_device, &device);
    free_device_list(&devices);
    if (error != TOBII_ERROR_NO_ERROR)
    {
        tobii_api_destroy(api);
        return -3;
    }

    latest_gaze_point.timestamp_us = 0;
    latest_gaze_point.validity = TOBII_VALIDITY_INVALID;
    latest_gaze_point.position_xy[0] = 0;
    latest_gaze_point.position_xy[1] = 0;
    error = tobii_gaze_point_subscribe(device, gaze_callback, NULL);
    if (error != TOBII_ERROR_NO_ERROR)
    {
        tobii_device_destroy(device);
        tobii_api_destroy(api);
        return -4;
    }
/*
    HANDLE reconnect_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    HANDLE timesync_event = CreateWaitableTimer(NULL, TRUE, NULL);
    HANDLE exit_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (reconnect_event == NULL || timesync_event == NULL || exit_event == NULL)
    {
        tobii_device_destroy(device);
        tobii_api_destroy(api);
        return -5;
    }

    thread_context.device = device;
    thread_context.reconnect_event = reconnect_event;
    thread_context.timesync_event = timesync_event;
    thread_context.exit_event = exit_event;
    thread_context.is_reconnecting = 0;

    HANDLE thread_handle = CreateThread(NULL, 0U, reconnect_and_timesync_thread, &thread_context, 0U, NULL);
    if (thread_handle == NULL)
    {
        tobii_device_destroy(device);
        tobii_api_destroy(api);
        return -6;
    }

    LARGE_INTEGER due_time;
    LONGLONG const timesync_update_interval = 300000000LL; // time sync every 30 s
    due_time.QuadPart = -timesync_update_interval;

    BOOL timer_error = SetWaitableTimer(thread_context.timesync_event, &due_time, 0, NULL, NULL, FALSE);
    if (timer_error == FALSE)
    {
        tobii_device_destroy(device);
        tobii_api_destroy(api);
        return -7;
    }
*/
    return 0;
}

JNIEXPORT jfloatArray JNICALL Java_tobii_Tobii_jniGazePosition(JNIEnv* env, jclass)
{
    jfloatArray return_jfloat_array = env->NewFloatArray(2);
    jfloat return_jfloat_array_elements[2];
    return_jfloat_array_elements[0] = latest_gaze_point.position_xy[0];
    return_jfloat_array_elements[1] = latest_gaze_point.position_xy[1];
    env->SetFloatArrayRegion(return_jfloat_array, 0, 2, return_jfloat_array_elements);
    return return_jfloat_array;
}

