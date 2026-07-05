extern int _Z15ApproachLinear2Rsss(short* dst, short to, short step);
extern void func_0203d630(int* p, int m);

void func_ov004_020adeb0(char* c)
{
    _Z15ApproachLinear2Rsss((short*)(c + 0x1a), 0, 1);
    func_0203d630((int*)(c + 8), 0xff8);
    int *p = (int*)(((int)c + 8) & 0xFFFFFFFFFFFFFFFF);
    int *p2 = (int*)(((int)c + 0xc) & 0xFFFFFFFFFFFFFFFF);
    *p += *(int*)(c + 0x10);
    *p2 += *(int*)(c + 0x14);
    p = (int*)(((int)c + 4) & 0xFFFFFFFFFFFFFFFF);
    *(int*)c += *(int*)(c + 8);
    *p += *(int*)(c + 0xc);
}
