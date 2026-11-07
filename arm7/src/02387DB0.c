extern void fn_02387F00(unsigned int, unsigned int);

int fn_02387DB0(void)
{
    volatile int *v0;
    fn_02387F00(0xB8000000, 0);
    *(volatile int *)0x40001A4 = (*(int *)0x27FFE60 & ~0x07000000) | 0xA7000000;
    v0 = (volatile int *)0x40001A4;
    while ((*v0 & 0x800000) == 0)
        ;
    return *(int *)0x4100010;
}
