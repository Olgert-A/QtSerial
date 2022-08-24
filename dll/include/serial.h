#include <windows.h>

class CSerial
{
private:
    HANDLE	m_port;                //port handle
    HANDLE	m_thread;              //thread handle
    bool	m_reading;             //reading cycle exit flag
    void    (*m_callback)(char*);     //callback method to send read bytes
public:
    CSerial();
    ~CSerial();
    bool    Open(int port_number, int baud_rate, void (*callback)(char*));  //open port with port_number and baud_rate
    bool    Connected();                                                    //return status of connection
    void    Close();                                                        //close port
    int     Write(char* data, int size);                                    //write bytes to port
private:
    bool    CreateConnection(int port_number);                              //create connection to port
    bool    SetupConnection(int baud_rate);                                 //setup timeouts and [pr
    bool    StartReading();                                                 //start reading thread
    static DWORD WINAPI Read(LPVOID lpParam);                               //read method in other thread
};

extern "C"{
    #define EXPORT __declspec(dllexport)
    EXPORT CSerial* __stdcall CreateSerial();
    EXPORT void __stdcall DeleteSerial(CSerial* serial);
    EXPORT bool __stdcall SerialOpen()
}