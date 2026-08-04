extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern unsigned char data_ov006_0212f3b0[];

void func_ov006_02129cb0(char *o, int i)
{
    int n = i * 0x24;
    if (*(unsigned short *)(o + 0xbea8 + n) != 0) {
        *(unsigned short *)(o + 0xbea8 + n) = *(unsigned short *)(o + 0xbea8 + n) - 1;
        if (*(short *)(o + 0xbea8 + n) < 0) *(short *)(o + 0xbea8 + n) = 0;
        return;
    }
    *(int *)(o + 0xbe9c + n) = 0;
    *(unsigned char *)(o + 0xbeb0 + n) = data_ov006_0212f3b0[(((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 1 >> 15];
    *(unsigned short *)(o + 0xbea8 + n) = (short)(unsigned char)((0x10 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf);
    *(unsigned short *)(o + 0xbeaa + n) = (short)(unsigned char)(((0x40 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf) + 0x60);
}
