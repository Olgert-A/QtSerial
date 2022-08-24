#include <windows.h>

class CSerial
{
private:
    HANDLE	m_hThread;

public:
    void (*m_pCallback)(char*);
    CSerial();
    ~CSerial();
    void read(void (*callback)(char*));
    static DWORD WINAPI thread_func(LPVOID lpParam);
};

extern "C"{
#define EXPORT __declspec(dllexport)
    EXPORT CSerial* __stdcall newCSerial();
    EXPORT void __stdcall delCSerial(CSerial* pSerial);
    EXPORT void __stdcall serialRead(CSerial* pSerial, void (*callback)(char*));
}