unsigned int _ZN3G2S13GetBG0CharPtrEv(void) {
    int v = *(volatile unsigned short *)0x4001008;
    return (((v & 0x3c) >> 2) << 0xe) + 0x6200000;
}
