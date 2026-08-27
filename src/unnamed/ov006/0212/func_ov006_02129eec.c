void func_ov006_02129eec(char *c, int idx)
{
    int n = idx * 0x24;
    unsigned short d;
    short r;
    *(int *)(c + 0xbe94 + n) = *(int *)(c + 0xbe94 + n) + *(int *)(c + 0xbe9c + n);
    *(int *)(c + 0xbe98 + n) = *(int *)(c + 0xbe98 + n) + *(int *)(c + 0xbea0 + n);
    d = *(unsigned short *)(c + 0xbea8 + n);
    if (d != 0) {
        r = (short)(d - 1);
        *(short *)(c + 0xbea8 + n) = r;
        if (*(short *)(c + 0xbea8 + n) < 0) *(short *)(c + 0xbea8 + n) = 0;
        return;
    }
    if (*(int *)(c + 0xbe9c + n) > -0x400) {
        *(int *)(c + 0xbe9c + n) -= 0x20;
        if (*(int *)(c + 0xbe9c + n) <= -0x400) *(int *)(c + 0xbe9c + n) = 0x400;
    }
    d = *(unsigned short *)(c + 0xbeaa + n);
    if (d != 0) {
        r = (short)(d - 1);
        *(short *)(c + 0xbeaa + n) = r;
        if (*(short *)(c + 0xbeaa + n) < 0) *(short *)(c + 0xbeaa + n) = 0;
        return;
    }
    *(unsigned char *)(c + 0xbeb0 + n) = 3;
}
