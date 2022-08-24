#include <cstddef>
#include "windows.h"
#include "serial.h"


CSerial::CSerial()
{
    m_hThread = NULL;
}

CSerial::~CSerial()
{
    if (m_hThread != NULL)
	{
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}

DWORD WINAPI CSerial::thread_func(LPVOID lpParam)
{
    CSerial* pSerial = (CSerial*)lpParam;

    char datas[]={0x43,0x4d,0x30,0x35,0x0f,0x0D};
    for (int i=0; i<6; i++)
    {
        char item[]={datas[i]};
        pSerial->m_pCallback(item);
        Sleep(1000);
    }

    return 0;
}

void CSerial::read(void (*callback)(char*))
{
    if (NULL != callback) {
        m_pCallback = callback;

        m_hThread = CreateThread(
            NULL,
            0,
            thread_func,
            static_cast<LPVOID>(this),
            0,
            NULL);
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

void __stdcall serialRead(CSerial* pSerial, void (*callback)(char*))
{
    pSerial->read(callback);
}