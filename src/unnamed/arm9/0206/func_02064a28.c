extern void MI_CpuCopy8(const void *src, void *dst, unsigned int size);

int func_02064a28(int off, unsigned short v, unsigned char b) {
    unsigned char tag = 0xb;
    short val = v;
    int c1, c2;
    MI_CpuCopy8(&tag, (void *)off, 1);
    c1 = off + 1;
    MI_CpuCopy8(&val, (void *)c1, 2);
    c2 = c1 + 2;
    MI_CpuCopy8(&b, (void *)c2, 1);
    return c2 + 1 - off;
}
