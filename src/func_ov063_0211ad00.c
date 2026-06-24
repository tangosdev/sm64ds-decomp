extern unsigned char func_ov063_021163d0(char *c);
extern int func_ov063_0211adb4(char *c);

int func_ov063_0211ad00(char *c)
{
    int r4;
    int v;
    int b = (int)(*(unsigned short*)(c + 0xc) == 0xd2);
    if (b != 0 && *(unsigned char*)(c + 0x5cf) != 0xf) {
        r4 = 0x2e6000;
    } else {
        v = *(unsigned char*)(c + 0x5cf);
        if (v == 9) r4 = 0x7fffffff;
        else r4 = 0x5dc000;
    }
    v = *(unsigned char*)(c + 0x5cf);
    if (v == 0xd || v == 1) {
        if (func_ov063_021163d0(c) != 0) return 1;
        return 0;
    }
    if (func_ov063_0211adb4(c) == 0) {
        if (*(int*)(c + 0x580) < r4) return 1;
    }
    return 0;
}
