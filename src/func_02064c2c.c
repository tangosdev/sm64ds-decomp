typedef unsigned char u8;
typedef unsigned short u16;

extern void func_0205a61c(void *src, int dest, int size);

int func_02064c2c(int dest, u16 a, u8 b, int c, int d, void *p, int n)
{
    u8 kind = 7;
    u16 a16 = a;
    int pos;
    int len;
    func_0205a61c(&kind, dest, 1);
    pos = dest + 1;
    func_0205a61c(&a16, pos, 2);
    pos = (int)(((long long)(pos + 2)) & 0xFFFFFFFFFFFFFFFFLL);
    func_0205a61c(&b, pos, 1);
    pos = (int)(((long long)(pos + 1)) & 0xFFFFFFFFFFFFFFFFLL);
    func_0205a61c(&c, pos, 4);
    pos = (int)(((long long)(pos + 4)) & 0xFFFFFFFFFFFFFFFFLL);
    func_0205a61c(&d, pos, 4);
    len = (int)(((long long)n) & 0xFFFFFFFFFFFFFFFFLL);
    pos = (int)(((long long)(pos + 4)) & 0xFFFFFFFFFFFFFFFFLL);
    func_0205a61c(p, pos, len);
    return (pos + len) - dest;
}
