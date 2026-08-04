extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;

void func_ov006_02129a34(char *base, int i)
{
    int o = i * 0x24;
    unsigned short *aa8;

    *(int *)(base + 0xbe94 + o) += *(int *)(base + 0xbe9c + o);
    *(int *)(base + 0xbe98 + o) = *(int *)(base + 0xbe98 + o) + *(int *)(base + 0xbea0 + o);

    aa8 = (unsigned short *)(base + 0xbea8 + o);
    if (*aa8 != 0) {
        *aa8 = *aa8 - 1;
        if (*(short *)aa8 < 0) *aa8 = 0;
        return;
    }

    {
        int *p9c = (int *)(base + 0xbe9c + o);
        if (*p9c > -0x300) {
            *p9c -= 8;
            if (*p9c <= -0x300) *p9c = 0x300;
        }
    }

    {
        unsigned short *aaa = (unsigned short *)(base + 0xbeaa + o);
        if (*aaa != 0) {
            *aaa = *aaa - 1;
            if (*(short *)aaa < 0) *aaa = 0;
            return;
        }
        *(unsigned char *)(base + 0xbeb0 + o) = 3;
        *aaa = (short)(((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf) + 0x20) & 0xff);
    }
}
