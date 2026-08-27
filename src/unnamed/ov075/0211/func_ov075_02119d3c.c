extern void _ZN3G2x18SetBlendBrightnessEPVtts(volatile unsigned short*, unsigned short, unsigned short, volatile unsigned short*);

void func_ov075_02119d3c(void* c) {
    volatile unsigned short* reg = (volatile unsigned short*)0x4000050;
    *reg = 0;
    _ZN3G2x18SetBlendBrightnessEPVtts((volatile unsigned short*)0x4001050, 0x1d, 0, reg);
    *(unsigned char*)((char*)c + 0x283) = 0;
    *(unsigned char*)((char*)c + 0x284) = 0;
}
