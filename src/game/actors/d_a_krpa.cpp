//cpp
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

/* Manually curated translation unit -- ov070/daKrpa_c (25 function(s)).
 * tubuild create refused this TU (legacy bodies wrapped in extern "C" { }),
 * so it began as a reverse-ROM-order concatenation. It now uses the real
 * class, typed members, Player/collision headers, a typed retail factory seam,
 * and compiler-owned inline lifecycle. mwcc emits one .text section per
 * ordinary definition in reverse source order; the destructor variant group
 * is emitted first as retail D1 then D0, with no D2.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02121118  src/_ZN8daKrpa_cD1Ev.cpp
 *   [1] 0x02121160  src/_ZN8daKrpa_cD0Ev.cpp
 *   [2] 0x021211bc  src/_ZN8daKrpa_c13OnYoshiTryEatEv.cpp
 *   [3] 0x021211c4  src/func_ov070_021211c4.cpp
 *   [4] 0x02121298  src/func_ov070_02121298.cpp
 *   [5] 0x02121310  src/func_ov070_02121310.cpp
 *   [6] 0x021213cc  src/func_ov070_021213cc.c
 *   [7] 0x02121438  src/func_ov070_02121438.cpp
 *   [8] 0x021214f8  src/func_ov070_021214f8.cpp
 *   [9] 0x02121548  src/func_ov070_02121548.c
 *   [10] 0x0212156c  src/func_ov070_0212156c.c
 *   [11] 0x021216b8  src/func_ov070_021216b8.c
 *   [12] 0x02121710  src/func_ov070_02121710.cpp
 *   [13] 0x021217ac  src/func_ov070_021217ac.c
 *   [14] 0x0212180c  src/func_ov070_0212180c.cpp
 *   [15] 0x02121848  src/func_ov070_02121848.cpp
 *   [16] 0x02121880  src/func_ov070_02121880.c
 *   [17] 0x0212189c  src/_ZN8daKrpa_c16CleanupResourcesEv.cpp
 *   [18] 0x021218c0  src/_ZN8daKrpa_c16OnPendingDestroyEv.cpp
 *   [19] 0x021218c4  src/_ZN8daKrpa_c6RenderEv.cpp
 *   [20] 0x021218f4  src/_ZN8daKrpa_c8BehaviorEv.cpp
 *   [21] 0x02121914  src/_ZN8daKrpa_c13InitResourcesEv.cpp
 *   [22] 0x02121a64  src/func_ov070_02121a64.c
 *   [23] 0x02121ae0  src/func_ov070_02121ae0.c
 *   [24] 0x02121af8  src/daKrpa_c_Spawn.c
 */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- class initializer, 0x02121af8, size 0x50 */
/* -------------------------------------------------------------------------- */
/* Natural `new daKrpa_c` targets `_Znwm`, not the retail actor allocator.
 * Keep this one typed C-ABI construction seam at the actor-table boundary. */
extern "C" {
extern void *_ZN7fBase_cnwEj(u32 size);
extern void _ZN8dActor_cC2Ev(dActor_c *actor);
extern void _ZN9ModelAnimC1Ev(ModelAnim *model);
extern void _ZN11ShadowModelC1Ev(ShadowModel *shadow);
extern void _ZN10dCcAcPos_cC1Ev(dCcAcPos_c *clsn);
extern void _ZN10dBgCh_ActrC1Ev(dBgCh_Actr *clsn);
extern int _ZTV8daKrpa_c[];

// @symbol daKrpa_c_classInit
daKrpa_c *daKrpa_c_classInit(void)
{
    daKrpa_c *actor = (daKrpa_c *)_ZN7fBase_cnwEj(sizeof(daKrpa_c));
    if (actor) {
        _ZN8dActor_cC2Ev(actor);
        *(int *)actor = (int)&_ZTV8daKrpa_c[2];
        _ZN9ModelAnimC1Ev(&actor->mModelAnim);
        _ZN11ShadowModelC1Ev(&actor->mShadowModel);
        _ZN10dCcAcPos_cC1Ev(&actor->mdCcAcPos_c);
        _ZN10dBgCh_ActrC1Ev(&actor->mWithMeshClsn);
    }
    return actor;
}
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov070_02121ae0, 0x02121ae0, size 0x18 */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov070_02121ae0(
    daKrpaFrameController *controller, u32 *frames, u32 count, u32 mode)
{
    controller->frames = frames;
    controller->count = count;
    controller->mode = mode;
    controller->cursor = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov070_02121a64, 0x02121a64, size 0x7c */
/* -------------------------------------------------------------------------- */
extern "C" u32 func_ov070_02121a64(daKrpaFrameController *controller)
{
    switch (controller->mode) {
    case 0:
        if (controller->cursor < controller->count)
            ++controller->cursor;
        break;
    case 1:
        ++controller->cursor;
        controller->cursor %= controller->count;
        break;
    }
    return controller->frames[controller->cursor];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- _ZN8daKrpa_c13InitResourcesEv, 0x02121914, size 0x150 */
/* -------------------------------------------------------------------------- */
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
extern void func_ov070_02121880(void *self, int state);
extern void func_ov070_02121310(char *self);
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
    func_ov070_02121880(this, 0);
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
    func_ov070_02121310((char *)this);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- _ZN8daKrpa_c8BehaviorEv, 0x021218f4, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKrpa_c8BehaviorEv
extern "C" {
extern void func_ov070_02121310(char* c);
extern void func_ov070_0212180c(char *self);
}

int daKrpa_c::Behavior()
{
    func_ov070_0212180c((char *)this);
    func_ov070_02121310((char *)this);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- _ZN8daKrpa_c6RenderEv, 0x021218c4, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKrpa_c6RenderEv
int daKrpa_c::Render()
{
    mModelAnim.Render((const Vector3 *)&mScaleX);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- _ZN8daKrpa_c16OnPendingDestroyEv, 0x021218c0, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKrpa_c16OnPendingDestroyEv

void daKrpa_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- _ZN8daKrpa_c16CleanupResourcesEv, 0x0212189c, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKrpa_c16CleanupResourcesEv

int daKrpa_c::CleanupResources()
{
    data_ov070_02123698.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov070_02121880, 0x02121880, size 0x1c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* Unresolved func_ names retain their current C ABI spelling. */
extern daKrpaState data_ov070_021236ac[];
extern void func_ov070_02121848(char *c);
void func_ov070_02121880(void *raw, int state) {
    daKrpa_c *self = (daKrpa_c *)raw;
    self->mStateMethods = &data_ov070_021236ac[state];
    func_ov070_02121848((char *)self);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov070_02121848, 0x02121848, size 0x38 */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov070_02121848(char *raw)
{
    daKrpa_c *self = (daKrpa_c *)raw;
    daKrpaStateMethod *method = &self->mStateMethods->init;
    (self->**method)();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov070_0212180c, 0x0212180c, size 0x3c */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov070_0212180c(char *raw)
{
    daKrpa_c *self = (daKrpa_c *)raw;
    daKrpaStateMethod *method = &self->mStateMethods->behavior;
    (self->**method)();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov070_021217ac, 0x021217ac, size 0x60 */
/* -------------------------------------------------------------------------- */
/* SetAnim is another proven Fix12-by-value caller seam. */
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    ModelAnim *model, BCA_File *file, int flags, Fix12i speed, u32 startFrame);
extern char data_ov070_021234c4[];
extern char data_ov070_021234dc[];
extern u32 data_ov070_02122404[];
extern u32 data_ov070_021222e8[];

int func_ov070_021217ac(char *raw) {
    daKrpa_c *self = (daKrpa_c *)raw;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &self->mModelAnim, (BCA_File *)data_ov070_021234c4, 0, 0x1000, 0);
    func_ov070_02121ae0(
        &self->mFrameController, data_ov070_02122404, 0x64, 1);
    self->mStateTimer = 0x73;
    self->mStateIndex = 0;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov070_02121710, 0x02121710, size 0x9c */
/* -------------------------------------------------------------------------- */
extern "C" {
extern u8 DecIfAbove0_Byte(u8 *value);
extern void func_ov070_02121298(char *self);
extern void func_ov070_021211c4(char *self);
extern int data_0209f32c;
int func_ov070_02121710(char *raw) {
    daKrpa_c *self = (daKrpa_c *)raw;
    if (self->mPlayer) {
        if (self->mPosY > data_0209f32c) {
            if (DecIfAbove0_Byte(&self->mStateTimer) == 0)
                func_ov070_02121880(self, 1);
        }
    } else {
        self->mStateTimer = 0x73;
    }
    self->mModelAnim.Advance();
    u32 frame = func_ov070_02121a64(&self->mFrameController);
    self->mScaleX = frame;
    self->mScaleY = frame;
    self->mScaleZ = frame;
    func_ov070_02121298((char *)self);
    func_ov070_021211c4((char *)self);
    self->mdCcAcPos_c.Clear();
    self->mdCcAcPos_c.Update();
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov070_021216b8, 0x021216b8, size 0x58 */
/* -------------------------------------------------------------------------- */
extern "C" {
int func_ov070_021216b8(void *raw) {
    daKrpa_c *self = (daKrpa_c *)raw;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &self->mModelAnim, (BCA_File *)data_ov070_021234dc,
        0x40000000, 0x1000, 0);
    func_ov070_02121ae0(
        &self->mFrameController, data_ov070_021222e8, 0x47, 0);
    self->mStateIndex = 1;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov070_0212156c, 0x0212156c, size 0x14c */
/* -------------------------------------------------------------------------- */
extern "C" {
extern short data_02082214[];
void func_0201267c(u32 soundID, const Vector3 *pos);

int func_ov070_0212156c(char *raw) {
    daKrpa_c *self = (daKrpa_c *)raw;
    if (self->mFrameController.cursor == 0x1e) {
        Vector3 pos;
        int idx = (int)(u16)self->mAngleY >> 4;
        int s = data_02082214[idx * 2 + 1];
        int cn = data_02082214[idx * 2];
        int offZ = (int)(((s64)s * 0x50000 + 0x800) >> 12);
        int offX = (int)(((s64)cn * 0x50000 + 0x800) >> 12);
        int x = self->mPosX + offX;
        int z = self->mPosZ + offZ;
        int y = self->mPosY - 0x29000;
        pos.x = x;
        pos.z = z;
        pos.y = y;
        dActor_c::Spawn(0x10f, 0, pos,
            (Vector3_16 *)&self->mAngleX, self->mAreaId, -1);
        func_0201267c(0x105, (Vector3 *)&self->mCamSpacePosX);
    }
    if (self->mFrameController.cursor == self->mFrameController.count)
        func_ov070_02121880(self, 0);
    self->mModelAnim.Advance();
    u32 frame = func_ov070_02121a64(&self->mFrameController);
    self->mScaleX = frame;
    self->mScaleY = frame;
    self->mScaleZ = frame;
    func_ov070_02121298((char *)self);
    func_ov070_021211c4((char *)self);
    self->mdCcAcPos_c.Clear();
    self->mdCcAcPos_c.Update();
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov070_02121548, 0x02121548, size 0x24 */
/* -------------------------------------------------------------------------- */
extern "C" int func_ov070_02121548(char *raw)
{
    daKrpa_c *self = (daKrpa_c *)raw;
    self->mdCcAcPos_c.Clear();
    self->mStateIndex = 2;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov070_021214f8, 0x021214f8, size 0x50 */
/* -------------------------------------------------------------------------- */
extern "C" int func_ov070_021214f8(char *raw)
{
    daKrpa_c *self = (daKrpa_c *)raw;
    int flags = self->mFlags;
    int blocked = (flags & 0x20000) != 0;
    if (!blocked) {
        blocked = (flags & 0x40000) != 0;
        if (!blocked) {
            self->PoofDust();
            self->MarkForDestruction();
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov070_02121438, 0x02121438, size 0xc0 */
/* -------------------------------------------------------------------------- */
/* Particle::System::NewSimple is not yet declared by its shared header; retain
 * this typed ABI import without guessing the unresolved state's source name. */
namespace Sound { void PlayBank0(u32 soundID, const Vector3 &pos); }
extern "C" u32 _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 effectID, Fix12i x, Fix12i y, Fix12i z);

extern "C" int func_ov070_02121438(char *raw)
{
    daKrpa_c *self = (daKrpa_c *)raw;
    Sound::PlayBank0(9, *(Vector3 *)&self->mCamSpacePosX);
    self->mFlags &= ~1;
    self->mVertAccel = -0x2000;
    self->mTerminalVelocity = -0x3c000;
    self->mHorzSpeed = 0xa000;
    self->mVertSpeed = 0x28000;
    self->mScaleX = 0x1000;
    self->mScaleY = 0x1000;
    self->mScaleZ = 0x1000;
    self->mStateTimer = 0x2d;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &self->mModelAnim, (BCA_File *)data_ov070_021234c4, 0, 0x1000, 0);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
        0x43, self->mPosX, self->mPosY, self->mPosZ);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
        0x44, self->mPosX, self->mPosY, self->mPosZ);
    self->mStateIndex = 3;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov070_021213cc, 0x021213cc, size 0x6c */
/* -------------------------------------------------------------------------- */
/* The collision update veneer is retained because it is the retail call
 * destination; the rest are ordinary real class calls. */
extern "C" {
extern int dBgCh_Actr_UpdateDiscreteNoLava_veneer(dBgCh_Actr *clsn);
int func_ov070_021213cc(char *raw) {
    daKrpa_c *self = (daKrpa_c *)raw;
    self->mAngleX = self->mAngleX - 0x1000;
    self->mModelAnim.Advance();
    self->UpdatePos(&self->mdCcAcPos_c);
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&self->mWithMeshClsn);
    if (!self->mWithMeshClsn.JustHitGround()) {
        if (DecIfAbove0_Byte(&self->mStateTimer) != 0)
            goto end;
    }
    self->PoofDust();
    self->MarkForDestruction();
end:
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov070_02121310, 0x02121310, size 0xbc */
/* -------------------------------------------------------------------------- */
/* DropShadowRadHeight is a Fix12-by-value caller seam for the same codegen
 * reason as the two Init imports above. */
extern "C" void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
extern "C" void Matrix4x3_FromRotationY(void *m, int angle);
extern "C" void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    dActor_c *actor, ShadowModel *shadow, Matrix4x3 *matrix,
    Fix12i radius, Fix12i depth, u32 opacity);

extern "C" void func_ov070_02121310(char *raw)
{
    daKrpa_c *self = (daKrpa_c *)raw;
    if (self->mStateIndex == 3) {
        Matrix4x3_FromRotationXYZExt(&self->mModelAnim.mat4x3,
            self->mAngleX, self->mAngleY, self->mAngleZ);
    } else {
        Matrix4x3_FromRotationY(&self->mModelAnim.mat4x3, self->mAngleY);
    }
    self->mModelAnim.mat4x3.t.x = self->mPosX >> 3;
    self->mModelAnim.mat4x3.t.y = self->mPosY >> 3;
    self->mModelAnim.mat4x3.t.z = self->mPosZ >> 3;
    self->mMatrix.t.x = self->mPosX >> 3;
    self->mMatrix.t.y = self->mPosY >> 3;
    self->mMatrix.t.z = self->mPosZ >> 3;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        self, &self->mShadowModel, &self->mMatrix,
        self->mScaleX * 0x46, self->mGroundDistance, 0xf);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov070_02121298, 0x02121298, size 0x78 */
/* -------------------------------------------------------------------------- */
int ApproachLinear(short &value, short target, short step);
extern "C" {
extern int Vec3_Dist(void* a, void* b);
extern short Vec3_HorzAngle(void* a, void* b);
}

extern "C" void func_ov070_02121298(char *raw) {
    daKrpa_c *self = (daKrpa_c *)raw;
    Player *player = self->ClosestNonVanishPlayer();
    if (!player) {
        self->mPlayer = 0;
        return;
    }
    if (Vec3_Dist(&self->mPosX, &player->mPosX) >= 0x2bc000) {
        self->mPlayer = 0;
        return;
    }
    self->mPlayer = player;
    ApproachLinear(self->mAngleY,
        Vec3_HorzAngle(&self->mPosX, &player->mPosX), 0x800);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov070_021211c4, 0x021211c4, size 0xd4 */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov070_021211c4(char *raw)
{
    daKrpa_c *self = (daKrpa_c *)raw;
    u32 id = self->mdCcAcPos_c.otherOwner;
    if (id == 0) return;
    dActor_c *found = dActor_c::FindWithID(id);
    if (found == 0) return;
    int isPlayer = (found->actorID == 0xbf);
    if (isPlayer == 0) return;
    Player *player = (Player *)found;
    int beingEaten = ((self->mFlags & 0x20000) != 0);
    if (beingEaten != 0) {
        func_ov070_02121880(self, 2);
        return;
    }
    if ((self->mdCcAcPos_c.hitFlags & 0x10) == 0) return;
    self->mPrevAngleY = Vec3_HorzAngle(&player->mPosX, &self->mPosX);
    self->mAngleY = (short)(self->mPrevAngleY + 0x8000);
    player->IncMegaKillCount();
    func_ov070_02121880(self, 3);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN8daKrpa_c13OnYoshiTryEatEv, 0x021211bc, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKrpa_c13OnYoshiTryEatEv

int daKrpa_c::OnYoshiTryEat()
{
    return 5;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN8daKrpa_cD0Ev, 0x02121160, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKrpa_cD0Ev

/* No separate body: the inline class destructor plus vtable instantiation
 * makes mwcc emit the retail deleting variant after D1. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN8daKrpa_cD1Ev, 0x02121118, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daKrpa_cD1Ev

/* No separate body: the inline class destructor emits this complete variant
 * first, through the class vtable instantiated in this TU. */

