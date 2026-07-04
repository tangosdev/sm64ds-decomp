//cpp
extern "C" {
extern int data_ov002_021102a4[];
extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
int func_ov002_020c6adc(char* c)
{
    unsigned short v = (unsigned short)(*(unsigned short*)(c + 0x6ce) & 8);
    if (v == 0)
        return 0;
    *(unsigned short*)((((long long)(int)(c + 0x6ce)) & 0xFFFFFFFFFFFFFFFFLL)) &= ~8;
    _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021102a4);
    return 1;
}
}
