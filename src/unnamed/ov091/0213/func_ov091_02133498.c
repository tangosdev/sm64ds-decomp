void func_ov091_02133498(char *c) {
    char *p = *(char**)(c + 0x32c);
    if (!p) return;
    unsigned short v = *(unsigned short*)(p + 0xc);
    if (v == 0x140) *(char*)(p + 0x10c) = 1;
}
