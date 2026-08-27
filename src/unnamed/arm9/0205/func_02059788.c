extern unsigned short data_020a6430;
extern int data_020a6438;
extern int data_020a6434;
extern void func_02059700(void);
extern void func_02059624(int bit);
extern void _ZN3IRQ13SetIRQHandlerEjPFvvE(unsigned int bit, void (*handler)(void));
extern void _ZN3IRQ10EnableIRQsEj(unsigned int bits);

void func_02059788(void)
{
    if (data_020a6430 != 0)
        return;
    data_020a6430 = 1;
    func_02059624(0);
    (&data_020a6438)[0] = 0;
    (&data_020a6438)[1] = 0;
    *(volatile unsigned short *)0x4000102 = 0;
    *(volatile unsigned short *)0x4000100 = 0;
    *(volatile unsigned short *)0x4000102 = 0xc1;
    _ZN3IRQ13SetIRQHandlerEjPFvvE(8, func_02059700);
    _ZN3IRQ10EnableIRQsEj(8);
    data_020a6434 = 0;
}
