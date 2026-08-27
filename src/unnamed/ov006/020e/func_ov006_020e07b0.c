extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;

void func_ov006_020e07b0(char *o, int i)
{
    int n = i * 0x24;
    if (*(int *)(o + 0x479c + n) > *(int *)(o + 0x4798 + n)) {
        *(int *)(o + 0x4798 + n) += 0x10;
        if (*(int *)(o + 0x479c + n) > *(int *)(o + 0x4798 + n))
            *(int *)(o + 0x4798 + n) = *(int *)(o + 0x479c + n);
    }
    if (*(unsigned short *)(o + 0x47a4 + n) != 0) {
        *(unsigned short *)(o + 0x47a4 + n) = *(unsigned short *)(o + 0x47a4 + n) - 1;
        if (*(short *)(o + 0x47a4 + n) < 0) *(short *)(o + 0x47a4 + n) = 0;
    } else {
        *(unsigned char *)(o + n + 0x47ab) = 2;
        *(short *)(o + 0x47a4 + n) = (short)(unsigned char)((((0x20 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf)) + 0x20);
    }
}
