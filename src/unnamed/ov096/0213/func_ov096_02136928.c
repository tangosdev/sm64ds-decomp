extern char data_ov096_02137b48;
extern void func_ov096_021368f0(char *c);
void func_ov096_02136928(char *c, int a) {
    *(int *)(c + 0x384) = (int)&data_ov096_02137b48 + (a << 4);
    func_ov096_021368f0(c);
}
