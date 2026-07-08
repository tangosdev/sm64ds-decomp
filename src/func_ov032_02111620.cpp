//cpp
// NONMATCHING: different op / idiom (div=58). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" void *Actor_ClosestPlayer(void *self);
extern "C" int func_ov032_02111350(void *c);
extern "C" int func_ov032_02111254(void *c);
extern "C" void Matrix4x3_FromRotationY(void *m, int angle);
extern "C" void Matrix4x3_ApplyInPlaceToRotationX(void *m, int angX);
extern "C" void MulVec3Mat4x3(void *in, void *m, void *out);
extern "C" int Animation_Finished(void *self);
extern "C" int ApproachLinear(short *p, int target, int step);
extern "C" int AngleDiff(int a, int b);
extern "C" void ModelAnim_SetAnim(void *self, void *bca, int a, int fix, unsigned int b);
extern "C" void func_ov032_02111ff4(void *c, void *p);
extern "C" short Vec3_VertAngle(void *a, void *b);

extern int MATRIX_SCRATCH_PAPER;
extern void **data_ov032_02113a50;
extern void *data_ov032_02113a8c;
extern int WATER_HEIGHT;

extern "C" int func_ov032_02111620(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    unsigned char *r5;

    r5 = (unsigned char *)Actor_ClosestPlayer(c);
    if (r5 == 0)
        return 1;

    if (func_ov032_02111350(c) == 1)
        goto zeroblock;
    if (func_ov032_02111254(c) != 0) {
        int in[3];
        int out[3];
        in[2] = 0;
        in[2] = 0x14000;
        in[0] = 0; in[1] = 0; out[0] = 0; out[1] = 0; out[2] = 0;
        Matrix4x3_FromRotationY(&MATRIX_SCRATCH_PAPER, *(short *)(c + 0x8e));
        Matrix4x3_ApplyInPlaceToRotationX(&MATRIX_SCRATCH_PAPER, *(short *)(c + 0x8c));
        MulVec3Mat4x3(in, &MATRIX_SCRATCH_PAPER, out);
        *(int *)(c + 0xa8) = out[1];
        goto afterblock;
    }
zeroblock:
    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0xa4) = 0;
    *(int *)(c + 0xa8) = 0;
    *(int *)(c + 0xac) = 0;
afterblock: ;

    if (*(int *)(c + 0x424) == 0) {
        if (Animation_Finished(c + 0x39c) != 0) {
            ApproachLinear((short *)(c + 0x92), 0, 0x200);
            *(int *)(c + 0x424) = 1;
            *(unsigned char *)(c + 0x428) = 0;
            if (AngleDiff(*(short *)(c + 0x92), 0) < 0x200) {
                *(short *)(c + 0x92) = 0;
                *(short *)(c + 0x400 + 0x2a) = 0x64;
                *(short *)(c + 0x400 + 0x30) = *(short *)(c + 0x8e);
                *(int *)(c + 0xb0) = 3;
                ModelAnim_SetAnim(c + 0x34c, (&data_ov032_02113a50)[1], 0, 0x1000, 0);
                func_ov032_02111ff4(c, &data_ov032_02113a8c);
            }
        }
    } else {
        int vec[3];
        unsigned int n;
        int *p = (int *)(r5 + 0x5c);
        vec[0] = p[0];
        vec[1] = p[1];
        vec[2] = p[2];
        if (WATER_HEIGHT > *(int *)(c + 0x60)) {
            ApproachLinear((short *)(c + 0x92), 0, 0x200);
        } else {
            ApproachLinear((short *)(c + 0x92), Vec3_VertAngle(c + 0x5c, vec), 0x200);
        }
        n = (unsigned int)(*(int *)(c + 0x3a4) << 4) >> 0x10;
        if (n <= 0x14) {
            *(unsigned char *)(c + 0x428) = 0;
        } else if (n < 0x3c) {
            *(unsigned char *)(c + 0x428) = 1;
        } else {
            *(unsigned char *)(c + 0x428) = 0;
        }
    }
    return 1;
}
