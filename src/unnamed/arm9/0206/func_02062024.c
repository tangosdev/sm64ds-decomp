int func_02062024(unsigned char *o)
{
    unsigned short a = *(unsigned short *)(o + 4);
    if (a > 0x70) return 0;
    a = *(unsigned short *)(o + 0x16);
    if (a >= 0xa && a <= 0x3e8) {
    } else {
        return 0;
    }
    a = *(unsigned short *)(o + 0x30);
    if (a >= 1 && a <= 0xe) {
    } else {
        return 0;
    }
    {
        unsigned short c = *(unsigned short *)(o + 0x34);
        unsigned short b = *(unsigned short *)(o + 0x32);
        unsigned short d = *(unsigned short *)(o + 0xe);
        int idx = ((c + 0x20) << 2) + 0x70;
        int base = 0x14a + ((b + 0x26) << 2);
        int v = d * idx + base;
        return v <= 0x15e0;
    }
}
