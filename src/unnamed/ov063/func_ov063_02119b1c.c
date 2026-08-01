extern int func_ov063_0211a0dc(void);
extern int func_ov063_0211adb4(char *c);
extern void func_ov063_021162c8(char *c);

void func_ov063_02119b1c(char *c) {
    int r4 = func_ov063_0211a0dc();
    if (func_ov063_0211adb4(c) != 0) *(unsigned char *)(c + 0x5cc) = 0;
    if (r4 == -1) {
        *(unsigned char *)(c + 0x5cc) = 2;
        return;
    }
    if (r4 != 1) return;
    func_ov063_021162c8(c);
}
