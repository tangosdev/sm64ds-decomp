extern void func_ov001_020ab5b0(char* r0, int r1, short r2, short r3, short s4, short s5);
void func_ov006_0210c478(char *c) {
    func_ov001_020ab5b0(c, 1, 0x90, 0xb0, 0x10, 8);
    *(int*)(c + 0x18) = 6;
}
