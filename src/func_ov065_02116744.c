typedef short s16;

extern int Vec3_Dist(void *a, void *b);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void *m);
extern s16 Vec3_HorzAngle(void *a, void *b);
extern void ApproachAngle(s16 *p, s16 target, int a, int b, int limit);
extern void Matrix4x3_FromRotationY(void *m, int ang);
extern void MulVec3Mat4x3(void *in, void *m, void *out);
extern int func_ov065_0211691c(void *c, void *p);
extern char *_ZN5Actor22ClosestNonVanishPlayerEv(void *self);

extern char data_020a0e68[];
extern char data_ov065_0211d670[];
extern char data_ov065_0211d680[];

int func_ov065_02116744(char *c)
{
    int in[3];
    int dp[3];
    char *pl;
    int *pos;

    in[0] = 0;
    in[1] = 0;
    in[2] = 0;

    if (Vec3_Dist(c + 0x5c, c + 0x3cc) > 0x1f4000 ||
        _ZNK12WithMeshClsn8IsOnWallEv(c + 0x144) != 0) {
        *(s16 *)(c + 0x300 + 0xe0) =
            Vec3_HorzAngle(c + 0x5c, c + 0x3cc);
        if (*(unsigned short *)(c + 0x100) < 0x14)
            *(unsigned short *)(c + 0x100) = 0x14;
    }

    ApproachAngle((s16 *)(c + 0x94),
                  *(s16 *)(c + 0x300 + 0xe0),
                  0xa, 0x200, 0x100);
    ApproachAngle((s16 *)(c + 0x92), 0, 1, 0x500, 0x500);

    in[2] = 0xa000;
    Matrix4x3_FromRotationY(data_020a0e68, *(s16 *)(c + 0x8e));
    MulVec3Mat4x3(in, data_020a0e68, c + 0xa4);

    if (*(unsigned short *)(c + 0x100) == 0) {
        func_ov065_0211691c(c, data_ov065_0211d670);
        return 1;
    }

    pl = _ZN5Actor22ClosestNonVanishPlayerEv(c);
    if (pl != 0) {
        pos = (int *)(((int)pl + 0x5c) & 0xffffffffffffffffLL);
        dp[0] = pos[0];
        dp[1] = pos[1];
        dp[2] = pos[2];

        if (Vec3_Dist(c + 0x5c, dp) < 0x3e8000) {
            *(unsigned short *)(c + 0x100) = 0x14;
            func_ov065_0211691c(c, data_ov065_0211d680);
        }
    }

    return 1;
}
