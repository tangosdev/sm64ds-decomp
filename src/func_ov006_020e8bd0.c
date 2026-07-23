//cpp
extern "C" void _ZN3G2x13SetBlendAlphaEPVttttt(
    volatile unsigned short *p, int a, int b, int c, int d);

extern "C" void func_ov006_020e8bd0(char *c, int i)
{
    int off = i * 0x14;
    char *b = c + 0x5288;
    int x = *(int *)(b + off);
    x = x - (x >> 7);
    *(int *)(b + off) = x;
    *(unsigned char *)(((int)c + 0x554e) & 0xFFFFFFFFFFFFFFFF) += 1;
    {
        unsigned char ip = *(unsigned char *)(c + 0x554e);
        _ZN3G2x13SetBlendAlphaEPVttttt(
            (volatile unsigned short *)0x4001050, 0, 4,
            0x10 - ip, ip);
    }
    if (*(unsigned char *)(c + 0x554e) < 0x10)
        return;
    *(unsigned char *)(c + 0x554e) = 0;
    *(unsigned char *)(c + off + 0x5292) = 0;
    *(unsigned char *)(c + off + 0x5290) = 0;
}
