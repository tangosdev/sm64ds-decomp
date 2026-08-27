extern short _Z15ApproachLinear2Rsss(short* a, short b, short c);
extern void _Z14ApproachLinearRiii(int* a, int b, int c);
extern void func_ov006_020eeef4(char* c);
extern void func_0203d630(int* p, int m);

void func_ov006_020eedc8(char* c) {
    if (*(short*)(c + 0x20) == 0) return;
    if (_Z15ApproachLinear2Rsss((short*)(c + 0x1c), 0, 1) != 0) {
        func_ov006_020eeef4(c);
        *(short*)(c + 0x20) = 0;
    }
    _Z14ApproachLinearRiii((int*)(c + 4), *(int*)(c + 0xc), *(int*)(c + 0x14));
    _Z14ApproachLinearRiii((int*)(c + 8), *(int*)(c + 0x10), *(int*)(c + 0x18));
    func_0203d630((int*)(c + 0x14), 0xf00);
}
