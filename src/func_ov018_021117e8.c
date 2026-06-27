void func_ov018_021117e8(char* c, char* a) {
    int val = *(unsigned short*)(a + 0xc);
    val = (val == 0xbf);
    if (val) {
        *(char**)(c + 0x320) = a;
    }
}
