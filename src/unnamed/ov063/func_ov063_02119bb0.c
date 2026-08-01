extern void func_ov063_0211adfc(char *p);
extern int func_ov063_02119c58(char *c);
extern int func_ov063_0211ad00(char *c);

void func_ov063_02119bb0(char *c) {
    if (*(int *)(c + 0x5a4) == 2) *(int *)(c + 0x5a8) = 0xa;
    func_ov063_0211adfc(c);
    func_ov063_02119c58(c);
    if (func_ov063_0211ad00(c) != 0) {
        if (*(int *)(c + 0x5a4) != 2) *(unsigned char *)(c + 0x5cc) = 6;
        else *(unsigned char *)(c + 0x5cc) = 1;
    }
    *(int *)(c + 0x584) = 0x1000;
    *(unsigned char *)(c + 0x5c9) = 0xff;
}
