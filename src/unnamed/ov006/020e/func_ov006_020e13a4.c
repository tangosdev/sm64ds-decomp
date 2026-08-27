extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;

void func_ov006_020e13a4(char *c)
{
    int i;
    char *p;
    unsigned int r;
    unsigned int v;
    int q;
    unsigned int m;

    i = 0;
    p = c;
    for (; i < 0x32; i++)
    {
        *(int *)(p + 0x4000 + 0x78c) = 0;
        *(int *)(p + 0x4000 + 0x790) = 0;
        *(int *)(p + 0x4000 + 0x794) = 0;
        *(int *)(p + 0x4000 + 0x798) = 0;
        *(short *)(p + 0x4700 + 0xa0) = 0;
        *(short *)(p + 0x4700 + 0xa2) = 0;
        *(short *)(p + 0x4700 + 0xa4) = 0;
        *(char *)(p + 0x4000 + 0x7a8) = 0;
        *(char *)(p + 0x4000 + 0x7a9) = 0;
        *(char *)(p + 0x4000 + 0x7aa) = 0;
        *(char *)(p + 0x4000 + 0x7ab) = 0;
        *(char *)(p + 0x4000 + 0x7ac) = 0;
        *(char *)(p + 0x4000 + 0x7ad) = 0;
        *(char *)(p + 0x4000 + 0x7ae) = 1;
        p += 0x24;
    }

    i = 0;
    p = c;
    for (; i < 0x32; i++)
    {
        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        m = ((r >> 16) & 0x7fff) << 5;
        *(int *)(p + 0x4000 + 0x78c) = (int)((m >> 0xf)) << 0xf;
        *(int *)(p + 0x4000 + 0x790) = -0x8000;
        *(char *)(p + 0x4000 + 0x7a8) = 1;
        *(char *)(p + 0x4000 + 0x7ac) = 1;
        *(char *)(p + 0x4000 + 0x7a9) = 0;
        *(char *)(p + 0x4000 + 0x7aa) = 0;

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        *(char *)(p + 0x4000 + 0x7ad) = (char)(((r >> 16) & 0x7fff) * 5 >> 0xf);

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        v = *(unsigned char *)(p + 0x4000 + 0x7ad) + ((((r >> 16) & 0x7fff) << 2) >> 0xf) + 1;
        v = v & 0xff;
        if (v >= 5)
            v = (v - 5) & 0xff;
        *(char *)(p + 0x4000 + 0x7ae) = (char)v;

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        *(short *)(p + 0x4700 + 0xa0) = (short)(((i & 7) << 6) + (((r >> 16) & 0x7fff) * 0x30 >> 0xf));

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        m = ((r >> 16) & 0x7fff) << 5;
        *(int *)(p + 0x4000 + 0x78c) = (int)((m >> 0xf)) << 0xf;

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        q = (((r >> 16) & 0x7fff) * 0x1a) >> 0xf;
        *(int *)(p + 0x4000 + 0x790) = (((q << 3) - 8)) << 0xc;
        *(short *)(p + 0x4700 + 0xa0) = 0;
        p += 0x24;
    }
}
