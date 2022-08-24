#include <cstddef>
#include "windows.h"
#include <stdio.h>
#include "serial.h"


CSerial::CSerial()
{
    m_port = NULL;
    m_thread = NULL;
    m_callback = NULL;
    m_reading = false;
}

CSerial::~CSerial()
{
    Close();
}

bool CSerial::Open(int port_number, int baud_rate, void (*callback)(char*))
{
    //check parameters
    if (port_number < 0 || baud_rate < 0 || callback == NULL)
        return false;

    //close port if already opened
    if (m_port != NULL)
        Close();

    //close and return false if anything wrong
    if (!CreateConnection(port_number) || !SetupConnection(baud_rate) || !StartReading())
    {
        Close();
        return false;
    }

    //save read callback
    m_callback = callback;
    return true;
}

bool CSerial::Connected()
{
    if (m_port != NULL && m_thread != NULL && m_reading && m_callback != NULL)
        return true;
    return false;
}

void CSerial::Close()
{
    if (m_thread != NULL)
	{
	    m_reading = false;
		WaitForSingleObject(m_thread, INFINITE);
		CloseHandle(m_thread);
		m_thread = NULL;
	}
	if (m_port != NULL)
	{
	    CloseHandle(m_port);
	    m_port = NULL;
	}
	m_callback = NULL;
}

int CSerial::Write(char* data, int size)
{
    return 0;
}

//================= private =======================

bool CSerial::CreateConnection(int port_number)
{
    char port_name[100];
    sprintf_s(port_name, "\\\\.\\COM%i", port_number);

	m_port = CreateFile(	port_name,
							GENERIC_READ | GENERIC_WRITE,
							0,
							NULL,
							OPEN_EXISTING,
							FILE_FLAG_OVERLAPPED,
							NULL);

	if (m_port == INVALID_HANDLE_VALUE)
	{
		m_port = NULL;
		return false;
	}
	return true;
}

bool CSerial::SetupConnection(int baud_rate)
{
    if (m_port == NULL )
        return false;

    //set port settings
    DCB dcb;
	FillMemory(&dcb, sizeof(dcb), 0);		//set 0 to dcb
	if (!GetCommState(m_port, &dcb))		// get current DCB
	    return false;						// Error in GetCommState

    dcb.BaudRate = baud_rate;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = 1;

	if (!SetCommState(m_port, &dcb))
		return false;

    // set timeouts
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = 20;					//устанавливает максимальный интервал между приходом двух байт (начинает работать после прихода 1 байта пакета чтения)
	timeouts.ReadTotalTimeoutMultiplier = 10;			//устанавливает множитель общего интервала чтения (это число умножается на количество байт для чтения)
	timeouts.ReadTotalTimeoutConstant = 100;			//устанавливает константу общего интервала чтения (это число прибавляется к произведению множителя на число байт для чтения)
																//если сработал один из двух таймаутов, событие чтение overlapped устанавливается, показывая, что операция чтения завершилась
	timeouts.WriteTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 100;

	if (!SetCommTimeouts(m_port, &timeouts))
		return false;

	return true;
}

bool CSerial::StartReading()
{
    m_reading = true;
	m_thread = CreateThread(
        NULL,
        0,
        Read,
        static_cast<LPVOID>(this),
        0,
        NULL);

	if (m_thread == NULL)
	{
		m_reading = false;
		return false;
	}

    return true;
}


DWORD WINAPI CSerial::Read(LPVOID lpParam)
{
    CSerial* pSerial = (CSerial*)lpParam;

    char datas[]={0x43,0x4d,0x30,0x35,0x0f,0x0D};
    for (int i=0; i<6; i++)
    {
        char item[]={datas[i]};
        pSerial->m_callback(item);
        Sleep(1000);
    }

    return 0;
}



CSerial* __stdcall newCSerial()
{
    return new CSerial();
}

void __stdcall delCSerial(CSerial* pSerial)
{
    delete pSerial;
}