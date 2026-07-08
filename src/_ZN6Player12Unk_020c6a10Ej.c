typedef unsigned int u32;
typedef unsigned char u8;

extern int _ZN6Player7IsStateERNS_5StateE(void* thiz, void* st);
extern int func_ov002_020d82f0(void* c);
extern int func_ov002_020d91e0(void* thiz, u32 a, int b);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, void* v);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* thiz, void* st);
extern char _ZN6Player9ST_SQUISHE;

int _ZN6Player12Unk_020c6a10Ej(char* c, u32 arg)
{
    int flag;

    if (!(*(u8*)(c + 0x6e9) & 1)) return 0;
    if (_ZN6Player7IsStateERNS_5StateE(c, &_ZN6Player9ST_SQUISHE)) return 0;
    if (!func_ov002_020d82f0(c)) return 0;

    flag = 1;
    if (*(u8*)(c + 0x6f9) == 0) {
        if (func_ov002_020d91e0(c, arg << 8, 1))
            flag = 0;
    }

    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8*)(c + 0x6d9), 7, c + 0x74);
    _ZN6Player11ChangeStateERNS_5StateE(c, &_ZN6Player9ST_SQUISHE);
    *(int*)(c + 0x84) = 0x100;
    *(u8*)(c + 0x6e3) = 4;
    *(short*)(c + 0x6a4) = 0x1e;
    *(u8*)(c + 0x70c) = flag;
    *(u8*)(c + 0x6f2) = 0;
    return 1;
}
