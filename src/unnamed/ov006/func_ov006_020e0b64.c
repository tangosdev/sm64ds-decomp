extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;

void func_ov006_020e0b64(char *base, int index)
{
    int i = index * 0x24;
    unsigned short v;

    *(int *)(base + 0x478c + i) += *(int *)(base + 0x4794 + i);
    *(int *)(base + 0x4790 + i) += *(int *)(base + 0x4798 + i);

    v = *(unsigned short *)(base + 0x47a0 + i);
    if (v != 0) {
        *(short *)(base + 0x47a0 + i) = v - 1;
        if (*(short *)(base + 0x47a0 + i) < 0)
            *(short *)(base + 0x47a0 + i) = 0;
        return;
    }

    if (*(int *)(base + 0x4794 + i) < 0x300) {
        *(int *)(base + 0x4794 + i) += 8;
        if (*(int *)(base + 0x4794 + i) >= 0x300)
            *(int *)(base + 0x4794 + i) = 0x300;
    }

    v = *(unsigned short *)(base + 0x47a2 + i);
    if (v != 0) {
        *(short *)(base + 0x47a2 + i) = v - 1;
        if (*(short *)(base + 0x47a2 + i) < 0)
            *(short *)(base + 0x47a2 + i) = 0;
        return;
    }

    *(char *)(base + 0x47aa + i) = 3;
    *(short *)(base + 0x47a2 + i) = (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf) + 0x20 & 0xff;
}
