extern int func_ov060_02111cc0(char *c, int a, int b);
extern int func_ov060_02115a30(char *c);
extern char *_ZN5Actor10FindWithIDEj(unsigned int id);
extern int _ZN12WithMeshClsn15ClearGroundFlagEv(char *c);
extern short data_02082214[];

#pragma opt_common_subs off
void func_ov060_021125f0(char *c)
{
    int v, nv;
    char *a;

    func_ov060_02111cc0(c, 0xe, 0x40000000);
    if (*(int *)(c + 0x410) == 3) {
        if (func_ov060_02115a30(c) == 0)
            return;
    }
    *(unsigned char *)(c + 0x425) = 0;
    *(int *)(c + 0x410) = 0;
    {
        int *q = (int *)(((long long)(int)(c + 0x378)) & 0xFFFFFFFFFFFFFFFFLL);
        *q &= ~1;
    }
    *(int *)(c + 0x40c) = 1;
    v = *(int *)(c + 0x3f0);
    if (v < 0)
        v = -v;
    v = v * 0x46 / 6000;
    if (v > 0x2d)
        v = v * 0x19 / 10;
    nv = -v;
    *(int *)(c + 0x98) = v * data_02082214[(*(unsigned short *)(c + 0x8c) >> 4 << 1) + 1];
    *(int *)(c + 0xa8) = nv * data_02082214[*(unsigned short *)(c + 0x8c) >> 4 << 1];
    a = _ZN5Actor10FindWithIDEj(*(unsigned int *)(c + 0x3a8));
    if (a != 0) {
        *(int *)(a + 0x110) = 1;
        {
            char *p = (char *)(((long long)(int)(a + 0x100)) & 0xFFFFFFFFFFFFFFFFLL);
            *(short *)(p + 0x14) = 0;
        }
    }
    *(short *)(c + 0x8c) = 0;
    {
        char *p = (char *)(((long long)(int)(c + 0x300)) & 0xFFFFFFFFFFFFFFFFLL);
        *(short *)(p + 0xfc) = 0;
    }
    {
        char *mesh = c + 0x14c;
        *(unsigned char *)(c + 0x423) = 0;
        _ZN12WithMeshClsn15ClearGroundFlagEv(mesh);
    }
}
