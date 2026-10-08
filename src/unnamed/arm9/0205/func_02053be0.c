int func_02053be0(int enable) {
    volatile unsigned short *reg = (volatile unsigned short *)0x4000004;
    int old = *reg & 8;
    if (enable) {
        *reg |= 8;
    } else {
        *reg &= ~8;
    }
    return old;
}
