int func_02053c10(int enable) {
    volatile unsigned short *reg = (volatile unsigned short *)0x4000004;
    int old = *reg & 0x10;
    if (enable) {
        *reg |= 0x10;
    } else {
        *reg &= ~0x10;
    }
    return old;
}
