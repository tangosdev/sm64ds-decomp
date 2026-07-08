//cpp
// NONMATCHING: register allocation (div=87). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct Vector3 { int x, y, z; };
struct Mat4x3 { int m[12]; };

struct C;
typedef int (C::*PMF)();
struct C { char pad[0x37c]; PMF *pp; };

struct RayParams { Vector3 start, end, in, out; };

extern "C" {
extern void *CAMERA;
extern Mat4x3 MATRIX_SCRATCH_PAPER;
extern Vector3 data_ov073_02123040;
extern char data_ov073_02123320[];
extern char data_ov073_02123330[];
extern char data_ov073_02123340[];
extern char data_ov073_02123350[];
extern char data_ov073_02123360[];
extern char data_ov073_02123380[];
extern char data_ov073_02123390[];
extern char data_ov073_021233a0[];
extern char data_ov073_021233c0[];
extern char data_ov073_021233d0[];
extern char data_ov073_021233f0[];
extern char data_ov073_02123400[];

extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
extern void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void *self, void *clsn);
extern int _ZNK9Animation12WillHitFrameEi(void *self, int f);
extern void MulMat4x3Mat4x3(void *d, void *a, void *b);
extern void Vec3_Lsl(Vector3 *d, Vector3 *s, int sh);
extern void func_02012694(int a, void *b);
extern void _ZN5Actor17HugeLandingDustAtER7Vector3b(void *self, Vector3 *v, int b);
extern void _ZN11RaycastLineC1Ev(void *self);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, int angX);
extern void MulVec3Mat4x3(void *a, void *m, void *out);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void *self, const Vector3 *a, const Vector3 *b, void *actor);
extern int _ZN11RaycastLine10DetectClsnEv(void *self);
extern void _ZN11RaycastLineD1Ev(void *self);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, void *wmc, unsigned int flags);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void *self, const Vector3 *v);
extern void func_ov073_0211f61c(void *self);
extern void _ZN12CylinderClsn5ClearEv(void *self);
extern void _ZN12CylinderClsn6UpdateEv(void *self);
extern void func_ov073_021215cc(void *self);
extern void _ZN14BlendModelAnim7AdvanceEv(void *self);
}

extern "C" int func_ov073_021218a0(C *c)
{
    char *self = (char *)c;
    Vector3 v0;
    RayParams rp;
    Vector3 v3C;
    Vector3 v48;
    Vector3 v54;
    int line[0x1f];

    *(C **)((char *)CAMERA + 0x114) = c;
    DecIfAbove0_Short((unsigned short *)(self + 0x100));

    if (*(void **)((char *)c->pp + 8) != 0) {
        PMF *p = c->pp + 1;
        (c->**p)();
    }

    if ((char *)c->pp != data_ov073_02123400
        && (char *)c->pp != data_ov073_02123320
        && (char *)c->pp != data_ov073_02123340) {
        _ZN5Actor9UpdatePosEP12CylinderClsn(self, self + 0x110);
    } else {
        int sum = *(int *)(self + 0xa8) + *(int *)(self + 0x9c);
        int m = *(int *)(self + 0xa0);
        int ac = *(int *)(self + 0xac);
        if (sum >= m) m = sum;
        *(int *)(self + 0xa8) = m;
        *(int *)(self + 0xac) = ac;
        _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(self, self + 0x110);
    }

    if (((char *)c->pp == data_ov073_02123330 || (char *)c->pp == data_ov073_02123350)
        && _ZNK9Animation12WillHitFrameEi(self + 0x35c, 7) != 0) {
        MATRIX_SCRATCH_PAPER = *(Mat4x3 *)(self + 0x328);
        MulMat4x3Mat4x3(*(char **)(self + 0x320) + 0x60, &MATRIX_SCRATCH_PAPER, &MATRIX_SCRATCH_PAPER);
        v0.x = MATRIX_SCRATCH_PAPER.m[9];
        v0.y = MATRIX_SCRATCH_PAPER.m[10];
        v0.z = MATRIX_SCRATCH_PAPER.m[11];
        Vec3_Lsl(&v3C, &v0, 3);
        v0 = v3C;
        func_02012694(0x167, self + 0x74);
        v48 = v0;
        _ZN5Actor17HugeLandingDustAtER7Vector3b(self, &v48, 1);
    }

    if ((char *)c->pp != data_ov073_021233c0
        && (char *)c->pp != data_ov073_021233d0
        && (char *)c->pp != data_ov073_021233f0
        && (char *)c->pp != data_ov073_02123400
        && (char *)c->pp != data_ov073_02123320
        && (char *)c->pp != data_ov073_02123340
        && (char *)c->pp != data_ov073_02123380) {
        _ZN11RaycastLineC1Ev(line);
        rp.start.x = 0; rp.start.y = 0; rp.start.z = 0;
        rp.end.x = 0; rp.end.y = 0; rp.end.z = 0;
        rp.in.x = 0; rp.in.y = 0; rp.in.z = 0;
        rp.out.x = 0; rp.out.y = 0; rp.out.z = 0;
        rp.start.x = *(int *)(self + 0x5c);
        rp.start.y = *(int *)(self + 0x60);
        rp.start.z = *(int *)(self + 0x64);
        rp.start.y = *(int *)(self + 0x60) + 0x78000;
        if (*(unsigned char *)(self + 0x4cb) > 1)
            rp.in.z = 0x258000;
        else
            rp.in.z = 0x12c000;
        Matrix4x3_FromRotationY(&MATRIX_SCRATCH_PAPER, *(short *)(self + 0x94));
        Matrix4x3_ApplyInPlaceToRotationX(&MATRIX_SCRATCH_PAPER, 0x2000);
        MulVec3Mat4x3(&rp.in, &MATRIX_SCRATCH_PAPER, &rp.out);
        rp.end.x = rp.start.x;
        rp.end.x = rp.start.x + rp.out.x;
        rp.end.y = rp.start.y;
        rp.end.y = rp.start.y + rp.out.y;
        rp.end.z = rp.start.z;
        rp.end.z = rp.start.z + rp.out.z;
        _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(line, &rp.start, &rp.end, self);
        if (_ZN11RaycastLine10DetectClsnEv(line) == 0) {
            if (*(int *)(self + 0x98) > 0xa000) {
                *(unsigned char *)(self + 0x4c9) = 1;
            }
            *(int *)(self + 0x4ec) = *(int *)(self + 0x5c);
            *(int *)(self + 0x4f0) = *(int *)(self + 0x60);
            *(int *)(self + 0x4f4) = *(int *)(self + 0x64);
            if ((char *)c->pp != data_ov073_021233a0) {
                *(int *)(self + 0x5c) = *(int *)(self + 0x68);
                *(int *)(self + 0x60) = *(int *)(self + 0x6c);
                *(int *)(self + 0x64) = *(int *)(self + 0x70);
                *(int *)(self + 0x98) = 0;
            }
        } else {
            *(unsigned char *)(self + 0x4c9) = 0;
        }
        _ZN11RaycastLineD1Ev(line);
    }

    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(self, self + 0x150, 0);

    v54 = data_ov073_02123040;
    _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(self + 0x110, &v54);

    if ((char *)c->pp == data_ov073_02123360
        || (char *)c->pp == data_ov073_02123390) {
        func_ov073_0211f61c(self);
    }
    _ZN12CylinderClsn5ClearEv(self + 0x110);
    _ZN12CylinderClsn6UpdateEv(self + 0x110);
    func_ov073_021215cc(self);
    _ZN14BlendModelAnim7AdvanceEv(self + 0x30c);
    return 1;
}
