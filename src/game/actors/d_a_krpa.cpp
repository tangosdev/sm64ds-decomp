//cpp
/* daKrpa_c -- the flame spitter (KERONPA), ov070; it spits KERONPA_FIRE.
 *
 * It turns toward the closest visible player within 700.0 (0x800 a frame)
 * and pulses in size from a frame table. With a player tracked, and while
 * it stands above data_0209f32c, it counts 115 frames and then spits: on
 * frame 30 of that animation it spawns KERONPA_FIRE (actor 0x10f, see
 * d_a_kp_fr.cpp) 80.0 in front of itself and plays sound 0x105. A collider
 * hit with flag 0x10 knocks it away tumbling (a mega kill, state 3). A
 * touch while it is being eaten (flag 0x20000) sends it to state 2, which
 * poofs it once neither 0x20000 nor 0x40000 is set.
 *
 * One TU, 25 functions. It began as the old one-function files
 * concatenated in reverse ROM order.
 *
 * DO NOT "TIDY" THESE -- each one is load-bearing:
 *   mwcc emits one .text section per ordinary definition in reverse source
 *   order; the inline destructor group comes out retail D1 then D0, no D2.
 *   M48, the array wrapper for InitResources' IDENTITY_MATRIX4X3 copy. This
 *   TU uses the nested Matrix4x3 (.t) that math/Matrix.h brings in via
 *   ModelAnim.h; putting common.h first would drop .t.
 *   func_ov070_021213cc calls dBgCh_Actr_UpdateDiscreteNoLava_veneer: the
 *   veneer is the retail call destination.
 *
 * WHY SOME CALLS ARE SPELLED AS MANGLED SYMBOLS (Fix12<int> by value, see
 * notes/mwccarm-codegen.md 6az):
 *   dCcAcPos_c::Init, dBgCh_Actr::Init (its header Fix12i also mangles as
 *   int), ModelAnim::SetAnim, DropShadowRadHeight and
 *   Particle::System::NewSimple.
 *
 * Known limits:
 *   The data_ov070_* SharedFilePtr / BCA / frame tables / state records are
 *   overlay .data, not this TU's data claim.
 *   func_0201267c is the sound call behind the spit (0x105).
 */
#include "daKrpa_c.h"
#include "dBgCh_Gnd.h"
#include "Player.h"
#include "SharedFilePtr.h"

/* Actor/process profile descriptor at ov070:0x0212334c. Field roles are
 * recovered from fBase_c/dActor_c consumers; exact original member spellings
 * are not preserved. */
struct daKrpaSpawnInfo {
    daKrpa_c *(*classInit)();
    s16 profileIDAndExecuteOrder;
    s16 drawOrder;
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};

typedef char daKrpaSpawnInfo_size_must_be_0x1c[
    sizeof(daKrpaSpawnInfo) == 0x1c ? 1 : -1];

// @symbol daKrpa_c_classInit
/* `return new daKrpa_c()` MATCHES (size 0x50); the leaf unsigned-long
 * operator new forwards `_ZN7fBase_cnwEj`. Historical aliases: daKrpa_c_Spawn
 * and FlameChomp_Spawn. */
extern "C" daKrpa_c *daKrpa_c_classInit()
{
    return new daKrpa_c();
}

/* Reconstructed source-style names: SM64DS proves the daKrpa_c RTTI identity,
 * KERONPA registry ID, descriptor/factory relationship, and object shape;
 * later EAD lineage supplies the spelling prior. Exact original SM64DS
 * spellings are not preserved. Historical aliases: daKrpa_c_Spawn and
 * daKrpa_c_SpawnInfo. */
extern "C" daKrpaSpawnInfo g_profile_KERONPA = {
    daKrpa_c_classInit,
    0x010e,
    0x0081,
    0x00000003,
    0x00000000,
    0x0002d000,
    0x01000000,
    0x00ed8000
};
// @symbol _ZN21daKrpaFrameController19func_ov070_02121ae0EPjjj
void daKrpaFrameController::func_ov070_02121ae0(u32 *frames, u32 count, u32 mode)
{
    this->frames = frames;
    this->count = count;
    this->mode = mode;
    this->cursor = 0;
}
// @symbol _ZN21daKrpaFrameController19func_ov070_02121a64Ev
u32 daKrpaFrameController::func_ov070_02121a64()
{
    switch (this->mode) {
    case 0:
        if (this->cursor < this->count)
            ++this->cursor;
        break;
    case 1:
        ++this->cursor;
        this->cursor %= this->count;
        break;
    }
    return this->frames[this->cursor];
}

// @symbol _ZN8daKrpa_c13InitResourcesEv
/* The Fix12-by-value Init methods are typed ABI seams: ordinary method calls
 * home their class-typed values and do not reproduce the retail callers. */
struct M48 { int w[12]; };
extern "C" {
extern SharedFilePtr data_ov070_02123698;
extern int IDENTITY_MATRIX4X3[];
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    dCcAcPos_c *clsn, dActor_c *actor, const Vector3 *offset,
    Fix12i radius, Fix12i height, u32 flags, u32 vulnFlags);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    dBgCh_Actr *clsn, dActor_c *actor, Fix12i radius, Fix12i height,
    Vector3_16 *a, Vector3_16 *b);
}

int daKrpa_c::InitResources()
{
    Vector3 offset;
    void *bmd = Model::LoadFile(data_ov070_02123698);
    int groundDistance;
    mModelAnim.SetFile((BMD_File *)bmd, 1, 1);
    if (!mShadowModel.InitCylinder())
        return 0;

    offset.x = 0;
    offset.y = -0x32000;
    offset.z = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mdCcAcPos_c, this, &offset,
        0x32000, 0x64000, 0x200002, 0x8000);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);

    mVertAccel = 0;
    mTerminalVelocity = 0;
    func_ov070_02121880(0);
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    *(M48 *)&mMatrix = *(M48 *)IDENTITY_MATRIX4X3;

    dBgCh_Gnd ground;
    ground.SetObjAndPos(*(Vector3 *)&mPosX, this);
    if (ground.DetectClsn())
        groundDistance = (mPosY - ground.clsnY) + 0x1e000;
    else
        groundDistance = 0x1f4000;
    mGroundDistance = groundDistance;
    func_ov070_02121310();
    return 1;
}

// @symbol _ZN8daKrpa_c8BehaviorEv
int daKrpa_c::Behavior()
{
    func_ov070_0212180c();
    func_ov070_02121310();
    return 1;
}

// @symbol _ZN8daKrpa_c6RenderEv
int daKrpa_c::Render()
{
    mModelAnim.Render((const Vector3 *)&mScaleX);
    return 1;
}

// @symbol _ZN8daKrpa_c16OnPendingDestroyEv

void daKrpa_c::OnPendingDestroy()
{
}

// @symbol _ZN8daKrpa_c16CleanupResourcesEv

int daKrpa_c::CleanupResources()
{
    data_ov070_02123698.Release();
    return 1;
}
extern "C" {
extern daKrpaState data_ov070_021236ac[];
}
// @symbol _ZN8daKrpa_c19func_ov070_02121880Ei
void daKrpa_c::func_ov070_02121880(int state) {
    this->mStateMethods = &data_ov070_021236ac[state];
    this->func_ov070_02121848();
}
// @symbol _ZN8daKrpa_c19func_ov070_02121848Ev
void daKrpa_c::func_ov070_02121848()
{
    daKrpaStateMethod *method = &this->mStateMethods->init;
    (this->**method)();
}
// @symbol _ZN8daKrpa_c19func_ov070_0212180cEv
void daKrpa_c::func_ov070_0212180c()
{
    daKrpaStateMethod *method = &this->mStateMethods->behavior;
    (this->**method)();
}
// @symbol _ZN8daKrpa_c19func_ov070_021217acEv
/* SetAnim is another proven Fix12-by-value caller seam. */
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    ModelAnim *model, BCA_File *file, int flags, Fix12i speed, u32 startFrame);
extern char data_ov070_021234c4[];
extern char data_ov070_021234dc[];
extern u32 data_ov070_02122404[];
extern u32 data_ov070_021222e8[];
}

int daKrpa_c::func_ov070_021217ac() {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &this->mModelAnim, (BCA_File *)data_ov070_021234c4, 0, 0x1000, 0);
    this->mFrameController.func_ov070_02121ae0(
        data_ov070_02122404, 0x64, 1);
    this->mStateTimer = 0x73;
    this->mStateIndex = 0;
    return 1;
}
// @symbol _ZN8daKrpa_c19func_ov070_02121710Ev
extern "C" {
extern u8 DecIfAbove0_Byte(u8 *value);
extern int data_0209f32c;
}
int daKrpa_c::func_ov070_02121710() {
    if (this->mPlayer) {
        if (this->mPosY > data_0209f32c) {
            if (DecIfAbove0_Byte(&this->mStateTimer) == 0)
                this->func_ov070_02121880(1);
        }
    } else {
        this->mStateTimer = 0x73;
    }
    this->mModelAnim.Advance();
    u32 frame = this->mFrameController.func_ov070_02121a64();
    this->mScaleX = frame;
    this->mScaleY = frame;
    this->mScaleZ = frame;
    this->func_ov070_02121298();
    this->func_ov070_021211c4();
    this->mdCcAcPos_c.Clear();
    this->mdCcAcPos_c.Update();
    return 1;
}
// @symbol _ZN8daKrpa_c19func_ov070_021216b8Ev
int daKrpa_c::func_ov070_021216b8() {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &this->mModelAnim, (BCA_File *)data_ov070_021234dc,
        0x40000000, 0x1000, 0);
    this->mFrameController.func_ov070_02121ae0(
        data_ov070_021222e8, 0x47, 0);
    this->mStateIndex = 1;
    return 1;
}
// @symbol _ZN8daKrpa_c19func_ov070_0212156cEv
extern "C" {
extern short data_02082214[];
void func_0201267c(u32 soundID, const Vector3 *pos);
}

int daKrpa_c::func_ov070_0212156c() {
    if (this->mFrameController.cursor == 0x1e) {
        Vector3 firePos;
        int idx = (int)(u16)this->mAngleY >> 4;
        int cosv = data_02082214[idx * 2 + 1];
        int sinv = data_02082214[idx * 2];
        int offZ = (int)(((s64)cosv * 0x50000 + 0x800) >> 12);
        int offX = (int)(((s64)sinv * 0x50000 + 0x800) >> 12);
        int x = this->mPosX + offX;
        int z = this->mPosZ + offZ;
        int y = this->mPosY - 0x29000;
        firePos.x = x;
        firePos.z = z;
        firePos.y = y;
        dActor_c::Spawn(0x10f, 0, firePos,
            (Vector3_16 *)&this->mAngleX, this->mAreaId, -1);
        func_0201267c(0x105, (Vector3 *)&this->mCamSpacePosX);
    }
    if (this->mFrameController.cursor == this->mFrameController.count)
        this->func_ov070_02121880(0);
    this->mModelAnim.Advance();
    u32 frame = this->mFrameController.func_ov070_02121a64();
    this->mScaleX = frame;
    this->mScaleY = frame;
    this->mScaleZ = frame;
    this->func_ov070_02121298();
    this->func_ov070_021211c4();
    this->mdCcAcPos_c.Clear();
    this->mdCcAcPos_c.Update();
    return 1;
}
// @symbol _ZN8daKrpa_c19func_ov070_02121548Ev
int daKrpa_c::func_ov070_02121548()
{
    this->mdCcAcPos_c.Clear();
    this->mStateIndex = 2;
    return 1;
}
// @symbol _ZN8daKrpa_c19func_ov070_021214f8Ev
int daKrpa_c::func_ov070_021214f8()
{
    int flags = this->mFlags;
    int blocked = (flags & 0x20000) != 0;
    if (!blocked) {
        blocked = (flags & 0x40000) != 0;
        if (!blocked) {
            this->PoofDust();
            this->MarkForDestruction();
        }
    }
    return 1;
}
// @symbol _ZN8daKrpa_c19func_ov070_02121438Ev
/* Particle::System::NewSimple is not yet declared by its shared header; retain
 * this typed ABI import without guessing the unresolved state's source name. */
namespace Sound { void PlayBank0(u32 soundID, const Vector3 &pos); }
extern "C" u32 _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 effectID, Fix12i x, Fix12i y, Fix12i z);

int daKrpa_c::func_ov070_02121438()
{
    Sound::PlayBank0(9, *(Vector3 *)&this->mCamSpacePosX);
    this->mFlags &= ~1;
    this->mVertAccel = -0x2000;
    this->mTerminalVelocity = -0x3c000;
    this->mHorzSpeed = 0xa000;
    this->mVertSpeed = 0x28000;
    this->mScaleX = 0x1000;
    this->mScaleY = 0x1000;
    this->mScaleZ = 0x1000;
    this->mStateTimer = 0x2d;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &this->mModelAnim, (BCA_File *)data_ov070_021234c4, 0, 0x1000, 0);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
        0x43, this->mPosX, this->mPosY, this->mPosZ);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
        0x44, this->mPosX, this->mPosY, this->mPosZ);
    this->mStateIndex = 3;
    return 1;
}
// @symbol _ZN8daKrpa_c19func_ov070_021213ccEv
/* The collision update veneer is retained because it is the retail call
 * destination; the rest are ordinary real class calls. */
extern "C" {
extern int dBgCh_Actr_UpdateDiscreteNoLava_veneer(dBgCh_Actr *clsn);
}
int daKrpa_c::func_ov070_021213cc() {
    this->mAngleX = this->mAngleX - 0x1000;
    this->mModelAnim.Advance();
    this->UpdatePos(&this->mdCcAcPos_c);
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&this->mWithMeshClsn);
    if (!this->mWithMeshClsn.JustHitGround()) {
        if (DecIfAbove0_Byte(&this->mStateTimer) != 0)
            goto end;
    }
    this->PoofDust();
    this->MarkForDestruction();
end:
    return 1;
}
// @symbol _ZN8daKrpa_c19func_ov070_02121310Ev
/* DropShadowRadHeight is a Fix12-by-value caller seam for the same codegen
 * reason as the two Init imports above. */
extern "C" void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
extern "C" void Matrix4x3_FromRotationY(void *m, int angle);
extern "C" void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
    dActor_c *actor, dExtShadowModel_c *shadow, Matrix4x3 *matrix,
    Fix12i radius, Fix12i depth, u32 opacity);

void daKrpa_c::func_ov070_02121310()
{
    if (this->mStateIndex == 3) {
        Matrix4x3_FromRotationXYZExt(&this->mModelAnim.mat4x3,
            this->mAngleX, this->mAngleY, this->mAngleZ);
    } else {
        Matrix4x3_FromRotationY(&this->mModelAnim.mat4x3, this->mAngleY);
    }
    this->mModelAnim.mat4x3.t.x = this->mPosX >> 3;
    this->mModelAnim.mat4x3.t.y = this->mPosY >> 3;
    this->mModelAnim.mat4x3.t.z = this->mPosZ >> 3;
    this->mMatrix.t.x = this->mPosX >> 3;
    this->mMatrix.t.y = this->mPosY >> 3;
    this->mMatrix.t.z = this->mPosZ >> 3;
    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
        this, &this->mShadowModel, &this->mMatrix,
        this->mScaleX * 0x46, this->mGroundDistance, 0xf);
}

bool ApproachLinear(short &value, short target, short step);
extern "C" {
extern int Vec3_Dist(void* a, void* b);
extern short Vec3_HorzAngle(void* a, void* b);
}
// @symbol _ZN8daKrpa_c19func_ov070_02121298Ev
void daKrpa_c::func_ov070_02121298() {
    Player *player = this->ClosestNonVanishPlayer();
    if (!player) {
        this->mPlayer = 0;
        return;
    }
    if (Vec3_Dist(&this->mPosX, &player->mPosX) >= 0x2bc000) {
        this->mPlayer = 0;
        return;
    }
    this->mPlayer = player;
    ApproachLinear(this->mAngleY,
        Vec3_HorzAngle(&this->mPosX, &player->mPosX), 0x800);
}
// @symbol _ZN8daKrpa_c19func_ov070_021211c4Ev
void daKrpa_c::func_ov070_021211c4()
{
    u32 id = this->mdCcAcPos_c.otherOwner;
    if (id == 0) return;
    dActor_c *found = dActor_c::FindWithID(id);
    if (found == 0) return;
    int isPlayer = (found->actorID == 0xbf);
    if (isPlayer == 0) return;
    Player *player = (Player *)found;
    int beingEaten = ((this->mFlags & 0x20000) != 0);
    if (beingEaten != 0) {
        this->func_ov070_02121880(2);
        return;
    }
    if ((this->mdCcAcPos_c.hitFlags & 0x10) == 0) return;
    this->mPrevAngleY = Vec3_HorzAngle(&player->mPosX, &this->mPosX);
    this->mAngleY = (short)(this->mPrevAngleY + 0x8000);
    player->IncMegaKillCount();
    this->func_ov070_02121880(3);
}

// @symbol _ZN8daKrpa_c13OnYoshiTryEatEv

int daKrpa_c::OnYoshiTryEat()
{
    return 5;
}

// @symbol _ZN8daKrpa_cD0Ev

/* No separate body: the inline class destructor plus vtable instantiation
 * makes mwcc emit the retail deleting variant after D1. */

// @symbol _ZN8daKrpa_cD1Ev

/* No separate body: the inline class destructor emits this complete variant
 * first, through the class vtable instantiated in this TU. */
