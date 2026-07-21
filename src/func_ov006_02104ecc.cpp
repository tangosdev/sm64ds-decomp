//cpp
extern "C" void func_ov004_020b0aa0(int arg);
extern "C" void func_ov006_02106048(char* c);
extern "C" void _ZN5Sound12PlayBank2_2DEj(unsigned int);

extern "C" void func_ov006_02104ecc(char* c)
{
    int* a = (int*)(((int)c + 0x4660) & 0xFFFFFFFFFFFFFFFF);
    *a += *(int*)(c + 0x4668);
    int* b = (int*)(((int)c + 0x4668) & 0xFFFFFFFFFFFFFFFF);
    *b -= 0x400;
    if ((*(int*)(c + 0x4660) >> 12) > -0x40) return;
    int* e = (int*)(((int)c + 0x4cac) & 0xFFFFFFFFFFFFFFFF);
    (*e)++;
    *(unsigned char*)(c + 0x4675) = 4;
    *(int*)(c + 0x4660) = 0x10000;
    *(int*)(c + 0x4664) = 0x24000;
    func_ov004_020b0aa0(0x1d);
    if (*(unsigned char*)(c + 0xc4) == 0) {
        *(unsigned char*)(c + 0xc3) = 1;
        *(unsigned char*)(c + 0xc4) = 1;
        *(unsigned short*)(c + 0xc0) = 0;
    }
    *(unsigned char*)(c + 0x4fe3) = 1;
    *(unsigned char*)(c + 0x4677) = *(unsigned char*)(c + 0x4fde);
    *(unsigned short*)(c + 0x4670) = 0;
    func_ov006_02106048(c);
    _ZN5Sound12PlayBank2_2DEj(0x1fb);
}
