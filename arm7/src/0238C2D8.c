extern int dword_38098C4;
extern int dword_38098CC[];
extern int fn_023808F8(int);
extern void fn_02388CD8(int);
extern int fn_0238C1C8(int);
extern void fn_0238BD84(int);

int fn_0238C2D8(void) {
    if (dword_38098C4 != 0)
        return (int)&dword_38098C4;
    dword_38098C4 = 1;
    int bit = *(volatile unsigned short *)0x04000300 & 1;
    if (bit == 0)
        return bit;
    int saved = fn_023808F8(0x40000);
    volatile unsigned short *reg = (volatile unsigned short *)0x04000208;
    unsigned short old = *reg;
    *reg = 1;
    while (dword_38098CC[4] != 1)
        fn_02388CD8(0x100);
    unsigned int bits = dword_38098CC[0];
    unsigned int addr = (bits & 0x01FFFFC0) >> 6;
    addr <<= 5;
    addr = (unsigned int)((long long)(addr + 0x2000000) & 0xFFFFFFFFFFFFFFFFLL);
    addr = (unsigned int)((long long)(addr + 4) & 0xFFFFFFFFFFFFFFFFLL);
    int got = fn_0238C1C8(addr);
    unsigned char *byte = (unsigned char *)0x027FFC30;
    byte = (unsigned char *)(unsigned)((long long)(byte + 5) & 0xFFFFFFFFFFFFFFFFLL);
    *byte = (*byte & ~1) | ((unsigned char)got & 1);
    fn_0238BD84(1);
    (void)*reg;
    *reg = old;
    return fn_023808F8(saved);
}
