#pragma once

#include "tobii_headers.h"
#include "non_copyable.h"

class TobiiAPI : public NonCopyable
{
    public :

        TobiiAPI() :
            api(nullptr)
        {
            tobii_error_t code = tobii_api_create(&api, NULL, NULL);
            if (code != TOBII_ERROR_NO_ERROR)
            {
                api = nullptr;
            }
        }

        ~TobiiAPI()
        {
            if (api != nullptr)
            {
                tobii_api_destroy(api);
            }
        }

    public :

        tobii_api_t* api;
};
