extern char data_ov080_0212847c[];
extern void func_ov080_02125104(void* c);

void func_ov080_0212513c(char* c, int i) {
    *(char**)(c + 0x36c) = data_ov080_0212847c + (i << 4);
    func_ov080_02125104(c);
}
