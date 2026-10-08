extern void func_02055780(void);
extern void func_020556d0(void);
extern void func_020554bc(void);

void func_0205583c(void)
{
    func_02055780();
    *(volatile unsigned int *)0x4000504 = 0;
    while (*(volatile unsigned int *)0x4000600 & 0x8000000)
        ;
    *(volatile unsigned short *)0x4000060 = 0;
    *(volatile unsigned int *)0x4000600 = 0;
    *(volatile unsigned int *)0x4000010 = 0;
    *(volatile unsigned short *)0x4000060 |= 0x2000;
    *(volatile unsigned short *)0x4000060 |= 0x1000;
    *(volatile unsigned short *)0x4000060 &= ~0x3002;
    *(volatile unsigned short *)0x4000060 = (*(volatile unsigned short *)0x4000060 & ~0x3000) | 0x10;
    *(volatile unsigned short *)0x4000060 &= 0xcffb;
    *(volatile unsigned int *)0x4000600 |= 0x8000;
    *(volatile unsigned int *)0x4000600 = (*(volatile unsigned int *)0x4000600 & ~0xc0000000) | 0x80000000;
    func_020556d0();
    *(volatile unsigned int *)0x4000350 = 0;
    *(volatile unsigned short *)0x4000354 = 0x7fff;
    *(volatile unsigned short *)0x4000356 = 0;
    *(volatile unsigned int *)0x4000358 = 0;
    *(volatile unsigned short *)0x400035c = 0;
    *(volatile unsigned short *)0x4000008 &= ~3;
    func_020554bc();
    *(volatile unsigned int *)0x40004a4 = 0x1f0080;
    *(volatile unsigned int *)0x40004a8 = 0;
    *(volatile unsigned int *)0x40004ac = 0;
}
