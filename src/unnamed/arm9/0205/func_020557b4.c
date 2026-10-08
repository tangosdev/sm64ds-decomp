extern void func_02055624(void);

void func_020557b4(void)
{
    while (*(volatile unsigned int *)0x4000600 & 0x8000000);
    *(volatile unsigned int *)0x4000600 |= 0x8000;
    *(volatile unsigned short *)0x4000060 |= 0x2000;
    *(volatile unsigned short *)0x4000060 |= 0x1000;
    func_02055624();
    *(volatile unsigned int *)0x40004a4 = 0x1f0080;
    *(volatile unsigned int *)0x40004a8 = 0;
    *(volatile unsigned int *)0x40004ac = 0;
}
