extern void CpuCopy8(const void *src, void *dst, unsigned int size);

int func_02064a28(int off, short v, char b)
{
    unsigned char tag = 0xb;
    short val = v;
    int c1, c2;
    CpuCopy8(&tag, (void *)off, 1);
    c1 = off + 1;
    CpuCopy8(&val, (void *)c1, 2);
    c2 = c1 + 2;
    CpuCopy8(&b, (void *)c2, 1);
    return c2 + 1 - off;
}
