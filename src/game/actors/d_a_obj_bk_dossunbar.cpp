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
 * Notes:
 * - The seven states are Enter/Execute member pairs. The unowned PMF
 *   descriptor records in ov015 .data point at them; the two dispatch
 *   tables and their compiler-generated initializer are owned by this TU.
 *   State order: 0 WaitOut, 1 EaseToMid, 2 WaitMid, 3 EaseHome (small bar)
 *   or 4 CoastHome (big bar), 5 WaitHome, 6 TravelOut.
 * - mSpeedX is dActor_c's X speed (velocity at 0xa4/a8/ac),
 *   not a field of this class.
 * - Resource table is two { model, KCL, CLPS } rows. Init must
 *   keep the three column symbols 02114534/38/3c; a typed
 *   ResourceDescriptor[2] size-DIFF Init and changes reloc destinations.
 *   sinit file IDs 0x58b / 0x58c / 0x58d / 0x58e. The registry profiles
 *   g_profile_BK_DOSSUNBAR_L / _S remain outside the owned ranges.
 * - func_01ffb0a4 / func_01ffb07c are MeshCollider ITCM
 *   (flag 0x35 / vec at +0x38). func_020393d4 stores the BeforeClsn
 *   callback; 020396d0 and Math_Function_0203b0fc / 0203b14c names
 *   belong with those callees.
 */

#include "daObjBk_Dossunbar_c.h"
#include "Sound.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "Particle__System.h"

/* Local names for the existing eight-byte model/collision resource families.
 * Their constructors and destructors remain ordinary cross-TU C++ imports. */
struct DossunbarModelFile : SharedFilePtr {
    DossunbarModelFile(u32 fileID);
    ~DossunbarModelFile();
};

struct DossunbarCollisionFile : SharedFilePtr {
    DossunbarCollisionFile(u32 fileID);
    ~DossunbarCollisionFile();
};

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

typedef void (daObjBk_Dossunbar_c::*PMF)();
struct StateEntry {
    PMF handler;
};

/* The installer walks the enter-table records by hand as {ptr, adj}. */
typedef void (*FnPtr)(void *);
struct VtEntry {
    int field0;
    int field1;
};

enum { kMovingBarBigActorId = 0x35 };

static inline int IsBigBar(dActor_c *actor) {
    return actor->actorID == kMovingBarBigActorId;
}

extern StateEntry data_ov015_021149ec[7];
extern VtEntry data_ov015_02114a24[7];
extern StateEntry data_ov015_02114500;
extern StateEntry data_ov015_021144f0;
extern StateEntry data_ov015_021144e0;
extern StateEntry data_ov015_021144d0;
extern StateEntry data_ov015_02114508;
extern StateEntry data_ov015_021144d8;
extern StateEntry data_ov015_021144e8;
extern VtEntry data_ov015_021144b0;
extern VtEntry data_ov015_021144f8;
extern VtEntry data_ov015_021144c8;
extern VtEntry data_ov015_021144c0;
extern VtEntry data_ov015_021144a8;
extern VtEntry data_ov015_021144b8;
extern VtEntry data_ov015_021144a0;

/* Three column symbols of one two-row {model, KCL, CLPS} table.
   A typed ResourceDescriptor[2] changes Init reloc destinations. */
extern SharedFilePtr *data_ov015_02114534;
extern SharedFilePtr *data_ov015_02114538;
extern CLPS_Block *data_ov015_0211453c;

/* local extern: arm9 math and MeshCollider helpers with no shared header. */
extern "C" {
void Math_Function_0203b0fc(int *p, int target, int scale, int max);
void Math_Function_0203b14c(int *p, int target, int scale, int max, int extra);
void func_01ffb0a4(dBgW_Kc *self);
void func_01ffb07c(dBgW_Kc *self, const Vector3 *v);
void func_020393d4(dBgW *self, void *fn);
void func_020396d0(dBgW *self, int v);
}

// @symbol daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_S
extern "C" daObjBk_Dossunbar_c *daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_S() {
    return new daObjBk_Dossunbar_c();
}

// @symbol daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_L
extern "C" daObjBk_Dossunbar_c *daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_L() {
    return new daObjBk_Dossunbar_c();
}

// @symbol _ZN19daObjBk_Dossunbar_c13InitResourcesEv
s32 daObjBk_Dossunbar_c::InitResources() {
    /* The ROM materialises the 0/1 test before choosing the store. */
    if (IsBigBar(this))
        mVariant = 1;
    else
        mVariant = 0;

    int j0 = mVariant * 0xc;
    void *modelFile =
        dExtSimpleModel_c::LoadFile(**(SharedFilePtr **)((char *)&data_ov015_02114534 + j0));
    mModel.SetFile((BMD_File *)modelFile, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    int j = mVariant * 0xc;
    void *kclFile = dBgW_Kc::LoadFile(**(SharedFilePtr **)((char *)&data_ov015_02114538 + j));
    ((dBgW_KcMbg *)(&mMeshCollider))
        ->dBgW_KcMbg::SetFile((KCL_File *)((int)kclFile), *(const Matrix4x3 *)(&mClsnMat), 0x1000,
                              mAngleY,
                              *(CLPS_Block *)(*(void **)((char *)&data_ov015_0211453c + j)));
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithVelocity);

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
    ChangeState(5);
    return 1;
}

// @symbol _ZN19daObjBk_Dossunbar_c8BehaviorEv
s32 daObjBk_Dossunbar_c::Behavior() {
    (this->*(data_ov015_021149ec[mState].handler))();
    UpdateModelPosAndRotY();
    if (IsClsnInRange(0, 0))
        UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN19daObjBk_Dossunbar_c6RenderEv
s32 daObjBk_Dossunbar_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN19daObjBk_Dossunbar_c16CleanupResourcesEv
s32 daObjBk_Dossunbar_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    (*(SharedFilePtr **)((char *)&data_ov015_02114534 + mVariant * 0xc))->Release();
    (*(SharedFilePtr **)((char *)&data_ov015_02114538 + mVariant * 0xc))->Release();
    return 1;
}

/* Install mState and run that state's enter function. */
// @symbol _ZN19daObjBk_Dossunbar_c11ChangeStateEi
void daObjBk_Dossunbar_c::ChangeState(int idx) {
    VtEntry *e = (VtEntry *)((char *)data_ov015_02114a24 + (idx << 3));
    int f1 = e->field1;
    void *obj = (void *)((char *)this + (f1 >> 1));
    FnPtr fn;
    if (f1 & 1)
        fn = (FnPtr) * (int *)((char *)*(int **)obj + e->field0);
    else
        fn = (FnPtr)e->field0;
    fn(obj);
    mState = idx;
}

/* State 0 enter: wait at the out position. */
// @symbol _ZN19daObjBk_Dossunbar_c12EnterWaitOutEv
void daObjBk_Dossunbar_c::EnterWaitOut() {
    mStateTimer = 20;
}

/* State 0 body. */
// @symbol _ZN19daObjBk_Dossunbar_c14ExecuteWaitOutEv
void daObjBk_Dossunbar_c::ExecuteWaitOut() {
    mStateTimer -= 1;
    if (mStateTimer > 0)
        return;
    ChangeState(1);
}

/* State 1 enter. */
// @symbol _ZN19daObjBk_Dossunbar_c14EnterEaseToMidEv
void daObjBk_Dossunbar_c::EnterEaseToMid() {
    mStateTimer = 10;
    Sound::PlayBank3(0xc3, mCamSpacePos);
}

/* State 1 body: ease X towards home minus 0x168000, then wait-mid. */
// @symbol _ZN19daObjBk_Dossunbar_c16ExecuteEaseToMidEv
void daObjBk_Dossunbar_c::ExecuteEaseToMid() {
    Math_Function_0203b0fc(&mPosX, mHomePosX - 0x168000, 0x800, 0x46000);
    mStateTimer -= 1;
    if (mStateTimer > 0)
        return;
    mPosX = mHomePosX - 0x168000;
    ChangeState(2);
}

/* State 2 enter. */
// @symbol _ZN19daObjBk_Dossunbar_c12EnterWaitMidEv
void daObjBk_Dossunbar_c::EnterWaitMid() {
    mStateTimer = 5;
}

/* State 2 body: small bar -> return-ease, large bar -> return-coast. */
// @symbol _ZN19daObjBk_Dossunbar_c14ExecuteWaitMidEv
void daObjBk_Dossunbar_c::ExecuteWaitMid() {
    mStateTimer -= 1;
    if (mStateTimer > 0)
        return;
    if (mVariant == 0)
        ChangeState(3);
    else
        ChangeState(4);
}

/* State 3 enter. */
// @symbol _ZN19daObjBk_Dossunbar_c13EnterEaseHomeEv
void daObjBk_Dossunbar_c::EnterEaseHome() {
    mStateTimer = 0x18;
    Sound::PlayBank3(0xc3, mCamSpacePos);
}

/* State 3 body: ease X back to home, then wait-home. */
// @symbol _ZN19daObjBk_Dossunbar_c15ExecuteEaseHomeEv
void daObjBk_Dossunbar_c::ExecuteEaseHome() {
    Math_Function_0203b14c(&mPosX, mHomePosX, 0x800, 0xb4000, 0x28000);
    mStateTimer -= 1;
    if (mStateTimer > 0)
        return;
    mPosX = mHomePosX;
    ChangeState(5);
}

/* State 4 enter: X speed rather than a timer. mSpeedX is dActor_c's
   velocity X (with mVertSpeed / mSpeedZ). */
// @symbol _ZN19daObjBk_Dossunbar_c14EnterCoastHomeEv
void daObjBk_Dossunbar_c::EnterCoastHome() {
    mSpeedX = 0xf000;
    Sound::PlayBank3(0xc3, mCamSpacePos);
}

/* State 4 body: coast until X reaches home, then wait-home. */
// @symbol _ZN19daObjBk_Dossunbar_c16ExecuteCoastHomeEv
void daObjBk_Dossunbar_c::ExecuteCoastHome() {
    UpdatePosWithOnlySpeed(0);
    if (mPosX < mHomePosX)
        return;
    mPosX = mHomePosX;
    ChangeState(5);
}

/* State 5 enter -- InitResources starts the cycle here. */
// @symbol _ZN19daObjBk_Dossunbar_c13EnterWaitHomeEv
void daObjBk_Dossunbar_c::EnterWaitHome() {
    mStateTimer = 10;
}

/* State 5 body. */
// @symbol _ZN19daObjBk_Dossunbar_c15ExecuteWaitHomeEv
void daObjBk_Dossunbar_c::ExecuteWaitHome() {
    mStateTimer -= 1;
    if (mStateTimer > 0)
        return;
    ChangeState(6);
}

/* State 6 enter: X speed toward the out position. */
// @symbol _ZN19daObjBk_Dossunbar_c14EnterTravelOutEv
void daObjBk_Dossunbar_c::EnterTravelOut() {
    mSpeedX = -0x14000;
    Sound::PlayBank3(0xc3, mCamSpacePos);
}

/* State 6 body: travel until X is 0x1ea000 below home, then wait-out. */
// @symbol _ZN19daObjBk_Dossunbar_c16ExecuteTravelOutEv
void daObjBk_Dossunbar_c::ExecuteTravelOut() {
    UpdatePosWithOnlySpeed(0);
    int v = mHomePosX + (int)0xffe16000;
    if (mPosX > v)
        return;
    mPosX = v;
    ChangeState(0);
}

// @symbol _ZN19daObjBk_Dossunbar_c15OnHitByMegaCharER6Player
void daObjBk_Dossunbar_c::OnHitByMegaChar(Player &player) {
    player.IncMegaKillCount();
    Kill();
}

// @symbol _ZN19daObjBk_Dossunbar_c4KillEv
void daObjBk_Dossunbar_c::Kill() {
    Vector3 pos;
    Vector3 dustPos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0xc8000;
    Particle::System::NewSimple(0x10a, pos.x, pos.y, pos.z);
    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    PoofDustAt(dustPos);
    Sound::PlayBank3(0x41, mCamSpacePos);
    MarkForDestruction();
}

/* Preserve the small/model, small/collision, large/model, large/collision
 * construction order before initializing the two dispatch tables. */
DossunbarModelFile gBkDossunbarSModelFile(0x058d);
DossunbarCollisionFile gBkDossunbarSCollisionFile(0x058e);
DossunbarModelFile gBkDossunbarLModelFile(0x058b);
DossunbarCollisionFile gBkDossunbarLCollisionFile(0x058c);

VtEntry data_ov015_02114a24[7] = {
    data_ov015_021144b0, data_ov015_021144f8, data_ov015_021144c8, data_ov015_021144c0,
    data_ov015_021144a8, data_ov015_021144b8, data_ov015_021144a0,
};
StateEntry data_ov015_021149ec[7] = {
    data_ov015_02114500, data_ov015_021144f0, data_ov015_021144e0, data_ov015_021144d0,
    data_ov015_02114508, data_ov015_021144d8, data_ov015_021144e8,
};
