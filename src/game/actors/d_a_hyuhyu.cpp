//cpp
/**
 * Fwoosh, the cloud that blows (profile HYUHYU).
 *
 * The cloud (mVariant 0) turns toward the nearest player. Once that player
 * is within 1000.0 and passes the signed bearing threshold, it plays its
 * blow animation; on
 * frames 31 and later it emits the two gust particle streams and, every
 * other frame, spawns a gust -- another HYUHYU actor with param1 = 1 --
 * aimed at the player with a little random spread in heading. When the
 * animation ends it waits 100 frames before it can blow again.
 *
 * A gust (mVariant 1) has no model. It flies 30.0 a frame along its pitch
 * and heading while its pitch climbs, and removes itself after 40 frames or
 * on touching the ground or a wall. A player it hits, unless metal, mega,
 * already collecting a cap, is blown away. Wings and balloon form prevent
 * the subsequent cap loss; otherwise an eligible cap is dropped as an
 * OBJ_MARIO_CAP actor that flies off.
 *
 * daHyuhyu_c_classInit is reconstructed (RTTI daHyuhyu_c, HYUHYU registry
 * profile). Retail does not store that spelling. Historical alias:
 * Fwoosh_Spawn.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm emits one .text
 * section per function in the reverse of source order.
 *
 * The six state handlers, the state setter and the two helpers are
 * daHyuhyu_c members under their address names. ov091's static
 * initializer copies their pointer-to-members into the three state
 * records mState points at.
 *
 * Known limits:
 * - dCcAc_c::Init, dBgCh_Actr::Init, ModelAnim::SetAnim and
 *   Particle::System::New stay mangled: each takes Fix12<int> by value
 *   (notes/mwccarm-codegen.md 6az).
 * - data_ov091_02135674 / 0213567c / 02135684 are this overlay's BMD and
 *   two BCA SharedFilePtr handles; data_ov002_0210da40 / 0210d9a0 /
 *   0210d9c0 are shared ov002 files the cloud also claims. symbols.txt has
 *   no recovered names for any of them.
 * - func_ov002_020aea30 (fire hit) and func_02012694 (sound at a
 *   camera-space position) are unnamed.
 */

#include "common.h"
#include "daHyuhyu_c.h"
#include "Player.h"
#include "SaveData.h"
#include "SharedFilePtr.h"

struct BCA_File;
struct BMD_File;

extern "C" {
extern SharedFilePtr data_ov002_0210da40;
extern SharedFilePtr data_ov002_0210d9a0;
extern SharedFilePtr data_ov002_0210d9c0;
extern SharedFilePtr data_ov091_02135674;         /* BMD */
extern SharedFilePtr data_ov091_0213567c;         /* BCA: blow */
extern SharedFilePtr data_ov091_02135684;         /* BCA: wait */
extern daHyuhyu_c_State data_ov091_021356b0; /* cloud: blowing */
extern daHyuhyu_c_State data_ov091_021356c0; /* gust: flying */
extern daHyuhyu_c_State data_ov091_021356d0; /* cloud: waiting */
extern Matrix4x3 data_020a0e68;                   /* scratch matrix */
extern s16 data_02082214[];                       /* sin/cos pairs */
extern int data_0209e650;                         /* random seed */

void func_ov002_020aea30(void *self, void *actor, void *collision);
void func_02012694(u32 id, void *pos);
unsigned short DecIfAbove0_Short(unsigned short *p);
int RandomIntInternal(int *seed);
s16 Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0);
s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
int Vec3_Dist(const Vector3 *a, const Vector3 *b);
int ApproachAngle(s16 *cur, s16 target, int a, int b, int c);
void Vec3_Asr(Vector3 *dst, const Vector3 *src, int n);
void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationXYZExt(Matrix4x3 *m, int rx, int ry, int rz);
void Matrix4x3_FromRotationY(Matrix4x3 *m, int angle);
void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *m, s16 angX);
void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *res);

void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    dCcAc_c *self, dActor_c *actor, int radius, int height, u32 flags, u32 vulnFlags);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    dBgCh_Actr *self, dActor_c *actor, int radius, int height, Vector3_16 *a, Vector3_16 *b);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    ModelAnim *self, BCA_File *file, int flags, int speed, u16 startFrame);
int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 slot, u32 effect, int x, int y, int z, const Vector3_16 *rot, void *cb);
}

// @symbol daHyuhyu_c_classInit
/* The dEnemyBase_c C2, the vptr store and the dCcAc_c / dBgCh_Actr /
   ModelAnim C1s at 0x110 / 0x144 / 0x300 all fall out of the one `new`. */
extern "C" daHyuhyu_c *daHyuhyu_c_classInit()
{
    return new daHyuhyu_c();
}

// @symbol _ZN10daHyuhyu_c13OnYoshiTryEatEv
int daHyuhyu_c::OnYoshiTryEat()
{
    return 4;
}

// @symbol _ZN10daHyuhyu_c13OnTurnIntoEggER6Player
void daHyuhyu_c::OnTurnIntoEgg(Player &player)
{
    MarkForDestruction();
}

// @symbol _ZN10daHyuhyu_c16OnAimedAtWithEggEv
int daHyuhyu_c::OnAimedAtWithEgg()
{
    return 0x1f000;
}

// @symbol _ZN10daHyuhyu_c13InitResourcesEv
int daHyuhyu_c::InitResources()
{
    mVariant = param1 & 0xff;
    mTerminalVelocity = -0x1e000;
    if (mVariant == 0xff)
        mVariant = 0;
    if (mVariant == 1) {
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x3c000, 0x14000, 0x200002, 0);
        func_ov091_02134044(&data_ov091_021356c0);
        _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x14000, 0, 0, 0);
        mFlags = 0;
        return 1;
    }
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x64000, 0x64000, 0x200002, 0x4a010);
    mFlags = 0x10000000;
    Model::LoadFile(data_ov002_0210da40);
    Model::LoadFile(data_ov002_0210d9a0);
    Model::LoadFile(data_ov002_0210d9c0);
    mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov091_02135674), 1, -1);
    dExtFrameCtrl_c::LoadFile(data_ov091_0213567c);
    dExtFrameCtrl_c::LoadFile(data_ov091_02135684);
    mModelAnim.speed = 0x1000;
    func_ov091_02134044(&data_ov091_021356d0);
    return 1;
}

// @symbol _ZN10daHyuhyu_c8BehaviorEv
int daHyuhyu_c::Behavior()
{
    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        mdCcAc_c.Clear();
        if (mEatenByYoshi != 0 && unk_104 == 0)
            mdCcAc_c.Update();
        func_ov091_02134094();
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    {
        daHyuhyu_c_State *state = mState;
        if (state->execute != 0)
            (this->*state->execute)();
    }
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    /* Fall, no faster than mTerminalVelocity. The cartridge reloads and
       rewrites unk_0ac here too (a whole-vector store, most likely); a plain
       clamp misses. */
    {
        int speed = mVertSpeed + mVertAccel;
        int limit = mTerminalVelocity;
        if (speed >= limit)
            limit = speed;
        int tmp = unk_0ac;
        mVertSpeed = limit;
        unk_0ac = tmp;
    }
    UpdatePosWithOnlySpeed(&mdCcAc_c);
    func_ov091_021339fc();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();

    if (mVariant == 1) {
        UpdateWMClsn(mWithMeshClsn, 0);
        return 1;
    }

    mModelAnim.Advance();
    func_ov091_02134094();
    return 1;
}

// @symbol _ZN10daHyuhyu_c6RenderEv
int daHyuhyu_c::Render()
{
    if (mVariant == 1)
        return 1;
    int hidden = (mFlags & 0x40000) != 0;
    if (hidden)
        return 1;
    mModelAnim.Render(0);
    return 1;
}

// @symbol _ZN10daHyuhyu_c16OnPendingDestroyEv
void daHyuhyu_c::OnPendingDestroy()
{
}

// @symbol _ZN10daHyuhyu_c16CleanupResourcesEv
int daHyuhyu_c::CleanupResources()
{
    if (mVariant == 1)
        return 1;
    data_ov002_0210da40.Release();
    data_ov002_0210d9a0.Release();
    data_ov002_0210d9c0.Release();
    data_ov091_02135674.Release();
    data_ov091_0213567c.Release();
    data_ov091_02135684.Release();
    return 1;
}

// @symbol _ZN10daHyuhyu_c19func_ov091_02134094Ev
/* The model matrix: translation at 1/8 scale, then the rotation. */
void daHyuhyu_c::func_ov091_02134094()
{
    Vector3 pos;
    Vec3_Asr(&pos, (Vector3 *)&this->mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, pos.x, pos.y, pos.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        this->mAngleX, this->mAngleY, this->mAngleZ);
    this->mModelAnim.mat4x3 = data_020a0e68;
}

// @symbol _ZN10daHyuhyu_c19func_ov091_02134044EP16daHyuhyu_c_State
/* Enter `state`, running its enter handler if it has one. */
int daHyuhyu_c::func_ov091_02134044(daHyuhyu_c_State *state)
{
    this->mState = state;
    daHyuhyu_c_State *cur = this->mState;
    if (cur->enter == 0)
        return 1;
    return (this->*cur->enter)();
}

// @symbol _ZN10daHyuhyu_c19func_ov091_0213400cEv
/* Cloud, waiting: enter. Loop the wait animation. */
int daHyuhyu_c::func_ov091_0213400c()
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&this->mModelAnim,
        ((BCA_File **)&data_ov091_02135684)[1], 0, 0x1000, 0);
    return 1;
}

// @symbol _ZN10daHyuhyu_c19func_ov091_02133f60Ev
/* Cloud, waiting: execute. Turn toward the nearest player; blow once the
   cool-down is over, that player is within 1000.0, and the signed bearing
   returned by Vec3_HorzAngle is below 0x1000. */
int daHyuhyu_c::func_ov091_02133f60()
{
    Player *player = this->ClosestPlayer();
    Vector3 target;
    s16 angle;

    if (player == 0)
        goto done;
    {
        Vector3 *pos = (Vector3 *)&player->mPosX;
        target.x = pos->x;
        target.y = pos->y;
        target.z = pos->z;
    }
    angle = Vec3_HorzAngle((Vector3 *)&this->mPosX, &target);
    ApproachAngle(&this->mPrevAngleY, angle, 0xa, 0x200, 0x100);
    if ((u16)this->mStateTimer != 0)
        goto done;
    if (Vec3_Dist((Vector3 *)&this->mPosX, &target) >= 0x3e8000)
        goto done;
    if (angle >= 0x1000)
        goto done;
    func_ov091_02134044(&data_ov091_021356b0);
done:
    return 1;
}

// @symbol _ZN10daHyuhyu_c19func_ov091_02133f24Ev
/* Cloud, blowing: enter. Play the blow animation once. */
int daHyuhyu_c::func_ov091_02133f24()
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&this->mModelAnim,
        ((BCA_File **)&data_ov091_0213567c)[1], 0x40000000, 0x1000, 0);
    return 1;
}

// @symbol _ZN10daHyuhyu_c19func_ov091_02133d30Ev
/* Cloud, blowing: execute. From frame 31 on, stream the gust particles
   and spawn a gust at the nearest player every other frame; when the
   animation ends, cool down for 100 frames and wait again. */
int daHyuhyu_c::func_ov091_02133d30()
{
    Vector3_16 rot;
    Vector3 pos;
    Vector3 target;
    dActor_c *gust;
    Player *player;
    s16 heading;

    pos.x = this->mPosX;
    pos.y = this->mPosY;
    pos.z = this->mPosZ;

    u16 frame = (u16)(this->mModelAnim.currFrame >> 12);
    if (frame == 0x1f)
        func_02012694(0x164, &this->mCamSpacePosX);

    if ((u16)(this->mModelAnim.currFrame >> 12) > 0x1e) {
        rot.x = data_02082214[((u16)this->mAngleY >> 4) * 2];
        rot.y = 0;
        rot.z = data_02082214[((u16)this->mAngleY >> 4) * 2 + 1];

        pos.y = pos.y + 0x28000;

        this->mGustParticle = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            this->mGustParticle, 0x72, pos.x, pos.y, pos.z, &rot, 0);
        this->mGustParticle2 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            this->mGustParticle2, 0x73, pos.x, pos.y, pos.z, &rot, 0);

        if (!((u16)(this->mModelAnim.currFrame >> 12) & 1)) {
            gust = dActor_c::Spawn(0xe7, 1, *(Vector3 *)&this->mPosX, 0, this->mAreaId, -1);
            if (gust != 0) {
                player = this->ClosestPlayer();
                if (player != 0) {
                    Vector3 *ppos = (Vector3 *)&player->mPosX;
                    target.x = ppos->x;
                    target.y = ppos->y;
                    target.z = ppos->z;
                    target.y = player->mGroundY;

                    {
                        s16 base = this->mPrevAngleY;
                        heading = base + (s16)(0x2000 - ((((u32)RandomIntInternal(&data_0209e650) >> 8) & 3) << 0xc));
                    }

                    gust->mPrevAngleX = Vec3_VertAngle((Vector3 *)&this->mPosX, &target);
                    gust->mPrevAngleY = heading;
                    gust->mPrevAngleZ = 0;
                }
            }
        }
    }

    if (this->mModelAnim.Finished() != 0) {
        this->mStateTimer = 100;
        func_ov091_02134044(&data_ov091_021356d0);
    }

    return 1;
}

// @symbol _ZN10daHyuhyu_c19func_ov091_02133d1cEv
/* Gust, flying: enter. Live 40 frames. */
int daHyuhyu_c::func_ov091_02133d1c()
{
    this->mStateTimer = 40;
    return 1;
}

// @symbol _ZN10daHyuhyu_c19func_ov091_02133c6cEv
/* Gust, flying: execute. Climb, fly 30.0 a frame along pitch and heading,
   and go away when the time runs out or the gust touches ground or wall. */
int daHyuhyu_c::func_ov091_02133c6c()
{
    Vector3 fwd;
    fwd.x = 0;
    fwd.y = 0;
    fwd.z = 0x1e000;
    this->mPrevAngleX -= 0x80;
    Matrix4x3_FromRotationY(&data_020a0e68, this->mPrevAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, this->mPrevAngleX);
    MulVec3Mat4x3(&fwd, &data_020a0e68, (Vector3 *)&this->unk_0a4);
    if ((u16)this->mStateTimer == 0
        || this->mWithMeshClsn.IsOnGround() != 0
        || this->mWithMeshClsn.IsOnWall() != 0
        || (this->mFlags & 8))
        this->MarkForDestruction();
    return 1;
}

// @symbol _ZN10daHyuhyu_c19func_ov091_021339fcEv
/* What the collision cylinder hit this frame.
   The cloud: fire hurts it, an egg or a metal player pops it, and a mega
   player destroys it for a mega-kill.
   A gust: blows a player away unless metal, mega or collecting a cap.
   Wings and balloon form prevent the subsequent cap loss. When mCharacter
   and the player's param1 differ the player is switched back with
   SetNewHatCharacter; otherwise the cap is recorded as lost (once). Either
   way an OBJ_MARIO_CAP is dropped and flies off along the gust. The
   isPlayer temporaries are the ROM's shape: a plain `!=` test misses. */
void daHyuhyu_c::func_ov091_021339fc()
{
    u32 hitFlags;
    u32 id = this->mdCcAc_c.otherOwner;
    Player *player;

    if (id == 0)
        return;
    player = (Player *)dActor_c::FindWithID(id);
    if (this->mVariant == 0) {
        hitFlags = this->mdCcAc_c.hitFlags;
        if ((hitFlags & 0x40000) != 0) {
            this->mDeathState = 4;
            func_ov002_020aea30(this, player, 0);
            return;
        }
        if ((hitFlags & 0x2000) != 0) {
            this->PoofDust();
            this->MarkForDestruction();
            return;
        }
        {
            int isPlayer = (int)(player->actorID == 0xbf);
            if (isPlayer == 0)
                return;
        }
        if (player->mIsMetal == 1) {
            this->PoofDust();
            this->MarkForDestruction();
            return;
        }
        if ((hitFlags & 0x10) == 0)
            return;
        this->PoofDust();
        this->SpawnMegaCharParticles(*player, 0);
        player->IncMegaKillCount();
        func_02012694(0x1d, &this->mCamSpacePosX);
        this->MarkForDestruction();
        return;
    }

    {
        int isPlayer = (int)(player->actorID == 0xbf);
        if (isPlayer == 0)
            return;
    }
    if (player->mIsMetal != 0)
        return;
    if (player->mIsMega != 0)
        return;
    if (player->IsCollectingCap() != 0)
        return;
    player->BlowAway(this->mPrevAngleY);
    {
        u8 hasWings = player->mHasWings;
        u8 character = player->mCharacter;
        if (hasWings != 0)
            return;
        if (player->mIsBalloon != 0)
            return;
        {
            u32 cur = player->param1;
            u32 capParam = 1;
            if (character != cur) {
                player->SetNewHatCharacter(character, 0, 0);
            } else {
                if (SaveData::HasPlayerLostCap() != 0)
                    return;
                SaveData::PlayerLoseCap();
            }
            {
                u32 capChar = player->param1;
                Vector3_16 rot;
                dActor_c *cap;
                rot.x = 0;
                rot.y = 0;
                rot.z = 0;
                rot.y = this->mPrevAngleY;
                capParam = capParam | (capChar << 8);
                cap = dActor_c::Spawn(0x10d, capParam, *(Vector3 *)&this->mPosX, &rot,
                    this->mAreaId, -1);
                if (cap == 0)
                    return;
                cap->mHorzSpeed = 0x32000;
                cap->unk_0a4 = 0;
                cap->mVertSpeed = 0x14000;
                cap->unk_0ac = 0;
            }
        }
    }
}

// @symbol _ZN10daHyuhyu_cD1Ev
// @symbol _ZN10daHyuhyu_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */
