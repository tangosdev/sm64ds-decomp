extern void Copy36Bytes(int *src, int *dst);

void func_0205532c(int *a, int *v)
{
    *(volatile unsigned int *)0x4000400 = 0x19;
    Copy36Bytes(a, (int *)0x4000400);
    *(volatile unsigned int *)0x4000400 = v[0];
    *(volatile unsigned int *)0x4000400 = v[1];
    *(volatile unsigned int *)0x4000400 = v[2];
}
