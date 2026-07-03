extern char *func_ov010_0211139c(char *c);

#define LAUNDER_ADDR(x) ((int)(((long long)(int)(x)) & 0xffffffffffffffffLL))

void func_ov010_021112b4(char *c)
{
    char *p = func_ov010_0211139c(c);
    short *a;
    short *b;
    int limit;

    if (p == 0) return;

    *(unsigned char *)(p + 0x3aa) = 0;
    a = (short *)LAUNDER_ADDR(c + 0x3a8);
    b = (short *)LAUNDER_ADDR(c + 0x90);
    limit = 0x3d00;
    *a = *a - 0x100;
    *b = *b + *(volatile short *)(c + 0x300 + 0xa8);

    if (*(volatile short *)(c + 0x90) < -limit) {
        *(short *)(c + 0x90) = -limit;
        *(int *)(c + 0x3a0) = 2;
    }
}
