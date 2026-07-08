//cpp
extern "C" {
void func_02054154(void);
void _ZN2GX16SetBankForSubOBJEt(unsigned short);
void func_02054430(int);
void _ZN3GXS15SetGraphicsModeEi(int);
extern unsigned char BOTTOM_SCREEN_RELATED;
void func_ov006_020e7508(void)
{
    *(volatile unsigned short *)0x4000304 &= ~0x8000;
    func_02054154();
    _ZN2GX16SetBankForSubOBJEt(8);
    func_02054430(4);
    *(volatile int *)0x4000064 = 0x80360010;
    _ZN3GXS15SetGraphicsModeEi(5);
    BOTTOM_SCREEN_RELATED |= 0x10;
    BOTTOM_SCREEN_RELATED &= ~4;
    *(volatile int *)0x4001000 = (*(volatile int *)0x4001000 & ~0x1f00) | (BOTTOM_SCREEN_RELATED << 8);
}
}
