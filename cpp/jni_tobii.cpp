#include "jni_tobii.h"

#include "tobii/tobii.h"
#include "tobii/tobii_streams.h"
#include "tobii/tobii_wearable.h"
#include "tobii/tobii_engine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <Windows.h>

static void gaze_callback(tobii_gaze_point_t const* gaze_point, void* user_data)
{
    tobii_gaze_point_t* gaze_point_storage = (tobii_gaze_point_t*)user_data;
    *gaze_point_storage = *gaze_point;
}

struct url_receiver_context_t
{
    char** urls;
    int capacity;
    int count;
};

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

struct device_list_t
{
    char** urls;
    int count;
};

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


static void free_device_list(struct device_list_t* list)
{
    for (int i = 0; i < list->count; ++i)
        free(list->urls[i]);

    free(list->urls);
}

static char const* select_device(struct device_list_t* devices)
{
    int tmp = 0, selection = 0;

    while (selection < 1 || selection > devices->count)
    {
        printf("\nSelect a device\n\n");
        for (int i = 0; i < devices->count; ++i) printf("%d. %s\n", i + 1, devices->urls[i]);
        if (scanf_s("%d", &selection) <= 0) while ((tmp = getchar()) != '\n' && tmp != EOF);
    }

    return devices->urls[selection - 1];
}

struct thread_context_t
{
    tobii_device_t* device;
    HANDLE reconnect_event;
    HANDLE timesync_event;
    HANDLE exit_event;
    volatile LONG is_reconnecting;
};

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

static void log_func(void* log_context, tobii_log_level_t level, char const* text)
{
    CRITICAL_SECTION* log_mutex = (CRITICAL_SECTION*)log_context;

    EnterCriticalSection(log_mutex);
    if (level == TOBII_LOG_LEVEL_ERROR)
        fprintf(stderr, "Logged error: %s\n", text);
    LeaveCriticalSection(log_mutex);
}

static tobii_device_t* device;
static struct thread_context_t thread_context;
static tobii_gaze_point_t latest_gaze_point;
static char* return_string;

JNIEXPORT jint JNICALL Java_jni_tobii_init(JNIEnv* env, jclass c)
{
    (void)(env);
    (void)(c);
    static CRITICAL_SECTION log_mutex;
    InitializeCriticalSection(&log_mutex);
    tobii_custom_log_t custom_log = { &log_mutex, log_func };

    tobii_api_t* api;
    tobii_error_t error = tobii_api_create(&api, NULL, &custom_log);
    if (error != TOBII_ERROR_NO_ERROR)
    {
        //fprintf(stderr, "Failed to initialize the Tobii Stream Engine API.\n");
        DeleteCriticalSection(&log_mutex);
        return -1;
    }

    struct device_list_t devices = list_devices(api);
    if (devices.count == 0)
    {
        //fprintf(stderr, "No stream engine compatible device(s) found.\n");
        free_device_list(&devices);
        tobii_api_destroy(api);
        DeleteCriticalSection(&log_mutex);
        return -2;
    }
    char const* selected_device = devices.count == 1 ? devices.urls[0] : select_device(&devices);
    printf("Connecting to %s.\n", selected_device);

    error = tobii_device_create(api, selected_device, &device);
    free_device_list(&devices);
    if (error != TOBII_ERROR_NO_ERROR)
    {
        //fprintf(stderr, "Failed to initialize the device with url %s.\n", selected_device);
        tobii_api_destroy(api);
        DeleteCriticalSection(&log_mutex);
        return -3;
    }

    latest_gaze_point.timestamp_us = 0LL;
    latest_gaze_point.validity = TOBII_VALIDITY_INVALID;
    // Start subscribing to gaze point data, in this sample we supply a tobii_gaze_point_t variable to store latest value.
    error = tobii_gaze_point_subscribe(device, gaze_callback, &latest_gaze_point);
    if (error != TOBII_ERROR_NO_ERROR)
    {
        //fprintf(stderr, "Failed to subscribe to gaze stream.\n");

        tobii_device_destroy(device);
        tobii_api_destroy(api);
        DeleteCriticalSection(&log_mutex);
        return -4;
    }

    // Create event objects used for inter thread signaling
    HANDLE reconnect_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    HANDLE timesync_event = CreateWaitableTimer(NULL, TRUE, NULL);
    HANDLE exit_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (reconnect_event == NULL || timesync_event == NULL || exit_event == NULL)
    {
        //fprintf(stderr, "Failed to create event objects.\n");

        tobii_device_destroy(device);
        tobii_api_destroy(api);
        DeleteCriticalSection(&log_mutex);
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
        //fprintf(stderr, "Failed to create reconnect_and_timesync thread.\n");

        tobii_device_destroy(device);
        tobii_api_destroy(api);
        DeleteCriticalSection(&log_mutex);
        return -6;
    }

    LARGE_INTEGER due_time;
    LONGLONG const timesync_update_interval = 300000000LL; // time sync every 30 s
    due_time.QuadPart = -timesync_update_interval;

    BOOL timer_error = SetWaitableTimer(thread_context.timesync_event, &due_time, 0, NULL, NULL, FALSE);
    if (timer_error == FALSE)
    {
        //fprintf(stderr, "Failed to schedule timer event.\n");

        tobii_device_destroy(device);
        tobii_api_destroy(api);
        DeleteCriticalSection(&log_mutex);
        return -7;
    }

    return_string = (char*) malloc(1024);
    return 1l;
}

JNIEXPORT jstring JNICALL Java_jni_tobii_gazePosition(JNIEnv* env, jclass c)
{
    (void)(c);

    sprintf(return_string, "init");

    if (!InterlockedCompareExchange(&thread_context.is_reconnecting, 0L, 0L))
    {
        tobii_error_t error = tobii_device_process_callbacks(device);

        if (error == TOBII_ERROR_CONNECTION_FAILED)
        {
            InterlockedExchange(&thread_context.is_reconnecting, 1L);
            SetEvent(thread_context.reconnect_event);
        }
        else if (error != TOBII_ERROR_NO_ERROR)
        {
            sprintf(return_string, "error");
        }

        if (latest_gaze_point.validity == TOBII_VALIDITY_VALID)
        {
            sprintf(return_string, "Gaze point: %f %f", (double)latest_gaze_point.position_xy[0], (double)latest_gaze_point.position_xy[1]);
        }

        else
        {
            sprintf(return_string, "invalid");
        }
    }
    return (*env)->NewStringUTF(env, return_string);
}

