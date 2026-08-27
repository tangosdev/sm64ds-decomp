int func_ov084_0212f1d0(char *c) {
    char *p = *(char**)(c + 0x460);
    if (p == 0) return 0;
    if (*(int*)(p + 0xa8) > 0xa000) return 1;
    return *(int*)(p + 0x98) > 0xa000;
}
