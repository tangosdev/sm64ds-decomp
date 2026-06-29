extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
extern void func_ov004_020b0aa0(int arg);

void func_ov006_020f05d8(char *c, int i)
{
    int n = i * 0x18;
    unsigned short *ctr = (unsigned short *)(c + 0x47b2 + n);
    int *b;
    int *a;
    int v;
    *ctr = *ctr + 1;
    if (*ctr == 0x1a)
        _ZN5Sound12PlayBank2_2DEj(0x1b9);
    b = (int *)(c + 0x47a8 + n);
    a = (int *)(c + 0x47a0 + n);
    *a = *a + *b;
    v = *a >> 12;
    if (i == 0) {
        *b = *b - 0x140;
        if (v < 0x80) return;
        if (*b <= 0) return;
        *a = 0x80000;
        *(unsigned char *)(c + n + 0x47b4) = 0;
        *(unsigned char *)(c + n + 0x47b7) = 0;
        func_ov004_020b0aa0(0xd);
    } else {
        *b = *b + 0x140;
        if (v > 0x80) return;
        if (*b >= 0) return;
        *a = 0x80000;
        *(unsigned char *)(c + n + 0x47b4) = 0;
        *(unsigned char *)(c + n + 0x47b7) = 0;
    }
}
