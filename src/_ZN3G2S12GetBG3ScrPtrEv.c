unsigned int _ZN3G2S12GetBG3ScrPtrEv(void) {
    int m = *(volatile int *)0x4001000 & 7;
    unsigned int v = *(volatile unsigned short *)0x400100e;
    unsigned int r1 = (v & 0x1f00) >> 8;
    switch (m) {
    case 0:
    case 1:
    case 2:
        return (r1 << 0xb) + 0x6200000;
    case 3:
    case 4:
    case 5:
        if (v & 0x80) return (r1 << 0xe) + 0x6200000;
        return (r1 << 0xb) + 0x6200000;
    case 6:
        return 0;
    default:
        return 0;
    }
}
