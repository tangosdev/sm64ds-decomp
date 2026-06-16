extern int func_ov060_02113ff4(char *c, int a, int b);
void func_ov060_02113fcc(char *c) {
    char *r4 = c;
    if (func_ov060_02113ff4(c, 0x3e, 0x200) != 0) {
        *(int*)(r4 + 0x40c) = 0;
    }
}
