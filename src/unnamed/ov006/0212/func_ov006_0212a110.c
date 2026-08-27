extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern unsigned char data_ov006_0212f3ac[];

void func_ov006_0212a110(char *c, int idx)
{
    int n = idx * 0x24;
    unsigned short d;
    short r;
    unsigned int v;
    d = *(unsigned short *)(c + 0xbea8 + n);
    if (d != 0) {
        r = (short)(d - 1);
        *(short *)(c + 0xbea8 + n) = r;
        if (*(short *)(c + 0xbea8 + n) < 0) *(short *)(c + 0xbea8 + n) = 0;
        return;
    }
    *(int *)(c + 0xbe9c + n) = 0;
    v = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(int *)(c + 0xbea0 + n) = (int)(((v << 5) >> 15) << 4) + 0x600;
    v = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(unsigned char *)(c + 0xbeb0 + n) = data_ov006_0212f3ac[(v << 1) >> 15];
    v = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(short *)(c + 0xbea8 + n) = (unsigned char)((v << 4) >> 15);
    v = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(short *)(c + 0xbeaa + n) = (unsigned char)(((v * 0x30) >> 15) + 0x30);
}
