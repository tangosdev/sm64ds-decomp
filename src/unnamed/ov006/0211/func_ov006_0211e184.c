#define LI(i) ((int)((long long)(i)) * 0x10)
#define A(p) ((int)(p))

void func_ov006_0211e184(char *base)
{
    int i;

    for (i = 0; i < 0x10; i++) {
        unsigned char *fp = (unsigned char *)A(base + LI(i) + 0x496d);
        unsigned short *c16;
        unsigned char *c8;
        if (*fp == 0) continue;
        c16 = (unsigned short *)A(base + LI(i) + 0x4968);
        *c16 += 1;
        if (*c16 < 4) continue;
        *c16 = 0;
        c8 = (unsigned char *)A(base + LI(i) + 0x496c);
        *c8 += 1;
        if (*c8 >= 5) {
            *c8 = 0;
            *(unsigned char *)(base + LI(i) + 0x496e) = 0;
            *fp = 0;
        }
    }
}
