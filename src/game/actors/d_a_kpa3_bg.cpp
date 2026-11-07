//cpp
/* daKpa3Bg_c -- Bowser's sky platform (KOOPA3BG, ov060).
 *
 * _ZTS10daKpa3Bg_c at ov060 0x0211a954, _ZTI at 0x0211a948 (base
 * _ZTI10dBgActor_c), _ZTV at 0x0211a9b0. The tree used to call this
 * class BowserSkyPlatform. Text is 0x02117d1c..0x02118438. The
 * destructor is under opt_propagation on and defer_codegen off, so it
 * emits D1, D0, then a D2 the cartridge does not keep, ahead of the
 * deferred functions. opt_propagation off stays last: deferred codegen
 * of func_ov060_02117db8 uses that setting (without it, 89 of its 202
 * words differ). Do not move it.
 *
 * Behavior runs data_ov060_0211b1ac[mState]: 0 finds Bowser, 1 watches
 * him, 2 drops the platform. The three handlers are declared in the
 * header so the table's pointer-to-member constants are real.
 * func_ov060_021181b4 and func_ov060_021180e0 are defined as members;
 * func_ov060_02117db8 is a free function under the member's mangled name.
 * func_ov060_021181b4 returns an int, so its table entry casts the
 * pointer-to-member to the table's void handler type.
 *
 * __sinit at the end of this file emits compiler-numbered statics; they
 * compile as @657..@681 and the manifest maps them to the ROM's
 * @456..@479 and @506. mwcc's counter for those names advances for
 * named variables, named parameters (prototypes included) and some
 * compiler temporaries declared before them (`new daKpa3Bg_c` alone
 * advances it by 25, and including daKpa_c.h by 171), so adding or
 * deleting one renumbers them. These
 * names are there for the count:
 * - InitResources loads the model through the named local bmd.
 * - func_ov060_021180e0 steps its timer through two named locals.
 *
 * leftovers:
 * - The three handlers keep their address names.
 * - func_ov060_02117db8 parses as C (cplusplus off): the same body as
 *   C++ DIFFs. Re-measured for #821, because this is the last hand-spelled
 *   mangled method in the tree and the language-mode metric counts it.
 *   `#pragma cplusplus off` is what forces the hand-spelling -- C has no
 *   member syntax -- so the question is only whether the pragma is needed,
 *   and it is. With the identical body and only the language mode changed
 *   (an extern "C" free function under the mangled name, nothing else
 *   touched), 89 of the 202 words differ, and the real member
 *   `void daKpa3Bg_c::func_ov060_02117db8()` differs in the same 89. The
 *   shape is right throughout -- instruction for instruction the same
 *   schedule -- but the C++ frontend keeps one more value live: it saves
 *   seven callee-saved registers against the cartridge's six, reserves
 *   0x4c of stack against 0x48, and holds `this` in r6 where the ROM holds
 *   it in r8, which renames registers through the whole body. Six
 *   respellings of the locals were tried against that (dropping the yimm
 *   staging, initialising it at its declaration, declaring it first,
 *   `state` as int, `saved` non-volatile, `saved` declared last): every one
 *   is still 89 words or worse, so it is the frontend's allocation order
 *   and not the spelling. It reaches the members through a struct daKpa3Bg_c *.
 *   Earthquake, PoofDustAt and MarkForDestruction go through
 *   kpaCallEarthquake, kpaCallPoof and kpaCallDestroy. A member call in
 *   the C body DIFFs; the bridges inline to the same calls.
 *   func_02012694 plays bank-3 sound 0xbc at a point written into
 *   mCamSpacePos, restored afterwards.
 * - dBgW_KcMbg::SetFile is called through its mangled name, with the
 *   Fix12<int> argument passed as a raw int.
 * - func_020393d4 stores dBgW::UpdatePosWithTransform.
 *   func_020393c4 stores the touch veneer func_ov060_021183f4.
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

struct BMD_File;
struct KCL_File;
struct CLPS_Block;

extern "C" {

int *Vec3_AsrInPlace(VecFx32 *v, int n);
/* Unnamed on purpose: see the @NNN note in the banner. */
void MulVec3Mat4x3(Vector3 *, Matrix4x3 *, Vector3 *);
void func_02012694(unsigned int, void *);
int CopyTexPalFromLevelModel(void *);

/* One row per mVariant: where that platform piece sits (whole units,
 * shifted to fix12 on use) and the angle it faces. */
typedef struct Kpa3BgPieceSpot {
    int x;
    int z;
    s16 angle;
} Kpa3BgPieceSpot;

extern s16 data_02082214[];
extern Kpa3BgPieceSpot data_ov060_02119564[10];
extern Matrix4x3 data_0209b3ec;

extern SharedFilePtr *data_ov060_02119514[];
extern SharedFilePtr *data_ov060_0211953c[];
extern CLPS_Block *data_ov060_0211a980[];
extern int data_0208e738;

/* Three parameter names dropped: kpaCallDestroy took those @NNN slots. */
extern void hold_d_a_kpa3_bg(void *, KCL_File *, const Matrix4x3 &, int fix, short sh,
                             CLPS_Block &b);
void func_020393d4(dBgW *collider, BeforeClsnCallback callback);
void func_020393c4(dBgW *collider, StoodOnCallback callback);
void func_ov060_021183f4(dBgW *collider, dActor_c *owner, dActor_c *other);
void func_ov060_021183cc(daKpa3Bg_c *self, dActor_c *other);

// local extern: defined below as a free function (cplusplus off has no member syntax); the header declares the member spelling for the PMF table.
void _ZN10daKpa3Bg_c19func_ov060_02117db8Ev(daKpa3Bg_c *self);

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

/* Historical alias BowserSkyPlatform_Spawn. */
extern "C" daKpa3Bg_c *daKpa3Bg_c_classInit(void)
{
    return new daKpa3Bg_c;
}

/* dBgW callback veneer, armed by func_020393c4 in InitResources. Drops the
   collider and forwards the platform and the touching actor into
   func_ov060_021183cc. */
extern "C" void func_ov060_021183f4(dBgW *collider, dActor_c *owner, dActor_c *other) {
    func_ov060_021183cc(static_cast<daKpa3Bg_c *>(owner), other);
}

/* Flags the platform when the actor that touched it is Bowser
   (actor ID 0x117); Behavior clears the flag every frame. The comparison
   is materialised into an int before the test, as the ROM does. */
extern "C" void func_ov060_021183cc(daKpa3Bg_c *self, dActor_c *other)
{
    int isKoopa = other->actorID == 0x117;
    if (isKoopa != 0)
        self->mTouchedKoopa = 1;
}

s32 daKpa3Bg_c::InitResources()
{
    mVariant = (u8)(param1 & 0xf);
    data_0208e738 = 0;
    /* bmd is named for the @NNN count (see the banner). */
    BMD_File *bmd = (BMD_File *)dExtSimpleModel_c::LoadFile(*data_ov060_02119514[mVariant]);
    mModel.SetFile(bmd, 1, -1);
    CopyTexPalFromLevelModel(&mModel.data);
    data_0208e738 = 1;
    UpdateClsnPosAndRot();
    ((dBgW_KcMbg *)(&mMeshCollider))
        ->dBgW_KcMbg::SetFile((KCL_File *)dBgW_Kc::LoadFile(*data_ov060_0211953c[mVariant]),
                              mClsnMat, 0x1000, mAngleY, *data_ov060_0211a980[mVariant]);
    func_020393d4(&mMeshCollider, &dBgW::UpdatePosWithTransform);
    func_020393c4(&mMeshCollider, &func_ov060_021183f4);
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
int daKpa3Bg_c::func_ov060_021181b4() {
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
void daKpa3Bg_c::func_ov060_021180e0()
{
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

/* The C body cannot spell a member call. These inline to the direct calls.
   They sit above that function. */
static inline void kpaCallEarthquake(daKpa3Bg_c *self, VecFx32 *v, int fix) {
    self->Earthquake(*(const Vector3 *)v, fix);
}
static inline void kpaCallPoof(daKpa3Bg_c *self, VecFx32 *v) {
    self->PoofDustAt(*(const Vector3 *)v);
}
static inline void kpaCallDestroy(daKpa3Bg_c *self) {
    self->MarkForDestruction();
}

/* mState 2: the platform lifts, then falls away. On mPhase 0 and 0x16 it
 * plays sound 0xbc at a point computed from this variant's (x, z, angle)
 * row. Before mPhase 0x16 it calls Earthquake and moves up at mVertSpeed
 * 0x8000 (8.0) with no gravity; from 0x16 mVertAccel is -0x4000. Every even
 * mPhase below 0xe puffs dust at two points either side of the row's angle,
 * closing in as mPhase grows. Past mPhase 0x12c the platform destroys
 * itself.
 *
 * Parsed as C: the C++ body DIFFs, including which sine-table symbol the
 * pool names. 0x12c000 goes through the local yimm: storing the constant
 * straight into v[1] swaps two pool entries. */
#pragma cplusplus off
void _ZN10daKpa3Bg_c19func_ov060_02117db8Ev(struct daKpa3Bg_c *self) {
    volatile int saved[3];
    int yimm;
    int v[6];
    VecFx32 eq;
    VecFx32 dust1;
    VecFx32 dust2;
    u16 state;
    int row;
    int ang;
    s16 mul;
    int bx, bz, k;
    int x, y, z;
    int a;

    state = self->mPhase;
    if (state == 0 || state == 0x16) {
        saved[0] = self->mCamSpacePosX;
        saved[1] = self->mCamSpacePosY;
        saved[2] = self->mCamSpacePosZ;
        yimm = 0x12c000;
        v[0] = self->mPosX;
        v[1] = self->mPosY;
        v[2] = self->mPosZ;
        row = self->mVariant;
        ang = (u16)data_ov060_02119564[row].angle;
        v[1] = yimm;
        v[0] = (s16)data_02082214[(ang >> 4) * 2] * (s16)0x366 + (data_ov060_02119564[row].x << 12);
        v[2] = (s16)data_02082214[(ang >> 4) * 2 + 1] * (s16)0x366 +
               (data_ov060_02119564[self->mVariant].z << 12);
        Vec3_AsrInPlace((VecFx32 *)v, 3);
        MulVec3Mat4x3((Vector3 *)v, &data_0209b3ec, &self->mCamSpacePos);
        func_02012694(0xbc, &self->mCamSpacePosX);
        self->mCamSpacePosX = saved[0];
        self->mCamSpacePosY = saved[1];
        self->mCamSpacePosZ = saved[2];
    }

    if (self->mPhase >= 0x16u) {
        self->mVertAccel = -0x4000;
    } else {
        eq.x = self->mPosX;
        eq.y = self->mPosY;
        eq.z = self->mPosZ;
        kpaCallEarthquake(self, &eq, 0x7d0000);
        self->mVertSpeed = 0x8000;
        self->mVertAccel = 0;
    }

    state = self->mPhase;
    if (state % 2 == 0 && state < 0xe) {
        row = self->mVariant;
        mul = data_ov060_02119564[row].angle;
        k = (6 - ((int)state >> 1)) * 0x122;
        bx = data_ov060_02119564[row].x;
        bz = data_ov060_02119564[row].z;

        a = ((int)(u16)(s16)(mul + 0x14b0) >> 4) * 2;
        x = k * (s16)data_02082214[a] + (bx << 12);
        y = 0x133000;
        z = k * (s16)data_02082214[a + 1] + (bz << 12);
        v[3] = x;
        v[4] = y;
        v[5] = z;
        dust1.x = x;
        dust1.y = y;
        dust1.z = z;
        kpaCallPoof(self, &dust1);

        a = ((int)(u16)(s16)(mul - 0x14b0) >> 4) * 2;
        x = k * (s16)data_02082214[a] + (bx << 12);
        y = 0x133000;
        z = k * (s16)data_02082214[a + 1] + (bz << 12);
        v[3] = x;
        v[4] = y;
        v[5] = z;
        dust2.x = x;
        dust2.y = y;
        dust2.z = z;
        kpaCallPoof(self, &dust2);
    }

    self->mVertSpeed += self->mVertAccel;
    if (self->mVertSpeed <= self->mTerminalVelocity)
        self->mVertSpeed = self->mTerminalVelocity;
    self->mPosY += self->mVertSpeed;
    if (self->mPhase > 0x12cu)
        kpaCallDestroy(self);
    self->mPhase++;
}
#pragma cplusplus on

#pragma opt_propagation on
#pragma defer_codegen off
daKpa3Bg_c::~daKpa3Bg_c()
{
}
#pragma defer_codegen on
#pragma opt_propagation off

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
    {(Handler)&daKpa3Bg_c::func_ov060_021181b4},
    {&daKpa3Bg_c::func_ov060_021180e0},
    {&daKpa3Bg_c::func_ov060_02117db8},
};
