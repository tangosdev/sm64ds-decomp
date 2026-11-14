//cpp
/* daKpa3Bg_c -- Bowser's sky platform (KOOPA3BG, ov060).
 *
 * _ZTS10daKpa3Bg_c at ov060 0x0211a954, _ZTI at 0x0211a948 (base
 * _ZTI10dBgActor_c), _ZTV at 0x0211a9b0. The tree used to call this
 * class BowserSkyPlatform. Text is 0x02117d1c..0x02118438. The
 * destructor is defined first, under defer_codegen off, so it emits D1,
 * D0, then a D2 the cartridge does not keep, ahead of the deferred
 * functions (which mwcc emits last to first). Defined after them, it
 * would flush them before itself and land D1 and D0 at the end of .text.
 *
 * Behavior runs data_ov060_0211b1ac[mState]: 0 finds Bowser, 1 watches
 * him, 2 drops the platform. The three handlers (StateStart, StateCheck,
 * StateFall) are members declared in the header, so the table's
 * pointer-to-member constants are real. StateStart returns an int, so its
 * table entry casts the pointer-to-member to the table's void handler type.
 *
 * __sinit at the end of this file emits compiler-numbered statics; they
 * compile as @578..@598, @600 and @602, and the manifest maps them to the ROM's
 * @456..@479 and @506. mwcc's counter for those names advances for
 * named variables, named parameters (prototypes included) and some
 * compiler temporaries declared before them (`new daKpa3Bg_c` alone
 * advances it by 25, and including daKpa_c.h by 171), so adding or
 * deleting one renumbers them. These
 * names are there for the count:
 * - InitResources loads the model through the named local bmd.
 * - StateCheck steps its timer through two named locals.
 *
 * leftovers:
 * - StateStart, StateCheck and StateFall take N64's names for the same
 *   actions (falling_bowser_plat_act_start/check/fall), in the State*
 *   spelling this tree's single-handler state tables use.
 * - dBgW::SetBeforeClsnCallback stores dBgW::UpdatePosWithTransform.
 *   dBgW::SetStoodOnCallback stores the touch veneer StoodOnCallback.
 * - data_ov060_02119564 is the ten-row (x, z, angle) table. Its pool
 *   words point at row 0's x, z and angle; the delinker named each of
 *   those (02119564 / 02119568 / 0211956c), and the struct fields' base
 *   plus addend resolve to the same three words.
 */

#include "common.h"
#include "daKpa3Bg_c.h"
#include "daKpa_c.h"
#include "SharedFilePtr.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"

struct BMD_File;
struct KCL_File;
struct CLPS_Block;

extern "C" {

int *Vec3_AsrInPlace(void *v, int n);
void MTX_MultVec43(Vector3 *v, Matrix4x3 *m, Vector3 *res);
void func_02012694(unsigned int, void *);
int CopyTexPalFromLevelModel(void *);

/* One row per mVariant: where that platform piece sits (whole units,
 * shifted to fix12 on use) and the angle it faces. */
typedef struct Kpa3BgPieceSpot {
    int x;
    int z;
    s16 angle;
} Kpa3BgPieceSpot;

extern Kpa3BgPieceSpot data_ov060_02119564[10];
extern Matrix4x3 data_0209b3ec;

extern SharedFilePtr *data_ov060_02119514[];
extern SharedFilePtr *data_ov060_0211953c[];
extern CLPS_Block *data_ov060_0211a980[];
extern int data_0208e738;
}

typedef void (daKpa3Bg_c::*Handler)();
struct HandlerEntry { Handler pmf; };
extern HandlerEntry data_ov060_0211b1ac[];

/* The ten model handles construct through func_02017acc and destroy through
 * func_02017ab4; the ten collision handles construct through func_02017b4c
 * and destroy through SharedFilePtr_Destruct_Clsn. The wrappers are declared,
 * never defined: the manifest aliases their members onto the ROM veneers. */
struct Kpa3BgModelFilePtr : SharedFilePtr {
    Kpa3BgModelFilePtr(u32 fileID);
    ~Kpa3BgModelFilePtr();
};

struct Kpa3BgCollisionFilePtr : SharedFilePtr {
    Kpa3BgCollisionFilePtr(u32 fileID);
    ~Kpa3BgCollisionFilePtr();
};

#pragma defer_codegen off
daKpa3Bg_c::~daKpa3Bg_c() {}
#pragma defer_codegen on

/* Historical alias BowserSkyPlatform_Spawn. */
extern "C" daKpa3Bg_c *daKpa3Bg_c_classInit(void)
{
    return new daKpa3Bg_c;
}

/* dBgW callback veneer, armed by dBgW::SetStoodOnCallback in InitResources. Drops the
   collider and forwards the touching actor to the platform's OnStoodOn. */
void daKpa3Bg_c::StoodOnCallback(dBgW *collider, dActor_c *owner, dActor_c *other) {
    static_cast<daKpa3Bg_c *>(owner)->OnStoodOn(other);
}

/* Flags the platform when the actor that touched it is Bowser
   (actor ID 0x117); Behavior clears the flag every frame. The comparison
   is materialised into an int before the test, as the ROM does. */
void daKpa3Bg_c::OnStoodOn(dActor_c *other) {
    int isKoopa = other->actorID == 0x117;
    if (isKoopa != 0)
        mTouchedKoopa = 1;
}

s32 daKpa3Bg_c::InitResources()
{
    mVariant = (param1 & 0xf);
    data_0208e738 = 0;
    /* bmd is named for the @NNN count (see the banner). */
    BMD_File *bmd = dExtSimpleModel_c::LoadFile(*data_ov060_02119514[mVariant]);
    mModel.SetFile(bmd, 1, -1);
    CopyTexPalFromLevelModel(&mModel.data);
    data_0208e738 = 1;
    UpdateClsnPosAndRot();
    mMeshCollider.SetFile(dBgW_Kc::LoadFile(*data_ov060_0211953c[mVariant]), mClsnMat, 0x1000,
                          mAngleY, *data_ov060_0211a980[mVariant]);
    mMeshCollider.SetBeforeClsnCallback(&dBgW::UpdatePosWithTransform);
    mMeshCollider.SetStoodOnCallback(&StoodOnCallback);
    mMeshCollider.Enable(this);
    mKoopaUniqueId = 0;
    mTouchedKoopa = 0;
    mState = 0;
    mActive = 0;
    mPhase = 0;
    mTimer = mPhase;
    mVertAccel = 0;
    mTerminalVelocity = -0x1e000;
    return 1;
}

s32 daKpa3Bg_c::Behavior()
{
    (this->*(data_ov060_0211b1ac[mState].pmf))();
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mTouchedKoopa = 0;
    return 1;
}

s32 daKpa3Bg_c::Render()
{
    mModel.Render(0);
    return 1;
}

s32 daKpa3Bg_c::CleanupResources()
{
    mMeshCollider.Disable();
    data_ov060_02119514[mVariant]->Release();
    data_ov060_0211953c[mVariant]->Release();
    return 1;
}

/* mState 0: finds the daKpa actor (id 0x117), remembers its uniqueID and
   moves to mState 1. Retries every frame until Bowser exists. */
int daKpa3Bg_c::StateStart() {
    dActor_c *koopa = dActor_c::FindWithActorID(0x117, 0);
    if (koopa) {
        mKoopaUniqueId = koopa->uniqueID;
        mState = 1;
        return 1;
    }
}

/* mState 1. Destroys the platform once Bowser is gone. Drops it at once
   (mState 2) if Bowser touched it this frame in state 0xd with condition
   bit 0x10000 set (BOWSER_STATE_JUMP and BOWSER_COND_RECOVERING in
   d_a_kpa.cpp). On his last hit point, in state 3 or while held or being released
   (mHoldState != 0), mActive latches; from then on mTimer counts each frame and the
   platform drops once it passes mVariant * 20. */
void daKpa3Bg_c::StateCheck() {
    daKpa_c *koopa = (daKpa_c *)dActor_c::FindWithID(mKoopaUniqueId);
    if (koopa == 0) {
        MarkForDestruction();
        return;
    }
    if (mTouchedKoopa != 0 && koopa->mState == 0xd) {
        if ((koopa->mCondFlags & 0x10000) != 0)
            mState = 2;
    }
    if (koopa->mHealth == 1) {
        if (koopa->mState == 3 || koopa->mHoldState != 0)
            mActive = 1;
    }
    if (mActive == 0) {
        mTimer = 0;
        return;
    }
    int dropAfter = mVariant * 20;
    u16 *timer = &mTimer;
    if (dropAfter < mTimer)
        mState = 2;
    (*timer)++;
}

/* mState 2, N64's falling_bowser_plat_act_fall: the platform lifts, then
 * falls away. On mPhase 0 and 22 it plays sound 0xbc at a point computed
 * from this variant's (x, z, angle) row: func_02012694 takes its position
 * from mCamSpacePos, so the point is written there and the old value put
 * back afterwards. Before mPhase 22 it calls Earthquake and moves up at
 * mVertSpeed 0x8000 (8.0) with no gravity; from 22 mVertAccel is -0x4000.
 * Every even mPhase below 14 puffs dust at two points either side of the
 * row's angle, closing in as mPhase grows. Past mPhase 300 the platform
 * destroys itself.
 *
 * Earthquake and PoofDustAt are handed a Vector3 copy, as the cartridge
 * does: it stores each vector into a stack temporary of its own before the
 * call. */
void daKpa3Bg_c::StateFall() {
    u16 phase = mPhase;
    if (phase == 0 || phase == 22) {
        Vector3 saved = mCamSpacePos;
        Vector3 pos = mPos;
        int row = mVariant;
        u16 ang = data_ov060_02119564[row].angle;
        pos.x = FX_SinIdx(ang >> 4) * 0x366 + (data_ov060_02119564[row].x << 12);
        pos.y = 0x12c000;
        pos.z = FX_CosIdx(ang >> 4) * 0x366 + (data_ov060_02119564[mVariant].z << 12);
        Vec3_AsrInPlace(&pos, 3);
        MTX_MultVec43(&pos, &data_0209b3ec, &mCamSpacePos);
        func_02012694(0xbc, &mCamSpacePos);
        mCamSpacePos = saved;
    }

    if (mPhase >= 22) {
        mVertAccel = -0x4000;
    } else {
        Earthquake(Vector3(mPos), 0x7d0000);
        mVertSpeed = 0x8000;
        mVertAccel = 0;
    }

    phase = mPhase;
    if (phase % 2 == 0 && phase < 14) {
        int row = mVariant;
        s16 ang = data_ov060_02119564[row].angle;
        int dist = (6 - (phase >> 1)) * 0x122;
        int baseX = data_ov060_02119564[row].x << 12;
        int baseZ = data_ov060_02119564[row].z << 12;
        Vector3 dustPos;

        dustPos.x = dist * FX_SinIdx((u16)(s16)(ang + 0x14b0) >> 4) + baseX;
        dustPos.y = 0x133000;
        dustPos.z = dist * FX_CosIdx((u16)(s16)(ang + 0x14b0) >> 4) + baseZ;
        PoofDustAt(Vector3(dustPos));

        dustPos.x = dist * FX_SinIdx((u16)(s16)(ang - 0x14b0) >> 4) + baseX;
        dustPos.y = 0x133000;
        dustPos.z = dist * FX_CosIdx((u16)(s16)(ang - 0x14b0) >> 4) + baseZ;
        PoofDustAt(Vector3(dustPos));
    }

    mVertSpeed += mVertAccel;
    if (mVertSpeed <= mTerminalVelocity)
        mVertSpeed = mTerminalVelocity;
    mPosY += mVertSpeed;
    if (mPhase > 300)
        MarkForDestruction();
    mPhase++;
}

/* The static-init globals -- mwcc emits __sinit_d_a_kpa3_bg.cpp from these:
 * one ctor veneer plus destructor registration per file handle, then the
 * three pointer-to-member records copied into the dispatch table. Order
 * matches the retail initializer exactly. */
Kpa3BgModelFilePtr data_ov060_0211b06c(0x625);
Kpa3BgModelFilePtr data_ov060_0211b034(0x627);
Kpa3BgModelFilePtr data_ov060_0211b0a4(0x629);
Kpa3BgModelFilePtr data_ov060_0211b03c(0x62b);
Kpa3BgModelFilePtr data_ov060_0211b0b4(0x62d);
Kpa3BgModelFilePtr data_ov060_0211b044(0x62f);
Kpa3BgModelFilePtr data_ov060_0211b094(0x631);
Kpa3BgModelFilePtr data_ov060_0211b09c(0x633);
Kpa3BgModelFilePtr data_ov060_0211b054(0x635);
Kpa3BgModelFilePtr data_ov060_0211b024(0x637);
Kpa3BgCollisionFilePtr data_ov060_0211b0ac(0x626);
Kpa3BgCollisionFilePtr data_ov060_0211b02c(0x628);
Kpa3BgCollisionFilePtr data_ov060_0211b05c(0x62a);
Kpa3BgCollisionFilePtr data_ov060_0211b064(0x62c);
Kpa3BgCollisionFilePtr data_ov060_0211b07c(0x62e);
Kpa3BgCollisionFilePtr data_ov060_0211b01c(0x630);
Kpa3BgCollisionFilePtr data_ov060_0211b074(0x632);
Kpa3BgCollisionFilePtr data_ov060_0211b084(0x634);
Kpa3BgCollisionFilePtr data_ov060_0211b08c(0x636);
Kpa3BgCollisionFilePtr data_ov060_0211b04c(0x638);
HandlerEntry data_ov060_0211b1ac[3] = {
    {(Handler)&daKpa3Bg_c::StateStart},
    {&daKpa3Bg_c::StateCheck},
    {&daKpa3Bg_c::StateFall},
};
