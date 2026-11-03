extern void CpuCopy8(const void *src, void *dst, unsigned int size);

int func_02064f54(int off, short b, int c, int d, int e)
{
    unsigned char tag = 2;
    short val = b;
    int c1, c2, c3, c4;
    CpuCopy8(&tag, (void *)off, 1);
    c1 = off + 1;
    CpuCopy8(&val, (void *)c1, 2);
    c2 = c1 + 2;
    CpuCopy8(&c, (void *)c2, 1);
    c3 = c2 + 1;
    CpuCopy8(&d, (void *)c3, 2);
    c4 = c3 + 2;
    CpuCopy8(&e, (void *)c4, 2);
    return c4 + 2 - off;
}
