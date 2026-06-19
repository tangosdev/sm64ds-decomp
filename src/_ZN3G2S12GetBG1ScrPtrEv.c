unsigned int _ZN3G2S12GetBG1ScrPtrEv(void) {
    int v = *(volatile unsigned short *)0x400100a;
    return (((v & 0x1f00) >> 8) << 0xb) + 0x6200000;
}
