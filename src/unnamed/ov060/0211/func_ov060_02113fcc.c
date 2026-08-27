extern int func_ov060_02113ff4(char *c, int a, int b);

void func_ov060_02113fcc(char *c) {
    int r0 = func_ov060_02113ff4(c, 0x3e, 0x200);
    if (r0 != 0) {
        *(int *)(c + 0x40c) = 0;
    }
}
