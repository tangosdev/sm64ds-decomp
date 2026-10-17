//cpp
/* ov100/daObjPathLift_c -- PATH_LIFT (31).
 *
 * SM64DS proves daObjPathLift_c through RTTI (_ZTI/_ZTS at ov100
 * 0x02148538/0x02148544), the PATH_LIFT registry profile, and the factory's
 * allocation/vptr shape. ov100 is mixed; this is the path lift, not STAR_DOOR
 * / daStarGate_c. Historical project alias: PathLift.
 *
 * mwccarm emits ordinary function sections in reverse source order.
 * classInit is this TU's highest ROM address (0x02147328), so it comes
 * first, ahead of InitResources. The inline destructor declared in
 * daObjPathLift_c emits the retail D1/D0 pair itself and emits no leaf D2
 * body.
 *
 * deslop
 * Leftover: classInit stays the typed C-ABI seam daObjRcCarpet_c already
 *   established for this same dPathLiftActor_c base (see
 *   src/game/actors/d_a_obj_rc_carpet.cpp), not `return new
 *   daObjPathLift_c()`. dPathLiftActor_c has no out-of-line ctor, so the
 *   intermediate vptr store stays the raw `data_ov002_0210af70` address
 *   point (no offset -- that symbol already names the address point in
 *   its own ov002 TU). This TU now defines _ZTV15daObjPathLift_c itself
 *   (the out-of-line destructor is the key function), so the compiler's
 *   own `_ZTV15daObjPathLift_c` label names the object start, two words
 *   before the address point; the final vptr store is
 *   `_ZTV15daObjPathLift_c[2]` (addend 8) to reach it instead of the
 *   RTTI header.
 * Leftover: data_0209f2f8 == 13 is a level-ID check (the current
 *   level); data_0209f2d8 == 1 is the mode-1 check used tree-wide.
 */

#include "daObjPathLift_c.h"
#include "dBgCh_Gnd.h"
#include "SharedFilePtr.h"
#include "nitro/fx/fx.h"

extern "C" {
extern CLPS_Block data_ov002_0210d7d4;
extern void func_ov100_02146e70(daObjPathLift_c *self);
extern int func_ov100_0214700c(daObjPathLift_c *self);

extern void Matrix4x3_FromRotationY(Matrix4x3 *m, short ang);
extern void Matrix4x3_FromRotationXYZExt(Matrix4x3 *dst, int a, int b, int c2);
extern short data_02082214[];
extern signed char data_0209f2f8;
extern unsigned char data_0209f2d8;
extern Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern void func_020393a4(dBgW_KcMbg *p, int v);
extern void func_02039394(dBgW_KcMbg *p, int v);
extern void func_020393d4(dBgW_KcMbg *p, void *v);

/* By-value Fix12<int> is wall 6az: constructing wrapper arguments adds stack
   traffic the ROM does not have. Keep the register-level spelling. */
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, void *kcl, const Matrix4x3 *mtx, int scale, short angY,
    CLPS_Block *clps);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(
    dBgActor_c *self, int a, int b);
void _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
    dActor_c *self, dExtShadowModel_c *sm, Matrix4x3 *mtx, int a, int b, int d,
    unsigned int e);
}

/* The fifth parameter is a signed short, not an unsigned int. Declared
   `unsigned int` this mangles _ZN5Sound8PlayLongEjjjRK7Vector3j. */
namespace Sound {
unsigned int PlayLong(unsigned int, unsigned int, unsigned int,
                      const Vector3 &, short);
}

extern SharedFilePtr data_ov002_0210d9f0;

/* File-scope objects at the end of this file construct the two resource
 * handles (model file 0x9c0d, collision file 0x9c0e). mwcc emits
 * __sinit_d_a_obj_path_lift.cpp from those definitions. The wrapper names
 * are local; the handle constructors and destructors are the ROM
 * resource-family functions, aliased in the manifest. */
struct PathLiftModelFile : SharedFilePtr {
    u32 words[2];

    PathLiftModelFile(u32 fileID);
    ~PathLiftModelFile();
};

struct PathLiftCollisionFilePtr : SharedFilePtr {
    u32 words[2];

    PathLiftCollisionFilePtr(u32 fileID);
    ~PathLiftCollisionFilePtr();
};

extern PathLiftModelFile data_ov100_02148a54;
extern PathLiftCollisionFilePtr data_ov100_02148a5c;

extern "C" {
extern void *_ZN10dBgActor_cC2Ev(dBgActor_c *actor);
extern void *_ZN5ModelC1Ev(Model *model);
extern Model *_ZN5ModelD1Ev(Model *model);
extern void *_ZN7PathPtrC1Ev(PathPtr *path);
extern void *_ZN17dExtShadowModel_cC1Ev(dExtShadowModel_c *model);
extern void __cxa_vec_ctor(
    Model *models, unsigned int count, unsigned int size,
    void (*ctor)(void *), void (*dtor)(void *));
}

extern int data_ov002_0210af70[];
extern int _ZTV15daObjPathLift_c[];

// @symbol daObjPathLift_c_classInit
extern "C" daObjPathLift_c *daObjPathLift_c_classInit()
{
    daObjPathLift_c *actor =
        (daObjPathLift_c *)_ZN7fBase_cnwEj(sizeof(daObjPathLift_c));
    if (actor) {
        _ZN10dBgActor_cC2Ev(actor);
        *(int *)actor = (int)data_ov002_0210af70;
        __cxa_vec_ctor(
            actor->mModels, 3, sizeof(Model),
            (void (*)(void *))_ZN5ModelC1Ev,
            (void (*)(void *))_ZN5ModelD1Ev);
        _ZN7PathPtrC1Ev(&actor->mPath);
        *(int *)actor = (int)&_ZTV15daObjPathLift_c[2];
        _ZN17dExtShadowModel_cC1Ev(&actor->mShadowModel);
    }
    return actor;
}

// @symbol _ZN15daObjPathLift_c13InitResourcesEv
int daObjPathLift_c::InitResources()
{
    Vector3 pos;
    Model::LoadFile(data_ov002_0210d9f0);
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov100_02148a54), 1, -1);
    mShadowModel.InitCuboid();
    func_ov100_0214700c(this);
    UpdateClsnPosAndRot();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, dBgW_Kc::LoadFile(data_ov100_02148a5c),
        &mClsnMat, 0x1000, mAngleY, &data_ov002_0210d7d4);
    func_020393d4(
        &mMeshCollider,
        (void *)&dBgW::UpdatePosAndAngs);
    mPathSpeed = 0xa000;
    mHorzSpeed = mPathSpeed;
    BaseInitResources();
    mPathDirection = 1;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y -= 0x14000;
    {
        dBgCh_Gnd rg;
        int b;
        rg.SetObjAndPos(pos, 0);
        mGroundY = pos.y;
        if (rg.DetectClsn() != 0)
            mGroundY = rg.clsnY;
        unk_42c = 1;
        b = (data_0209f2d8 == 1);
        if (b)
            mTimer = 0xb4;
    }
    return 1;
}

// @symbol _ZN15daObjPathLift_c8BehaviorEv
int daObjPathLift_c::Behavior()
{
    UpdatePathModels();
    BaseBehavior();
    if (Vec3_Dist(&mPos, &mPrevPos) != 0) {
        if (DecIfAbove0_Byte(&mTimer) == 0) {
            mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x82, mCamSpacePos, 0);
        }
    }
    func_ov100_0214700c(this);
    UpdateClsnPosAndRot();
    func_ov100_02146e70(this);
    func_020393a4(&mMeshCollider, 0x150000);
    func_02039394(&mMeshCollider, 0x1000);
    int b = (int)(data_0209f2d8 == 1);
    if (b != 0) {
        if (mMeshCollider.IsEnabled() == 0) {
            mMeshCollider.Enable(this);
        }
    } else {
        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0);
    }
    return 1;
}

// @symbol _ZN15daObjPathLift_c6RenderEv
int daObjPathLift_c::Render()
{
    unsigned short h = mWaitTimer;
    if (h < 0x5a) {
        if (h & 1)
            return 1;
    }
    RenderPathModels();
    mModel.Render(0);
    return 1;
}

// @symbol _ZN15daObjPathLift_c16CleanupResourcesEv
int daObjPathLift_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov002_0210d9f0.Release();
    data_ov100_02148a54.Release();
    data_ov100_02148a5c.Release();
    return 1;
}

// @symbol func_ov100_0214700c
extern "C" int func_ov100_0214700c(daObjPathLift_c *self)
{
    Matrix4x3_FromRotationXYZExt(
        &self->mModel.mat4x3, self->mAngleX, self->mAngleY, 0);
    /* Direct nested-member stores make b56 materialise a second matrix base
       and grow this function. Retail keeps `this` as the base for the three
       translation stores. */
    *(s32 *)((char *)self + 0x114) = self->mPosX >> 3;
    *(s32 *)((char *)self + 0x118) = self->mPosY >> 3;
    int z = self->mPosZ >> 3;
    *(s32 *)((char *)self + 0x11c) = z;
    return z;
}

// @symbol func_ov100_02146e70
extern "C" void func_ov100_02146e70(daObjPathLift_c *self)
{
    int scaleZ;
    int yOff;
    int groundDepth;
    int idx;
    int sinVal;
    int cosVal;
    int t;

    if (data_0209f2f8 == 13)
        return;
    if (self->mGroundY > self->mPosY)
        return;

    groundDepth = (self->mPosY - self->mGroundY) + 0x64000;

    idx = (unsigned short)self->mAngleX >> 4;
    sinVal = data_02082214[idx << 1];
    yOff = FX_MUL(sinVal, 0x17c000);
    if (yOff < 0)
        yOff = -yOff;
    cosVal = data_02082214[(idx << 1) + 1];
    scaleZ = FX_MUL(cosVal, 0x17c000);
    if (scaleZ < 0)
        scaleZ = -scaleZ;

    Matrix4x3_FromRotationY((Matrix4x3 *)self->unk_478, self->mAngleY);
    *(s32 *)((char *)self + 0x49c) = self->mPosX >> 3;
    *(s32 *)((char *)self + 0x4a0) = (self->mPosY - yOff) >> 3;
    *(s32 *)((char *)self + 0x4a4) = self->mPosZ >> 3;

    t = data_0209f2d8 == 1;
    if (t != false) {
        _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
            self, &self->mShadowModel, (Matrix4x3 *)self->unk_478, 0x118000,
            0x7d0000, scaleZ, 0xf);
    } else {
        _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
            self, &self->mShadowModel, (Matrix4x3 *)self->unk_478, 0x118000,
            groundDepth, scaleZ, 0xf);
    }
}
// @symbol _ZN15daObjPathLift_cD1Ev
// @symbol _ZN15daObjPathLift_cD0Ev
/* -------------------------------------------------------------------------- */
/* ROM ordinals 0 and 1 -- the destructor group.                              */
/*   _ZN15daObjPathLift_cD1Ev  0x02146d7c  size 0x70                          */
/*   _ZN15daObjPathLift_cD0Ev  0x02146dec  size 0x84                          */
/* Both come from the `~daObjPathLift_c() {}` in the header: the compiler     */
/* writes the group, and only the INLINE form writes it in the cartridge's    */
/* order. An out-of-line definition here emits D2, D0, D1 instead, which is   */
/* why this class's destructor is declared with a body and defined nowhere    */
/* in this file. mwcc still emits _ZTV15daObjPathLift_c for it, so the        */
/* vtable and the RTTI records stay comparable to the cartridge -- see this   */
/* TU's compiler_only_output rows, which check every one of them and then     */
/* discard the copy dsd already delinks.                                     */
/* -------------------------------------------------------------------------- */

/* Static-init globals (was the handwritten __sinit_ov100_02147d7c shard).
 * Definition order is the retail initializer's construction order: the
 * model handle (file 0x9c0d), then the collision handle (file 0x9c0e). */
PathLiftModelFile data_ov100_02148a54(0x9c0d);
PathLiftCollisionFilePtr data_ov100_02148a5c(0x9c0e);
