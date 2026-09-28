//cpp
/* daSnowman_c — Mr. Blizzard (SNOWMAN, registry id 0xdf).
 *
 * param1's low byte is the path, the next byte is the kind. Kind 0 walks
 * that path and throws a snowball (SNOWBALL, 0xe0). Any other kind starts
 * buried and rises when a player comes near. Kind 2 wears Mario's cap
 * (OBJ_MARIO_CAP, 0x10d). When that one is gone it plants a kind-3 watcher
 * here (param 0x300) which turns into kind 2 the next time the player loses
 * the cap. Kind 3 does not render.
 *
 * The ten state handlers stay address-named: each one is a pointer-to-member
 * in a daSnowman_c::State, and the ROM symbol is the address.
 *
 * deslop leftovers:
 * - func_ov081_0212423c: naming player mPosX/Y/Z is 0x190 -> 0x18c.
 * - func_ov081_021245e8: the same loads are 0xb8 -> 0xb4.
 * - func_ov081_02124b98: the same loads differ by 3 words; size stays 0x17c.
 * - func_ov081_021243cc: `if (actorID == PLAYER)` is 0x21c -> 0x210.
 *   KillByInvincibleChar as the method is 0x21c -> 0x228.
 * - func_ov081_021246a0: SpawnCoins as the method is 0xfc -> 0x104.
 * - func_ov081_02124894: SpawnCoins as the method is 0xf8 -> 0x100.
 * - ModelAnim::SetAnim as the method is +8. func_ov081_02124b08
 *   0x90 -> 0x98, 02124d14 0x3c -> 0x44, 02124ec0 0x60 -> 0x68,
 *   02125068 0x60 -> 0x68, 021250c8 0x138 -> 0x140, 02125208
 *   0x184 -> 0x18c, 0212538c 0xfc -> 0x104.
 * - func_ov081_021254d8: DropShadowRadHeight as the method is 0x2d8 -> 0x2e8.
 * - InitResources: dCcAcPos_c::Init as the method, measured together
 *   with dBgCh_Actr::Init, is 0x318 -> 0x328. dBgCh_Actr::Init alone
 *   stays 0x318 but mangles ciiP10Vector3_16S3_, not the ROM
 *   5Fix12IiES3_P10Vector3_16S5_.
 * - Behavior: cap->SetRanges does not compile. dActor_c has no SetRanges.
 */

#pragma defer_codegen off

#include "common.h"
#include "daSnowman_c.h"
#include "SharedFilePtr.h"
#include "PathPtr.h"
#include "SaveData.h"
#include "Player.h"

int ApproachLinear(int &ref, int target, int step);

namespace cstd {
int fdiv(int numerator, int denominator);
}

/* Registry ids this file spawns or compares. */
enum {
    kPlayerId = 0xbf,     /* PLAYER */
    kSnowmanId = 0xdf,    /* SNOWMAN */
    kSnowballId = 0xe0,   /* SNOWBALL */
    kMarioCapId = 0x10d   /* OBJ_MARIO_CAP */
};

/* param1 values whose high byte is the kind. */
enum {
    kSpawnCapWearer = 0x200, /* kind 2 */
    kSpawnWatcher = 0x300    /* kind 3 */
};

extern "C" {
extern int RandomIntInternal(int *seed);
/* Sound::Play2D(2, id). This TU calls the wrapper, not Play2D. */
extern void func_02012790(int);
extern int data_0209e650;
extern s16 data_ov081_021289a4;
extern s16 data_ov081_021289a6;
extern s16 data_ov081_021289a8;
extern s16 Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
extern void ApproachAngle(void *p, int target, int a, int b, int c);
extern int AngleDiff(int a, int b);
extern void func_ov002_020aea30(void *self, void *actor, void *collision);
/* Sound::Play(3, id, pos) — PlayBank3. This TU calls the wrapper. */
extern void func_02012694(int a, void *p);
extern void _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(
    void *self, void *v, void *a);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
    void *p, void *v, u32 a, int f, u32 c, u32 d, u32 e);
extern Vector3 data_ov081_02128998; /* cylinder offset (0, 0x1e000, 0) */
extern unsigned char data_0209f21c[];
extern void *data_0209f394[];
extern int _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(
    void *a, Vector3 *v, unsigned n, Fix12i f, short s);
extern unsigned _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned a, unsigned b, int c, int d, int e, const void *f, void *g);
extern void Vec3_Sub(struct Vector3 *out, struct Vector3 *a, struct Vector3 *b);
extern int LenVec3(struct Vector3 *v);
extern Fix12i Vec3_Dist(const struct Vector3 *a, const struct Vector3 *b);
extern void Vec3_MulScalar(struct Vector3 *out, struct Vector3 *in, int s);
extern void SubVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern void Vec3_Asr(int *d, int *s, int sh);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
extern void MulMat4x3Mat4x3(void *dst, void *a, void *b);
extern void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, void *sm, void *mtx, int a, int b, unsigned int g);
extern struct Matrix4x3 data_020a0e68;
extern void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void *self, int a, int b, int c, int d);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    void *self, void *actor, const void *v, int d, int e, u32 f, u32 g);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *self, void *actor, int b, int c, void *v16, int e);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern void Matrix4x3_FromRotationY(void *m, int ang);
extern int Vec3_HorzLen(void *v);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ang);
extern void MulVec3Mat4x3(void *a, void *m, void *b);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    void *self, void *bca, int a, int fix, unsigned int b);
extern SharedFilePtr data_ov081_02128d90; /* snowball model; this TU only LoadFile's it */
extern SharedFilePtr data_ov081_02128db0; /* body model, SetFile'd in InitResources */
extern SharedFilePtr data_ov081_02128d88; /* hop start */
extern SharedFilePtr data_ov081_02128da0; /* hop land */
extern SharedFilePtr data_ov081_02128d98; /* wait */
extern SharedFilePtr data_ov081_02128da8; /* sink */
extern SharedFilePtr data_ov081_02128db8; /* throw */

void func_ov081_02124134(daSnowman_c *self);
void func_ov081_0212423c(daSnowman_c *self, int idx);
void func_ov081_021243cc(daSnowman_c *self);
int func_ov081_021245e8(daSnowman_c *self);
int func_ov081_021246a0(daSnowman_c *self);
int func_ov081_0212479c(daSnowman_c *self);
int func_ov081_02124894(daSnowman_c *self);
int func_ov081_0212498c(daSnowman_c *self);
int func_ov081_021249f4(daSnowman_c *self);
int func_ov081_02124b08(daSnowman_c *self);
int func_ov081_02124b98(daSnowman_c *self);
int func_ov081_02124d14(daSnowman_c *self);
int func_ov081_02124d50(daSnowman_c *self);
int func_ov081_02124dfc(daSnowman_c *self);
int func_ov081_02124e64(daSnowman_c *self);
int func_ov081_02124ec0(daSnowman_c *self);
int func_ov081_02124f20(daSnowman_c *self);
int func_ov081_02124f7c(daSnowman_c *self);
int func_ov081_02125038(daSnowman_c *self);
int func_ov081_02125068(daSnowman_c *self);
int func_ov081_021250c8(daSnowman_c *self);
int func_ov081_02125200(void);
int func_ov081_02125208(daSnowman_c *self);
int func_ov081_0212538c(daSnowman_c *self);
int func_ov081_02125488(daSnowman_c *self, daSnowman_c::State *state);
void func_ov081_021254d8(daSnowman_c *self);
}

/* The ten states, by the address of their .bss record, with the handlers
   the static initializer pairs into each as (enter, execute). */
extern daSnowman_c::State data_ov081_02128e14;   /* 02124ec0, 02124e64: face the player */
extern daSnowman_c::State data_ov081_02128e24;   /* 0212479c, 021246a0: melt away */
extern daSnowman_c::State data_ov081_02128e34;   /* 02124dfc, 02124d50: make a snowball */
extern daSnowman_c::State data_ov081_02128e44;   /* 02124d14, 02124b98: throw it */
extern daSnowman_c::State data_ov081_02128e54;   /* 0212538c, 02125208: walk the path */
extern daSnowman_c::State data_ov081_02128e64;   /* 02124b08, 021249f4: sink */
extern daSnowman_c::State data_ov081_02128e74;   /* 02125200, 021250c8: hop to a node */
extern daSnowman_c::State data_ov081_02128e84;   /* 02125068, 02125038: wait buried */
extern daSnowman_c::State data_ov081_02128e94;   /* 0212498c, 02124894: spin and fall */
extern daSnowman_c::State data_ov081_02128ea4;   /* 02124f7c, 02124f20: rise */

/* The loaded file behind a SharedFilePtr is its second word. */
#define LOADED_FILE(ptr) ((void *)((int *)&(ptr))[1])

// @symbol _ZN11daSnowman_cD1Ev
// @symbol _ZN11daSnowman_cD0Ev
daSnowman_c::~daSnowman_c()
{
}

// @symbol func_ov081_02124134
/* Drops the snowball and flings the cap. Kind 2 also plants a kind-3
   watcher at home. */
extern "C" void func_ov081_02124134(daSnowman_c *self)
{
    dActor_c *a;
    unsigned int id = self->mSnowballID;
    if (id) {
        a = dActor_c::FindWithID(id);
        if (a) {
            a->mVertAccel = -0x2000;
            a->mTerminalVelocity = -0x28000;
            self->mSnowballID = 0;
        }
    }
    id = self->mCapUniqueID;
    if (id) {
        a = dActor_c::FindWithID(id);
        if (a) {
            a->mVertAccel = -0x2000;
            unsigned int rv = ((unsigned int)RandomIntInternal(&data_0209e650) >> 8) & 0xf;
            a->mPrevAngleX = 0;
            a->mPrevAngleY = rv << 0xc;
            a->mPrevAngleZ = 0;
            a->unk_0a4 = 0;
            a->mVertSpeed = 0x14000;
            a->unk_0ac = 0;
            a->mHorzSpeed = 0xa000;
            *(int *)((char *)a + 0xc8) = 0;   /* the carry matrix; dActor_c pad */
        }
        func_02012790(0xa);
        self->mCapUniqueID = 0;
    }
    if (self->mType == 2) {
        dActor_c::Spawn(kSnowmanId, kSpawnWatcher, self->mHomePos, 0, self->mAreaId, -1);
    }
}

// @symbol func_ov081_0212423c
/* Turns toward the closest visible player at the rate row idx gives. With
   idx 1 it also leans back while the player stays off to one side, and
   after 40 frames of that topples over. */
extern "C" void func_ov081_0212423c(daSnowman_c *self, int idx)
{
    Player *player;
    struct Vector3 v;
    int ang;
    int off;
    s16 *maxStep;
    s16 *band;
    s16 *divisor;
    int lim;

    player = self->ClosestNonVanishPlayer();
    if (player == 0)
        return;

    /* Read through one base pointer, y and z first: naming the three
       fields directly reschedules the loads. */
    {
        int *pp = &player->mPosX;
        int py = pp[1];
        int pz = pp[2];
        v.x = pp[0];
        v.y = py;
        v.z = pz;
    }
    ang = Vec3_HorzAngle((struct Vector3 *)&self->mPosX, &v);

    /* Two rows of (divisor, band, maxStep), six bytes apart. Both rows in
       the ROM are 1, 0x1000, 0x1000; idx still selects which one. */
    off = idx * 6;
    maxStep = (s16 *)((char *)&data_ov081_021289a8 + off);
    band = (s16 *)((char *)&data_ov081_021289a6 + off);
    divisor = (s16 *)((char *)&data_ov081_021289a4 + off);

    ApproachAngle(&self->mPrevAngleY, ang, *divisor, *band, *maxStep);

    if (idx != 1) {
        self->mInitAngleY = 0;
        self->mTimer = 0;
    } else if (AngleDiff(ang, self->mAngleY) > 0x200) {
        lim = -0x2000;
        self->mInitAngleY -= 0x100;
        if (self->mInitAngleY < lim) {
            self->mInitAngleY = (s16)lim;
            self->mTimer++;
            if (self->mTimer > 0x28) {
                self->mStep = self->mAngleY - ang;
                func_ov081_02125488(self, &data_ov081_02128e94);
                return;
            }
        }
    } else {
        self->mTimer = 0;
        ApproachAngle(&self->mInitAngleY, 0, 1, 0x500, 0x500);
    }

    ApproachAngle(&self->mAngleX, self->mInitAngleY, *divisor, *band, *maxStep);
    self->mAngleY = self->mPrevAngleY;
}

// @symbol func_ov081_021243cc
/* Reacts to whatever the collision cylinder touched this frame. */
extern "C" void func_ov081_021243cc(daSnowman_c *self)
{
    Vector3 v;
    v = data_ov081_02128998;
    self->mdCcAcPos_c.SetPosRelativeToActor(v);

    u32 id = self->mdCcAcPos_c.otherOwner;
    if (id == 0) return;
    Player *found = (Player *)dActor_c::FindWithID(id);
    if (found == 0) return;

    int hit;
    s32 flags = self->mdCcAcPos_c.hitFlags;
    hit = 0;

    if (flags & 0x2000) {
        self->mDeathState = 2;
        func_ov002_020aea30(self, found, 0);
        self->mPrevAngleY = (u16)(self->HorzAngleToCPlayer() + 0x8000);
        hit = 1;
    }
    if (flags & 0x40000) {
        func_02012694(0xdb, &self->mCamSpacePosX);
        hit = 1;
        func_ov081_02125488(self, &data_ov081_02128e24);
    }
    /* The flag is materialised; testing actorID in the if directly
       changes the compare. */
    int isPlayer = (int)(found->actorID == kPlayerId);
    if (isPlayer) {
        if (found->mIsVanish != 0) return;
        if (found->mIsMetal == 1) {
            self->mDeathState = 2;
            func_ov002_020aea30(self, found, 0);
            self->mPrevAngleY = (u16)(self->HorzAngleToCPlayer() + 0x8000);
            hit = 1;
        }
        if (flags & 0x10) {
            Vector3_16 vv;
            vv.x = (s16)-0x1200;
            vv.y = 0;
            vv.z = 0;
            _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(self, &vv, found);
            func_02012694(0x1d, &self->mCamSpacePosX);
            hit = 1;
        }
        if ((flags & 0x40) && (s32)found->param1 == 2) {
            self->mDeathState = 2;
            func_ov002_020aea30(self, found, 0);
            self->mDeathSound = 2;
            self->mPrevAngleY = (u16)(self->HorzAngleToCPlayer() + 0x8000);
            hit = 1;
        }
        if (hit == 0) {
            Vector3 hv;
            hv = *(Vector3 *)&self->mPosX;
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(found, &hv, 2, 0xc000, 1, 0, 1);
        }
    }
    if (hit != 1) return;
    func_ov081_02124134(self);
}

// @symbol func_ov081_021245e8
/* Distance to the closest player; leaves that player's index behind. */
extern "C" int func_ov081_021245e8(daSnowman_c *self)
{
    int min = 0x2710000;
    int i = 0;
    Vector3 v;
    self->mClosestPlayerIdx = -1;
    if ((int)data_0209f21c[0] > 0) {
        do {
            Player *obj = (Player *)data_0209f394[i];
            if (obj != 0) {
                int dist;
                int *s = &obj->mPosX;   /* one base pointer; see 0212423c */
                v.x = s[0];
                v.y = s[1];
                v.z = s[2];
                dist = Vec3_Dist((Vector3 *)&self->mPosX, &v);
                if (i == 0) {
                    min = dist;
                    self->mClosestPlayerIdx = 0;
                } else if (dist < min) {
                    min = dist;
                    self->mClosestPlayerIdx = i;
                }
            }
            i++;
        } while (i < (int)data_0209f21c[0]);
    }
    return min;
}

// @symbol func_ov081_021246a0
/* State "melt", execute: shrink and sink, then pay out coins and die. */
extern "C" int func_ov081_021246a0(daSnowman_c *self)
{
    Vector3 v;
    if (self->mType != 0 || self->mWithMeshClsn.IsOnGround() != 0) {
        ApproachLinear(self->mScaleX, 0x1700, 0x50);
        ApproachLinear(self->mScaleZ, 0x1700, 0x50);
        ApproachLinear(self->mScaleY, 0, 0x50);
        ApproachLinear(self->mSinkOffsetY, -0x4000, 0x199);
        /* Every read of mStateTimer here is ldrh: it is counted down
           through DecIfAbove0_Short, which takes it unsigned. */
        if ((u16)self->mStateTimer == 0) self->mStateTimer = 0x14;
        if (self->mScaleY < 0x50) {
            if (self->mCapPhase == 0) {
                v.x = self->mPosX;
                v.y = self->mPosY;
                v.z = self->mPosZ;
                _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(self, &v, self->unk_10a + 1, 0xa000, 0);
            }
            func_ov081_02124134(self);
            self->KillAndTrackInDeathTable();
        }
    }
    return 1;
}

// @symbol func_ov081_0212479c
/* State "melt", enter. */
extern "C" int func_ov081_0212479c(daSnowman_c *self)
{
    self->mInitAngleY = 0;
    self->mTimer = 0;
    self->mStateTimer = 0;
    if (self->mType == 0)
        self->mVertAccel = -0x2000;
    if (self->mCapUniqueID != 0) {
        dActor_c *a = dActor_c::FindWithID(self->mCapUniqueID);
        if (a != 0) {
            a->mVertAccel = -0x2000;
            int rv = ((unsigned int)RandomIntInternal(&data_0209e650) >> 8) & 0xf;
            a->mPrevAngleX = 0;
            a->mPrevAngleY = rv << 0xc;
            a->mPrevAngleZ = 0;
            a->mHorzSpeed = 0xa000;
            a->unk_0a4 = 0;
            a->mVertSpeed = 0x14000;
            a->unk_0ac = 0;
            *(int *)((char *)a + 0xc8) = 0;   /* the carry matrix; dActor_c pad */
        }
        func_02012790(0xa);
        self->mCapUniqueID = 0;
    }
    if (self->mType == 2)
        dActor_c::Spawn(kSnowmanId, kSpawnWatcher, self->mHomePos, 0, self->mAreaId, -1);
    return 1;
}

// @symbol func_ov081_02124894
/* State "spin and fall", execute. */
extern "C" int func_ov081_02124894(daSnowman_c *self)
{
    Vector3 v;
    /* Source written as (>0 ? sub : add) so mwccarm inversion emits the
       ROM's le/add arm first, then the unconditional sub arm. */
    if (self->mStep > 0)
        self->mInitAngleY -= 0x200;
    else
        self->mInitAngleY += 0x200;

    self->mPrevAngleY += self->mInitAngleY;
    ApproachAngle(&self->mAngleX, -0x2800, 1, 0x500, 0x500);
    self->mTimer++;
    if (self->mTimer > 0x28) {
        if (self->mCapPhase == 0) {
            v.x = self->mPosX;
            v.y = self->mPosY;
            v.z = self->mPosZ;
            _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(self, &v, self->unk_10a + 1, 0xa000, 0);
        }
        func_ov081_02124134(self);
        self->KillAndTrackInDeathTable();
    }
    return 1;
}

// @symbol func_ov081_0212498c
/* State "spin and fall", enter. */
extern "C" int func_ov081_0212498c(daSnowman_c *self)
{
    int id = self->mSnowballID;
    if (id != 0) {
        dActor_c *a = dActor_c::FindWithID(id);
        if (a != 0) {
            a->mVertAccel = -0x2000;
            a->mTerminalVelocity = -0x28000;
            self->mSnowballID = 0;
        }
    }
    func_02012694(0xdb, &self->mCamSpacePosX);
    self->mInitAngleY = 0;
    self->mTimer = 0;
    return 1;
}

// @symbol func_ov081_021249f4
/* State "sink", execute. */
extern "C" int func_ov081_021249f4(daSnowman_c *self)
{
    struct Vector3 pos;
    int t;

    pos = self->mHomePos;
    self->mParticleID1 =
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            self->mParticleID1, 0x11d, pos.x, pos.y, pos.z, 0, 0);
    pos.y = pos.y + 0x1e000;
    self->mParticleID2 =
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            self->mParticleID2, 0x11e, pos.x, pos.y, pos.z, 0, 0);

    if (self->mHomePos.y > self->mPosY) {
        ApproachAngle(&self->mAngleX, self->mInitAngleY, 1, 0x1000, 0x1000);
    }
    t = self->mHomePos.y - 0x118000;
    if (t > self->mPosY) {
        self->mPosY = t;
        self->mVertSpeed = 0;
        self->mVertAccel = 0;
        func_ov081_02125488(self, &data_ov081_02128e84);
    }
    self->mPrevAngleY += 0x2000;
    self->mAngleY = self->mPrevAngleY;
    return 1;
}

// @symbol func_ov081_02124b08
/* State "sink", enter. */
extern "C" int func_ov081_02124b08(daSnowman_c *self)
{
    unsigned int id = self->mSnowballID;
    if (id != 0) {
        dActor_c *a = dActor_c::FindWithID(id);
        if (a != 0) {
            a->mVertAccel = -0x2000;
            a->mTerminalVelocity = -0x28000;
            self->mSnowballID = 0;
        }
    }
    self->mVertSpeed = 0xa000;
    self->mVertAccel = -0x4000;
    self->mInitAngleY = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &self->mModelAnim, LOADED_FILE(data_ov081_02128da8), 0, 0x1000, 0);
    return 1;
}

// @symbol func_ov081_02124b98
/* State "throw", execute: lets go on frame 10, aimed at the closest player. */
extern "C" int func_ov081_02124b98(daSnowman_c *self)
{
    Vector3 in, out, v[2];
    dActor_c *target;
    Player *player;

    if (self->mSnowballID != 0) {
        target = dActor_c::FindWithID(self->mSnowballID);
        if (target != 0) {
            target->mPosX = self->mHandPos.x;
            target->mPosY = self->mHandPos.y;
            target->mPosZ = self->mHandPos.z;
            if (self->mModelAnim.WillHitFrame(0xa) != 0) {
                in.x = 0; in.y = 0; in.z = 0x1e000;
                out.x = 0; out.y = 0; out.z = 0;
                v[0].x = 0; v[0].y = 0; v[0].z = 0;
                player = self->ClosestPlayer();
                if (player != 0) {
                    int angle;
                    int *q = &player->mPosX;   /* one base pointer; see 0212423c */
                    v[1].x = q[0];
                    v[1].y = q[1];
                    v[1].z = q[2];
                    v[0].x = v[1].x - self->mHandPos.x;
                    v[0].y = v[1].y - self->mHandPos.y;
                    v[0].z = v[1].z - self->mHandPos.z;
                    angle = _ZN4cstd5atan2E5Fix12IiES1_(v[0].x, v[0].z);
                    Matrix4x3_FromRotationY(&data_020a0e68, angle);
                    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68,
                        (short)(-_ZN4cstd5atan2E5Fix12IiES1_(v[0].y, Vec3_HorzLen(&v[0]))));
                    MulVec3Mat4x3(&in, &data_020a0e68, &out);
                }
                out.y += 0x14000;
                target->unk_0a4 = out.x;
                target->mVertSpeed = out.y;
                target->unk_0ac = out.z;
                target->mVertAccel = -0x2000;
                self->mSnowballID = 0;
            }
        }
    }
    if (self->mModelAnim.Finished() != 0) {
        func_ov081_02125488(self, &data_ov081_02128e14);
    }
    return 1;
}

// @symbol func_ov081_02124d14
/* State "throw", enter. */
extern "C" int func_ov081_02124d14(daSnowman_c *self)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, LOADED_FILE(data_ov081_02128db8), 0x40000000, 0x1000, 0);
    return 1;
}

// @symbol func_ov081_02124d50
/* State "make a snowball", execute. */
extern "C" int func_ov081_02124d50(daSnowman_c *self)
{
    dActor_c *ball = dActor_c::FindWithID(self->mSnowballID);
    if (ball != 0 && ball->mVertAccel == 0) {
        func_ov081_0212423c(self, 1);
        ball->mPosX = self->mHandPos.x;
        ball->mPosY = self->mHandPos.y;
        ball->mPosZ = self->mHandPos.z;
    }

    if ((u16)self->mStateTimer == 0) {
        func_ov081_02125488(self, &data_ov081_02128e44);
        return 1;
    }

    if (func_ov081_021245e8(self) > 0x320000) {
        func_ov081_02125488(self, &data_ov081_02128e64);
    }
    return 1;
}

// @symbol func_ov081_02124dfc
/* State "make a snowball", enter: spawns it in the hand. */
extern "C" int func_ov081_02124dfc(daSnowman_c *self)
{
    dActor_c *a = dActor_c::Spawn(kSnowballId, 0, self->mHandPos, 0, self->mAreaId, -1);
    if (a)
        self->mSnowballID = a->uniqueID;
    self->mStateTimer = 0x64;
    self->mStep = 0;
    return 1;
}

// @symbol func_ov081_02124e64
/* State "face the player", execute. */
extern "C" int func_ov081_02124e64(daSnowman_c *self)
{
    func_ov081_0212423c(self, 0);
    if ((u16)self->mStateTimer == 0)
        func_ov081_02125488(self, &data_ov081_02128e34);
    if (func_ov081_021245e8(self) > 0x320000)
        func_ov081_02125488(self, &data_ov081_02128e64);
    return 1;
}

// @symbol func_ov081_02124ec0
/* State "face the player", enter. */
extern "C" int func_ov081_02124ec0(daSnowman_c *self)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, LOADED_FILE(data_ov081_02128d98), 0, 0x1000, 0);
    if (self->mType == 2)
        self->mFlags = 0x18000002;
    else
        self->mFlags = 0x10000002;
    self->mStateTimer = 0x14;
    return 1;
}

// @symbol func_ov081_02124f20
/* State "rise", execute. */
extern "C" int func_ov081_02124f20(daSnowman_c *self)
{
    if (self->mVertSpeed < 0) {
        if (self->mHomePos.y > self->mPosY) {
            self->mVertSpeed = 0;
            self->mVertAccel = 0;
            self->mPosY = self->mHomePos.y;
            func_ov081_02125488(self, &data_ov081_02128e14);
        }
    }
    func_ov081_0212423c(self, 0);
    return 1;
}

// @symbol func_ov081_02124f7c
/* State "rise", enter. */
extern "C" int func_ov081_02124f7c(daSnowman_c *self)
{
    struct Vector3 pos;

    self->mVertSpeed = 0x3c000;
    self->mVertAccel = -0x4000;
    func_02012694(0xdc, &self->mCamSpacePosX);
    pos.x = self->mHomePos.x;
    pos.y = self->mHomePos.y;
    pos.z = self->mHomePos.z;
    self->mParticleID1 =
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            self->mParticleID1, 0x11a, pos.x, pos.y, pos.z, 0,
            0);
    pos.y = pos.y + 0x1e000;
    self->mParticleID2 =
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            self->mParticleID2, 0x11b, pos.x, pos.y, pos.z, 0,
            0);
    return 1;
}

// @symbol func_ov081_02125038
/* State "wait buried", execute: rise when a player comes near. */
extern "C" int func_ov081_02125038(daSnowman_c *self)
{
    if (func_ov081_021245e8(self) < 0x258000)
        func_ov081_02125488(self, &data_ov081_02128ea4);
    return 1;
}

// @symbol func_ov081_02125068
/* State "wait buried", enter. */
extern "C" int func_ov081_02125068(daSnowman_c *self)
{
    int v = self->mType;
    if (v != 2 && v != 3) {
        self->mFlags = 3;
    } else if (v == 2) {
        self->mFlags = 0x8000002;
    }
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, LOADED_FILE(data_ov081_02128d98), 0, 0x1000, 0);
    return 1;
}

// @symbol func_ov081_021250c8
/* State "hop to a node", execute. */
extern "C" int func_ov081_021250c8(daSnowman_c *self)
{
    struct Vector3 node;
    struct Vector3 diff;
    struct Vector3 scaled;
    int len;

    if (self->mWithMeshClsn.IsOnGround()) {
        func_02012694(0xe5, &self->mCamSpacePosX);
        func_ov081_02125488(self, &data_ov081_02128e54);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, LOADED_FILE(data_ov081_02128da0), 0x40000000, 0x1000, 0);
        return 1;
    }

    PathPtr p;
    p.FromID(self->mPathId);
    p.GetNode(node, self->mPathNodeIndex);
    node.y = self->mPosY;
    Vec3_Sub(&diff, (struct Vector3 *)&self->mPosX, &node);
    len = LenVec3(&diff);
    ApproachAngle(&self->mPrevAngleY, Vec3_HorzAngle((struct Vector3 *)&self->mPosX, &node), 1, 0x1000, 0x500);
    if (len == 0 || len <= self->mHopStep)
        return 1;
    {
        int q = cstd::fdiv(self->mHopStep, len);
        Vec3_MulScalar(&scaled, &diff, q);
        SubVec3((struct Vector3 *)&self->mPosX, &scaled, (struct Vector3 *)&self->mPosX);
    }
    return 1;
}

// @symbol func_ov081_02125200
/* State "hop to a node", enter: nothing to do. */
extern "C" int func_ov081_02125200(void)
{
    return 1;
}

// @symbol func_ov081_02125208
/* State "walk the path", execute. */
extern "C" int func_ov081_02125208(daSnowman_c *self)
{
    struct Vector3 node;

    if ((u16)self->mStateTimer == 0) {
        if (self->mStep == 1) {
            self->mVertSpeed = 0x14000;
            self->mVertAccel = -0x4000;
            self->mStep = 2;
            func_02012694(0xdd, &self->mCamSpacePosX);
        }

        PathPtr p;
        p.FromID(self->mPathId);
        p.GetNode(node, self->mPathNodeIndex);
        ApproachAngle(&self->mPrevAngleY, Vec3_HorzAngle((struct Vector3 *)&self->mPosX, &node), 1, 0x1000, 0x500);

        if (AngleDiff(self->mPrevAngleY, Vec3_HorzAngle((struct Vector3 *)&self->mPosX, &node)) < 0x100
            && self->mWithMeshClsn.IsOnGround()) {
            int st = self->mStep;
            if (st != 0) {
                if (st == 1)
                    func_02012694(0xe5, &self->mCamSpacePosX);
                self->mStep++;
                if (self->mStep < 0xb)
                    return 1;
            }
            self->mStep = 0;
            self->mVertSpeed = 0x3a000;
            self->mVertAccel = -0x4000;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, LOADED_FILE(data_ov081_02128d88), 0x40000000, 0x1000, 0);
            func_02012694(0xdd, &self->mCamSpacePosX);
            func_ov081_02125488(self, &data_ov081_02128e74);
        }
    }
    return 1;
}

// @symbol func_ov081_0212538c
/* State "walk the path", enter: step onto the next node when close enough. */
extern "C" int func_ov081_0212538c(daSnowman_c *self)
{
    struct Vector3 node;
    struct Vector3 diff;
    int len;

    PathPtr pp;
    pp.FromID(self->mPathId);
    pp.GetNode(node, self->mPathNodeIndex);
    node.y = self->mPosY;
    Vec3_Sub(&diff, (struct Vector3 *)&self->mPosX, &node);
    len = LenVec3(&diff);
    self->mHopStep = 0xa000;
    self->mStep = 0;
    if (len == 0 || len <= self->mHopStep) {
        self->mPosX = node.x;
        self->mPosY = node.y;
        self->mPosZ = node.z;
        self->mPathNodeIndex++;
        if (self->mPathNodeIndex >= self->mPathNodeCount)
            self->mPathNodeIndex = 0;
        self->mStep = 1;
    }
    self->mStateTimer = 0xa;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &self->mModelAnim, LOADED_FILE(data_ov081_02128d98), 0, 0x1000, 0);
    return 1;
}

// @symbol func_ov081_02125488
/* Enters a state: records it, then runs its enter handler if it has one. */
extern "C" int func_ov081_02125488(daSnowman_c *self, daSnowman_c::State *state)
{
    self->mState = state;
    if (self->mState->enter == 0)
        return 1;
    return (self->*self->mState->enter)();
}

// @symbol func_ov081_021254d8
/* Poses the model, then carries the cap on bone 5 and the snowball on
   bone 3, and drops the shadow while standing on the path. */
extern "C" void func_ov081_021254d8(daSnowman_c *self)
{
    int src[3], dst[3];
    int t;
    dActor_c *actor;

    self->mModelAnim.UpdateVerts();

    src[0] = self->mPosX;
    t = self->mPosY;
    src[1] = t;
    src[2] = self->mPosZ;
    src[1] = t + self->mSinkOffsetY;
    Vec3_Asr(dst, src, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, dst[0], dst[1], dst[2]);

    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        self->mAngleX, self->mAngleY, self->mAngleZ);
    self->mModelAnim.mat4x3 = data_020a0e68;

    if (self->mCapUniqueID != 0) {
        self->mCapPos.x = 0;
        self->mCapPos.y = 0;
        self->mCapPos.z = 0;
        actor = dActor_c::FindWithID(self->mCapUniqueID);
        if (actor != 0) {
            MulMat4x3Mat4x3(&self->mModelAnim.data.transforms[5], &self->mModelAnim.mat4x3, &self->mCapMatrix);

            self->mCapPos.x = data_020a0e68.m[9];
            self->mCapPos.y = data_020a0e68.m[10];
            self->mCapPos.z = data_020a0e68.m[11];
            self->mCapPos.x <<= 3;
            self->mCapPos.y <<= 3;
            self->mCapPos.z <<= 3;
            actor->mPosX = self->mCapPos.x;
            actor->mPosY = self->mCapPos.y;
            actor->mPosZ = self->mCapPos.z;

            Matrix4x3_FromTranslation(&data_020a0e68, 0, 0x4000, 0);
            Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, 0, 0x2000, 0);
            MulMat4x3Mat4x3(&data_020a0e68, &self->mCapMatrix, &self->mCapMatrix);
            *(Matrix4x3 **)((char *)actor + 0xc8) = &self->mCapMatrix;   /* dActor_c pad */
        }
    }

    self->mHandPos.x = 0;
    self->mHandPos.y = 0;
    self->mHandPos.z = 0;
    data_020a0e68 = self->mModelAnim.mat4x3;
    MulMat4x3Mat4x3(&self->mModelAnim.data.transforms[3], &data_020a0e68, &data_020a0e68);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0xc000, -0x1000, 0);

    self->mHandPos.x = data_020a0e68.m[9];
    self->mHandPos.y = data_020a0e68.m[10];
    self->mHandPos.z = data_020a0e68.m[11];
    self->mHandPos.x <<= 3;
    self->mHandPos.y <<= 3;
    self->mHandPos.z <<= 3;

    if (self->mType != 0)
        return;

    Matrix4x3_FromTranslation(&data_020a0e68,
        self->mPosX >> 3,
        (self->mPosY - 0xe000) >> 3,
        self->mPosZ >> 3);
    self->mShadowMatrix = data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        self, &self->mShadowModel, &self->mShadowMatrix, 0x78000, 0xc8000, 0xf);
}

// @symbol _ZN11daSnowman_c16CleanupResourcesEv
int daSnowman_c::CleanupResources()
{
    data_ov081_02128d90.Release();
    data_ov081_02128db0.Release();
    data_ov081_02128d98.Release();
    data_ov081_02128db8.Release();
    data_ov081_02128da8.Release();
    data_ov081_02128d88.Release();
    data_ov081_02128da0.Release();
    return 1;
}

// @symbol _ZN11daSnowman_c16OnPendingDestroyEv
void daSnowman_c::OnPendingDestroy()
{
}

// @symbol _ZN11daSnowman_c6RenderEv
int daSnowman_c::Render()
{
    int s = mType;
    if (s == 3) return 1;
    if (s == 2) {
        if (mCapUniqueID)
            mModelAnim.HideMaterial(0, 2);
        else
            mModelAnim.ShowMaterial(0, 2);
    }
    mModelAnim.Model::Render((const Vector3 *)&mScaleX);
    return 1;
}

// @symbol _ZN11daSnowman_c8BehaviorEv
int daSnowman_c::Behavior()
{
    Player *p;
    dActor_c *cap;
    if (mType == 3) {
        switch (mCapPhase) {
        case 0:
            if (SaveData::HasPlayerLostCap()) mCapPhase = 1;
            else mCapPhase = 2;
            break;
        case 1:
            if (!SaveData::HasPlayerLostCap()) mCapPhase = 2;
            break;
        case 2:
            if (SaveData::HasPlayerLostCap()) mType = 2;
            break;
        }
        return 1;
    }
    if (UpdateKillByInvincibleChar(mWithMeshClsn, mModelAnim, 3)) return 1;
    if (mDeathState != 0) {
        if (UpdateDeath(mWithMeshClsn) && mType == 2) {
            dActor_c::Spawn(kSnowmanId, kSpawnWatcher, mHomePos, 0, mAreaId, -1);
            mType = 0;
        }
        if (mDeathState == 0 && mDeathSound != 0) {
            func_02012694(0x166, &mCamSpacePosX);
            mDeathSound = 0;
        }
        func_ov081_021254d8(this);
        return 1;
    }
    if (mType == 2
        && mState != &data_ov081_02128e94
        && mState != &data_ov081_02128e24
        && SaveData::HasPlayerLostCap()
        && mCapUniqueID == 0) {

        p = ClosestPlayer();
        if (p != 0) {
            int param = 0xc;
            param = param | (p->mCharacter << 8);
            cap = dActor_c::Spawn(kMarioCapId, param, *(Vector3 *)&mPosX, 0, mAreaId, -1);
            if (cap != 0) {
                _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(cap, 0x64000, 0xc8000, 0x1000000, 0x1000000);
                mCapUniqueID = cap->uniqueID;
            }
        }

    }
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    if (mState->execute != 0)
        (this->*mState->execute)();
    UpdatePos(&mdCcAcPos_c);
    if (mType == 0)
        UpdateWMClsn(mWithMeshClsn, 0);
    func_ov081_021254d8(this);
    if (mState != &data_ov081_02128e84
        && mState != &data_ov081_02128e64
        && mState != &data_ov081_02128e94
        && mState != &data_ov081_02128e24)
        func_ov081_021243cc(this);
    mdCcAcPos_c.Clear();
    {
        p = ClosestPlayer();
        if (p != 0 && p->mIsVanish == 0)
            mdCcAcPos_c.Update();
    }
    mModelAnim.speed = 0x1000;
    mModelAnim.Advance();
    return 1;
}

// @symbol _ZN11daSnowman_c13InitResourcesEv
int daSnowman_c::InitResources()
{
    Vector3 v;

    mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov081_02128db0), 1, -1);
    Model::LoadFile(data_ov081_02128d90);
    mShadowModel.InitCylinder();
    Animation::LoadFile(data_ov081_02128d98);
    Animation::LoadFile(data_ov081_02128db8);
    Animation::LoadFile(data_ov081_02128da8);
    Animation::LoadFile(data_ov081_02128d88);
    Animation::LoadFile(data_ov081_02128da0);

    mPathId = (s32)param1 & 0xff;
    mType = ((s32)param1 & 0xff00) >> 8;
    if (mPathId == 0xff)
        mPathId = 0;
    if (mType == 0xff)
        mType = 0;

    mHomePos.x = mPosX;
    mHomePos.y = mPosY;
    mHomePos.z = mPosZ;

    if (GetBitInDeathTable() != 0) {
        if (mType == 2) {
            if (SaveData::HasPlayerLostCap() != 0) {
                dActor_c::Spawn(kSnowmanId, kSpawnCapWearer, mHomePos, 0, mAreaId, -1);
            } else {
                dActor_c::Spawn(kSnowmanId, kSpawnWatcher, mHomePos, 0, mAreaId, -1);
            }
        }
        if (mType != 3)
            return 0;
    }

    mSnowballID = 0;
    mCapUniqueID = 0;

    if (mType == 0) {
        PathPtr pp;
        pp.FromID(mPathId);
        mPathNodeCount = pp.NumNodes();
    }

    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mTerminalVelocity = -0xc8000;

    v = data_ov081_02128998;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mdCcAcPos_c, this, &v, 0x3c000, 0x96000, 0x200004, 0x42050);

    mAngleY = mPrevAngleY;
    mInitAngleY = mAngleY;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x14000, 0xf000, 0, 0);

    unk_108 = 1;
    unk_10a = 2;

    if (mType == 0) {
        PathPtr pp;
        pp.FromID(mPathId);
        pp.GetNode(*(Vector3 *)&mPosX, mPathNodeIndex);
        mPathNodeIndex = 1;
        mVertAccel = -0x2000;
        mFlags = 0x10000000;
        func_ov081_02125488(this, &data_ov081_02128e54);
    } else {
        mPosY -= 0x118000;
        func_ov081_02125488(this, &data_ov081_02128e84);
        if (mType == 3) {
            unk_108 = 0;
            unk_10a = 0;
            mFlags = 2;
        }
        if (mType == 2)
            mFlags = 0x8000002;
        else
            mFlags = 3;
    }

    return 1;
}

// @symbol _ZN11daSnowman_c16OnAimedAtWithEggEv
/* Slot 29. Callers add this to the actor's Y before the egg-aim particles. */
s32 daSnowman_c::OnAimedAtWithEgg() {
    return 0x98000;
}
