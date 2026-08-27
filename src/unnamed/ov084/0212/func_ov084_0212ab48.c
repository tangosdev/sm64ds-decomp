extern void func_ov084_02129c9c(char *c);

void func_ov084_0212ab48(char *c)
{
    unsigned char *p;
    int b;
    func_ov084_02129c9c(c);
    b = (int)(*(unsigned short *)(c + 0xc) == 0xca);
    if (b != 0) {
        *(int *)(c + 0xa8) = (int)(((long long)*(int *)(c + 0xa8) * 0x1800 + 0x800) >> 0xc);
    }
    *(short *)(c + 0x45c) = *(short *)(c + 0x45a);
    p = (unsigned char *)(((int)c + 0x468));
    *p &= ~1;
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
}
