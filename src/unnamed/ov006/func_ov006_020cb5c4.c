extern int data_ov006_021405c8[];
extern int data_ov006_021405c0;
extern short data_ov006_02140538;
extern void _Z14ApproachLinearRiii(int* p, int b, int c);
extern void _Z14ApproachLinearRsss(short* p, short b, short c);
extern int _ZN9Animation8FinishedEv(void* a);
extern void func_ov006_020cb528(char* c);
extern void func_ov006_020cc8c8(char* c);

void func_ov006_020cb5c4(char* c)
{
    _Z14ApproachLinearRiii((int*)(c + 0x20), data_ov006_021405c8[*(short*)(c + 0x52)], 0x4000);
    if (*(int*)(c + 0x1c) > 0)
        _Z14ApproachLinearRsss((short*)(c + 0x4a), 0x4000, 0x1000);
    else
        _Z14ApproachLinearRsss((short*)(c + 0x4a), -0x4000, 0x1000);
    *(int*)(c + 0x34) = 0;
    *(int*)(c + 0x38) = 0;
    if (*(int*)(c + 0xcc) != data_ov006_021405c0) return;
    if (_ZN9Animation8FinishedEv(c + 0xbc) == 0) return;
    if (*(short*)(c + 0x52) == data_ov006_02140538)
        func_ov006_020cb528(c);
    else
        func_ov006_020cc8c8(c);
}
