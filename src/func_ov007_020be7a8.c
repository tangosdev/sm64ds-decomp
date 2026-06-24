struct G {
    char pad24[0x24];
    int f24;     // 0x24
    char pad28[0x30 - 0x28];
    int f30;     // 0x30
    char padec[0xec - 0x34];
    int fec;     // 0xec
};

extern void func_ov007_020bfd70(int a, int b, int *p0, int *p1);
extern struct G *data_ov007_0210342c;
extern unsigned short data_ov007_020d7c58[];

void func_ov007_020be7a8(int a0, unsigned short *out1, unsigned short *out2, int *out3)
{
    int loc0;
    int loc1;
    struct G *g;

    g = data_ov007_0210342c;
    func_ov007_020bfd70(g->f30, a0, &loc0, &loc1);

    g = data_ov007_0210342c;
    if (g->f24 == 4) {
        loc1 = loc1 + 0x14b;
    } else {
        loc1 = loc1 + data_ov007_020d7c58[g->fec];
    }

    *out1 = (unsigned short)loc0;
    *out2 = (unsigned short)loc1;
    *out3 = 0xfff;
}
