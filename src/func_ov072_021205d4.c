extern char data_ov072_02122c00;
extern void func_ov072_0212059c(char *c);
void func_ov072_021205d4(char *c, int a) {
    *(int *)(c + 0x328) = (int)&data_ov072_02122c00 + (a << 4);
    func_ov072_0212059c(c);
}
