unsigned short *func_ov002_020e6ad4(unsigned short *p) {
    unsigned short *dst = p;
    unsigned short *q = (unsigned short *)((char *)p + 0x40);
    int i = 0;
    do {
        unsigned short v = (i * 0x400) | (i | (i * 0x20));
        *dst++ = v;
        *q++ = v;
        i++;
    } while (i < 0x20);
    *(volatile unsigned short *)0x4000060 &= ~0x3002;
    return p;
}
