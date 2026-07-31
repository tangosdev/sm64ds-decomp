extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
extern int Sound_PlayIfNotActive(int a, int b, int c, int d);
extern void func_ov004_020b0aa0(int arg);

void func_ov006_020f088c(char *c, int i)
{
    int n = i * 0x18;
    unsigned short *ctr = (unsigned short *)(c + 0x47b2 + n);
    int *b;
    int *a;
    int v;
    *ctr = *ctr + 1;
    if (*ctr == 0x35)
        _ZN5Sound12PlayBank2_2DEj(0x1b9);
    if (i == 0) {
        if (*ctr <= 0x35) {
            int *p = (int *)(c + 0x47ac + n);
            *p = Sound_PlayIfNotActive(*p, 2, 0x1b8, 0);
        }
    }
    {
        unsigned short *q = (unsigned short *)(c + 0x47b0 + n);
        if (*q != 0) {
            *q = *q - 1;
            if ((short)*q < 0) *q = 0;
            return;
        }
    }
    b = (int *)(c + 0x47a8 + n);
    a = (int *)(c + 0x47a0 + n);
    *a = *a + *b;
    v = *a >> 12;
    if (i == 0) {
        *b = *b + 0x140;
        if (v > 0x80) return;
        *a = 0x80000;
        *(unsigned char *)(c + n + 0x47b4) = 0;
        *(unsigned char *)(c + n + 0x47b7) = 0;
        func_ov004_020b0aa0(0x1d);
        if (*(unsigned char *)(c + 0xc4) != 0) return;
        *(unsigned char *)(c + 0xc3) = 1;
        *(unsigned char *)(c + 0xc4) = 1;
        *(unsigned short *)(c + 0xc0) = 0;
    } else {
        *b = *b - 0x140;
        if (v < 0x80) return;
        *a = 0x80000;
        *(unsigned char *)(c + n + 0x47b4) = 0;
        *(unsigned char *)(c + n + 0x47b7) = 0;
        func_ov004_020b0aa0(0x1d);
        if (*(unsigned char *)(c + 0xc4) == 0) {
            *(unsigned char *)(c + 0xc3) = 1;
            *(unsigned char *)(c + 0xc4) = 1;
            *(unsigned short *)(c + 0xc0) = 0;
        }
    }
}
