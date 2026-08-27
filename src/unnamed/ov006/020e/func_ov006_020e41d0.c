extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern unsigned char data_ov006_0212e4f4[];

void func_ov006_020e41d0(char *o, int i)
{
    int n = i * 0x24;
    if (*(unsigned short *)(o + 0x48d4 + n) != 0) {
        *(unsigned short *)(o + 0x48d4 + n) = *(unsigned short *)(o + 0x48d4 + n) - 1;
        if (*(short *)(o + 0x48d4 + n) < 0) *(short *)(o + 0x48d4 + n) = 0;
        return;
    }
    *(int *)(o + 0x48c8 + n) = 0;
    *(unsigned char *)(o + 0x48de + n) = data_ov006_0212e4f4[(((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 1 >> 15];
    *(unsigned short *)(o + 0x48d4 + n) = (short)(unsigned char)((0x10 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf);
    *(unsigned short *)(o + 0x48d6 + n) = (short)(unsigned char)(((0x40 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf) + 0x60);
}
