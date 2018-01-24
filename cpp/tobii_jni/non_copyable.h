#pragma once

class NonCopyable
{
    public :

        NonCopyable() {}

        NonCopyable(const NonCopyable&) = delete;

        NonCopyable& operator=(const NonCopyable&) = delete;

        ~NonCopyable() {}
};
