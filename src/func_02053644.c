typedef short s16;

int func_02053644(s16 *v)
{
    unsigned long long sq = (long long)(v[0] * v[0]);
    sq += (long long)(v[1] * v[1]);
    sq += (long long)(v[2] * v[2]);
    volatile unsigned short *ime = (volatile unsigned short *)0x4000208;
    unsigned short saved = *ime;
    *ime = 0;
    *(volatile unsigned short *)0x40002b0 = 1;
    *(volatile unsigned long long *)0x40002b8 = sq << 2;
    *ime;
    *ime = saved;
    while (*(volatile unsigned short *)0x40002b0 & 0x8000);
    return (*(volatile int *)0x40002b4 + 1) >> 1;
}
