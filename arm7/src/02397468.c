int fn_02397468(unsigned short a1) {
    int *base;
    if (a1 < 1 || a1 > 0xFF)
        return 5;
    base = *(int **)0x0380FFF4;
    *(unsigned short *)((char *)base + 0x300 + 0x70) = a1;
    *(volatile unsigned short *)0x0480808E = a1;
    return 0;
}
