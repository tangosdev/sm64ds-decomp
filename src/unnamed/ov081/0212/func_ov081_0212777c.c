extern char data_ov081_02128f40;
extern void func_ov081_02127744(char *c);
void func_ov081_0212777c(char *c, int a) {
    *(int *)(c + 0x3dc) = (int)&data_ov081_02128f40 + (a << 4);
    func_ov081_02127744(c);
}
