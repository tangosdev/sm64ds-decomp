//cpp
/* daGrock_c -- Hazy Maze Cave rolling rock (GORO_ROCK 221), ov021.
 *
 * param1's low nibble is the kind. 0 and 1 are invisible spawners: a rock
 * is born only while the player is farther than 1500.0, faster inside
 * 6000.0 than beyond it. Kind 1's fifth rock is the star rock. 2 rolls,
 * 3 rolls with a star, 4 is placed still and uses a wider cylinder. The
 * next nibble is the star index. Spawned rocks are actor 0xdd, this class.
 * 0xbf is the player.
 *
 * The travel heading lives in mPrevAngleY. Lateral velocity is unk_0a4 /
 * unk_0ac around mVertSpeed; the base header still spells those unk_.
 * The rolling Sound::PlayLong handle is the s32 at 0x3b8 (pad_3b8) and the
 * rolling Particle::System::New handle is the s32 at 0x3c4 (pad_3c3).
 * unk_3b4 is the other rock's uniqueID (spawner on the child, and the
 * child clears the spawner's unk_3c2 when it falls). unk_3c0 is the signed
 * star slot. unk_3c1 is the star index, 0xff when there is none. unk_3c2
 * is set once the star rock has been spawned.
 *
 * #pragma defer_codegen off stays above the includes. The out-of-line
 * destructor is the key function, so this TU emits _ZTV/_ZTI/_ZTS, and the
 * pragma lays .text down D1, D0, then a D2 the cartridge does not keep.
 * daGrock_c_classInit stays in src/d_a_grock.c. g_profile_GORO_ROCK is not
 * in this TU.
 *
 * deslop leftovers:
 * - func_ov021_02112294: one UntrackAndSpawnStar with a chosen mode
 *   size-DIFFs (999 words). The two call sites, mode 2 and mode 4, match.
 * - func_ov021_02112544: GetSubtraction(short, short) size-DIFFs (999
 *   words). The wall angle is an int; _ZN8dActor_c14GetSubtractionEss
 *   passes it through. ReflectAngle(Fix12<int>, Fix12<int>, s16)
 *   size-DIFFs the same function. cstd::atan2(Fix12<int>, Fix12<int>)
 *   size-DIFFs it and mis-aims IsOnWall. The scalar externs match.
 * - func_ov021_021122fc: DropShadowRadHeight is void in dActor_c.h and
 *   takes Fix12<int> by value. This function returns the scalar call;
 *   the method form does not compile.
 * - InitResources: dCcAcPos_c::Init with Fix12<int> by value size-DIFFs
 *   (999 words). The scalar extern matches. dBgCh_Actr::Init is called by
 *   its ROM symbol: the header's Fix12i (= s32) mangles the radii as `i`
 *   and names a symbol no object defines, so the member form linked to
 *   nothing.
 * - Particle::System::New and NewSimple are not declared on
 *   Particle::System (include/Particle__System.h). Player::Hurt is not
 *   declared on Player. Those 5Fix12IiE externs stay.
 */

#pragma defer_codegen off

#include "decl_common.h"
#include "daGrock_c.h"
#include "Player.h"
#include "Sound.h"
#include "dBgPi.h"

/* Kind in param1's low nibble. */
enum {
    kGrockSpawner = 0,
    kGrockStarSpawner = 1,
    kGrockRolling = 2,
    kGrockStar = 3,
    kGrockStill = 4
};

/* SharedFilePtr.h has the methods and no fields. InitResources reads the
 * loaded BMD at +4, so this TU completes the forward declaration. */
struct SharedFilePtr {
    void *file;
    void *bmd;
    void Release();
};

/* 02112544 copies three words as a value. The empty constructor and
 * destructor are why that copy matches; a POD Vec3 changes the function.
 * InitResources keeps its own POD Vec3 inside the function. */
struct Vec3 {
    int x, y, z;
    Vec3() {}
    ~Vec3() {}
};

/* The copy at mPrevAngleX has to go through a struct whose only member is
 * an array. Vector3_16's s16 members scalarise to LDRSH and interleave the
 * store; an unsigned array keeps the ROM's LDRH load-load-store. */
struct AngleWords { u16 w[3]; };

/* 0x3b8 / 0x3c4 are inside the class pads. Naming them on daGrock_c is a
 * header change; these are the words this TU actually stores. */
#define grock_roll_sound(rock) (*(s32 *)&(rock)->pad_3b8[0])
#define grock_dust(rock)       (*(s32 *)&(rock)->pad_3c3[1])
#define grock_star_slot(rock)  (*(s8 *)&(rock)->unk_3c0)

extern "C" {
/* dBgCh_Actr::Init's header takes Fix12i (= s32), so the method form
   mangles the two radii as `i` and names
   _ZN10dBgCh_Actr4InitEP8dActor_ciiP10Vector3_16S3_, which no object
   defines. The ROM's is ..._5Fix12IiES3_P10Vector3_16S5_. */
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    dBgCh_Actr *self, int actor, Fix12i radius, Fix12i height, int a, int b);
extern void Vec3_Asr(void *dst, void *src, int n);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, short a, short b, short c);
extern int _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *thiz, void *sm, void *m, int rad, int h, unsigned u);
extern struct Matrix4x3 data_020a0e68;
/* Same body as Sound::PlayBank3 (0x02012664). The ROM calls these copies. */
extern void func_02012694(int id, void *pos);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, int x, int y, int z);
extern void dBgCh_Actr_UpdateContinuous_Veneer(void *clsn);
extern void *_ZNK10dBgCh_Actr14GetFloorResultEv(void *clsn);
extern void func_0201267c(int id, void *pos);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int prev, unsigned int effect, int x, int y, int z, void *dir, void *cb);
extern int Vec3_HorzLen(void *v);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern void *_ZNK10dBgCh_Actr13GetWallResultEv(void *clsn);
/* dActor_c.h types GetSubtraction(short, short). The wall angle is an int
 * passed straight through; the short spelling adds lsl/asr #16. */
extern int _ZN8dActor_c14GetSubtractionEss(void *a, s16 x, int y);
extern s16 _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void *a, int x, int y, s16 ang);
extern s16 data_02082214[];
extern void AddVec3(struct Vector3 *dst, struct Vector3 *a, struct Vector3 *b);
/* Clears the cylinder flags bit 2, then sets it when the closest player
 * is vanish Luigi. MakeVanishLuigiWork is the other copy. */
extern void func_0200f760(void *self, void *cyl);
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    void *clsn, void *actor, void *offset, int radius, int height, u32 flags, u32 vuln);
extern SharedFilePtr data_ov021_02114a50;
extern int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
    void *player, struct Vector3 *pos, u32 kind, int knockback, u32 a, u32 b, u32 c);
}

/* D1 stores this vtable, destroys the members in reverse declaration order,
 * then dEnemyBase_c::~dEnemyBase_c. D0 adds dEnemyBase_c's operator delete. */
// @symbol _ZN9daGrock_cD1Ev
// @symbol _ZN9daGrock_cD0Ev
daGrock_c::~daGrock_c()
{
}

// @symbol _ZN9daGrock_c16OnAimedAtWithEggEv
s32 daGrock_c::OnAimedAtWithEgg()
{
    return 0;
}

/* Retire the marker and spawn the star. Still rocks pass spawn mode 2;
 * rolling rocks pass 4. No star (index 0xff) does nothing. */
// @symbol func_ov021_02112294
extern "C" void func_ov021_02112294(void *raw)
{
    daGrock_c *rock = (daGrock_c *)raw;

    if (rock->unk_3c1 == 0xff)
        return;
    if (rock->mType == kGrockStill) {
        rock->UntrackAndSpawnStar(grock_star_slot(rock), rock->unk_3c1,
                                  *(Vector3 *)&rock->mPosX, 2);
    } else {
        rock->UntrackAndSpawnStar(grock_star_slot(rock), rock->unk_3c1,
                                  *(Vector3 *)&rock->mPosX, 4);
    }
}

/* Shift the position down 3, build the scratch matrix, keep the unrotated
 * copy in pad_188 for the shadow, then rotate it into the model. */
// @symbol func_ov021_021122fc
extern "C" int func_ov021_021122fc(char *raw)
{
    daGrock_c *rock = (daGrock_c *)raw;
    int shifted[3];

    Vec3_Asr(shifted, &rock->mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, shifted[0], shifted[1], shifted[2]);
    *(Matrix4x3 *)rock->pad_188 = data_020a0e68;
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
                                            rock->mAngleX, rock->mAngleY, rock->mAngleZ);
    rock->mModel.mat4x3 = data_020a0e68;
    return _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        rock, &rock->mShadowModel, rock->pad_188, 0x1f4000, 0x1f4000, 0xf);
}

/* Cylinder hit. Mega always breaks the rock. Punch/kick/breakdance/slide
 * (0x3c0) breaks it only for character 2 (Wario; SaveData's unlock bit 2)
 * and otherwise just plays the bonk. A still rock never hurts the player. */
// @symbol func_ov021_021123b0
extern "C" void func_ov021_021123b0(char *raw)
{
    daGrock_c *rock = (daGrock_c *)raw;
    dActor_c *other;
    u32 hit;
    int isPlayer;

    if (rock->mdCcAcPos_c.otherOwner == 0)
        return;
    other = dActor_c::FindWithID(rock->mdCcAcPos_c.otherOwner);
    if (other == 0)
        return;

    hit = rock->mdCcAcPos_c.hitFlags;
    if ((hit & 0x10) != 0) {
        func_02012694(0x17a, &rock->mCamSpacePosX);
        ((Player *)other)->IncMegaKillCount();
        rock->TriplePoofDust();
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
            0x67, rock->mPosX, rock->mPosY, rock->mPosZ);
        func_ov021_02112294(rock);
        rock->MarkForDestruction();
        return;
    }

    isPlayer = (int)(other->actorID == 0xbf);
    if (isPlayer == 0)
        return;
    if (((Player *)other)->mIsVanish != 0)
        return;

    if ((hit & 0x3c0) != 0) {
        if (other->param1 == 2) {
            func_02012694(0x17a, &rock->mCamSpacePosX);
            rock->MarkForDestruction();
            rock->TriplePoofDust();
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
                0x67, rock->mPosX, rock->mPosY, rock->mPosZ);
            func_ov021_02112294(rock);
            return;
        }
        Sound::PlayBank0(0xb5, *(Vector3 *)&rock->mCamSpacePosX);
        return;
    }

    if (rock->mType == kGrockStill)
        return;
    {
        Vector3 pos;
        pos.x = rock->mPosX;
        pos.y = rock->mPosY;
        pos.z = rock->mPosZ;
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(other, &pos, 3, 0xc000, 1, 0, 1);
    }
}

/* Floor: bounce, dust, and steer the lateral velocity along the normal,
 * then cap it. Wall: reflect the heading when the approach is steep. */
// @symbol func_ov021_02112544
extern "C" void func_ov021_02112544(char *raw)
{
    daGrock_c *rock = (daGrock_c *)raw;
    Vec3 normal;
    Vec3 pos;
    Vec3 wallNormal;
    dBgPi *hit;

    dBgCh_Actr_UpdateContinuous_Veneer(&rock->mWithMeshClsn);
    if (rock->mWithMeshClsn.IsOnGround() != 0) {
        hit = (dBgPi *)_ZNK10dBgCh_Actr14GetFloorResultEv(&rock->mWithMeshClsn);
        hit->surface.CopyNormalTo(*(Vector3 *)&normal);

        pos = *(Vec3 *)&rock->mPosX;
        pos.x -= normal.x * 0x12c;
        pos.y -= normal.y * 0x12c;
        pos.z -= normal.z * 0x12c;

        if (rock->mWithMeshClsn.JustHitGround() != 0) {
            if (rock->mVertSpeed < (rock->mVertAccel << 1) && normal.y >= 0xc00)
                rock->mVertSpeed = -rock->mVertSpeed * 6 / 10;
            pos.y += 0x3c000;
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x68, pos.x, pos.y, pos.z);
            func_0201267c(0x48, &rock->mCamSpacePosX);
        } else {
            grock_dust(rock) = (int)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                grock_dust(rock), 0x69, pos.x, pos.y, pos.z, 0, 0);
            grock_roll_sound(rock) = Sound::PlayLong(
                grock_roll_sound(rock), 3, 0x8a, *(Vector3 *)&rock->mCamSpacePosX, 0);
        }
        {
            int *vx = (int *)(int)&rock->unk_0a4;
            int *vz = (int *)(int)&rock->unk_0ac;
            *vx = *vx + normal.x * 5;
            *vz = *vz + normal.z * 5;
        }
        rock->mHorzSpeed = Vec3_HorzLen(&rock->unk_0a4);
        rock->mPrevAngleY = (s16)_ZN4cstd5atan2E5Fix12IiES1_(rock->unk_0a4, rock->unk_0ac);
        if (rock->mHorzSpeed >= 0x22000) {
            rock->mHorzSpeed = 0x22000;
            rock->unk_0a4 = data_02082214[((*(u16 *)&rock->mPrevAngleY >> 4) << 1)] * 0x22;
            rock->unk_0ac = data_02082214[((*(u16 *)&rock->mPrevAngleY >> 4) << 1) + 1] * 0x22;
        }
    }

    if (rock->mWithMeshClsn.IsOnWall() == 0)
        return;

    hit = (dBgPi *)_ZNK10dBgCh_Actr13GetWallResultEv(&rock->mWithMeshClsn);
    hit->surface.CopyNormalTo(*(Vector3 *)&wallNormal);
    {
        int wallAngle = _ZN4cstd5atan2E5Fix12IiES1_(wallNormal.x, wallNormal.z);
        if (_ZN8dActor_c14GetSubtractionEss(rock, rock->mPrevAngleY, wallAngle) <= 0x1000)
            return;
        rock->mPrevAngleY = _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(
            rock, wallNormal.x, wallNormal.z, rock->mPrevAngleY);
        rock->UpdatePosWithHorzSpeedAndAng();
    }
}

/* Gravity, clamped at the terminal velocity, then step by the velocity. */
// @symbol func_ov021_021127b4
extern "C" void func_ov021_021127b4(char *raw)
{
    daGrock_c *rock = (daGrock_c *)raw;
    int sum = rock->mVertSpeed + rock->mVertAccel;
    int clamped = rock->mTerminalVelocity;

    if (sum >= clamped)
        clamped = sum;
    rock->mVertSpeed = clamped;
    AddVec3((Vector3 *)&rock->mPosX, (Vector3 *)&rock->unk_0a4, (Vector3 *)&rock->mPosX);
}

// @symbol _ZN9daGrock_c16CleanupResourcesEv
int daGrock_c::CleanupResources()
{
    data_ov021_02114a50.Release();
    return 1;
}

// @symbol _ZN9daGrock_c6RenderEv
int daGrock_c::Render()
{
    if (mType >= kGrockRolling)
        mModel.Render(0);
    return 1;
}

// @symbol _ZN9daGrock_c8BehaviorEv
int daGrock_c::Behavior()
{
    if (mType >= kGrockRolling) {
        func_0200f760(this, &mdCcAcPos_c);
        func_ov021_021123b0((char *)this);
        if (mDeathState == 8)
            return 1;

        if (mType != kGrockStill) {
            s16 *roll;

            func_ov021_021127b4((char *)this);
            roll = (s16 *)(int)&mAngleX;
            *roll = (s16)(*roll + (mHorzSpeed >> 12) * 0x43);
            mAngleY = mPrevAngleY;
            func_ov021_02112544((char *)this);
        }
        func_ov021_021122fc((char *)this);
        mdCcAcPos_c.Clear();

        if (mPosZ >= (int)0xfe82c000) {
            Vector3 offset;
            offset.x = 0;
            offset.y = (int)0xffebb000;
            offset.z = 0;
            mdCcAcPos_c.SetPosRelativeToActor(offset);
        } else {
            Vector3 offset;
            offset.x = 0;
            offset.y = -0xe1000;
            offset.z = 0;
            mdCcAcPos_c.SetPosRelativeToActor(offset);
        }
        mdCcAcPos_c.dCc_c::Update();

        if (mType != kGrockStill) {
            if (mPosY < -0x3e8000) {
                daGrock_c *spawner;

                MarkForDestruction();
                if (unk_3b4 != 0) {
                    spawner = (daGrock_c *)dActor_c::FindWithID(unk_3b4);
                    if (spawner)
                        spawner->unk_3c2 = 0;
                }
                UntrackStar(grock_star_slot(this));
                TriplePoofDust();
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x67, mPosX, mPosY, mPosZ);
                func_02012694(0x17a, &mCamSpacePosX);
            }
        }
    } else {
        int dist = DistToCPlayer();
        if (dist > 0x5dc000) {
            u32 threshold = (dist < 0x1770000) ? 0x70 : 0xe0;
            if (*(u16 *)&mStateTimer >= threshold) {
                u8 *spawned = (u8 *)(int)&unk_3bf;
                Vector3_16 rot;
                u32 rnd;

                *spawned = (u8)(*spawned + 1);
                *(u16 *)&mStateTimer = 0;
                *(AngleWords *)&rot = *(AngleWords *)&mPrevAngleX;
                rnd = (u32)RandomIntInternal(&data_0209e650);
                rot.y = rot.y + (rnd >> 16) % 0xc00;
                if (mType == kGrockStarSpawner && unk_3bf >= 5 && unk_3c2 == 0) {
                    daGrock_c *child;

                    unk_3c2 = 1;
                    child = (daGrock_c *)dActor_c::Spawn(
                        0xdd, (unk_3c1 << 8) | kGrockStar, *(Vector3 *)&mPosX,
                        &rot, mAreaId, -1);
                    unk_3bf = 0;
                    if (child)
                        child->unk_3b4 = uniqueID;
                } else {
                    dActor_c::Spawn(0xdd, kGrockRolling, *(Vector3 *)&mPosX,
                                    &rot, mAreaId, -1);
                }
            }
        }
        {
            u16 *timer = (u16 *)(int)&mStateTimer;
            *timer = (u16)(*timer + 1);
        }
    }
    return 1;
}

// @symbol _ZN9daGrock_c13InitResourcesEv
int daGrock_c::InitResources()
{
    struct Vec3 { s32 x, y, z; };

    mType = (u8)(param1 & 0xf);
    unk_3c1 = 0xff;
    grock_star_slot(this) = -1;
    unk_3bf = 0;

    Model::LoadFile(data_ov021_02114a50);

    if (mType >= kGrockRolling) {
        if (mModel.SetFile((BMD_File *)data_ov021_02114a50.bmd, 1, 1) == 0)
            return 0;
        if (mShadowModel.InitCylinder() == 0)
            return 0;

        _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, (int)this, 0x12c000, 0, 0, 0);
        mWithMeshClsn.SetLimMovFlag();
        grock_roll_sound(this) = 0;

        if (mType == kGrockStill) {
            Vec3 offset;
            unk_3c1 = (u8)((param1 >> 8) & 0xf);
            offset.x = 0;
            offset.y = 0;
            offset.z = 0;
            _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
                &mdCcAcPos_c, this, &offset, 0x10e000, 0x226000, 0x200004, 0x3c0);
        } else {
            Vec3 offset;
            offset.x = 0;
            offset.y = 0;
            offset.z = 0;
            _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
                &mdCcAcPos_c, this, &offset, 0xf3000, 0x226000, 0x200004, 0x3c0);

            if (mType == kGrockStar)
                unk_3c1 = (u8)((param1 >> 8) & 0xf);
            mVertAccel = -0x4000;
            mTerminalVelocity = -0x28000;
            mHorzSpeed = 0x1e000;
            unk_0a4 = data_02082214[((*(u16 *)&mPrevAngleY >> 4) << 1)] * 0x1e;
            unk_0ac = data_02082214[((*(u16 *)&mPrevAngleY >> 4) << 1) + 1] * 0x1e;
        }
    } else if (mType == kGrockStarSpawner) {
        unk_3c1 = (u8)((param1 >> 8) & 0xf);
    }

    if (mType != kGrockStarSpawner && unk_3c1 != 0xff)
        unk_3c0 = (u8)TrackStar(unk_3c1, 2);
    grock_dust(this) = 0;
    mDeathState = 0;
    unk_3b4 = 0;
    unk_3c2 = 0;
    return 1;
}
