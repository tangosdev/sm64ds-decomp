extern int func_02052fdc(void);

int func_020531a4(int a)
{
    volatile unsigned short *ime = (volatile unsigned short *)0x4000208;
    volatile int *div = (volatile int *)0x40002b8;
    unsigned short saved;
    if (a <= 0) return 0;
    saved = *ime;
    *ime = 0;
    *(volatile unsigned short *)0x40002b0 = 1;
    div[0] = 0;
    div[1] = a;
    *ime;
    *ime = saved;
    return func_02052fdc();
}
