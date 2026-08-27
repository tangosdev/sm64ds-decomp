extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;

void func_ov006_020e4094(char *base, int index)
{
    int i = index * 0x24;
    unsigned short v;

    *(int *)(base + 0x48c0 + i) += *(int *)(base + 0x48c8 + i);
    *(int *)(base + 0x48c4 + i) += *(int *)(base + 0x48cc + i);

    v = *(unsigned short *)(base + 0x48d4 + i);
    if (v != 0) {
        *(short *)(base + 0x48d4 + i) = v - 1;
        if (*(short *)(base + 0x48d4 + i) < 0)
            *(short *)(base + 0x48d4 + i) = 0;
        return;
    }

    if (*(int *)(base + 0x48c8 + i) < 0x300) {
        *(int *)(base + 0x48c8 + i) += 8;
        if (*(int *)(base + 0x48c8 + i) >= 0x300)
            *(int *)(base + 0x48c8 + i) = 0x300;
    }

    v = *(unsigned short *)(base + 0x48d6 + i);
    if (v != 0) {
        *(short *)(base + 0x48d6 + i) = v - 1;
        if (*(short *)(base + 0x48d6 + i) < 0)
            *(short *)(base + 0x48d6 + i) = 0;
        return;
    }

    *(char *)(base + 0x48de + i) = 3;
    *(short *)(base + 0x48d6 + i) = (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf) + 0x20 & 0xff;
}
