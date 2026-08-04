extern void func_ov004_020b0d8c(void *c, int arg1, int arg2);

void func_ov006_020f392c(char *c) {
    if (*(unsigned char *)(c + 0x5312) == 0) return;
    func_ov004_020b0d8c(c, 0xe0, 0xa0);
}
