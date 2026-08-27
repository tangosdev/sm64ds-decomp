extern void func_ov084_0212af74(char *c);
extern void func_ov084_0212abd4(char *c);

void func_ov084_0212b2dc(char *c) {
    if (*(int *)(c + 0x460) == 3) {
        func_ov084_0212af74(c);
    } else {
        func_ov084_0212abd4(c);
    }
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
}
