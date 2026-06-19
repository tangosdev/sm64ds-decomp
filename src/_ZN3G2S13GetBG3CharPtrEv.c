unsigned int _ZN3G2S13GetBG3CharPtrEv(void) {
    int cfg = *(volatile int *)0x4001000;
    unsigned int v = *(volatile unsigned short *)0x400100e;
    int m = cfg & 7;
    if (m >= 3) {
        if (m >= 6) goto zero;
        if (v & 0x80) goto zero;
    }
    return (((v & 0x3c) >> 2) << 0xe) + 0x6200000;
zero:
    return 0;
}
