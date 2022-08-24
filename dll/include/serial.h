class CSerial
{
public:
    CSerial();
    ~CSerial();
    char add(char a, char b);
    void read(void (*callback)(char*));
};

extern "C"{
#define EXPORT __declspec(dllexport)
    EXPORT CSerial* __stdcall newCSerial();
    EXPORT void __stdcall delCSerial(CSerial* pSerial);
    EXPORT char __stdcall serialAdd(CSerial* pSerial, char a, char b);
    EXPORT void __stdcall serialRead(CSerial* pSerial, void (*callback)(char*));
}