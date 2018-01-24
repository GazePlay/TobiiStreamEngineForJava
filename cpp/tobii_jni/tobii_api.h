#pragma once

#include "non_copyable.h"

struct tobii_api_t;

class TobiiAPI : public NonCopyable
{
    public :

        TobiiAPI();

        ~TobiiAPI();

        tobii_api_t* get_api();

    private :

        tobii_api_t* api;
};
