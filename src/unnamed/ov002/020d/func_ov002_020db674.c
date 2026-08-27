extern int func_ov002_020d82f0(void* c);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, void* v);
extern void _ZN6Player11ChangeStateERNS_5StateE(char* c, void* st);
extern int data_ov002_02110094;

int func_ov002_020db674(char* c, int a1, int a2, int a3)
{
    if (func_ov002_020d82f0(c) == 0) return 0;
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c + 0x6d9), 0x1d, c + 0x74);
    *(short*)(c + 0x94) = a3;
    *(short*)(c + 0x8e) = a3 + 0x8000;
    *(int*)(c + 0x98) = a1;
    *(int*)(c + 0xa8) = a2;
    *(int*)(c + 0x674) = 0;
    c[0x6e3] = 4;
    c[0x6de] = 1;
    c[0x6df] = 0;
    _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_02110094);
    return 1;
}
