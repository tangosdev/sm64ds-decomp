//cpp
/* ov002/daChScene_c -- EXIT (349), a scene-transition trigger volume.
 *
 * ov002 is mixed. RTTI at ov002 0x02108680 reads `11daChScene_c`; the debug
 * table names EXIT. Historical coined alias: Exit. Ugly RTTI name is final.
 *
 * mwccarm emits ordinary function sections in reverse source order. Keep the
 * ROM-high factory first. The inline destructor in daChScene_c emits the
 * retail D1/D0 pair first and emits no D2 body. Leaf: _ZTI at ov002
 * 0x02108674 is pointed at by exactly one word, 0x021086b0, this class's
 * own vtable slot.
 *
 * deslop leftovers:
 * - dCamera_c::SetFlag_3 / LookAtExit stay mangled: dCamera_c.h has no LookAtExit,
 *   and dCamera_c.h is out of scope.
 * - func_ov002_020b0a0c writes the exit band and calls LoadLevel; a member
 *   spelling would change the ROM symbol.
 * - LoadLevel / StartExitFaderWipe / func_02012790 / IsStarCollectedInLevel
 *   stay free functions; no owning header.
 * - data_0209f394 / data_0209f250 / data_0209f318 / data_0209f2c0 /
 *   data_0209f2f8 / data_0209f220 / data_02092110 / data_020a0e68 are arm9
 *   globals this TU does not own.
 */

#include "common.h"
#include "daChScene_c.h"
#include "dScene_c.h"
#include "dFdColor_c.h"
#include "Player.h"
#include "dCamera_c.h"

extern "C" {
extern void LoadLevel(s8 levelID, u8 entranceID, s8 starID, u32 d, s8 e);
extern u8 data_0209f2c0[];
extern void MulVec3Mat4x3(void *in, void *m, void *out);
extern void InvMat4x3(void *in, void *out);
extern void func_ov002_020b0a0c(daChScene_c *self);
extern void StartExitFaderWipe(int a);
extern int Vec3_Dist(void *a, void *b);
extern void func_02012790(int id);
extern unsigned char data_0209f250;
extern dActor_c *data_0209f394[];
extern signed char data_02092110;
extern signed char data_0209f2f8;
extern unsigned char data_0209f220;
extern int IsStarCollectedInLevel(signed char levelID, int starID);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, short angY);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, short angX);
}

extern void *data_0209f318;
extern dFdColor_c data_0209f5e8;
extern Matrix4x3 data_020a0e68;

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daChScene_c_classInit
/* Reconstructed source-style name: SM64DS proves daChScene_c through RTTI,
 * allocation size, vtable identity, and the CH_SCENE registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: daChScene_c_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`. */
extern "C" daChScene_c *daChScene_c_classInit()
{
    return new daChScene_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daChScene_c13InitResourcesEv
s32 daChScene_c::InitResources()
{
    if (data_0209f2f8 == 8 && data_0209f220 != 1) {
        if (IsStarCollectedInLevel(8, 1) != 0) return 0;
    }

    if ((unsigned char)((unsigned int)param1 >> 24) == 0x12) {
        if (IsStarCollectedInLevel(0x12, 1) != 0) {
            mPosX += 0x802000;
        }
    }

    if (data_0209f2f8 == 0x20) {
        mScaleX = 0x2bc0000;
        mScaleY = 0x3200000;
    } else if (data_0209f2f8 == 0x22) {
        mScaleX = 0x3e80000;
        mScaleY = 0x3e80000;
    } else {
        mScaleX = (unsigned int)(((param1 & 0xf) + 1) * 0x64000) >> 1;
        mScaleY = ((((unsigned int)param1 >> 4) & 0xf) + 1) * 0x64000;
    }

    if (mAngleX == 0) {
        mScaleX += 0x64000;
        mScaleY += 0x64000;
        mPosY -= 0x32000;
    }

    Matrix4x3_FromTranslation(&mInvMat, mPosX, mPosY, mPosZ);
    Matrix4x3_ApplyInPlaceToRotationY(&mInvMat, -mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&mInvMat, -mAngleX);
    InvMat4x3(&mInvMat, &mInvMat);

    mPosY += mScaleY >> 1;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daChScene_c8BehaviorEv
s32 daChScene_c::Behavior()
{
    int out1[3];
    int out2[3];
    dActor_c *player = data_0209f394[data_0209f250];
    if (mHorzSpeed != 0) {
        MulVec3Mat4x3(&player->mPosX, &mInvMat, out1);
        if (out1[2] > mHorzSpeed || out1[2] < -0x300000) {
            out1[2] = mHorzSpeed;
            data_020a0e68 = mInvMat;
            InvMat4x3(&data_020a0e68, &data_020a0e68);
            MulVec3Mat4x3(out1, &data_020a0e68, &player->mPosX);
        }
        mHorzSpeed -= 0x20000;
        if (mHorzSpeed < -0x300000) {
            mHorzSpeed = -0x300000;
            if (data_02092110 < 0) {
                func_ov002_020b0a0c(this);
                dScene_c::SetAndStopColorFader();
                data_0209f5e8.color = 0x7fff;
            }
        }
    } else {
        if (data_02092110 < 0) {
            if (((Player *)player)->IsStateEnteringLevel() == 0) {
                MulVec3Mat4x3(&player->mPosX, &mInvMat, out2);
                {
                    int a = out2[0];
                    if (a < 0)
                        a = -a;
                    if (a <= mScaleX) {
                        if (out2[1] >= 0 && out2[1] <= mScaleY) {
                            if ((out2[2] ^ mScaleZ) < 0) {
                                void *cam;
                                mAngleZ = 0;
                                if (out2[1] > 0x100000) {
                                    mAngleZ = 2;
                                } else if (out2[1] > 0x60000) {
                                    mAngleZ = 1;
                                }
                                cam = data_0209f318;
                                if (((Player *)player)->Unk_020c9e5c(7)) {
                                    func_ov002_020b0a0c(this);
                                    StartExitFaderWipe(6);
                                    ((dCamera_c *)cam)->SetFlag_3();
                                } else {
                                    ((Player *)player)->SetNoControlState(6, -1, 0);
                                    if (mAngleX != 0) {
                                        dActor_c *o;
                                        func_ov002_020b0a0c(this);
                                        StartExitFaderWipe(5);
                                        ((dCamera_c *)cam)->SetFlag_3();
                                        o = dActor_c::FindWithActorID(0x12a, 0);
                                        while (o != 0) {
                                            if (Vec3_Dist(&o->mPosX, &mPosX) < 0x12c000)
                                                break;
                                            o = dActor_c::FindWithActorID(0x12a, o);
                                        }
                                        if (o != 0) {
                                            func_02012790(0x16);
                                        } else if (data_02092110 == 0xd) {
                                            func_02012790(0x1e);
                                        } else {
                                            func_02012790(0x19);
                                        }
                                    } else {
                                        int t = (signed char)(int)(((unsigned int)param1 >> 0x18));
                                        if (t == 0x1b || t == 0x12) {
                                            func_ov002_020b0a0c(this);
                                            dScene_c::SetAndStopColorFader();
                                            data_0209f5e8.color = 0x7fff;
                                        }
                                        mHorzSpeed = out2[2];
                                        ((dCamera_c *)cam)->LookAtExit(*this);
                                        func_02012790(0x1e);
                                    }
                                }
                            }
                        }
                    }
                }
                mScaleZ = out2[2];
            }
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daChScene_c6RenderEv
s32 daChScene_c::Render()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daChScene_c16OnPendingDestroyEv
void daChScene_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daChScene_c16CleanupResourcesEv
s32 daChScene_c::CleanupResources()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020b0a0c
extern "C" {
void func_ov002_020b0a0c(daChScene_c *self)
{
    data_0209f2c0[0] = (u8)self->mAngleZ;
    {
        u32 v = self->param1;
        u32 hi = v >> 0x18;
        u32 mid = (v >> 8) & 0xff;
        if (mid == 0xff) mid = -1;
        LoadLevel((s8)hi, (u8)(v >> 0x10), -1, 0, (s8)mid);
    }
}
}
/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN11daChScene_cD0Ev, 0x020b09d4, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daChScene_cD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 destroys through this class and its bases, then returns the object to its
 * heap; the deallocation is dActor_c's inline `operator delete`, which is why
 * nothing here mentions a heap. Nobody writes that body -- declaring the
 * destructor is enough.
 *
 * Deleted here twice over. The legacy file carried a hand-written mangled-name
 * D0 free function, which next to a real out-of-line D1 in the same TU is an
 * mwcc internal compiler error (ELFgen.c:483), confirmed on
 * InvisiblePole/daCamTag_c/daChRoom_c; and the out-of-line D1 it sat beside is
 * now gone too, into the class body. With the destructor inline, mwcc emits
 * exactly D1 then D0 and no D2 -- the ROM's shape -- so both variants are
 * licensed functions of this TU and neither needs a deadstrip policy row. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN11daChScene_cD1Ev, 0x020b09b0, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daChScene_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vptr store and the base chain's own destruction, every bit of it a
 * consequence of `struct daChScene_c : dActor_c`. This class adds no member
 * with a destructor of its own; Matrix4x3 is a plain aggregate.
 *
 * The destructor body lives in the class declaration in
 * include/daChScene_c.h. Out of line here, mwcc emitted D2, D0, D1; the ROM
 * has D1 at 0x020b09b0 then D0 at 0x020b09d4 and no D2, and production
 * isolation places .text into the spanning delink in emission order, so the
 * out-of-line spelling fails linkcheck even with every byte matching. Inline
 * in the class body it emits D1, D0 and no D2. */
