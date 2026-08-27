extern void Copy48BytesFixed(int *src, int *dst);

void func_020553a4(int *m)
{
    *(volatile unsigned int *)0x4000400 = 0x17;
    Copy48BytesFixed(m, (int *)0x4000400);
}
