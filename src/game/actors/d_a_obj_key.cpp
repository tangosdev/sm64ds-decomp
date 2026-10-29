//cpp
struct Vector3_16f;
namespace Particle {
struct Callback;
struct System {
    static int New(unsigned, unsigned, int, int, int, const ::Vector3_16f *, Callback *);
};
}
/* daObjKey_c (ov089): OBJ_KEY is actor 0x11a (282) and LAST_STAR is 0x11b
 * (283); both factories install this vtable. param1 & 7 picks the kind: 7 uses
 * the power-star model (data_ov002_0211094c, file 0x8015) and StateStarJump; 3
 * uses StateFlyToCenter; the other six kinds are StateDrop.
 *
 * The TU is the whole of ov089's .text, 0x02130f00..0x02132880: the destructor
 * (key function) first, the helpers, the three states, the virtuals, then
 * both factories. __sinit_ov089_021328d4, which fills the state table, stays
 * its own .init file.
 *
 * Known limits:
 * - StateDrop: mWithMeshClsn.UpdateContinuous() is the same size (0x2b4) but
 *   the reloc target is _ZN10dBgCh_Actr16UpdateContinuousEv. The ROM calls the
 *   veneer at arm9:0x020383fc.
 * - StateDrop: Particle::System::New (Fix12<int> x/y/z) size 0x2b4 -> 0x2c8
 *   (+0x14). Behavior's two calls size 0x310 -> 0x344 (+0x34). The header
 *   does not declare New, and a bare int does not convert to Fix12<int>.
 * - InitResources: dExtAnmModel_c::SetAnim, both calls, Fix12<int> speed, size
 *   0x32c -> 0x344 (+0x18).
 * - InitResources: mWithMeshClsn.Init(...) links to the undefined
 *   _ZN10dBgCh_Actr4InitEP8dActor_ciiP10Vector3_16S3_: dBgCh_Actr.h spells
 *   the radii Fix12i, a plain s32, so the ROM's Fix12<int> mangling is
 *   called by name.
 * - func_ov089_02131df4: one control arm (mState != 7) size 0x110 -> 0xe8
 *   (-0x28). The ROM has both copies.
 * - LoadKeyModels / UnloadKeyModels are C by necessity: daDoor_c, Player and the
 *   bosses call them by name.
 * - StateFlyToCenter / StateStarJump: the Fix12<int> calls (Particle::System::New,
 *   ApproachLinear, Sound::ChangeMusicVolume) go by symbol for the same reason
 *   as StateDrop's. mStateTimer is counted through a u16 pointer; the ROM
 *   compares it unsigned.
 * - common.h's flat Matrix4x3 keeps the translation in m[9..11].
 * - The carry sparkle reads the first bone's word at +0xc; BMD_Bone does not
 *   name it.
 * - g_profile_OBJ_KEY / LAST_STAR stay outside this TU.
 * - data_0209f318 as dCamera_c * matches, but the plurality is void * so the
 *   cast stays.
 *
 * leftovers:
 * - func_ov089_02130fb4, func_ov089_0213115c, func_ov089_02131dcc and
 *   func_ov089_02131df4 are members under those addresses; the cartridge
 *   records no names for them.
 * - LoadKeyModels and UnloadKeyModels stay C. The first parameter is the
 *   kind, and daDoor_c, Player and the bosses call them by name.
 * - func_ov002_020c3dbc stays a call into ov002.
 * - The Fix12<int>-by-value calls, the UpdateContinuous veneer, both
 *   control arms in func_ov089_02131df4, the u16* state timer, the s32*
 *   position walks, v.y = v.y + Y_LOOK, and IsOnGround() == 0 stay as
 *   written. Each spelling was already measured; the other form moves bytes.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjKey_c_AssetId {
    ASSET_KOOPA_KEY_BMD = 0x044d,
    ASSET_KOOPA_KEY_GET_BCA = 0x044e,
    ASSET_KOOPA_KEY_GET_DEMO_BCA = 0x044f,
    ASSET_KOOPA_KEY_OPEN_BCA = 0x0450,
    ASSET_KOOPA_KEY_W_GET_BCA = 0x0451,
    ASSET_LUIGI_KEY_BMD = 0x049c,
    ASSET_MARIO_KEY_BMD = 0x049d,
    ASSET_WARIO_KEY_BMD = 0x049e
};

#include "types.h"
#include "common.h"
#include "decl_common.h"
#include "daObjKey_c.h"
#include "Player.h"
#include "dCamera_c.h"
#include "Sound.h"
#include "SharedFilePtr.h"
#include "Message.h"
#include "cstd.h"
#include "math/Approach.h"

#pragma defer_codegen off

namespace Event { void SetBit(unsigned int bit); int ClearBit(unsigned int bit); }

namespace Sound {
void LoadAndSetMusic_Layer3(unsigned int musicId);
}

/* This TU's file handle type. The star's idle animation
 * data_ov002_02110964 is declared through it as well. */
struct ObjKeyFile : SharedFilePtr {};

/* File-scope objects at the end of this file construct the eight resource
 * handles and fill the eight-entry state table. mwcc emits
 * __sinit_d_a_obj_key.cpp from those definitions. */
struct ObjKeyModelFile : ObjKeyFile {
    ObjKeyModelFile(u32 fileID);
    ~ObjKeyModelFile();
};

struct ObjKeyAnimationFilePtr : ObjKeyFile {
    ObjKeyAnimationFilePtr(u32 fileID);
    ~ObjKeyAnimationFilePtr();
};

/* Plain three-word offset (data_ov089_02132ca4). A Vector3 static would
 * register a destructor and grow __sinit, which the retail initializer
 * does not do; the zero-init here emits plain BSS. */
struct ObjKeyVec3 {
    int x, y, z;
};

enum {
    ACTOR_OBJ_KEY = 0x11a,
    KEY_KIND_STAR = 7,
    ANIM_CARRY = 3,
    EVENT_KEY = 0x1d,
    FLAG_CAM_TAKEOVER = 0x4000000,
    FLAG_HIDE = 0x40000,
    HIT_PLAYER = 0x400000,
    CC_DISABLED = 1,
    FX_FALL = 0x81,
    FX_BURST_A = 0x82,
    FX_BURST_B = 0x83,
    SND_LAND = 0x36,
    SND_APPEAR = 0x57,
    MUSIC_KEY = 0x17,
    Y_LOOK = 0x64000,
    Y_SPARK = 0x1c000,
    Y_BONE = 0x48000,
    SPIN_FAST = 0x400,
    SPIN_STEP = 0x100,
    SCALE_STAR = 0x2000,
    SCALE_KEY = 0x3000,
    GRAVITY = -0x2000,
    TERMINAL_VY = -0x32000,
    POP_VY = 0x23000,
    STAR_CLSN_R = 0xa0000,
    KEY_CLSN_R = 0x48000,
    STAR_CC_H = 0xfa000,
    KEY_CC_H = 0x64000,
    CC_R = 0x50000,
    CC_FLAGS = 0x800003,
    CC_VULN = 0x8000,
    SHADOW_R = 0x96000,
    SHADOW_DEPTH = 0x3e8000,
    SHADOW_OPACITY = 0xf,
    BONE_Y_MUL = 0x23,
    ANIM_FLAGS = 0x40000000,
    ANIM_SPEED = 0x1000,
    JUMP_FRAMES = 30,
    JUMP_HEIGHT = 0x190000,
    JUMP_TARGET_Y = 0x64000,
    CAM_Y = 0x78000,
    LAND_Y = 0xa000,
    SND_FLY = 0x73,
    SND_BOUNCE = 0x74,
    FLY_VY = 0x50000,
    BOUNCE_VY = 0x41000,
    HOVER_Y = 0xc8000,
    CAM_STEP = 0x28000,
    FX_TRAIL_A = 0xb4,
    FX_TRAIL_B = 0xb5,
    FX_TRAIL_C = 0xb6
};

/* The eight state slots, filled by __sinit_d_a_obj_key.cpp and indexed by
 * mState: three StateDrop, then StateStarJump, three more StateDrop, then
 * StateFlyToCenter. */
typedef void (daObjKey_c::*StateFunc)();

struct ObjKeyStateTable {
    StateFunc slots[8];
    /* Section-alignment fill: retail BSS runs 0x14 bytes past the eighth
     * slot, to the section end at 0x02132d40. The constructor never stores
     * here. */
    u8 tailFill[0x14];

    ObjKeyStateTable()
    {
        slots[0] = &daObjKey_c::StateDrop;
        slots[1] = &daObjKey_c::StateDrop;
        slots[2] = &daObjKey_c::StateDrop;
        slots[3] = &daObjKey_c::StateStarJump;
        slots[4] = &daObjKey_c::StateDrop;
        slots[5] = &daObjKey_c::StateDrop;
        slots[6] = &daObjKey_c::StateDrop;
        slots[7] = &daObjKey_c::StateFlyToCenter;
    }
};

extern ObjKeyStateTable data_ov089_02132cec;

/* local extern: dCamera_c.h has no SetFlag_3. Particle::System::New and
 * dExtAnmModel_c::SetAnim take Fix12<int> by value; the scalar call does not compile and the Fix12
 * temporary size-DIFFs (see the file comment). The ROM calls the
 * UpdateContinuous veneer, not the method. */
extern "C" {
extern void *data_0209f318;
extern int data_0209b454;
extern void *d_a_obj_keyNewHold(u32 a, u32 b, int c, int d, int e, void *f, void *g);
extern void func_02012694(unsigned int id, const Vector3 *pos);
extern void func_ov002_020c3dbc(void *player);
extern int data_0209caa0[];
void Matrix4x3_FromRotationY(void *m, short angle);
extern Matrix4x3 IDENTITY_MATRIX4X3;
extern void *data_ov089_021328b4[];
extern ObjKeyFile data_ov002_02110964;
extern ObjKeyAnimationFilePtr data_ov089_02132c60;
extern ObjKeyAnimationFilePtr data_ov089_02132c40;
extern ObjKeyAnimationFilePtr data_ov089_02132c70;
extern ObjKeyAnimationFilePtr data_ov089_02132c48;
extern ObjKeyModelFile data_ov089_02132c50;
extern ObjKeyModelFile gLuigiKeyModelFile;
extern ObjKeyModelFile gMarioKeyModelFile;
extern ObjKeyModelFile gWarioKeyModelFile;
extern Vector3 data_ov089_02132b40;
extern ObjKeyVec3 data_ov089_02132ca4;
extern Matrix4x3 data_020a0e68;
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, int ang);
extern void MulMat4x3Mat4x3(void *d, void *a, void *b);
extern void SubVec3(void *d, void *a, void *b);
extern void Vec3_LslInPlace(void *v, int sh);
extern void AddVec3(void *d, void *a, void *b);
extern void LoadKeyModels(int idx);
/* Parameter names keep this TU's @NNN. */
typedef void BgchInitAt_(void *thiz, void *actor, int a, int b, void *v, void *w);
/* An ObjKeyFile in layout, but daDemo_c::InitResources declares it `char`;
   keep that spelling so the declarations of this symbol stay in agreement. */
extern char data_ov002_0211094c;
extern int data_0209cef0;
extern int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
extern short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
/* The collect animations, by mAnimID. */
extern ObjKeyFile *data_ov089_02132880[];
/* Names stay on this unused prototype so this file's @NNN numbers do not move. */
extern int d_a_obj_keyChangeMusicVolumeHold(unsigned int id, int vol);
/* Level-overlay BSS: ov089 relocs record the store as claimed by fourteen
   overlays, and ov055 is the one that names 0x02111b68 unambiguous kind:bss
   (the func_ov002_020e3e00 precedent for its neighbours 0x02111b64/6c). */
extern int data_ov055_02111b68;
}

/* The key function, defined first. Under defer_codegen off it emits D1, D0 and
 * a D2 the cartridge does not keep, then the vtable and RTTI chain. */
// @symbol _ZN10daObjKey_cD1Ev
// @symbol _ZN10daObjKey_cD0Ev
daObjKey_c::~daObjKey_c()
{
}

// @symbol _ZN10daObjKey_c19func_ov089_02130fb4EP7Vector3i
/* Aim a jump at target so it lands in 30 frames with its peak `height` above
 * the higher end. Sets gravity, the launch speed and the heading. */
void daObjKey_c::func_ov089_02130fb4(Vector3 *target, int height)
{
    /* One register each: dy becomes the frames to the peak, height the frames after it. */
    int dy = target->y - mPosY;
    if (dy < 0)
        dy = -dy;
    {
        int s0 = cstd::sqrt((unsigned long long)(long long)height);
        int s1 = cstd::sqrt((unsigned long long)(long long)(height + dy));
        int s2 = cstd::sqrt((unsigned long long)(long long)height);
        dy = (s0 * JUMP_FRAMES) / (s1 + s2);
    }
    {
        int left = -(height << 1);
        int den = dy * dy;
        height = JUMP_FRAMES - dy;
        mVertAccel = left / den;
    }
    if (target->y >= mPosY) {
        int a = mVertAccel;
        if (a < 0)
            a = -a;
        mVertSpeed = height * a;
    } else {
        int a = mVertAccel;
        if (a < 0)
            a = -a;
        mVertSpeed = (dy + 1) * a;
    }
    mTerminalVelocity = TERMINAL_VY;
    mHorzSpeed = Vec3_HorzDist(&mPos, target) / JUMP_FRAMES;
    mPrevAngleY = Vec3_HorzAngle(&mPos, target);
}

/* UnloadKeyModels, 0x021310cc */
// @symbol UnloadKeyModels
/* daDoor_c, Player and the bosses that drop a key call these two by name. */
extern "C" void UnloadKeyModels(int kind)
{
    if (kind >= 8)
        return;
    ((SharedFilePtr *)data_ov089_02132894[kind])->Release();
    if (data_ov089_021328b4[kind] == 0)
        return;
    ((SharedFilePtr *)data_ov089_021328b4[kind])->Release();
}

/* LoadKeyModels, 0x02131114 */
// @symbol LoadKeyModels
extern "C" void LoadKeyModels(int kind)
{
    SharedFilePtr *extra;
    if (kind >= 8)
        return;
    dExtSimpleModel_c::LoadFile(*(SharedFilePtr *)data_ov089_02132894[kind]);
    extra = (SharedFilePtr *)data_ov089_021328b4[kind];
    if (extra == 0)
        return;
    dExtSimpleModel_c::LoadFile(*extra);
}

// @symbol _ZN10daObjKey_c19func_ov089_0213115cEi
/* Start collect animation `anim` (1..4); 0 or anything past 4 clears it. */
void daObjKey_c::func_ov089_0213115c(int anim)
{
    if (anim == 0 || anim >= 5) {
        mAnimID = 0;
        return;
    }
    mAnimID = anim;
    mAnmModel.SetAnim((BCA_File *)(data_ov089_02132880[anim]->filePtr), ANIM_FLAGS, ANIM_SPEED, 0);
}

// @symbol _ZN10daObjKey_c16StateFlyToCenterEv
/* Kind 3. Take the camera, fade the music, then fly toward the room's origin,
 * bounce once on the home height and settle 800 units above it until touched. */
void daObjKey_c::StateFlyToCenter()
{
    Vector3 v;
    Vector3 center1;
    Vector3 center2;
    dCamera_c *cam = (dCamera_c *)data_0209f318;

    switch (mStep) {
    case 0:
        {
            /* Pointer copies: indexing cam directly folds the offsets into the loads. */
            Vector3 *lookAt;
            Vector3 *pos;
            Vector3 *camLookAt;
            mFlags |= FLAG_CAM_TAKEOVER;
            data_0209b454 |= FLAG_CAM_TAKEOVER;
            lookAt = &cam->lookAt;
            camLookAt = &mCamLookAt;
            mCamLookAt.x = lookAt->x;
            mCamLookAt.y = lookAt->y;
            mCamLookAt.z = lookAt->z;
            pos = &cam->pos;
            mSavedCamLookAt.x = camLookAt->x;
            mSavedCamLookAt.y = camLookAt->y;
            mSavedCamLookAt.z = camLookAt->z;
            mSavedCamPos.x = pos->x;
            mSavedCamPos.y = pos->y;
            mSavedCamPos.z = pos->z;
            cam->SetFlag_3();
            mStep++;
            Sound::ChangeMusicVolume(0, data_0209b490 / 15);
            break;
        }

    case 1:
        {
            u16 *timer = (u16 *)&mStateTimer;
            *timer = *timer + 1;
            if (*timer < 70)
                break;
            mStep++;
            *timer = 0;
            Sound::PlayBank3(SND_FLY, mCamSpacePos);
            center1.x = 0;
            center1.y = 0;
            center1.z = 0;
            mVertSpeed = FLY_VY;
            mHorzSpeed = Vec3_HorzDist(&mPos, &center1) / 80;
            mPrevAngleY = Vec3_HorzAngle(&mPos, &center1);
            mBounceCount = 0;
            mVertAccel = GRAVITY;
            Message::EndTalk();
            break;
        }

    case 2:
        v.x = mPosX;
        v.y = mPosY;
        v.z = mPosZ;
        v.y = v.y + Y_LOOK;
        ApproachLinear(mCamLookAt, v, CAM_STEP);
        cam->SetLookAt(mCamLookAt);
        center2.x = 0;
        center2.y = 0;
        center2.z = 0;
        Vec3_ApproachHorz(&mPosX, &center2, mHorzSpeed);
        if (mBounceCount == 0) {
            if (mVertSpeed < 0) {
                int floor = mHomePos.y;
                if (mPosY < floor) {
                    mPosY = floor;
                    mVertSpeed = BOUNCE_VY;
                    mHorzSpeed = 0;
                    mBounceCount++;
                    Sound::PlayBank3(SND_BOUNCE, mCamSpacePos);
                }
            }
        } else {
            if (mVertSpeed < 0) {
                int hover = mHomePos.y + HOVER_Y;
                if (mPosY < hover) {
                    mPosY = hover;
                    mVertSpeed = 0;
                    mVertAccel = 0;
                    mStep++;
                    mStateTimer = 0;
                    mdCcAcPos_c.flags &= ~CC_DISABLED;
                    cam->mFlags &= ~8;
                    mFlags &= ~FLAG_CAM_TAKEOVER;
                    data_0209b454 &= ~FLAG_CAM_TAKEOVER;
                }
            }
        }
        break;

    case 3:
        {
            u32 id = mdCcAcPos_c.otherOwner;
            dActor_c *found;
            if (id == 0)
                break;
            found = dActor_c::FindWithID(id);
            if (found == 0)
                break;
            if ((mdCcAcPos_c.hitFlags & HIT_PLAYER) == 0)
                break;
            func_ov089_02131dcc((Player &)*found);
            return;
        }
    }

    Matrix4x3_FromTranslation(&data_020a0e68, mPosX, mPosY, mPosZ);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mPrevAngleY);
    MulMat4x3Mat4x3(mAnmModel.data.transforms, &data_020a0e68, &data_020a0e68);
    v.x = data_020a0e68.m[9];
    v.y = data_020a0e68.m[10];
    v.z = data_020a0e68.m[11];
    SubVec3(&v, &mPosX, &v);
    Vec3_LslInPlace(&v, 3);
    AddVec3(&v, &mPosX, &v);
    v.y = mScaleX * 13 + v.y;
    mParticleID[0] = (u32)Particle::System::New(mParticleID[0], FX_TRAIL_A, v.x, v.y, v.z, 0, 0);
    mParticleID[1] = (u32)Particle::System::New(mParticleID[1], FX_TRAIL_B, v.x, v.y, v.z, 0, 0);
    mParticleID[2] = (u32)Particle::System::New(mParticleID[2], FX_TRAIL_C, v.x, v.y, v.z, 0, 0);
}

// @symbol _ZN10daObjKey_c13StateStarJumpEv
/* Kind 7, the power star. Take the camera, jump to a spot beside the nearest
 * player, bounce to rest, hand the camera back and wait to be collected, then
 * ride along with the player while the collect animation plays. */
void daObjKey_c::StateStarJump()
{
    Vector3 v;
    Vector3 target;
    dCamera_c *cam = (dCamera_c *)data_0209f318;

    if (mStep < 3) {
        v.x = mPosX;
        v.y = mPosY;
        v.z = mPosZ;
        v.y = v.y + Y_SPARK;
        mParticleID[0] = (u32)Particle::System::New(mParticleID[0], FX_FALL, v.x, v.y, v.z, 0, 0);
    }
    if (mStep >= 2)
        dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);

    switch (mStep) {
    case 0:
        {
            Vector3 *lookAt;
            Vector3 *pos;
            Player *player;
            s32 *playerPos;
            mFlags |= FLAG_CAM_TAKEOVER;
            data_0209b454 |= FLAG_CAM_TAKEOVER;
            lookAt = &cam->lookAt;
            mSavedCamLookAt.x = lookAt->x;
            pos = &cam->pos;
            mSavedCamLookAt.y = lookAt->y;
            mSavedCamLookAt.z = lookAt->z;
            mSavedCamPos.x = pos->x;
            mSavedCamPos.y = pos->y;
            mSavedCamPos.z = pos->z;
            cam->SetFlag_3();
            v.x = mPosX;
            v.y = mPosY;
            v.z = mPosZ;
            v.y = v.y + Y_LOOK;
            cam->SetLookAt(v);
            player = ClosestPlayer();
            playerPos = &player->mPosX;
            v.x = playerPos[0];
            v.y = playerPos[1];
            v.z = playerPos[2];
            v.y = v.y + CAM_Y;
            cam->SetPos(v);
            mJumpTarget.x = v.x;
            mJumpTarget.z = -v.z;
            mJumpTarget.y = JUMP_TARGET_Y;
            if (v.x >= -0x1f4000)
                mJumpTarget.x = v.x - 0x12c000;
            else
                mJumpTarget.x = v.x;
            {
                int az = v.z < 0 ? -v.z : v.z;
                if (az < 0xc8000) {
                    mJumpTarget.z = -v.z;
                    if (v.z >= 0)
                        mJumpTarget.z -= 0xc8000;
                    else
                        mJumpTarget.z += 0xc8000;
                } else {
                    mJumpTarget.z = -v.z;
                }
            }
            target.x = mJumpTarget.x;
            target.y = mJumpTarget.y;
            target.z = mJumpTarget.z;
            func_ov089_02130fb4(&target, JUMP_HEIGHT);
            mStep++;
            return;
        }

    case 1:
        cam->SetLookAt(v);
        if (Vec3_HorzDist(&mPos, &mJumpTarget) >= mHorzSpeed)
            return;
        mPosX = mJumpTarget.x;
        mPosY = mJumpTarget.y;
        mPosZ = mJumpTarget.z;
        mPosY += LAND_Y;
        mVertSpeed = POP_VY;
        mVertAccel = GRAVITY;
        mHorzSpeed = 0;
        mStep++;
        return;

    case 2:
        cam->SetLookAt(v);
        if (mWithMeshClsn.JustHitGround()) {
            mVertSpeed = -mVertSpeed >> 1;
            func_02012694(SND_LAND, &mCamSpacePos);
            return;
        }
        if (mWithMeshClsn.IsOnGround() == 0)
            return;
        mStep++;
        mdCcAcPos_c.flags &= ~CC_DISABLED;
        mParticleID[0] = 0;
        mStateTimer = 0;
        data_ov055_02111b68 = 1;
        return;

    case 3:
        {
            u16 *timer = (u16 *)&mStateTimer;
            *timer = *timer + 1;
            if (*timer < 30)
                return;
            cam->SetLookAt(mSavedCamLookAt);
            cam->SetPos(mSavedCamPos);
            cam->mFlags &= ~8;
            mFlags &= ~FLAG_CAM_TAKEOVER;
            data_0209b454 &= ~FLAG_CAM_TAKEOVER;
            mStep++;
            return;
        }

    case 4:
        {
            u32 id = mdCcAcPos_c.otherOwner;
            dActor_c *found;
            if (id == 0)
                return;
            found = dActor_c::FindWithID(id);
            if (found == 0)
                return;
            if ((mdCcAcPos_c.hitFlags & HIT_PLAYER) == 0)
                return;
            func_ov089_02131df4((Player &)*found);
            mStep++;
            return;
        }

    case 5:
        {
            s32 *pos = &mPlayer->mPosX;
            mPosX = pos[0];
            mPosY = pos[1];
            mPosZ = pos[2];
            mAngleY = mPlayer->mAngleY;
            mAnmModel.Advance();
            if (mAnmModel.Finished())
                mStep++;
            return;
        }
    }
}

// @symbol _ZN10daObjKey_c9StateDropEv
void daObjKey_c::StateDrop()
{
    dCamera_c *cam = (dCamera_c *)data_0209f318;
    Vector3 v;

    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);

    switch (mStep) {
    case 0:
        {
            /* Pointer copies: indexing cam directly folds the offsets into the loads. */
            Vector3 *lookAt = &cam->lookAt;
            Vector3 *pos = &cam->pos;
            mFlags |= FLAG_CAM_TAKEOVER;
            data_0209b454 |= FLAG_CAM_TAKEOVER;
            mSavedCamLookAt.x = lookAt->x;
            mSavedCamLookAt.y = lookAt->y;
            mSavedCamLookAt.z = lookAt->z;
            mSavedCamPos.x = pos->x;
            mSavedCamPos.y = pos->y;
            mSavedCamPos.z = pos->z;
            cam->SetFlag_3();
            v.x = mPosX;
            v.y = mPosY;
            v.z = mPosZ;
            v.y = v.y + Y_LOOK;
            cam->SetLookAt(v);
            mStep++;
            return;
        }

    case 1:
        /* Offset after the copy: mPosY + Y_LOOK in the copy reorders the loads. */
        v.x = mPosX;
        v.y = mPosY;
        v.z = mPosZ;
        v.y = v.y + Y_LOOK;
        cam->SetLookAt(v);
        v.x = mPosX;
        v.y = mPosY;
        v.z = mPosZ;
        v.y = v.y + Y_SPARK;
        mParticleID[0] = (u32)Particle::System::New(mParticleID[0], FX_FALL, v.x, v.y, v.z, 0, 0);
        if (mWithMeshClsn.JustHitGround()) {
            mVertSpeed = -mVertSpeed >> 1;
            func_02012694(SND_LAND, &mCamSpacePos);
            return;
        }
        if (mWithMeshClsn.IsOnGround() == 0)
            return;
        cam->SetLookAt(mSavedCamLookAt);
        cam->SetPos(mSavedCamPos);
        cam->mFlags &= ~8;
        mFlags &= ~FLAG_CAM_TAKEOVER;
        data_0209b454 &= ~FLAG_CAM_TAKEOVER;
        mStep++;
        mdCcAcPos_c.flags &= ~CC_DISABLED;
        mParticleID[0] = 0;
        return;

    case 2:
        {
            u32 id = mdCcAcPos_c.otherOwner;
            dActor_c *found;
            if (id == 0)
                return;
            found = dActor_c::FindWithID(id);
            if (found == 0)
                return;
            if ((mdCcAcPos_c.hitFlags & HIT_PLAYER) == 0)
                return;
            func_ov089_02131df4((Player &)*found);
            mStep++;
            return;
        }
    }
}

// @symbol _ZN10daObjKey_c19func_ov089_02131dccER6Player
/* Last-star path. Hands the collector to ov002 and removes the star. */
void daObjKey_c::func_ov089_02131dcc(Player &player)
{
    func_ov002_020c3dbc(&player);
    Event::SetBit(EVENT_KEY);
    MarkForDestruction();
}

// @symbol _ZN10daObjKey_c19func_ov089_02131df4ER6Player
void daObjKey_c::func_ov089_02131df4(Player &p)
{
    Player *player = &p;

    /* Word 1, bit (2 << kind): already collected. 1 asks dScStage_c/dMeter/Player
     * for the new-star fanfare; 0 suppresses it. Kind 7 takes neither arm.
     * One arm is shorter (0x110 -> 0xe8); the ROM has both. */
    if (data_0209caa0[1] & (2 << mState))
        data_0209f2ac = 0;
    else
        data_0209f2ac = 1;
    data_0209caa0[1] |= (2 << mState);

    if (mState <= 1) {
        player->SetNoControlState(3, -1, 0);
        Sound::LoadAndSetMusic_Layer3(MUSIC_KEY);
    } else if (mState != KEY_KIND_STAR) {
        player->SetNoControlState(3, -1, 0);
        Sound::LoadAndSetMusic_Layer3(MUSIC_KEY);
    }

    func_ov089_0213115c(ANIM_CARRY);
    mPlayer = player;
    {
        /* Through a pointer: mPlayer->mPosX folds the offset into each load. */
        s32 *pos = &mPlayer->mPosX;
        mPosX = pos[0];
        mPosY = pos[1];
        mPosZ = pos[2];
        mAngleY = mPlayer->mAngleY;
        mFlags &= ~FLAG_HIDE;
        Event::SetBit(EVENT_KEY);
    }
}

// @symbol _ZN10daObjKey_c13OnTurnIntoEggER6Player
void daObjKey_c::OnTurnIntoEgg(Player &player)
{
    /* The flag keeps the ROM's moveq and movne pair. */
    unsigned isKey = (actorID == ACTOR_OBJ_KEY);
    if (isKey)
        return func_ov089_02131df4(player);
    return func_ov089_02131dcc(player);
}

// @symbol _ZN10daObjKey_c13OnYoshiTryEatEv
s32 daObjKey_c::OnYoshiTryEat() {
    return 4;
}

// @symbol _ZN10daObjKey_c20UpdateModelTransformEv
void daObjKey_c::UpdateModelTransform()
{
    Matrix4x3_FromRotationY(&mAnmModel.mat4x3, mAngleY);
    mAnmModel.mat4x3.m[9] = mPosX >> 3;
    mAnmModel.mat4x3.m[10] = mPosY >> 3;
    mAnmModel.mat4x3.m[11] = mPosZ >> 3;
    if (data_ov089_021328b4[mState] != 0 && mAnimID == 0) {
        Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
        mModel.mat4x3.m[9] = mPosX >> 3;
        mModel.mat4x3.m[10] = (mPosY + Y_LOOK) >> 3;
        mModel.mat4x3.m[11] = mPosZ >> 3;
    }
    mShadowMatrix = IDENTITY_MATRIX4X3;
    mShadowMatrix.m[9] = mPosX >> 3;
    mShadowMatrix.m[10] = mPosY >> 3;
    mShadowMatrix.m[11] = mPosZ >> 3;
    /* Shadow only while the idle anim (file 0x801b) is showing. */
    if (mAnmModel.file == (BCA_File *)data_ov002_02110964.filePtr)
        DropShadowRadHeight(mShadowModel, mShadowMatrix, SHADOW_R, SHADOW_DEPTH, SHADOW_OPACITY);
}

// @symbol _ZN10daObjKey_c16CleanupResourcesEv
int daObjKey_c::CleanupResources()
{
    UnloadKeyModels(mState);
    ((SharedFilePtr *)&data_ov002_02110964)->Release();
    if (mState != KEY_KIND_STAR) {
        ((SharedFilePtr *)&data_ov089_02132c60)->Release();
        ((SharedFilePtr *)&data_ov089_02132c40)->Release();
        ((SharedFilePtr *)&data_ov089_02132c70)->Release();
        ((SharedFilePtr *)&data_ov089_02132c48)->Release();
    }
    Event::ClearBit(EVENT_KEY);
    return 1;
}

// @symbol _ZN10daObjKey_c6RenderEv
int daObjKey_c::Render()
{
    int hidden = (mFlags & FLAG_HIDE) != 0;
    if (hidden)
        return 1;
    if (mAnimID != 0) {
        mAnmModel.Render(0);
    } else {
        mAnmModel.Render(&mScale);
        /* mAnimID == 0 is already the else; the second test is in the ROM. */
        if (data_ov089_021328b4[mState] != 0 && mAnimID == 0)
            mModel.Render(0);
    }
    return 1;
}

// @symbol _ZN10daObjKey_c8BehaviorEv
int daObjKey_c::Behavior()
{
    Vector3 vec;
    Vector3 offset7;
    Vector3 offset;
    int animID = mAnimID;

    if (animID != 0) {
        if (animID == ANIM_CARRY) {
            Player *player = mPlayer;
            if (player != 0) {
                /* Through a pointer: player->mPosX folds the offset into each load. */
                s32 *pos = &player->mPosX;
                mPosX = pos[0];
                mPosY = pos[1];
                mPosZ = pos[2];
                mAngleY = mPlayer->mAngleY;
            }
            if (mAnmModel.Finished() == 0) {
                Matrix4x3_FromTranslation(&data_020a0e68, mPosX, mPosY, mPosZ);
                Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
                MulMat4x3Mat4x3(mAnmModel.data.transforms, &data_020a0e68, &data_020a0e68);
                vec.x = data_020a0e68.m[9];
                vec.y = data_020a0e68.m[10];
                vec.z = data_020a0e68.m[11];
                SubVec3(&vec, &mPosX, &vec);
                Vec3_LslInPlace(&vec, 3);
                AddVec3(&vec, &mPosX, &vec);
                /* First bone, word at +0xc. BMD_Bone has no field there. */
                vec.y = *(int *)((char *)mAnmModel.data.bones + 0xc) * BONE_Y_MUL + vec.y;
                vec.y = vec.y - Y_BONE;
                mParticleID[0] = (u32)Particle::System::New(mParticleID[0], FX_BURST_A, vec.x,
                                                            vec.y, vec.z, 0, 0);
                mParticleID[1] = (u32)Particle::System::New(mParticleID[1], FX_BURST_B, vec.x,
                                                            vec.y, vec.z, 0, 0);
            }
        }

        mAnmModel.Advance();
        UpdateModelTransform();
        if (mAnmModel.Finished()) {
            /* The flag keeps the ROM's moveq and movne pair. */
            int isKey = (actorID == ACTOR_OBJ_KEY);
            if (isKey != 0) {
                /* Anim 3's file (data_ov089_02132c40, id 0x44f) stays; the others die. */
                if (mAnmModel.file != (BCA_File *)data_ov089_02132c40.filePtr)
                    MarkForDestruction();
            }
        }
        return 1;
    }

    if (UpdateYoshiEat(mWithMeshClsn)) {
        UpdateModelTransform();
        mdCcAcPos_c.Clear();
        return 1;
    }
    mEatingPlayer = 0;
    if (mSpinSpeed > SPIN_FAST)
        mSpinSpeed -= SPIN_STEP;
    else if (mSpinSpeed == 0)
        mSpinSpeed = SPIN_FAST;
    mAngleY += mSpinSpeed;
    UpdatePos(0);
    (this->*data_ov089_02132cec.slots[mState])();
    UpdateModelTransform();
    mdCcAcPos_c.Clear();
    if (mState == KEY_KIND_STAR) {
        offset7.x = data_ov089_02132b40.x;
        offset7.y = data_ov089_02132b40.y;
        offset7.z = data_ov089_02132b40.z;
        mdCcAcPos_c.SetPosRelativeToActor(offset7);
    } else {
        offset.x = data_ov089_02132ca4.x;
        offset.y = data_ov089_02132ca4.y;
        offset.z = data_ov089_02132ca4.z;
        mdCcAcPos_c.SetPosRelativeToActor(offset);
    }
    mdCcAcPos_c.Update();
    return 1;
}

// @symbol _ZN10daObjKey_c13InitResourcesEv
int daObjKey_c::InitResources()
{
    Vector3 offset7;
    Vector3 offset;
    int kind = param1 & 7;
    mState = kind;
    LoadKeyModels(mState);
    dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov002_02110964);

    if (mState == KEY_KIND_STAR) {
        if (mAnmModel.SetFile((BMD_File *)((ObjKeyFile *)&data_ov002_0211094c)->filePtr, 1, 1) == 0)
            return 0;
        mAnmModel.SetAnim((BCA_File *)data_ov002_02110964.filePtr, ANIM_FLAGS, ANIM_SPEED, 0);
        mScaleX = SCALE_STAR;
        mScaleY = SCALE_STAR;
        mScaleZ = SCALE_STAR;
        offset7.x = data_ov089_02132b40.x;
        offset7.y = data_ov089_02132b40.y;
        offset7.z = data_ov089_02132b40.z;
        mdCcAcPos_c.Init(this, offset7, CC_R, STAR_CC_H, CC_FLAGS, CC_VULN);
        mWithMeshClsn.Init(this, STAR_CLSN_R, 0, 0, 0);
        mSpinSpeed = SPIN_FAST;
        mVertAccel = 0;
        Sound::PlayBank3(SND_APPEAR, mCamSpacePos);
    } else {
        dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov089_02132c60);
        dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov089_02132c40);
        dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov089_02132c70);
        dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov089_02132c48);
        if (mAnmModel.SetFile((BMD_File *)((ObjKeyFile *)data_ov089_02132894[mState])->filePtr, 1,
                              1) == 0)
            return 0;
        {
            void *extra = data_ov089_021328b4[mState];
            if (extra != 0) {
                if (mModel.SetFile((BMD_File *)((ObjKeyFile *)extra)->filePtr, 1, -1) == 0)
                    return 0;
            }
        }
        mAnmModel.SetAnim((BCA_File *)data_ov002_02110964.filePtr, ANIM_FLAGS, ANIM_SPEED, 0);
        mScaleX = SCALE_KEY;
        mScaleY = SCALE_KEY;
        mScaleZ = SCALE_KEY;
        mVertSpeed = POP_VY;
        offset.x = data_ov089_02132ca4.x;
        offset.y = data_ov089_02132ca4.y;
        offset.z = data_ov089_02132ca4.z;
        mdCcAcPos_c.Init(this, offset, CC_R, KEY_CC_H, CC_FLAGS, CC_VULN);
        mWithMeshClsn.Init(this, KEY_CLSN_R, 0, 0, 0);
        mVertAccel = GRAVITY;
        mSpinSpeed = 0;
    }

    if (mShadowModel.InitCylinder() == 0)
        return 0;
    mWithMeshClsn.SetLimMovFlag();
    mHomePos.x = mPosX;
    mHomePos.y = mPosY;
    mHomePos.z = mPosZ;
    mStateTimer = 0;
    mStep = 0;
    mBounceCount = 0;
    mTerminalVelocity = TERMINAL_VY;
    mAnimID = 0;
    mPlayer = 0;
    mParticleID[0] = mParticleID[1] = mParticleID[2] = 0;
    if (data_0209cef0 == 0)
        Event::ClearBit(EVENT_KEY);
    return 1;
}

/* Both profiles build the same class. fBase_c's inline operator new forwards to
 * _ZN7fBase_cnwEm; the implicit constructor inlines dEnemyBase_c's base step,
 * the vptr store and the five member constructors. */
// @symbol daObjKey_c_classInit_LAST_STAR
extern "C" daObjKey_c *daObjKey_c_classInit_LAST_STAR()
{
    return new daObjKey_c();
}

// @symbol daObjKey_c_classInit_OBJ_KEY
extern "C" daObjKey_c *daObjKey_c_classInit_OBJ_KEY()
{
    return new daObjKey_c();
}

/* Static-init globals (was the handwritten __sinit_ov089_021328d4 shard).
 * Definition order is the retail initializer's construction order: the four
 * model handles, then the four animation handles, then the state table the
 * descriptors are copied into. The offset vector between the nodes carries
 * no initializer and emits plain BSS. */
ObjKeyModelFile data_ov089_02132c50(ASSET_KOOPA_KEY_BMD);
ObjKeyModelFile gLuigiKeyModelFile(ASSET_LUIGI_KEY_BMD);
ObjKeyModelFile gMarioKeyModelFile(ASSET_MARIO_KEY_BMD);
ObjKeyModelFile gWarioKeyModelFile(ASSET_WARIO_KEY_BMD);
ObjKeyAnimationFilePtr data_ov089_02132c60(ASSET_KOOPA_KEY_GET_BCA);
ObjKeyAnimationFilePtr data_ov089_02132c40(ASSET_KOOPA_KEY_GET_DEMO_BCA);
ObjKeyAnimationFilePtr data_ov089_02132c70(ASSET_KOOPA_KEY_OPEN_BCA);
ObjKeyAnimationFilePtr data_ov089_02132c48(ASSET_KOOPA_KEY_W_GET_BCA);

ObjKeyVec3 data_ov089_02132ca4;

ObjKeyStateTable data_ov089_02132cec;
