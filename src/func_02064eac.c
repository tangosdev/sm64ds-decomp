extern void CpuCopy8(const void *src, void *dst, unsigned int size);

int func_02064eac(unsigned char *dst, unsigned short a1, unsigned char a2, unsigned short a3, unsigned short a4, unsigned short a5)
{
    unsigned char hdr = 3;
    unsigned short s = a1;
    unsigned char *p1, *p2, *p3, *p4, *p5;
    CpuCopy8(&hdr, dst, 1);
    p1 = dst + 1;
    CpuCopy8(&s, p1, 2);
    p2 = p1 + 2;
    CpuCopy8(&a2, p2, 1);
    p3 = p2 + 1;
    CpuCopy8(&a3, p3, 2);
    p4 = p3 + 2;
    CpuCopy8(&a4, p4, 2);
    p5 = p4 + 2;
    CpuCopy8(&a5, p5, 2);
    return (p5 + 2) - dst;
}
