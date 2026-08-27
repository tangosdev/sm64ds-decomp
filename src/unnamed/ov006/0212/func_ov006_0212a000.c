void func_ov006_0212a000(char *base, int index)
{
    int n;

    *(int *)(base + 0xbe94 + index * 0x24) += *(int *)(base + 0xbe9c + index * 0x24);
    *(int *)(base + 0xbe98 + index * 0x24) += *(int *)(base + 0xbea0 + index * 0x24);

    n = *(unsigned short *)(base + 0xbea8 + index * 0x24);
    if (n != 0) {
        *(unsigned short *)(base + 0xbea8 + index * 0x24) = (unsigned short)(n - 1);
        if (*(short *)(base + 0xbea8 + index * 0x24) < 0)
            *(short *)(base + 0xbea8 + index * 0x24) = 0;
        return;
    }

    if (*(int *)(base + 0xbe9c + index * 0x24) < 0x400) {
        *(int *)(base + 0xbe9c + index * 0x24) += 0x20;
        if (*(int *)(base + 0xbe9c + index * 0x24) >= 0x400)
            *(int *)(base + 0xbe9c + index * 0x24) = 0x400;
    }

    n = *(unsigned short *)(base + 0xbeaa + index * 0x24);
    if (n != 0) {
        *(unsigned short *)(base + 0xbeaa + index * 0x24) = (unsigned short)(n - 1);
        if (*(short *)(base + 0xbeaa + index * 0x24) < 0)
            *(short *)(base + 0xbeaa + index * 0x24) = 0;
        return;
    }

    *(unsigned char *)(base + 0xbeb0 + index * 0x24) = 3;
}
