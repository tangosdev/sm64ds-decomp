extern void CpuCopy8(const void *src, void *dst, unsigned int size);

int func_02064e18(unsigned char *dst, unsigned short a1, unsigned char a2, unsigned int a3, unsigned int a4)
{
    unsigned char hdr = 4;
    unsigned short s = a1;
    unsigned char *p1, *p2, *p3, *p4;
    CpuCopy8(&hdr, dst, 1);
    p1 = dst + 1;
    CpuCopy8(&s, p1, 2);
    p2 = p1 + 2;
    CpuCopy8(&a2, p2, 1);
    p3 = p2 + 1;
    CpuCopy8(&a3, p3, 4);
    p4 = p3 + 4;
    CpuCopy8(&a4, p4, 4);
    return (p4 + 4) - dst;
}
