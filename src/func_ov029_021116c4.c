void func_ov029_021116c4(char *a, char *b) {
    int cond = *(unsigned short*)(b + 0xc);
    cond = (cond == 0xbf);
    if (cond) {
        *(a + 0x326) = 1;
    }
}
