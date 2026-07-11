extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern unsigned char data_ov006_0212e454[];

void func_ov006_020e1100(char *c, int idx)
{
    int n = idx * 0x24;
    unsigned short d;
    short r;
    unsigned int v;
    d = *(unsigned short *)(c + 0x47a0 + n);
    if (d != 0) {
        r = (short)(d - 1);
        *(short *)(c + 0x47a0 + n) = r;
        if (*(short *)(c + 0x47a0 + n) < 0) *(short *)(c + 0x47a0 + n) = 0;
        return;
    }
    *(int *)(c + 0x4794 + n) = 0;
    v = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(int *)(c + 0x4798 + n) = (int)(((v << 5) >> 15) << 4) + 0x600;
    v = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(unsigned char *)(c + 0x47aa + n) = data_ov006_0212e454[(v << 1) >> 15];
    v = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(short *)(c + 0x47a0 + n) = (unsigned char)((v << 4) >> 15);
    v = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(short *)(c + 0x47a2 + n) = (unsigned char)(((v * 0x30) >> 15) + 0x30);
}
