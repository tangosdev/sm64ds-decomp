extern int func_0200e728(unsigned char *p);
extern void _Z14ApproachLinearRiii(int *ref, int a, int b);

int func_02007fcc(void *self, const void *src)
{
    int v1 = func_0200e728((unsigned char *)src);
    int v2 = func_0200e728((unsigned char *)src + 4);
    _Z14ApproachLinearRiii((int *)((char *)self + 0x170), v1, v2);
    return 1;
}
