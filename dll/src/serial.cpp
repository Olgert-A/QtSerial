#include "serial.h"

extern "C"
{
    __declspec(dllexport) char add(char a, char b)
    {
        return a + b;
    }
}