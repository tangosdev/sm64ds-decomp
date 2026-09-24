//cpp
/* Production translation unit for ov098/daWbm_c, hand-curated.
 * 14 function(s), .text 0x0213b47c..0x0213bf60.
 *
 * The water bomb. Variant 0 is an invisible dropper that waits for a player
 * to pass beneath it and drops a variant-1 bomb ahead of them. A variant-1
 * bomb falls, squashes and hops toward the closest player, and bursts into
 * five variant-2 fragments on its third landing or when it hits a player.
 * Behavior runs one of three states out of a pointer-to-member table that
 * the module's static initializer fills:
 *
 *   0 func_ov098_0213b9d8  dropper: wait for a player below, drop a bomb
 *   1 func_ov098_0213bb1c  falling
 *   2 func_ov098_0213b7e8  squash, stretch and hop after a landing
 *
 * NAME: daWbm_c is the cartridge's RTTI spelling. _ZTS at ov098 0x0213c734
 * is the string "7daWbm_c", and the _ZTI at 0x0213c740 names the vtable at
 * 0x0213c770 as this class's. The tree called the class WaterBomb until then.
 *
 * THE DESTRUCTOR IS INLINE AND EMPTY in the class header, so Behavior is the
 * key function and this TU emits _ZTV7daWbm_c, _ZTI7daWbm_c and _ZTS7daWbm_c
 * with the inherited bases' RTTI records. D1 and D0 are emitted from the
 * header, so there is no destructor text here to mark. The factory's
 * `new daWbm_c` synthesizes the constructor the ROM inlines into it.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so
 * the highest-address ROM function, the factory, is written FIRST here. Do
 * not reorder.
 *
 * Leftover: dCcAc_c::Init, dBgCh_Actr::Init, dActor_c::DropShadowRadHeight,
 *   Player::Hurt and Particle::System::NewSimple stay mangled. Each takes
 *   Fix12<int> by value (wall 6az), and a member call homes the argument
 *   and changes the code. ModelBase::SetFile stays mangled because
 *   include/ModelBase.h declares it void and InitResources reads its result.
 * Leftover: the three state bodies and their four helpers keep their
 *   func_ov098_* linker names as C-linkage helpers over a daWbm_c pointer.
 *   A member spelling would coin seven mangled names; that renaming is
 *   separate work.
 * Leftover: the state table (data_ov098_0213c930, .bss), the fragment
 *   angle table (data_ov098_0213bf90) and the bomb model file
 *   (data_ov098_0213c91c) are unnamed ov098 rows this TU does not own;
 *   func_0201267c, the sound call, is an unnamed arm9 function.
 */

/* common.h first, so its flat Matrix4x3 is the spelling this TU sees: with
   math/Matrix.h's {Matrix3x3, Vector3} form, the matrix copies in
   func_ov098_0213b584 are memberwise and come out 0x34 bytes longer. */
#include "common.h"
#include "daWbm_c.h"
#include "SharedFilePtr.h"
#include "Player.h"

typedef void (daWbm_c::*State)();

int ApproachLinear(int &value, int target, int step);
namespace cstd {
s32 fdiv(s32 numerator, s32 denominator);
}

extern "C" {
/* Model files: the fragment's, which the other variants load too, and the
   bomb's. */
extern SharedFilePtr data_ov002_0210da38;
extern SharedFilePtr data_ov098_0213c91c;
/* The state table, indexed by mState. */
extern State data_ov098_0213c930[];
/* The five fragments' launch angles. */
extern u16 data_ov098_0213bf90[];
/* The sine and cosine table. */
extern s16 data_02082214[];
/* The scratch matrix the matrix helpers build in. */
extern Matrix4x3 data_020a0e68;

void Vec3_Asr(Vector3 *dst, const Vector3 *src, int shift);
void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *m, s16 angY);
s16 Vec3_HorzAngle(const Vector3 *from, const Vector3 *to);
s32 Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
u16 DecIfAbove0_Short(u16 *counter);
void func_0201267c(u32 id, void *pos);

void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(dCcAc_c *self, dActor_c *actor,
    s32 radius, s32 height, u32 flags, u32 unk);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    dBgCh_Actr *self, dActor_c *actor, s32 radius, s32 height,
    Vector3_16 *a, Vector3_16 *b);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    dActor_c *self, ShadowModel *shadow, Matrix4x3 *matrix, s32 radius,
    s32 depth, u8 opacity);
int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(Player *self, const Vector3 *from,
    u32 a, s32 b, u8 c, u8 d, u8 e);
void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, s32 x, s32 y, s32 z);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(ModelBase *self, BMD_File *file, int a, int b);

void func_ov098_0213b520(daWbm_c *bomb);
void func_ov098_0213b584(daWbm_c *bomb);
void func_ov098_0213b63c(daWbm_c *bomb);
int func_ov098_0213b6e0(daWbm_c *bomb);
void func_ov098_0213b7e8(daWbm_c *bomb);
void func_ov098_0213b9d8(daWbm_c *bomb);
void func_ov098_0213bb1c(daWbm_c *bomb);
}

/* -------------------------------------------------------------------------- */
// @symbol daWbm_c_classInit
/* Reconstructed source-style name: SM64DS proves daWbm_c through RTTI,
 * allocation size, vtable identity and the WATERBOMB registry profile; later
 * EAD lineage supplies classInit. Historical alias: WaterBomb_Spawn. */
extern "C" int *daWbm_c_classInit(void)
{
    return (int *)new daWbm_c;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daWbm_c13InitResourcesEv
int daWbm_c::InitResources()
{
    BMD_File *bmd;

    mVariant = param1 & 0xf;

    if (mVariant == 2)
    {
        bmd = (BMD_File *)Model::LoadFile(data_ov002_0210da38);
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, bmd, 1, 0x16) == 0)
            return 0;
    }
    else
    {
        Model::LoadFile(data_ov002_0210da38);
        bmd = (BMD_File *)Model::LoadFile(data_ov098_0213c91c);
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, bmd, 1, 0x16) == 0)
            return 0;
    }

    if (mShadowModel.InitCylinder() == 0)
        return 0;

    if (mVariant == 0)
        mState = 0;
    else
        mState = 1;

    if (mVariant != 0)
    {
        if (mVariant == 2)
        {
            mScaleX = 0x800;
            mScaleY = 0x800;
            mScaleZ = 0x800;
            _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x14000, 0x28000, 0x200004, 0);
            _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x1e000, 0x1e000, 0, 0);
        }
        else
        {
            mScaleX = 0x1000;
            mScaleY = 0x1000;
            mScaleZ = 0x1000;
            _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x28000, 0x50000, 0x204004, 0);
            _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);
        }
    }

    mStateTimer = 0;
    mBounceCount = 0;
    unk_3b4 = 0;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;

    if ((unsigned int)(mVariant - 1) <= 1)
    {
        mVertAccel = -0x3000;
        mTerminalVelocity = -0x50000;
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daWbm_c8BehaviorEv
int daWbm_c::Behavior()
{
    if (func_ov098_0213b6e0(this)) {
        func_ov098_0213b584(this);
        mdCcAc_c.Clear();
        return 1;
    }

    (this->*data_ov098_0213c930[mState])();

    if (mVariant != 0) {
        UpdatePos(&mdCcAc_c);
        UpdateWMClsn(mWithMeshClsn, 0);
        if (mWithMeshClsn.IsOnWall()) {
            if (mVariant == 1)
                func_ov098_0213b63c(this);
            else
                MarkForDestruction();
            return 0;
        }
        func_ov098_0213b584(this);
        mdCcAc_c.Clear();
        mdCcAc_c.Update();
    } else {
        DecIfAbove0_Short((u16 *)&mStateTimer);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daWbm_c6RenderEv
int daWbm_c::Render()
{
    if (mVariant != 0) {
        mModel.Render((Vector3 *)&mScaleX);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daWbm_c16CleanupResourcesEv
int daWbm_c::CleanupResources()
{
    if (mVariant == 2) {
        data_ov002_0210da38.Release();
    } else {
        data_ov002_0210da38.Release();
        data_ov098_0213c91c.Release();
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov098_0213bb1c
/* State 1, falling. A bomb that lands squashes and hops; a fragment that
 * lands is gone. A variant-3 bomb ignores the ground and is destroyed once
 * it rises to 6144 units; nothing in this TU spawns one. */
extern "C" void func_ov098_0213bb1c(daWbm_c *bomb)
{
    if (bomb->mVariant != 3) {
        if (bomb->mWithMeshClsn.IsOnGround() == 0)
            return;
        if (bomb->mVariant == 1) {
            bomb->mState = 2;
            func_ov098_0213b520(bomb);
            func_ov098_0213b7e8(bomb);
            return;
        }
        bomb->MarkForDestruction();
        return;
    }
    if (bomb->mPosY < 0x1800000)
        return;
    bomb->MarkForDestruction();
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov098_0213b9d8
/* State 0, the dropper. Once the cooldown has run out, and the closest player
 * is within 1500 units horizontally and not above the dropper, drop a bomb
 * 1152 units above the player, led ahead of them by their speed. The arithmetic is
 * kept one step per statement; folded into fewer statements it allocates
 * registers differently. */
extern "C" void func_ov098_0213b9d8(daWbm_c *bomb)
{
    Vector3 v;
    Player *player;
    s32 angle;
    s32 speed;
    u32 uangle;
    int lead;
    int a;
    s16 sine, cosine;
    int x, y, z;
    int playerY, bombY;

    if ((u16)bomb->mStateTimer != 0)
        return;
    player = bomb->ClosestPlayer();
    if (player == 0)
        return;
    {
        const Vector3 &pos = *(const Vector3 *)&player->mPosX;
        v.x = pos.x;
        v.y = pos.y;
        v.z = pos.z;
    }
    if (Vec3_HorzDist(&v, (Vector3 *)&bomb->mPosX) > 0x5dc000)
        return;
    playerY = v.y;
    bombY = bomb->mPosY;
    if (playerY > bombY)
        return;

    angle = player->mPrevAngleY;
    speed = player->mHorzSpeed;
    y = playerY + 0x480000;
    uangle = (u32)(angle << 16) >> 16;
    a = (int)uangle >> 4;
    lead = speed >> 12;
    lead = lead * 0x1c;
    cosine = data_02082214[a * 2];
    sine = data_02082214[a * 2 + 1];
    lead = lead + 0x64;
    x = lead * cosine + v.x;
    v.y = y;
    v.x = x;
    z = lead * sine + v.z;
    v.z = z;
    dActor_c::Spawn(0xd0, 1, v, 0, bomb->mAreaId, -1)->mHorzSpeed = 0;
    bomb->mStateTimer = 0x96;
    func_0201267c(0xd8, &bomb->mCamSpacePosX);
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov098_0213b7e8
/* State 2, after a landing. Phase 0 squashes, and on the third landing the
 * bomb bursts at the bottom of the squash. Phase 1 stretches back up and hops
 * toward the closest player. Phase 2 is the hop; on the way down the bomb is
 * falling again. */
extern "C" void func_ov098_0213b7e8(daWbm_c *bomb)
{
    int state = bomb->mBouncePhase;
    if (state == 0) {
        if (bomb->mBounceCount >= 3) {
            if (ApproachLinear(bomb->mSquashScale, 0x100, bomb->mSquashSpeed) != 0) {
                func_ov098_0213b63c(bomb);
                return;
            }
        } else {
            if (ApproachLinear(bomb->mSquashScale, 0x300, bomb->mSquashSpeed) != 0)
                bomb->mBouncePhase++;
        }
        bomb->mSquashSpeed = cstd::fdiv((int)(((long long)bomb->mSquashSpeed * 0xb00 + 0x800) >> 12), 0x1000);
        bomb->mScaleY = bomb->mSquashScale;
        bomb->mScaleZ = 0x2000 - bomb->mScaleY;
        bomb->mScaleX = bomb->mScaleZ;
    } else if (state == 1) {
        int approach = ApproachLinear(bomb->mSquashScale, 0x1000, bomb->mSquashSpeed);
        if (approach != 0) {
            Player *player;
            bomb->mBouncePhase++;
            bomb->mVertSpeed = (bomb->mBounceCount << 13) + 0x30000;
            bomb->mHorzSpeed = 0x6000;
            player = bomb->ClosestPlayer();
            if (player != 0) {
                bomb->mPrevAngleY = Vec3_HorzAngle((Vector3 *)&bomb->mPosX, (Vector3 *)&player->mPosX);
            } else {
                func_ov098_0213b63c(bomb);
                return;
            }
        }
        bomb->mSquashSpeed = cstd::fdiv((int)(((long long)bomb->mSquashSpeed * 0x1400 + 0x800) >> 12), 0x1000);
        bomb->mScaleY = bomb->mSquashScale;
        bomb->mScaleZ = 0x2000 - bomb->mScaleY;
        bomb->mScaleX = bomb->mScaleZ;
    } else {
        if (bomb->mVertSpeed >= 0) {
            bomb->mScaleX = 0xf00;
            bomb->mScaleY = 0x1200;
            bomb->mScaleZ = 0xf00;
        } else {
            bomb->mState = 1;
            bomb->mScaleX = 0x1000;
            bomb->mScaleY = 0x1000;
            bomb->mScaleZ = 0x1000;
        }
    }
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov098_0213b6e0
/* The hit check. Touching a player (actor 0xbf) hurts them, and a bomb
 * bursts with a splash; touching actor 0xce bursts a bomb too. Returns 1
 * when the bomb burst. */
extern "C" int func_ov098_0213b6e0(daWbm_c *bomb)
{
    Vector3 pos;
    u32 id = bomb->mdCcAc_c.otherOwner;
    if (id != 0) {
        dActor_c *other = dActor_c::FindWithID(id);
        if (other != 0) {
            u16 actorID = other->actorID;
            /* Tested as a flag first: tested inline, the branch comes out a
               different shape. */
            int is = actorID == 0xbf;
            if (is) {
                pos.x = bomb->mPosX;
                pos.y = bomb->mPosY;
                pos.z = bomb->mPosZ;
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj((Player *)other, &pos, 1, 0xc000, 1, 0, 1);
                if (bomb->mVariant == 1) {
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x1c, bomb->mPosX, bomb->mPosY, bomb->mPosZ);
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x1d, bomb->mPosX, bomb->mPosY, bomb->mPosZ);
                    func_ov098_0213b63c(bomb);
                    return 1;
                }
            } else {
                is = actorID == 0xce;
                if (is) {
                    if (bomb->mVariant == 1) {
                        func_ov098_0213b63c(bomb);
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov098_0213b63c
/* Burst: throw five fragments out at the table's angles, play the splash
 * and go. */
extern "C" void func_ov098_0213b63c(daWbm_c *bomb)
{
    int i;
    for (i = 0; i < 5; i++) {
        dActor_c *a = dActor_c::Spawn(0xd0, 2, *(const Vector3 *)&bomb->mPosX,
                                      (const Vector3_16 *)0, bomb->mAreaId, -1);
        a->unk_0a4 = 0;
        a->mVertSpeed = 0x14000;
        a->unk_0ac = 0;
        a->mHorzSpeed = 0x14000;
        a->mPrevAngleX = 0;
        a->mPrevAngleY = data_ov098_0213bf90[i];
        a->mPrevAngleZ = 0;
    }
    func_0201267c(0xda, &bomb->mCamSpacePosX);
    bomb->MarkForDestruction();
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov098_0213b584
/* Place the model and the drop shadow at the bomb's position. */
extern "C" void func_ov098_0213b584(daWbm_c *bomb)
{
    Vector3 v;
    Vec3_Asr(&v, (Vector3 *)&bomb->mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    bomb->mShadowMat = data_020a0e68;
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, bomb->mAngleY);
    bomb->mModel.mat4x3 = data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        bomb, &bomb->mShadowModel, &bomb->mShadowMat,
        bomb->mScaleX * 0xa0, 0x3e8000, 6);
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov098_0213b520
/* A landing: stop, start the squash from the current height and count the
 * bounce. */
extern "C" void func_ov098_0213b520(daWbm_c *bomb)
{
    bomb->mSquashSpeed = 0x600;
    bomb->mSquashScale = bomb->mScaleY;
    bomb->mBounceCount++;
    bomb->unk_3b4 = 0;
    bomb->mVertSpeed = 0;
    bomb->mHorzSpeed = 0;
    bomb->mBouncePhase = 0;
    func_0201267c(0xd9, &bomb->mCamSpacePosX);
}
