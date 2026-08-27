extern int func_0203f818(int a);
extern void func_0203f6e8(short v);
extern void func_0203f6a4(int v);
extern short data_020a0f18;
extern short data_020a0f34;

void func_0203f8a4(void) {
    int r;
    data_020a0f18 = 0;
    data_020a0f34 = 0x65;
    r = func_0203f818(1);
    if (r == 1) {
        func_0203f6e8(-9);
    } else if (r == 2) {
        func_0203f6e8(-3);
    } else {
        func_0203f6a4(1);
    }
}
