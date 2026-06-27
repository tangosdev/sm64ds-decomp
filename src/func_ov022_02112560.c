extern void func_020393a4(int *p, int v);
extern int func_ov080_0212714c(char *c, int *d);
extern int data_ov022_02112c98;

int func_ov022_02112560(char *c) {
    func_020393a4((int *)(c + 0x124), 0x500000);
    return func_ov080_0212714c(c, &data_ov022_02112c98) & 0xff;
}
