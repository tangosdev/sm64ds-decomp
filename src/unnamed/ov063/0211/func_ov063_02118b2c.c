extern int func_ov063_0211a0dc(void *);
extern int func_0201267c(int, void *);
int func_ov063_02118b2c(char *c) {
    int r;
    *(int *)(c + 0x98) = 0x5000;
    r = func_ov063_0211a0dc(c);
    if (r == -1) {
        *(char *)(c + 0x5cc) = 2;
        return 2;
    }
    if (r == 1) {
        *(char *)(c + 0x5cc) = 3;
        return func_0201267c(0xc7, c + 0x74);
    }
    {
        unsigned x = *(unsigned short *)(c + 0x100);
        if (x > 0x64) {
            x = 1;
            *(char *)(c + 0x5cc) = 1;
        }
        return x;
    }
}
