extern void Copy48BytesFixed(const void *src, void *dst);

void func_02055388(int *m)
{
    *(volatile unsigned int *)0x4000400 = 0x19;
    Copy48BytesFixed(m, (int *)0x4000400);
}
