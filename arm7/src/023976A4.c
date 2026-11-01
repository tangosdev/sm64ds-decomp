int fn_023976A4(unsigned int a1) {
    int *base;
    unsigned short *hp;
    unsigned int bit;
    unsigned int w;
    if (a1 > 1)
        return 5;
    base = *(int **)0x0380FFF4;
    hp = (unsigned short *)((char *)base + 0x2FE);
    w = *hp;
    w &= ~0x40u;
    bit = (unsigned short)a1;
    bit &= 1;
    *hp = w | (bit << 6);
    return 0;
}
