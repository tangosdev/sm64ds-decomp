//cpp
/**
 * daObjSimpleLift_c -- the plain sliding slab (ov091, 0x02132404..0x02132938).
 *
 * Seven registry profiles share this class: BK_TRANSBAR, KM1_DERU,
 * KM2_RIFUT01, KM2_RIFUT02, KM3_DERU01, KM3_DERU02 and RC_RIFT01. Each
 * classInit allocates 0x330 and installs the same vtable. InitResources
 * maps actorID onto a variant 0..6, which selects the model, the collision
 * mesh, the heading offset and the travel time.
 *
 * Behavior waits out mPauseTimer, then counts mMoveTimer down while
 * UpdatePos slides the slab. At zero it adds a half turn (0x8000) to the
 * heading, reloads the travel time and waits 15 frames. The mesh collider
 * is refreshed only while the slab is in range.
 *
 * The seven daObjSimpleLift_c_classInit_* factories (0x021327e8..0x02132938)
 * are reconstructed names (RTTI daObjSimpleLift_c, the seven registry
 * profiles above); retail does not store them. Historical aliases:
 * SlidingPlatformWf_Spawn (BK_TRANSBAR), SlidingPlatformBfsRectangle_Spawn
 * (KM2_RIFUT02), SlidingPlatformBfsSquare_Spawn (KM2_RIFUT01),
 * SlidingPlatformRr_Spawn (RC_RIFT01), SlidingPlatformBsLong_Spawn
 * (KM3_DERU02), SlidingPlatformBsWide_Spawn (KM3_DERU01),
 * SlidingPlatformBdw_Spawn (KM1_DERU). Each factory is `operator new(0x330)`
 * + the inherited dBgActor_c ctor + this class's vtable store -- exactly
 * `new daObjSimpleLift_c()`, since the class has no user-declared
 * constructor of its own.
 *
 * #pragma defer_codegen off: one out-of-line destructor emits D1 then D0,
 * the cartridge order; the same pragma lays the whole TU down in source
 * order, so this file is ROM-ascending and the classInit factories append
 * after InitResources. (Compare src/actors/daObjHatenaBlock_c.cpp, which has
 * no such pragma and so emits in the reverse of source order -- its
 * classInit block sits at the top of the file instead.)
 *
 * deslop leftovers:
 * - Behavior: dBgActor_c::IsClsnInRange(Fix12<int>, Fix12<int>) homes both
 *   arguments and grows the function from 0xa8 to 0xc8 (+0x20). The scalar
 *   extern keeps the two zeros in registers (6az).
 * - InitResources: dBgW_KcMbg::SetFile with Fix12<int> scale by value grows
 *   the function from 0x214 to 0x220 (+12) and emits two extra .rodata
 *   words. The scalar extern stays (6az).
 * - InitResources: one SetFile with a ternary scale is 0x1d8 against the
 *   ROM's 0x214 (-60). Both LoadFile/SetFile arms stay.
 * - InitResources: assigning mMeshCollider.beforeClsnCallback directly is
 *   0x210 against 0x214 (-4). The ROM calls func_020393d4.
 */

#include "daObjSimpleLift_c.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

/* Per-variant asset row is three pointers, stride 0xc. Model and KCL are
 * data_ov091_02135024 / data_ov091_02135028; the CLPS word is named here.
 * Heading offsets and travel times are data_ov091_02134514 / 02134504.
 * IsClsnInRange and SetFile stay scalar externs: the Fix12<int> method
 * forms size-DIFF (see the file comment). func_020393d4 stores the
 * beforeClsn callback; an inlined store is four bytes short. */
struct SimpleLiftModelFilePtr : SharedFilePtr {
    u32 words[2];

    SimpleLiftModelFilePtr(u32 fileID);
    ~SimpleLiftModelFilePtr();
};

struct SimpleLiftClsnFileHandle : SharedFilePtr {
    u32 words[2];

    SimpleLiftClsnFileHandle(u32 fileID);
    ~SimpleLiftClsnFileHandle();
};

extern "C" {
unsigned char DecIfAbove0_Byte(unsigned char *p);
unsigned short DecIfAbove0_Short(unsigned short *p);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *mc, void *kcl, void *mtx, int scale, s16 angY, void *clps);
void func_020393d4(void *collider, void *callback);
extern char data_ov091_0213502c[];

extern SimpleLiftModelFilePtr  data_ov091_02135520;
extern SimpleLiftClsnFileHandle data_ov091_02135538;
extern SimpleLiftModelFilePtr  data_ov091_02135560;
extern SimpleLiftClsnFileHandle data_ov091_021354f8;
extern SimpleLiftModelFilePtr  data_ov091_02135528;
extern SimpleLiftClsnFileHandle data_ov091_02135518;
extern SimpleLiftModelFilePtr  data_ov091_02135558;
extern SimpleLiftClsnFileHandle data_ov091_02135550;
extern SimpleLiftModelFilePtr  data_ov091_02135510;
extern SimpleLiftClsnFileHandle data_ov091_02135540;
extern SimpleLiftModelFilePtr  data_ov091_02135548;
extern SimpleLiftClsnFileHandle data_ov091_02135500;
extern SimpleLiftModelFilePtr  data_ov091_02135508;
extern SimpleLiftClsnFileHandle data_ov091_02135530;
}

/* Emission order is ROM order. Do not reorder. */
#pragma defer_codegen off

// @symbol _ZN17daObjSimpleLift_cD1Ev
// @symbol _ZN17daObjSimpleLift_cD0Ev
/* One definition; mwccarm emits D1 then D0. The body is the inherited
 * dBgW_KcMbg, Model and dActor_c destruction. This class adds nothing
 * that needs destroying. */
daObjSimpleLift_c::~daObjSimpleLift_c()
{
}

// @symbol _ZN17daObjSimpleLift_c16CleanupResourcesEv
int daObjSimpleLift_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    (*(SharedFilePtr **)(data_ov091_02135024 + mVariant * 0xc))->Release();
    (*(SharedFilePtr **)(data_ov091_02135028 + mVariant * 0xc))->Release();
    return 1;
}

// @symbol _ZN17daObjSimpleLift_c6RenderEv
int daObjSimpleLift_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN17daObjSimpleLift_c8BehaviorEv
int daObjSimpleLift_c::Behavior()
{
    if (DecIfAbove0_Byte(&mPauseTimer) == 0) {
        if (DecIfAbove0_Short((unsigned short *)&mMoveTimer) == 0) {
            mMoveTimer = data_ov091_02134504[mVariant];
            mPrevAngleY += 0x8000;
            mPauseTimer = 0xf;
        } else {
            UpdatePos(0);
        }
    }
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
        UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN17daObjSimpleLift_c13InitResourcesEv
int daObjSimpleLift_c::InitResources()
{
    switch (actorID) {
        case 0x37: mVariant = 6; break; /* BK_TRANSBAR */
        case 0x7c: mVariant = 3; break; /* RC_RIFT01 */
        case 0x93: mVariant = 4; break; /* KM2_RIFUT01 */
        case 0x9b: mVariant = 2; break; /* KM3_DERU02 */
        case 0x8a: mVariant = 0; break; /* KM1_DERU */
        case 0x9a: mVariant = 1; break; /* KM3_DERU01 */
        case 0x92: mVariant = 5; break; /* KM2_RIFUT02 */
    }

    void *bmd = Model::LoadFile(
        **(SharedFilePtr **)(data_ov091_02135024 + mVariant * 0xc));
    mModel.SetFile((BMD_File *)bmd, 1, -1);

    mPrevAngleY = mAngleY + data_ov091_02134514[mVariant];
    if (mAngleX != 0)
        mPrevAngleY = mAngleY + mAngleX;

    mMoveTimer = data_ov091_02134504[mVariant];
    mHorzSpeed = 0xa000;
    mBasePosX = mPosX;
    mBasePosY = mPosY;
    mBasePosZ = mPosZ;
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    /* Variant 6 (BK_TRANSBAR) is full scale; the other six are 0x199.
     * Both arms stay duplicated so each LoadFile/SetFile pair is emitted. */
    if (mVariant == 6) {
        int tableOffset = mVariant * 0xc;
        void *kcl = dBgW_Kc::LoadFile(
            **(SharedFilePtr **)(data_ov091_02135028 + tableOffset));
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY,
            *(void **)(data_ov091_0213502c + tableOffset));
    } else {
        int tableOffset = mVariant * 0xc;
        void *kcl = dBgW_Kc::LoadFile(
            **(SharedFilePtr **)(data_ov091_02135028 + tableOffset));
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY,
            *(void **)(data_ov091_0213502c + tableOffset));
    }
    func_020393d4(&mMeshCollider, (void *)dBgW::UpdatePosWithTransform);
    return 1;
}

// @symbol daObjSimpleLift_c_classInit_BK_TRANSBAR
extern "C" daObjSimpleLift_c *daObjSimpleLift_c_classInit_BK_TRANSBAR()
{
    return new daObjSimpleLift_c();
}

// @symbol daObjSimpleLift_c_classInit_KM2_RIFUT02
extern "C" daObjSimpleLift_c *daObjSimpleLift_c_classInit_KM2_RIFUT02()
{
    return new daObjSimpleLift_c();
}

// @symbol daObjSimpleLift_c_classInit_KM2_RIFUT01
extern "C" daObjSimpleLift_c *daObjSimpleLift_c_classInit_KM2_RIFUT01()
{
    return new daObjSimpleLift_c();
}

// @symbol daObjSimpleLift_c_classInit_RC_RIFT01
extern "C" daObjSimpleLift_c *daObjSimpleLift_c_classInit_RC_RIFT01()
{
    return new daObjSimpleLift_c();
}

// @symbol daObjSimpleLift_c_classInit_KM3_DERU02
extern "C" daObjSimpleLift_c *daObjSimpleLift_c_classInit_KM3_DERU02()
{
    return new daObjSimpleLift_c();
}

// @symbol daObjSimpleLift_c_classInit_KM3_DERU01
extern "C" daObjSimpleLift_c *daObjSimpleLift_c_classInit_KM3_DERU01()
{
    return new daObjSimpleLift_c();
}

// @symbol daObjSimpleLift_c_classInit_KM1_DERU
extern "C" daObjSimpleLift_c *daObjSimpleLift_c_classInit_KM1_DERU()
{
    return new daObjSimpleLift_c();
}

/* One model+KCL SharedFilePtr pair per variant, constructed in the retail
 * sinit's interleaved order (variant's model first, then its collision). */
SimpleLiftModelFilePtr data_ov091_02135520(0x647);
SimpleLiftClsnFileHandle data_ov091_02135538(0x648);
SimpleLiftModelFilePtr data_ov091_02135560(0x679);
SimpleLiftClsnFileHandle data_ov091_021354f8(0x67a);
SimpleLiftModelFilePtr data_ov091_02135528(0x67b);
SimpleLiftClsnFileHandle data_ov091_02135518(0x67c);
SimpleLiftModelFilePtr data_ov091_02135558(0x6a9);
SimpleLiftClsnFileHandle data_ov091_02135550(0x6aa);
SimpleLiftModelFilePtr data_ov091_02135510(0x665);
SimpleLiftClsnFileHandle data_ov091_02135540(0x666);
SimpleLiftModelFilePtr data_ov091_02135548(0x667);
SimpleLiftClsnFileHandle data_ov091_02135500(0x668);
SimpleLiftModelFilePtr data_ov091_02135508(0x59f);
SimpleLiftClsnFileHandle data_ov091_02135530(0x5a0);
