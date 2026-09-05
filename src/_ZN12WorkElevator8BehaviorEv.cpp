//cpp
// @symbol _ZN12WorkElevator8BehaviorEv
/* WorkElevator::Behavior (ov021 0x021116c8, 0x5a4 bytes), a real C++ method on
 * include/WorkElevator.h. Per frame: while a platform is lowered (unk_c7d) the
 * spin timer unk_c78 counts down and tilts the elevator (angle X or Z, sign by
 * mLoweredPlatform) from the sine table; when it runs out the lowered index
 * steps (0/2 up, 1/3 down) and the pose resets to the table angle at
 * data_ov021_02114740. Otherwise, with a platform selected, it plays the
 * motor sound, sets the yaw from the table, and raycasts three line
 * colliders (dBgCh_Lin, constructed and destroyed here) from the elevator
 * position through the rotated arm offsets va[]; a hit within 0x1f000 starts
 * the lowered state. The rest is the shared tail: slerp the pose quaternion
 * (unk_c4c[0..3]) toward the target (unk_c4c[4..7]), snap to mTargetPos when
 * the hold timer unk_c74 expires, and run the two platform updaters.
 *
 * Shapes that were load-bearing (see notes 6bs/6bt on the lane):
 *   - the arm offsets are written through a per-element pointer (`Vector3 *p =
 *     &va[1]; p->x = ...`): the ROM computes each element base with
 *     `add rN, sp, #imm` and stores through it; direct `va[1].x` folds to
 *     [sp, #imm] and loses eight words. Element 0 is written directly.
 *   - va, vb, vc, sp74 declared in that order (ascending frame slots).
 *   - the spin RMW is `*(u16 *)(c + 0xc76) += 0x1f00` and the angle re-read is
 *     the member with a double cast, (u16)(s16)unk_c76, which reloads with
 *     ldrsh and the u16 narrowing pair instead of forwarding the stored value.
 *   - the two quaternion resets read data_02092768 through volatile: the ROM
 *     loads all eight words, plain reads CSE the second group.
 *   - the raycast loop uses a fresh counter declared first, then pc, pb, pl,
 *     and bumps pc before pb (colouring: sl, sb, r8, r7).
 *   - three separate dBgCh_Lin locals in a nested scope give the ROM's three
 *     C1 calls, the loop over them by pointer, and the reverse D1 order after
 *     UpdatePos. */
#include "WorkElevator.h"
#include "dBgCh_Lin.h"

extern "C" {
u16 DecIfAbove0_Short(u16 *p);
u8 DecIfAbove0_Byte(u8 *p);
u32 _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 a, u32 b, u32 c, const void *v, s16 d);
void _ZN5Sound9PlayBank3EjRK7Vector3(u32 a, const void *v);
void Matrix4x3_FromRotationY(Matrix4x3 *m, s16 angle);
void MulVec3Mat4x3(const Vector3 *src, const Matrix4x3 *mtx, Vector3 *dst);
void AddVec3(const Vector3 *a, const Vector3 *b, Vector3 *dst);
void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(dBgCh_Lin *self, const Vector3 *a, const Vector3 *b, dActor_c *actor);
s32 _ZN9dBgCh_Lin10DetectClsnEv(dBgCh_Lin *self);
void Quaternion_SLerp(void *q0, void *q1, int t, void *out);
s32 Vec3_Equal(const Vector3 *a, const Vector3 *b);
void func_ov021_02111434(WorkElevator *c);
void func_ov021_0211129c(WorkElevator *c);
extern s16 data_02082214[];
extern u16 data_ov021_02114740[];
extern Matrix4x3 data_020a0e68;
extern s32 data_02092768[];
}

s32 WorkElevator::Behavior()
{
    char *c = (char *)this;
    Vector3 va[3];
    Vector3 vb[3];
    Vector3 vc[3];
    Vector3 sp74;
    s32 i;

    if (unk_c7d != 0) {
        if (DecIfAbove0_Short(&unk_c78) != 0) {
            *(u16 *)(c + 0xc76) += 0x1f00;
            {
                s16 v = data_02082214[((u16)(s16)unk_c76 >> 4) * 2];
                s32 prod = unk_c78 * 0x1a;
                s8 sel = mLoweredPlatform;
                s16 t = (s16)(s32)(((long long)prod * v + 0x800) >> 12);
                switch (sel) {
                case 0: mAngleX = -t; break;
                case 1: mAngleX = t; break;
                case 2: mAngleZ = t; break;
                case 3: mAngleZ = -t; break;
                }
            }
        } else {
            s8 s = mLoweredPlatform;
            if (s == 0 || s == 2) {
                s8 *p = &mLoweredPlatform;
                *p += 1;
            } else {
                s8 *p = &mLoweredPlatform;
                *p -= 1;
            }
            unk_c7d = 0;
            mPrevAngleY = data_ov021_02114740[mLoweredPlatform];
            mAngleX = 0;
            mAngleZ = 0;
            mPosY = mTargetPos.y;
        }
        if (unk_c7b != mLoweredPlatform) {
            unk_c7d = 0;
            mPrevAngleY = data_ov021_02114740[mLoweredPlatform];
            mAngleX = 0;
            mAngleZ = 0;
            mPosY = mTargetPos.y;
        }
    } else {
        if (mLoweredPlatform != -1) {
            
            unk_c70 = _ZN5Sound8PlayLongEjjjRK7Vector3s(unk_c70, 3, 0x88, &mCamSpacePosX, 0);
            mPrevAngleY = data_ov021_02114740[mLoweredPlatform];
            mHorzSpeed = 0x5000;
            va[0].x = 0; va[0].y = -0xa000; va[0].z = 0x10e000;
            {
                Vector3 *p = &va[1];
                p->x = 0x12c000; p->y = -0xa000; p->z = 0x10e000;
                p = &va[2];
                p->x = -0x12c000; p->y = -0xa000; p->z = 0x10e000;
            }
            vb[0].x = 0; vb[0].y = 0; vb[0].z = 0;
            {
                Vector3 *p = &vb[1];
                p->x = 0; p->y = 0; p->z = 0;
                p = &vb[2];
                p->x = 0; p->y = 0; p->z = 0;
            }
            vc[0].x = 0; vc[0].y = 0; vc[0].z = 0;
            {
                Vector3 *p = &vc[1];
                p->x = 0; p->y = 0; p->z = 0;
                p = &vc[2];
                p->x = 0; p->y = 0; p->z = 0;
            }
            Matrix4x3_FromRotationY(&data_020a0e68, mPrevAngleY);
            {
                Vector3 *pa = va;
                Vector3 *pb = vb;
                Vector3 *pc = vc;
                for (i = 0; i < 3; i++) {
                    sp74 = *pa;
                    sp74.z += 0x28000;
                    MulVec3Mat4x3(pa, &data_020a0e68, pb);
                    MulVec3Mat4x3(&sp74, &data_020a0e68, pc);
                    AddVec3(pb, (Vector3 *)&mPosX, pb);
                    AddVec3(pc, (Vector3 *)&mPosX, pc);
                    pa++; pb++; pc++;
                }
            }
            {
                dBgCh_Lin l0;
                dBgCh_Lin l1;
                dBgCh_Lin l2;
                {
                    s32 j = 0;
                    Vector3 *pc = vc;
                    Vector3 *pb = vb;
                    dBgCh_Lin *pl = &l0;
                    u32 snd = 0x1b;
                    for (; j < 3; j++) {
                        _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(pl, pb, pc, this);
                        if (_ZN9dBgCh_Lin10DetectClsnEv(pl) != 0 && pl->clsnDist <= 0x1f000) {
                            mHorzSpeed = 0;
                            unk_c7d = 1;
                            unk_c78 = 0x3c;
                            _ZN5Sound9PlayBank3EjRK7Vector3(snd, &mCamSpacePosX);
                        }
                        pc++; pb++; pl++;
                    }
                }
                UpdatePos(0);
            }
        }
        if (DecIfAbove0_Byte(&unk_c7c) == 0) {
            unk_c4c[4] = data_02092768[0];
            unk_c4c[5] = data_02092768[1];
            unk_c4c[6] = data_02092768[2];
            unk_c4c[7] = data_02092768[3];
        }
    }
    Quaternion_SLerp(&unk_c4c[0], &unk_c4c[4], 0x199, &unk_c4c[0]);
    if (Vec3_Equal((Vector3 *)&mPosX, &mTargetPos) != 0)
        unk_c74 = 0x12c;
    if (DecIfAbove0_Short(&unk_c74) == 0) {
        mPosX = mTargetPos.x;
        mPosY = mTargetPos.y;
        mPosZ = mTargetPos.z;
        unk_c4c[0] = *(volatile s32 *)&data_02092768[0];
        unk_c4c[1] = *(volatile s32 *)&data_02092768[1];
        unk_c4c[2] = *(volatile s32 *)&data_02092768[2];
        unk_c4c[3] = *(volatile s32 *)&data_02092768[3];
        unk_c4c[4] = *(volatile s32 *)&data_02092768[0];
        unk_c4c[5] = *(volatile s32 *)&data_02092768[1];
        unk_c4c[6] = *(volatile s32 *)&data_02092768[2];
        unk_c4c[7] = *(volatile s32 *)&data_02092768[3];
        mLoweredPlatform = -1;
        unk_c76 = 0;
        unk_c7d = 0;
        unk_c78 = 0;
        mAngleX = 0;
        mAngleZ = 0;
        mHorzSpeed = 0;
    }
    if (unk_c7d == 0 && unk_c74 < 0x10e) {
        mLoweredPlatform = -1;
        mHorzSpeed = 0;
    }
    func_ov021_02111434(this);
    func_ov021_0211129c(this);
    unk_c7e = 0;
    mHorzSpeed = 0;
    if (unk_c7b != mLoweredPlatform)
        _ZN5Sound9PlayBank3EjRK7Vector3(0x3e, &mCamSpacePosX);
    unk_c7b = mLoweredPlatform;
    return 1;
}
