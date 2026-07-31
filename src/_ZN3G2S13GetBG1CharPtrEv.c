unsigned int _ZN3G2S13GetBG1CharPtrEv(void) {
    int v = *(volatile unsigned short *)0x400100a;
    return (((v & 0x3c) >> 2) << 0xe) + 0x6200000;
}
