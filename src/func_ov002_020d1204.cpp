//cpp
struct State;
extern "C" void _ZN6Player11ChangeStateERNS_5StateE(void*, State&);
extern unsigned char data_020a0e40[];
extern unsigned char data_0209f49e[];
extern State _ZN6Player12ST_BACK_FLIPE;
extern State _ZN6Player12ST_LONG_JUMPE;
extern State _ZN6Player7ST_JUMPE;

extern "C" int func_ov002_020d1204(void* thisptr)
{
    unsigned char* p = (unsigned char*)thisptr;
    if (*(unsigned short*)(data_0209f49e + data_020a0e40[0] * 0x18) & 2) {
        int v = *(int*)(p + 0x98);
        if (v == 0) {
            _ZN6Player11ChangeStateERNS_5StateE(thisptr, _ZN6Player12ST_BACK_FLIPE);
        } else if (*(unsigned short*)(p + 0x600 + 0xa4) != 0 && v > 0xa000) {
            _ZN6Player11ChangeStateERNS_5StateE(thisptr, _ZN6Player12ST_LONG_JUMPE);
            return 1;
        } else {
            _ZN6Player11ChangeStateERNS_5StateE(thisptr, _ZN6Player7ST_JUMPE);
        }
        return 1;
    }
    return 0;
}
