// NONMATCHING: register allocation (div=24). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *c);
extern void ApproachAngle(short *a, short b, short c, short d, int e);
extern short SINE_TABLE[];
void func_ov074_021216f4(char *c)
{
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x40c)) {
        char *b = c + 0x500;
        *(short*)(c + 0x5f4) += *(short*)(b + 0xf6);
        *(int*)(c+0x5c) = (int)(((long long)SINE_TABLE[(*(unsigned short*)(b+0xf4) >> 4) << 1] * 0x546000 + 0x800) >> 12);
        *(int*)(c+0x64) = (int)(((long long)SINE_TABLE[((*(unsigned short*)(b+0xf4) >> 4) << 1) + 1] * 0x546000 + 0x800) >> 12);
    }
    {
        char *b = c + 0x500;
        ApproachAngle((short*)(c+0x8e),
                      (short)(*(int*)(c+0x5f0) * 0x4500 + *(short*)(b+0xf4)),
                      0xa, 0x800, 0x10);
    }
}
