#pragma optimize_for_size on
extern void func_ov063_0211a964(char *c, int arg1);
extern void func_ov063_02119b1c(void *);

void func_ov063_02119a50(char *c) {
    *(unsigned short *)(((int)c + 0x5d4) & 0xFFFFFFFFFFFFFFFF) &= ~0x40;
    if (*(unsigned short *)(c + 0x100) >= 0x1e) {
        *(unsigned char *)(c + 0x5cc) = 1;
    } else {
        *(int *)(c + 0xa8) = 0;
        *(int *)(c + 0x98) = 0x7ccc;
        func_ov063_0211a964(c, 0);
    }
    func_ov063_02119b1c(c);
}
