//cpp
// NONMATCHING: different op / idiom (div=22). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern int data_ov034_02114488;
extern char MATRIX_SCRATCH_PAPER[];

struct Vec3 { int x, y, z; };

extern "C" int _ZN5Actor18HorzAngleToCPlayerEv(void *self);
extern "C" void Matrix4x3_FromRotationY(void *m, int angle);
extern "C" void MulVec3Mat4x3(struct Vec3 *in, void *m, struct Vec3 *out);
extern "C" void Vec3_Add(struct Vec3 *out, struct Vec3 *a, struct Vec3 *b);

extern "C" void func_ov034_02112484(char *sl)
{
    int sb;
    struct Vec3 in;
    struct Vec3 out;
    struct Vec3 sum;
    char *r8 = sl;
    char *r7 = sl;
    *(int *)(sl + 0x8c8) = 0;
    *(int *)(sl + 0x98) = data_ov034_02114488;
    for (sb = 0; sb < 5; sb++) {
        if (sb == 0) {
            *(int *)(r8 + 0x3cc) = *(int *)(sl + 0x5c);
            *(int *)(r8 + 0x3d0) = *(int *)(sl + 0x60);
            *(int *)(r8 + 0x3d4) = *(int *)(sl + 0x64);
            *(short *)(r7 + 0x400 + 0x46) = (short)_ZN5Actor18HorzAngleToCPlayerEv(sl);
            *(short *)(sl + 0x94) = *(short *)(r7 + 0x400 + 0x46);
        } else {
            int r6 = sb - 1;
            in.x = 0;
            in.y = 0;
            in.z = -*(int *)(sl + sb * 4 + 0x464);
            out.x = 0;
            out.y = 0;
            out.z = 0;
            Matrix4x3_FromRotationY(MATRIX_SCRATCH_PAPER, (short)(*(short *)(sl + 0x400 + 0x46) + 0x200));
            MulVec3Mat4x3(&in, MATRIX_SCRATCH_PAPER, &out);
            Vec3_Add(&sum, (struct Vec3 *)(sl + 0x3cc + r6 * 0xc), &out);
            *(int *)(r8 + 0x3cc) = sum.x;
            *(int *)(r8 + 0x3d0) = sum.y;
            *(int *)(r8 + 0x3d4) = sum.z;
            *(short *)(r7 + 0x400 + 0x46) =
                *(short *)(sl + r6 * 6 + 0x400 + 0x46);
        }
        r8 += 0xc;
        r7 += 6;
    }
}
