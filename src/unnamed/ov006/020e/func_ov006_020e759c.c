extern void func_02054140(void);
extern void func_02054430(int);
extern void _ZN2GX15SetBankForSubBGEt(unsigned short);
extern unsigned char data_0209d454;

void func_ov006_020e759c(void) {
    *(volatile unsigned short *)0x4000304 |= 0x8000;
    func_02054140();
    _ZN2GX15SetBankForSubBGEt(4);
    func_02054430(8);
    *(volatile unsigned int *)0x4000064 = 0x80330010;
    data_0209d454 &= ~0x10;
    data_0209d454 |= 4;
    *(volatile unsigned int *)0x4001000 =
        (*(volatile unsigned int *)0x4001000 & ~0x1f00) | (data_0209d454 << 8);
    *(volatile unsigned short *)0x400100c =
        (*(volatile unsigned short *)0x400100c & 0x43) | 0x4284;
    *(volatile unsigned short *)0x400100c &= ~3;
    *(volatile unsigned short *)0x400100c &= ~0x40;
}
