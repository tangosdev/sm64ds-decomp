int func_ov084_0212cac0(char *c) {
    int r1 = *(short*)((char*)c + 0x8c);
    int r0 = 1;
    r1 = r1 & 0xff;
    if (r1 == 1) return r0;
    if (r1 != 2) r0 = 0;
    return r0;
}
