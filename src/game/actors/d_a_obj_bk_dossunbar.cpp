//cpp
/**
 * Whomp's Fortress sliding bars (`dossunbar`). One class, two
 * spawn profiles (BK_DOSSUNBAR_L / BK_DOSSUNBAR_S). A seven-state
 * cycle slides the mesh out from home and back.
 *
 * daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_L / _S are reconstructed
 * (RTTI daObjBk_Dossunbar_c, those registry ids). Retail does not
 * store those spellings.
 *
 * deslop
 * Leftover: state bodies stay extern "C" (func_ov015_02111ce0..02111fb8).
 *   A C++ method would emit _ZN19daObjBk_Dossunbar_c* and miss the
 *   unowned PMF records. Tables 02114a24 / 021149ec are filled by
 *   __sinit_ov015_02113048, not this TU.
 * Leftover: dBgW_KcMbg::SetFile, IsClsnInRange, Particle::System::NewSimple
 *   stay mangled (Fix12-by-value, wall 6az). SetFile as a method
 *   size-DIFF Init.
 * Leftover: unk_0a4 is dActor_c's X speed (velocity at 0xa4/a8/ac),
 *   not a field of this class.
 * Leftover: resource table is two { model, KCL, CLPS } rows. Init must
 *   keep the three column symbols 02114534/38/3c; a typed
 *   ResourceDescriptor[2] size-DIFF Init and changes reloc destinations.
 *   sinit file IDs 0x58b / 0x58c / 0x58d / 0x58e. Text-only TU, so
 *   g_profile_BK_DOSSUNBAR_L / _S are not defined here (S14).
 * Leftover: func_01ffb0a4 / func_01ffb07c are MeshCollider ITCM
 *   (flag 0x35 / vec at +0x38). func_020393d4 stores the BeforeClsn
 *   callback; 020396d0 and Math_Function_0203b0fc / 0203b14c names
 *   belong with those callees.
 */

#include "daObjBk_Dossunbar_c.h"
#include "Sound.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

typedef void (daObjBk_Dossunbar_c::*PMF)();
struct StateEntry { PMF handler; };

/* The installer walks the enter-table records by hand as {ptr, adj}. */
typedef void (*FnPtr)(void *);
struct VtEntry {
    int field0;
    int field1;
};

enum { kMovingBarBigActorId = 0x35 };

extern "C" {
/* Three column symbols of one two-row {model, KCL, CLPS} table.
   A typed ResourceDescriptor[2] changes Init reloc destinations. */
extern SharedFilePtr *data_ov015_02114534;
extern SharedFilePtr *data_ov015_02114538;
extern CLPS_Block *data_ov015_0211453c;
extern StateEntry data_ov015_021149ec[];
extern VtEntry data_ov015_02114a24[];

void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, Fix12i x, Fix12i y, Fix12i z);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *, int, int);
int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *, int, void *, int, int, void *);

void Math_Function_0203b0fc(int *p, int target, int scale, int max);
void Math_Function_0203b14c(int *p, int target, int scale, int max, int extra);
void func_01ffb0a4(dBgW_Kc *self);
void func_01ffb07c(dBgW_Kc *self, const Vector3 *v);
void func_020393d4(dBgW *self, void *fn);
void func_020396d0(dBgW *self, int v);

void func_ov015_02111fb8(daObjBk_Dossunbar_c *self, int idx);
}

// @symbol daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_S
extern "C" daObjBk_Dossunbar_c *daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_S()
{
    return new daObjBk_Dossunbar_c();
}

// @symbol daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_L
extern "C" daObjBk_Dossunbar_c *daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_L()
{
    return new daObjBk_Dossunbar_c();
}

// @symbol _ZN19daObjBk_Dossunbar_c13InitResourcesEv
s32 daObjBk_Dossunbar_c::InitResources()
{
    /* Temporary is load-bearing: ROM materialises 0/1 then stores. */
    int isBig = (actorID == kMovingBarBigActorId) ? 1 : 0;
    if (isBig)
        mVariant = 1;
    else
        mVariant = 0;

    int j0 = mVariant * 0xc;
    void *modelFile = Model::LoadFile(
        **(SharedFilePtr **)((char *)&data_ov015_02114534 + j0));
    mModel.SetFile((BMD_File *)modelFile, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    int j = mVariant * 0xc;
    void *kclFile = dBgW_Kc::LoadFile(
        **(SharedFilePtr **)((char *)&data_ov015_02114538 + j));
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (int)kclFile, &mClsnMat, 0x1000, mAngleY,
        *(void **)((char *)&data_ov015_0211453c + j));
    func_020393d4(
        &mMeshCollider,
        (void *)&dBgW::UpdatePosWithVelocity);

    int tmp[3];
    tmp[0] = 0x1000;
    tmp[1] = 0;
    tmp[2] = 0;
    func_01ffb0a4(&mMeshCollider);
    func_01ffb07c(&mMeshCollider, (const Vector3 *)tmp);
    func_020396d0(&mMeshCollider, 0xccd);

    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    func_ov015_02111fb8(this, 5);
    return 1;
}

// @symbol _ZN19daObjBk_Dossunbar_c8BehaviorEv
s32 daObjBk_Dossunbar_c::Behavior()
{
    (this->*(data_ov015_021149ec[mState].handler))();
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN19daObjBk_Dossunbar_c6RenderEv
s32 daObjBk_Dossunbar_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN19daObjBk_Dossunbar_c16CleanupResourcesEv
s32 daObjBk_Dossunbar_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    (*(SharedFilePtr **)((char *)&data_ov015_02114534 + mVariant * 0xc))->Release();
    (*(SharedFilePtr **)((char *)&data_ov015_02114538 + mVariant * 0xc))->Release();
    return 1;
}

/* Install mState and run that state's enter function. */
// @symbol func_ov015_02111fb8
extern "C" void func_ov015_02111fb8(daObjBk_Dossunbar_c *self, int idx)
{
    VtEntry *e = (VtEntry *)((char *)data_ov015_02114a24 + (idx << 3));
    int f1 = e->field1;
    void *obj = (void *)((char *)self + (f1 >> 1));
    FnPtr fn;
    if (f1 & 1)
        fn = (FnPtr)*(int *)((char *)*(int **)obj + e->field0);
    else
        fn = (FnPtr)e->field0;
    fn(obj);
    self->mState = idx;
}

/* State 0 enter: wait at the out position. */
// @symbol func_ov015_02111fac
extern "C" void func_ov015_02111fac(daObjBk_Dossunbar_c *self)
{
    self->mStateTimer = 20;
}

/* State 0 body. */
// @symbol func_ov015_02111f6c
extern "C" void func_ov015_02111f6c(daObjBk_Dossunbar_c *self)
{
    self->mStateTimer -= 1;
    if (self->mStateTimer > 0)
        return;
    func_ov015_02111fb8(self, 1);
}

/* State 1 enter. */
// @symbol func_ov015_02111f4c
extern "C" void func_ov015_02111f4c(daObjBk_Dossunbar_c *self)
{
    self->mStateTimer = 10;
    Sound::PlayBank3(0xc3, *(Vector3 *)&self->mCamSpacePosX);
}

/* State 1 body: ease X towards home minus 0x168000, then wait-mid. */
// @symbol func_ov015_02111eec
extern "C" void func_ov015_02111eec(daObjBk_Dossunbar_c *self)
{
    Math_Function_0203b0fc(
        &self->mPosX, self->mHomePosX - 0x168000, 0x800, 0x46000);
    self->mStateTimer -= 1;
    if (self->mStateTimer > 0)
        return;
    self->mPosX = self->mHomePosX - 0x168000;
    func_ov015_02111fb8(self, 2);
}

/* State 2 enter. */
// @symbol func_ov015_02111ee0
extern "C" void func_ov015_02111ee0(daObjBk_Dossunbar_c *self)
{
    self->mStateTimer = 5;
}

/* State 2 body: small bar -> return-ease, large bar -> return-coast. */
// @symbol func_ov015_02111e80
extern "C" void func_ov015_02111e80(daObjBk_Dossunbar_c *self)
{
    self->mStateTimer -= 1;
    if (self->mStateTimer > 0)
        return;
    if (self->mVariant == 0)
        func_ov015_02111fb8(self, 3);
    else
        func_ov015_02111fb8(self, 4);
}

/* State 3 enter. */
// @symbol func_ov015_02111e60
extern "C" void func_ov015_02111e60(daObjBk_Dossunbar_c *self)
{
    self->mStateTimer = 0x18;
    Sound::PlayBank3(0xc3, *(Vector3 *)&self->mCamSpacePosX);
}

/* State 3 body: ease X back to home, then wait-home. */
// @symbol func_ov015_02111df4
extern "C" void func_ov015_02111df4(daObjBk_Dossunbar_c *self)
{
    Math_Function_0203b14c(
        &self->mPosX, self->mHomePosX, 0x800, 0xb4000, 0x28000);
    self->mStateTimer -= 1;
    if (self->mStateTimer > 0)
        return;
    self->mPosX = self->mHomePosX;
    func_ov015_02111fb8(self, 5);
}

/* State 4 enter: X speed rather than a timer. unk_0a4 is dActor_c's
   velocity X (with mVertSpeed / unk_0ac). */
// @symbol func_ov015_02111dd4
extern "C" void func_ov015_02111dd4(daObjBk_Dossunbar_c *self)
{
    self->unk_0a4 = 0xf000;
    Sound::PlayBank3(0xc3, *(Vector3 *)&self->mCamSpacePosX);
}

/* State 4 body: coast until X reaches home, then wait-home. */
// @symbol func_ov015_02111d98
extern "C" void func_ov015_02111d98(daObjBk_Dossunbar_c *self)
{
    self->UpdatePosWithOnlySpeed(0);
    if (self->mPosX < self->mHomePosX)
        return;
    self->mPosX = self->mHomePosX;
    func_ov015_02111fb8(self, 5);
}

/* State 5 enter -- InitResources starts the cycle here. */
// @symbol func_ov015_02111d8c
extern "C" void func_ov015_02111d8c(daObjBk_Dossunbar_c *self)
{
    self->mStateTimer = 10;
}

/* State 5 body. */
// @symbol func_ov015_02111d4c
extern "C" void func_ov015_02111d4c(daObjBk_Dossunbar_c *self)
{
    self->mStateTimer -= 1;
    if (self->mStateTimer > 0)
        return;
    func_ov015_02111fb8(self, 6);
}

/* State 6 enter: X speed toward the out position. */
// @symbol func_ov015_02111d28
extern "C" void func_ov015_02111d28(daObjBk_Dossunbar_c *self)
{
    self->unk_0a4 = -0x14000;
    Sound::PlayBank3(0xc3, *(Vector3 *)&self->mCamSpacePosX);
}

/* State 6 body: travel until X is 0x1ea000 below home, then wait-out. */
// @symbol func_ov015_02111ce0
extern "C" void func_ov015_02111ce0(daObjBk_Dossunbar_c *self)
{
    self->UpdatePosWithOnlySpeed(0);
    int v = self->mHomePosX + (int)0xffe16000;
    if (self->mPosX > v)
        return;
    self->mPosX = v;
    func_ov015_02111fb8(self, 0);
}

// @symbol _ZN19daObjBk_Dossunbar_c15OnHitByMegaCharER6Player
void daObjBk_Dossunbar_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    Kill();
}

// @symbol _ZN19daObjBk_Dossunbar_c4KillEv
void daObjBk_Dossunbar_c::Kill()
{
    Vector3 pos;
    Vector3 dustPos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0xc8000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x10a, pos.x, pos.y, pos.z);
    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    PoofDustAt(dustPos);
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    MarkForDestruction();
}
