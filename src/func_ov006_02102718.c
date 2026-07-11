extern unsigned func_02054de8(void);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);

void func_ov006_02102718(char* c)
{
    volatile unsigned short tmp;
    int n = *(unsigned char*)(c + 0x5676);
    int a, b;
    char* base;

    if (n == 0) return;

    base = c + (n - 1) * 0x40;
    a = 0x80 - (*(int*)(base + 0x4660) >> 12);
    b = 0x20 - (*(int*)(base + 0x4664) >> 12);

    if (a < -6) return;
    if (a > 6) return;
    if (b < -6) return;
    if (b > 6) return;
    if (*(unsigned char*)(base + 0x4699) != 2) return;

    *(unsigned char*)(c + 0x5676) = 0;
    {
        char* dst = (char*)func_02054de8();
        tmp = 0;
        MultiStore16(tmp, dst, 0x6000);
    }
}
