#include <windows.h>

class CSerial
{
private:
    HANDLE	m_hPort;                //port handle
    HANDLE	m_hThread;              //thread handle
    bool	m_fReading;             //reading cycle exit flag
    void    (*m_pCallback)(char*);     //callback method to send read bytes
public:
    CSerial();
    ~CSerial();
    bool Open(int nPortNumber, int nBaudRate); //open port with nPortNumber and
    void read(void (*callback)(char*));
private:
    bool OpenPort();
    bool SetupConnection();
    bool StartReading();
    static DWORD WINAPI thread_func(LPVOID lpParam);  //read method in other thread
};

extern "C"{
    #define EXPORT __declspec(dllexport)
    EXPORT CSerial* __stdcall newCSerial();
    EXPORT void __stdcall delCSerial(CSerial* pSerial);
    EXPORT void __stdcall serialRead(CSerial* pSerial, void (*callback)(char*));
}