struct S;

extern int func_0200e728(unsigned char *p);

int func_02008008(struct S *p, int v)
{
    int r0 = func_0200e728((unsigned char *)v);
    *(int *)((char *)p + 0x170) = r0;
    return 1;
}
