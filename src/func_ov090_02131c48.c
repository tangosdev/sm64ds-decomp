// NONMATCHING: different op / idiom (div=32). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void func_02012694(int a, void* p);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void* m);
extern int AngleDiff(int a, int b);
extern void ApproachAngle(void* p, int target, int a, int b, int limit);
extern int _ZN9Animation8FinishedEv(void* a);
extern int func_ov090_02131e00(void* c, void* p);
extern int RandomIntInternal(int* seed);
extern char data_ov090_02134504[];
extern char data_ov090_021344e4[];
extern int RNG_STATE[];
int func_ov090_02131c48(char* c) {
    unsigned int s;
    s = (unsigned)((unsigned)(*(int*)(c+0x364) << 4) >> 0x10);
    if (s >= 4u) *(int*)(c+0x98) = 0x19000;
    s = (unsigned)((unsigned)(*(int*)(c+0x364) << 4) >> 0x10);
    if (s == 4u) {
        func_02012694(0xfd, c+0x74);
    }
    if (*(unsigned short*)(c+0x394) == 0) {
        if (_ZNK12WithMeshClsn8IsOnWallEv(c+0x150) != 0) {
            if (AngleDiff((int)*(short*)(c+0x94), (int)*(short*)(c+0x39a)) < 0x200) {
                *(short*)(c+0x39a) = (short)(*(short*)(c+0x39a) + 0x4000);
                *(unsigned short*)(c+0x394) = 8;
            }
        }
    }
    if (*(unsigned short*)(c+0x394) != 0) {
        ApproachAngle(c+0x94, *(short*)(c+0x39a), 1, 0x1000, 0x1000);
    } else {
        if (_ZN9Animation8FinishedEv(c+0x35c) != 0) {
            *(int*)(c+0x390) = *(int*)(c+0x390) + 1;
            if (*(int*)(c+0x390) > 0x14) {
                func_ov090_02131e00(c, data_ov090_02134504);
                if (*(unsigned char*)(c+0x39c) == 0) {
                    *(int*)(c+0x374) = *(int*)(c+0x5c);
                    *(int*)(c+0x378) = *(int*)(c+0x60);
                    *(int*)(c+0x37c) = *(int*)(c+0x64);
                    int r = RandomIntInternal(RNG_STATE);
                    *(short*)(c+0x39a) = (short)((r >> 8) << 0xd);
                    func_ov090_02131e00(c, data_ov090_021344e4);
                }
            }
        }
    }
    return 1;
}
