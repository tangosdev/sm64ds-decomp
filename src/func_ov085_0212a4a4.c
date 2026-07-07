extern char data_ov085_0213055c;
extern void func_ov085_0212a46c(char *c);
void func_ov085_0212a4a4(char *c, int a) {
    *(int *)(c + 0x350) = (int)&data_ov085_0213055c + (a << 4);
    func_ov085_0212a46c(c);
}
