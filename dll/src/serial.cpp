#include <cstddef>
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

void CSerial::read(void (*callback)(char*))
{
    if (NULL != callback) {
        char datas[]={0x43,0x4d,0x30,0x30,0x0f,0x0D};
        callback(datas);
    }
}

CSerial* __stdcall newCSerial()
{
    return new CSerial();
}

void __stdcall delCSerial(CSerial* pSerial)
{
    delete pSerial;
}

char __stdcall serialAdd(CSerial* pSerial, char a, char b)
{
    return pSerial->add(a, b);
}

void __stdcall serialRead(CSerial* pSerial, void (*callback)(char*))
{
    pSerial->read(callback);
}