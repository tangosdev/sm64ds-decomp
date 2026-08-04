extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;

void func_ov006_020e48d4(char *c)
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
        *(int *)(p + 0x4000 + 0x8c0) = 0;
        *(int *)(p + 0x4000 + 0x8c4) = 0;
        *(int *)(p + 0x4000 + 0x8c8) = 0;
        *(int *)(p + 0x4000 + 0x8cc) = 0;
        *(short *)(p + 0x4800 + 0xd4) = 0;
        *(short *)(p + 0x4800 + 0xd6) = 0;
        *(short *)(p + 0x4800 + 0xd8) = 0;
        *(char *)(p + 0x4000 + 0x8dc) = 0;
        *(char *)(p + 0x4000 + 0x8dd) = 0;
        *(char *)(p + 0x4000 + 0x8de) = 0;
        *(char *)(p + 0x4000 + 0x8df) = 0;
        *(char *)(p + 0x4000 + 0x8e0) = 0;
        *(char *)(p + 0x4000 + 0x8e1) = 0;
        *(char *)(p + 0x4000 + 0x8e2) = 1;
        p += 0x24;
    }

    i = 0;
    p = c;
    for (; i < 0x32; i++)
    {
        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        m = ((r >> 16) & 0x7fff) << 5;
        *(int *)(p + 0x4000 + 0x8c0) = (int)((m >> 0xf)) << 0xf;
        *(int *)(p + 0x4000 + 0x8c4) = -0x8000;
        *(char *)(p + 0x4000 + 0x8dc) = 1;
        *(char *)(p + 0x4000 + 0x8e0) = 1;
        *(char *)(p + 0x4000 + 0x8dd) = 0;
        *(char *)(p + 0x4000 + 0x8de) = 0;

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        *(char *)(p + 0x4000 + 0x8e1) = (char)(((r >> 16) & 0x7fff) * 5 >> 0xf);

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        v = *(unsigned char *)(p + 0x4000 + 0x8e1) + ((((r >> 16) & 0x7fff) << 2) >> 0xf) + 1;
        v = v & 0xff;
        if (v >= 5)
            v = (v - 5) & 0xff;
        *(char *)(p + 0x4000 + 0x8e2) = (char)v;

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        *(short *)(p + 0x4800 + 0xd4) = (short)(((i & 7) << 6) + (((r >> 16) & 0x7fff) * 0x30 >> 0xf));

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        m = ((r >> 16) & 0x7fff) << 5;
        *(int *)(p + 0x4000 + 0x8c0) = (int)((m >> 0xf)) << 0xf;

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        q = (((r >> 16) & 0x7fff) * 0x1a) >> 0xf;
        *(int *)(p + 0x4000 + 0x8c4) = (((q << 3) - 8)) << 0xc;
        *(short *)(p + 0x4800 + 0xd4) = 0;
        p += 0x24;
    }
}
