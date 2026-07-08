// NONMATCHING: base materialization / addressing (div=21). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern int Vec3_Dist(void* a, void* b);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void* m);
extern int Vec3_HorzAngle(void* a, void* b);
extern void ApproachAngle(void* p, int target, int a, int b, int limit);
extern void Matrix4x3_FromRotationY(void* m, int ang);
extern void MulVec3Mat4x3(void* in, void* m, void* out);
extern int func_ov065_0211691c(void* c, void* p);
extern char* _ZN5Actor22ClosestNonVanishPlayerEv(void* self);
extern char MATRIX_SCRATCH_PAPER[];
extern char data_ov065_0211d670[];
extern char data_ov065_0211d680[];
int func_ov065_02116744(char* c) {
    int sp4[3];
    char* pl;
    int dp[3];
    sp4[0] = 0;
    sp4[1] = 0;
    sp4[2] = 0;
    if (Vec3_Dist(c+0x5c, c+0x3cc) <= 0x1f4000) {
        if (_ZNK12WithMeshClsn8IsOnWallEv(c+0x144) != 0) {
            *(short*)(c+0x3e0) = (short)Vec3_HorzAngle(c+0x5c, c+0x3cc);
            if (*(unsigned short*)(c+0x100) < 0x14) *(unsigned short*)(c+0x100) = 0x14;
            ApproachAngle(c+0x94, *(short*)(c+0x3e0), 0xa, 0x200, 0x100);
            ApproachAngle(c+0x92, 0, 1, 0x500, 0x500);
            sp4[2] = 0xa000;
            Matrix4x3_FromRotationY(MATRIX_SCRATCH_PAPER, *(short*)(c+0x8e));
            MulVec3Mat4x3(sp4, MATRIX_SCRATCH_PAPER, c+0xa4);
            if (*(unsigned short*)(c+0x100) == 0) {
                func_ov065_0211691c(c, data_ov065_0211d670);
            }
            return 1;
        }
    }
    pl = _ZN5Actor22ClosestNonVanishPlayerEv(c);
    if (pl != 0) {
        dp[0] = *(int*)(pl+0x5c);
        dp[1] = *(int*)(pl+0x60);
        dp[2] = *(int*)(pl+0x64);
        if (Vec3_Dist(c+0x5c, dp) < 0x3e8000) {
            *(unsigned short*)(c+0x100) = 0x14;
            func_ov065_0211691c(c, data_ov065_0211d680);
        }
    }
    return 1;
}
