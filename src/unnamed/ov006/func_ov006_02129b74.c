extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;

void func_ov006_02129b74(char *base, int index)
{
    int i = index * 0x24;
    unsigned short v;

    *(int *)(base + 0xbe94 + i) += *(int *)(base + 0xbe9c + i);
    *(int *)(base + 0xbe98 + i) += *(int *)(base + 0xbea0 + i);

    v = *(unsigned short *)(base + 0xbea8 + i);
    if (v != 0) {
        *(short *)(base + 0xbea8 + i) = v - 1;
        if (*(short *)(base + 0xbea8 + i) < 0)
            *(short *)(base + 0xbea8 + i) = 0;
        return;
    }

    if (*(int *)(base + 0xbe9c + i) < 0x300) {
        *(int *)(base + 0xbe9c + i) += 8;
        if (*(int *)(base + 0xbe9c + i) >= 0x300)
            *(int *)(base + 0xbe9c + i) = 0x300;
    }

    v = *(unsigned short *)(base + 0xbeaa + i);
    if (v != 0) {
        *(short *)(base + 0xbeaa + i) = v - 1;
        if (*(short *)(base + 0xbeaa + i) < 0)
            *(short *)(base + 0xbeaa + i) = 0;
        return;
    }

    *(char *)(base + 0xbeb0 + i) = 3;
    *(short *)(base + 0xbeaa + i) = (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf) + 0x20 & 0xff;
}
