extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;

void func_ov006_0212a3c0(char *c)
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
        *(int *)(p + 0xb000 + 0xe94) = 0;
        *(int *)(p + 0xb000 + 0xe98) = 0;
        *(int *)(p + 0xb000 + 0xe9c) = 0;
        *(int *)(p + 0xb000 + 0xea0) = 0;
        *(short *)(p + 0xbe00 + 0xa8) = 0;
        *(short *)(p + 0xbe00 + 0xaa) = 0;
        *(short *)(p + 0xbe00 + 0xac) = 0;
        *(char *)(p + 0xb000 + 0xeae) = 0;
        *(char *)(p + 0xb000 + 0xeaf) = 0;
        *(char *)(p + 0xb000 + 0xeb0) = 0;
        *(char *)(p + 0xb000 + 0xeb1) = 0;
        *(char *)(p + 0xb000 + 0xeb2) = 0;
        *(char *)(p + 0xb000 + 0xeb3) = 0;
        *(char *)(p + 0xb000 + 0xeb4) = 1;
        p += 0x24;
    }

    i = 0;
    p = c;
    for (; i < 0x32; i++)
    {
        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        m = ((r >> 16) & 0x7fff) << 5;
        *(int *)(p + 0xb000 + 0xe94) = (int)((m >> 0xf)) << 0xf;

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        q = (((r >> 16) & 0x7fff) * 0x18) >> 0xf;
        *(int *)(p + 0xb000 + 0xe98) = *(int *)(c + 0xab6c) - 0x8000 + (q << 0xf);
        *(char *)(p + 0xb000 + 0xeae) = 1;
        *(char *)(p + 0xb000 + 0xeb2) = 1;
        *(char *)(p + 0xb000 + 0xeaf) = 0;
        *(char *)(p + 0xb000 + 0xeb0) = 0;

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        *(char *)(p + 0xb000 + 0xeb3) = (char)(((r >> 16) & 0x7fff) * 5 >> 0xf);

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        v = *(unsigned char *)(p + 0xb000 + 0xeb3) + ((((r >> 16) & 0x7fff) << 2) >> 0xf) + 1;
        v = v & 0xff;
        if (v >= 5)
            v = (v - 5) & 0xff;
        *(char *)(p + 0xb000 + 0xeb4) = (char)v;
        *(short *)(p + 0xbe00 + 0xa8) = 0;
        p += 0x24;
    }
}
