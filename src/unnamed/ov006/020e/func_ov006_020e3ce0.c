extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;

void func_ov006_020e3ce0(char *o, int i)
{
    int n = i * 0x24;
    if (*(int *)(o + 0x48d0 + n) > *(int *)(o + 0x48cc + n)) {
        *(int *)(o + 0x48cc + n) += 0x10;
        if (*(int *)(o + 0x48d0 + n) > *(int *)(o + 0x48cc + n))
            *(int *)(o + 0x48cc + n) = *(int *)(o + 0x48d0 + n);
    }
    if (*(unsigned short *)(o + 0x48d8 + n) != 0) {
        *(unsigned short *)(o + 0x48d8 + n) = *(unsigned short *)(o + 0x48d8 + n) - 1;
        if (*(short *)(o + 0x48d8 + n) < 0) *(short *)(o + 0x48d8 + n) = 0;
    } else {
        *(unsigned char *)(o + n + 0x48df) = 2;
        *(short *)(o + 0x48d8 + n) = (short)(unsigned char)((((0x20 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf)) + 0x20);
    }
}
