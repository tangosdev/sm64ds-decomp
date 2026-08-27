extern char data_ov072_02122d6c[];
extern void func_ov072_02121d18(void* c);

void func_ov072_02121d50(char* c, int i) {
    *(char**)(c + 0x35c) = data_ov072_02122d6c + (i << 4);
    func_ov072_02121d18(c);
}
