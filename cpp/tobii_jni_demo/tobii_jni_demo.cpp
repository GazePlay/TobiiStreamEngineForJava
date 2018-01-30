#include <tobii_jni.h>
#include <iostream>

using namespace std;

int main()
{
    auto error_code = Java_tobii_Tobii_jniInit(NULL, NULL);
    cout << "Error code : " << error_code << endl;
    return 0;
}
