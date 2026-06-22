// NONMATCHING: base materialization / addressing (div=8). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern int func_ov060_02111cc0(char *c, int a, int b);
extern int func_ov060_02115a30(char *c);
extern char *_ZN5Actor10FindWithIDEj(unsigned int id);
extern int _ZN12WithMeshClsn15ClearGroundFlagEv(char *c);
extern short data_02082214[];
int func_ov060_021125f0(char *c)
{
    int v, sc, r;
    char *a;
    int *q;
    func_ov060_02111cc0(c, 0xe, 0x40000000);
    if (*(int*)(c + 0x410) == 3) {
        r = func_ov060_02115a30(c);
        if (r == 0)
            return r;
    }
    *(unsigned char*)(c + 0x425) = 0;
    *(int*)(c + 0x410) = 0;
    q = (int*)(c + 0x378);
    *q &= ~1;
    *(int*)(c + 0x40c) = 1;
    v = *(int*)(c + 0x3f0);
    if (v < 0) v = -v;
    v = v * 0x46 / 6000;
    if (v > 0x2d)
        v = v * 0x19 / 10;
    sc = data_02082214[((*(unsigned short*)(c + 0x8c) >> 4) << 1) + 1];
    *(int*)(c + 0x98) = v * sc;
    *(int*)(c + 0xa8) = -v * data_02082214[(*(unsigned short*)(c + 0x8c) >> 4) << 1];
    a = _ZN5Actor10FindWithIDEj(*(unsigned int*)(c + 0x3a8));
    if (a) {
        *(int*)(a + 0x110) = 1;
        *(short*)(a + 0x114) = 0;
    }
    *(short*)(c + 0x8c) = 0;
    *(short*)(c + 0x3fc) = 0;
    *(unsigned char*)(c + 0x423) = 0;
    _ZN12WithMeshClsn15ClearGroundFlagEv(c + 0x14c);
    return 1;
}
