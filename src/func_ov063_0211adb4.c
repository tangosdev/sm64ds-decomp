extern unsigned char func_ov063_021163d0(char *c);
int func_ov063_0211adb4(char *c) {
    unsigned char v = *(unsigned char*)(c + 0x5cf);
    if (v == 0xd || v == 1) {
        return func_ov063_021163d0(c) == 0 ? 1 : 0;
    }
    return 0;
}
