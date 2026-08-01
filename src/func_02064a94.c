extern void CpuCopy8(const void *src, void *dst, unsigned int size);

int func_02064a94(unsigned char *dst, unsigned short a1, unsigned char a2, const void *a3, unsigned char a4)
{
    unsigned char hdr = 10;
    unsigned short s = a1;
    unsigned char *p1, *p2, *p3, *p4;
    CpuCopy8(&hdr, dst, 1);
    p1 = dst + 1;
    CpuCopy8(&s, p1, 2);
    p2 = p1 + 2;
    CpuCopy8(&a2, p2, 1);
    p3 = p2 + 1;
    CpuCopy8(&a4, p3, 1);
    p4 = p3 + 1;
    CpuCopy8(a3, p4, 9);
    return (p4 + 9) - dst;
}
