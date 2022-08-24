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

    DCB dcb;
    char settings[100];
    FillMemory(&dcb, sizeof(dcb), 0);
	dcb.DCBlength = sizeof(dcb);
	sprintf_s(settings, "%i,n,8,1", baud_rate);

	if (!BuildCommDCB(settings, &dcb) || !SetCommState(m_port, &dcb))
	    return false;								// Couldn't build the DCB. Usually a problem with the communications specification string.

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
    CSerial* serial = (CSerial*)lpParam;
    OVERLAPPED osRead = { 0 };
    int TIMEOUT = 500;
    bool fWaitingOnRead = false;
    DWORD dwReaded;

    const int buffer_size = 1;
    char buffer[buffer_size];


    while(serial->m_reading)
    {
        if (!fWaitingOnRead)
			{
				if (osRead.hEvent != NULL)
				{
					CloseHandle(osRead.hEvent);
					ZeroMemory(&osRead, sizeof(osRead));
				}

				osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
				if (osRead.hEvent == NULL)
					return 0;

				if (!ReadFile(serial->m_port, buffer, buffer_size, &dwReaded, &osRead))
				{
					if (GetLastError() != ERROR_IO_PENDING)			// Error in communications;
					{
						CloseHandle(osRead.hEvent);
						return 0;
					}
					else
						fWaitingOnRead = true;		//включает ожидание события чтения
				}
				else													// read completed immediately
				{
					if (dwReaded == buffer_size)
					{
					    serial->m_callback(buffer);
						//TODO post
						//PostMessage();
					}
					else
					{
					}
				}
			}

			if (fWaitingOnRead)
			{
				switch (WaitForSingleObject(osRead.hEvent, TIMEOUT))
				{
				case WAIT_OBJECT_0:					// Read completed.
					if (!GetOverlappedResult(serial->m_port, &osRead, &dwReaded, FALSE))		// Error in communications;
					{
						CloseHandle(osRead.hEvent);
						return 0;
					}
					else							// Read completed successfully.
					{
						if (dwReaded == buffer_size)
						{
						    serial->m_callback(buffer);
							//TODO post
						    //PostMessage();
						}
						else
						{

						}

						fWaitingOnRead = false;
					}
					break;

				case WAIT_TIMEOUT:
					break;

				default:
					CloseHandle(osRead.hEvent);
					return 0;
					break;
				}
			}
    }

    return 0;
}

// ===================== EXTERNAL METHODS =====================

CSerial* CreateSerial()
{
    return new CSerial();
}

void DeleteSerial(CSerial* pSerial)
{
    delete pSerial;
}

bool SerialOpen(CSerial* serial, int port_number, int baud_rate, void (*callback)(char*))
{
    return serial->Open(port_number, baud_rate, callback);
}

bool SerialConnected(CSerial* serial)
{
    return serial->Connected();
}

void SerialClose(CSerial* serial)
{
    serial->Close();
}

int SerialWrite(CSerial* serial, char* data, int size)
{
    return serial->Write(data, size);
}