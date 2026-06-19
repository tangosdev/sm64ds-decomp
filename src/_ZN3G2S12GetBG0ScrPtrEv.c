unsigned int _ZN3G2S12GetBG0ScrPtrEv(void) {
    int v = *(volatile unsigned short *)0x4001008;
    return (((v & 0x1f00) >> 8) << 0xb) + 0x6200000;
}
