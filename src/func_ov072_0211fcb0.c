extern char data_ov072_02122b64;
extern void func_ov072_0211fc78(char *c);
void func_ov072_0211fcb0(char *c, int a) {
    *(int *)(c + 0x38c) = (int)&data_ov072_02122b64 + (a << 4);
    func_ov072_0211fc78(c);
}
