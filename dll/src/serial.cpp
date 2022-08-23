#include "serial.h"

CSerial::CSerial()
{

}

CSerial::~CSerial()
{

}

char CSerial::add(char a, char b)
{
    return a + b;
}

CSerial* __stdcall newCSerial()
{
    return new CSerial();
}

void __stdcall delCSerial(CSerial* pSerial)
{
    pSerial->~CSerial();
}
char __stdcall serialAdd(CSerial* pSerial, char a, char b)
{
    return pSerial->add(a, b);
}