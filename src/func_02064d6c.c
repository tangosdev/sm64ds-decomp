typedef unsigned char u8;
typedef unsigned short u16;

extern void CpuCopy8(void *src, int dest, int size);

int func_02064d6c(int dest, u16 a, u8 b, int c, int d, void *p, int n)
{
    u8 kind = 5;
    u16 a16 = a;
    int pos;
    int len;
    CpuCopy8(&kind, dest, 1);
    pos = dest + 1;
    CpuCopy8(&a16, pos, 2);
    pos = (int)(((long long)(pos + 2)) & 0xFFFFFFFFFFFFFFFFLL);
    CpuCopy8(&b, pos, 1);
    pos = (int)(((long long)(pos + 1)) & 0xFFFFFFFFFFFFFFFFLL);
    CpuCopy8(&c, pos, 4);
    pos = (int)(((long long)(pos + 4)) & 0xFFFFFFFFFFFFFFFFLL);
    CpuCopy8(&d, pos, 4);
    len = (int)(((long long)n) & 0xFFFFFFFFFFFFFFFFLL);
    pos = (int)(((long long)(pos + 4)) & 0xFFFFFFFFFFFFFFFFLL);
    CpuCopy8(p, pos, len);
    return (pos + len) - dest;
}
