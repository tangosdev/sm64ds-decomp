extern int func_02064b2c(int off, unsigned short b, int c, int d);

int func_02063da0(char *base, int idx, int c) {
    char *e;
    unsigned short bit;
    int r;
    e = base + 0x1d4 + idx * 0x68;
    bit = (unsigned short)(1 << idx);
    r = 0;
    if (*(unsigned char *)e == 2) {
        if (*(unsigned char *)(e + 1) == 8) {
            r = func_02064b2c(c, bit, *(unsigned char *)(e + 2), *(int *)(e + 0x10));
            *(unsigned char *)e = 1;
        }
    }
    return r;
}
