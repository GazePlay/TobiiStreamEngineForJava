#include "tobii_api.h"
#include "tobii_headers.h"

using namespace std;

TobiiAPI::TobiiAPI() :
    api(nullptr)
{
    tobii_error_t code = tobii_api_create(&api, NULL, NULL);
    if (code != TOBII_ERROR_NO_ERROR)
    {
        api = nullptr;
        throw -1;
    }
}

tobii_api_t* TobiiAPI::get_api()
{
    return api;
}

TobiiAPI::~TobiiAPI()
{
    if (api != nullptr)
    {
        tobii_api_destroy(api);
    }
}
