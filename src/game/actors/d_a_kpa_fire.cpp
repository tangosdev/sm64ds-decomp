//cpp
/* daKpaFire_c -- Bowser's fire (KOOPAFIRE, ov060).
 *
 * _ZTS11daKpaFire_c at ov060 0x0211a7c0, _ZTI at 0x0211a7b4 (base
 * _ZTI12dEnemyBase_c), _ZTV at 0x0211a7f4. The tree used to call this
 * class BowserFire. The out-of-line destructor is the key function:
 * D1 at 0x02116484, D0 at 0x021164c4, then a D2 the cartridge does not
 * keep. Codegen is deferred, so .text is the reverse of source order.
 * The destructor alone sits under defer_codegen off, which is what
 * emits D1 before D0.
 *
 * Eight variants, param1 & 7. Behavior runs data_ov060_0211afb4.
 * InitResources runs data_ov060_0211af74, then finishes setup.
 *   0  rides Bowser's mouth and spits variant 5
 *   1  grows, then splits into variant 2
 *   2  flies, then lands as variant 7 or 3
 *   3  one frame: spawns three variant 4
 *   4  bounces; vanishes within 150 units of Bowser while his hold state is
 *      idle, or after 150 frames
 *   5  sprays on a pitch and heading, then lands as variant 6
 *   6  falls, burns on the ground, ends in a burst
 *   7  same handler as 6, tossed straight up
 * FIRE_PARAM packs variant in bits 0..2 and the sub-variant in bits 4..5.
 *
 * Actor creation, lookup, movement and destruction use the existing native
 * interfaces, as do the ground predicate and Player::Burn. Spawn still views
 * the base actor's contiguous scalar position/angle fields as vectors; that
 * inherited flat-field layout is not reconstructed here.
 *
 * leftovers:
 * - func_ov060_02116740, 02117624 stay free functions with
 *   the char * / unsigned char * decl_common.h declares. Their bodies
 *   compile as C++; 02117624 uses a POD matrix view for its block copy.
 * - dCcAc_c::Init, dBgCh_Actr::Init, DropShadowRadHeight,
 *   Particle::System::New / NewSimple / NewUnkCallback818 and
 *   SaveData::IsCharacterUnlocked stay mangled. Those calls pass
 *   Fix12<int> by value; the header spelling of dBgCh_Actr::Init is
 *   Fix12i, which mangles as int.
 * - GetFrameCount stays the void* extern. The argument is
 *   static_cast<dExtFrameCtrl_c *>(&o->mModelAnim), ModelAnim's
 *   second base, which is Bowser + 0x124. The header method is not
 *   that symbol's spelling at this site.
 * - Particle::System::callbackScale and callbackVelocity are named.
 *   The word at +0x44 sits in pad_042 and stays a raw store.
 * - DropShadowRadHeight's matrix is pad_32c. daKpaFire_c.h has no
 *   Matrix4x3 member there.
 * - Sound 0x180, player param1 == 3, and the particle kind pairs are
 *   named only by the use-site enums below.
 * - data_ov060_0211af74 and data_ov060_0211afb4 are this TU's owned
 *   eight-entry InitResources and Behavior PMF tables, respectively.
 *   data_ov060_0211934c / 02119358 / 02119364 and g_profile_KOOPAFIRE
 *   remain external to this TU.
 */

#include "daKpaFire_c.h"
#include "types.h"
#include "common.h"
#include "decl_common.h"
#include "dBgCh_Actr.h"
#include "dBgCh_Gnd.h"
#include "dActor_c.h"
#include "daKpa_c.h"
#include "Player.h"
#include "SaveData.h"
#include "Particle__System.h"
#include "nitro/fx/fx.h"

/* POD view of the 0x30-byte matrix. The single integer array preserves
 * the shadow update's block copy in C++ with Vector3's user-declared
 * constructor in scope. */
struct RawMatrix4x3 { int m[12]; };
typedef struct RawMatrix4x3 RawMatrix4x3;

/* The variant tables. Behavior calls data_ov060_0211afb4[mVariant].pmf and
 * InitResources calls data_ov060_0211af74[mVariant]. Both hold
 * pointer-to-member on dActor_c, which is what the sinit records store. */
typedef void (dActor_c::*PMF)();
struct Entry { PMF pmf; };

/* Actor ids, symbols/actor_debug_names.tsv. */
enum {
    ACTOR_PLAYER = 191,
    ACTOR_OBJ_MARIO_CAP = 269,
    ACTOR_KOOPA = 279,        /* daKpa_c, the registry profile that classInit names KOOPA */
    ACTOR_KOOPAFIRE = 280,    /* this class */
    ACTOR_COIN = 288
};

/* mVariant values the code compares against directly: 0 (InitResources) and 4
   (Behavior) have names here; func_ov060_02116f90 also tests for 7 and still
   spells it as a bare literal. The rest are told apart only by which handler
   the tables pick (see the banner). */
enum {
    VARIANT_MOUTH = 0,        /* cylinder disabled, no shadow, rides Bowser's mouth (its handler starts no particle) */
    VARIANT_BOUNCING = 4      /* the one whose Behavior keeps its gravity after touching ground */
};

/* The param1 word a spawner hands to a new fire: variant in bits 0..2,
   sub-variant in bits 4..5 (what InitResources reads back out). */
#define FIRE_PARAM(variant, sub) (((sub) << 4) | (variant))

/* Particle ids, named by where they are used, not recovered names. 0x9a, 0x9c
   and 0xa6 are passed to func_ov060_02116518, which also starts id + 1 in the
   second handle. 0x9e goes straight to NewUnkCallback818 (one handle only);
   0x9f is a one-shot NewSimple. */
enum {
    PARTICLE_LANDED_BURN = 0x9c,   /* variants 6/7 once mLanded is set */
    PARTICLE_FALLING = 0x9e,       /* variants 6/7 while airborne */
    PARTICLE_SCALED_9A = 0x9a,     /* variants 1 and 5; the one id that also gets the extra +0x44 / +0x4c sizing */
    PARTICLE_VARIANT_4 = 0xa6,     /* variant 4 */
    PARTICLE_END_PUFF = 0x9f       /* one-shot at the end of variant 6/7 (NewSimple, not a handle) */
};

/* Sound id variant 0 plays through its mSoundHandle; named by use site, not decoded. */
enum { SOUND_ID_MOUTH = 0x180 };

extern "C" {
/* This file's own helpers, defined further down. */
extern Entry data_ov060_0211afb4[];
extern PMF data_ov060_0211af74[];

/* Fix12<int>-by-value and other mangled-name calls (see the banner). */
extern u32 _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f( u32 a, u32 b, Fix12i c, Fix12i d, Fix12i e, const struct Vector3_16f* f);
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE( u32 a, u32 b, Fix12i c, Fix12i d, Fix12i e, const void* f, void* g);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, Fix12i x, Fix12i y, Fix12i z);
/* local extern: the header method is not this symbol's spelling here. */
extern int _ZNK15dExtFrameCtrl_c13GetFrameCountEv(void *anim);
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 a, u32 b, u32 c, void *pos, u32 d);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *actor, int a, int b, unsigned int c, unsigned int d);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int a, int b, void *v, int c);
void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(void *thisp, void *sm, void *mtx, int rad, int t, unsigned int j);
extern void dBgCh_Actr_UpdateDiscreteNoLava_veneer(void *p);

extern short Vec3_HorzAngle(const struct Vector3* a, const struct Vector3* b);
extern Fix12i Vec3_HorzDist(const Vector3* a, const Vector3* b);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern int RandomIntInternal(int* seed);
extern int data_0209e650[];          /* RNG_STATE (symbols/verified.tsv) */
extern short data_02082214[];        /* SINE_TABLE: a sin/cos pair per angle >> 4 */
extern Matrix4x3 data_020a0e68;      /* MATRIX_SCRATCH_PAPER */

/* ov060 tables indexed by mSubVariant (02119358, 0211934c) and the mouth-pose
 * rows variant 0 reads (02119364); see the handlers below. */
extern int data_ov060_02119358[];
extern int data_ov060_0211934c[];
extern u16 data_ov060_02119364[];
}

// @symbol daKpaFire_c_classInit
/* Historical alias BowserFire_Spawn. First in source because codegen is deferred. */
extern "C" daKpaFire_c *daKpaFire_c_classInit(void)
{
    return new daKpaFire_c;
}

// @symbol _ZN11daKpaFire_c13InitResourcesEv
/* Sets up collision and state (no files of its own; daKpa_c owns the fight's).
 * The ground probe uses the real dBgCh_Gnd (clsnY is seed on entry, hit on
 * exit); the variant dispatch through data_ov060_0211af74 measured
 * byte-identical with the real dActor_c. The `|= 1` sets dCc_c::flags bit 0
 * inside mdCcAc_c when the variant is mouth. The doubled pos.y store is the
 * ROM's own shape. dCcAc_c/dBgCh_Actr Init stay mangled: they take
 * Fix12<int> by value (notes/mwccarm-codegen.md, 6az). */
int daKpaFire_c::InitResources()
{
    Vector3 pos;

    if (this->mShadowModel.InitCylinder() == 0)
        return 0;

    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &this->mdCcAc_c, this, 0x28000, 0x50000, 0x200002, 0);   /* radius 40, height 80 units; flags 0x200002, vulnFlags 0 */
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &this->mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);     /* radius and height 50 units; the two Vector3_16 * args are null */

    /* Fix12: gravity -4.0 units per frame per frame, fall speed floor -30.0
       units per frame. The variant's init (below) may overwrite both. */
    this->mVertAccel = -0x4000;
    this->mTerminalVelocity = -0x1e000;
    this->mVariant = this->param1 & 7;
    this->mFrameCount = 0;
    if (this->mVariant == VARIANT_MOUTH)
        this->mDropsShadow = 0;
    else
        this->mDropsShadow = 1;
    this->mLanded = 0;
    this->mSubVariant = ((unsigned int)this->param1 >> 4) & 3;
    /* flags bit 0 set = this cylinder is disabled (dCc_c::flags). */
    if (this->mVariant == VARIANT_MOUTH)
        this->mdCcAc_c.flags |= 1;
    this->mFireScale = 0x2000;      /* 2.0 */
    this->mParticleHandle2 = 0;
    this->mParticleHandle = this->mParticleHandle2;
    this->mKpaUniqueID = 0;

    /* constructed here (not at function top: the ROM constructs after the
       collider setup), destroyed at the single exit below -- both synthesized */
    dBgCh_Gnd gnd;
    {
        int y;
        pos.x = this->mPosX;
        y = this->mPosY;
        pos.y = y;
        pos.z = this->mPosZ;
        pos.y = y + 0x32000;        /* search seed: 50 units above the fire */
    }
    gnd.SetObjAndPos(pos, 0);
    if (gnd.DetectClsn())
        this->mGroundY = gnd.clsnY;
    else
        this->mGroundY = this->mPosY;

    /* the variant's init, from the ROM-filled table */
    (((dActor_c *)this)->*data_ov060_0211af74[this->mVariant])();

    this->mSoundHandle = 0;
    this->mSoundID = 0;
    return 1;
}

// @symbol _ZN11daKpaFire_c8BehaviorEv
/* Once per frame: bump mTickCount, run the variant's handler from the
 * data_ov060_0211afb4 table, bump mFrameCount, then -- while gravity is still
 * on -- step the mesh collision and, for every variant but the bouncing one,
 * stop the fall (speed and accel zero) the first frame it is on the ground.
 * Then the Player-burn check, the shadow update, and the cylinder's Clear /
 * Update. Always returns 1. */
int daKpaFire_c::Behavior()
{
    mTickCount += 1;
    (this->*data_ov060_0211afb4[mVariant].pmf)();
    mFrameCount += 1;
    if (mVertAccel != 0) {
        dBgCh_Actr_UpdateDiscreteNoLava_veneer(&mWithMeshClsn);
        if (mVariant != VARIANT_BOUNCING) {
            if (mWithMeshClsn.IsOnGround() != 0) {
                mVertSpeed = 0;
                mVertAccel = 0;
            }
        }
    }
    func_ov060_02116740((char *)this);
    func_ov060_02117624((char *)this);
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN11daKpaFire_c6RenderEv
/* The flame is drawn by the particle system. This slot only reports success. */
int daKpaFire_c::Render()
{
    return 1;
}

// @symbol _ZN11daKpaFire_c16CleanupResourcesEv
/* No files of its own. daKpa_c loads and releases the fight's models. */
int daKpaFire_c::CleanupResources()
{
    return 1;
}

// @symbol func_ov060_02117624
/* Per-frame drop shadow. Does nothing when mDropsShadow is 0. Otherwise it
 * builds a translation matrix in the shared scratch matrix data_020a0e68 at
 * the fire's x, its mGroundY and its z (each >> 3), copies it into the
 * Matrix4x3 at 0x32c of the actor, and draws the dExtShadowModel_c at 0x304 with
 * radius mShadowRadiusMul * mFireScale, the second Fix12 argument (dActor_c.h
 * calls it `depth`) 30 units (0x1e000), and the `opacity` argument 0xf (what
 * that value means is not decoded; every caller passes 0xf).
 * The Matrix4x3 at 0x32c remains the header's pad_32c storage; copying it
 * through the RawMatrix4x3 view keeps the block copy intact in C++. */
extern "C" void func_ov060_02117624(char *c) {
    struct daKpaFire_c *self = (struct daKpaFire_c *)c;
    if (self->mDropsShadow == 0) return;
    Matrix4x3_FromTranslation(&data_020a0e68, self->mPosX>>3, self->mGroundY>>3, self->mPosZ>>3);
    *(RawMatrix4x3*)self->pad_32c = *(RawMatrix4x3*)&data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(self, &self->mShadowModel, self->pad_32c, self->mShadowRadiusMul * self->mFireScale, 0x1e000, 0xf);
}

// @symbol _ZN11daKpaFire_c19func_ov060_0211747cEv
/* Variant 0 Behavior: ride Bowser's mouth.
 *
 * Looks up the daKpa_c by mKpaUniqueID; if it is gone, destroys this fire.
 * Otherwise it does nothing unless Bowser's mState is 0xf (d_a_kpa.cpp's
 * BOWSER_STATE_ANIM_CHAIN) and func_ov060_02111c68 returns a row number n --
 * from his animation 8 only once its frame is 0x31 or later (n = frame - 0x31),
 * from his animation 6 always (n = frame + 0xb) -- or -1 when neither is
 * playing. n wraps to 0 when it reaches the frame count of the
 * dExtFrameCtrl_c at Bowser +0x124.
 *
 * Row n of the u16 table data_ov060_02119364 (five halfwords per row, t[0..4])
 * gives the mouth pose. In whole units, turned by Bowser's yaw (mAngleY >> 4,
 * times two, indexes the sine table; s0 and s1 are the two entries there):
 * t[0] sideways, t[2] + 20 forward, and t[1] - 90 up. t[4] + 0x818 becomes
 * the fire's pitch word (0x92) and t[3] + yaw - 0x4700 its heading word
 * (0x94). The fire moves to Bowser's position plus that offset and plays
 * sound SOUND_ID_MOUTH on mSoundHandle (the handle is reset when the id
 * changes). On every second row (n even) it also spawns a variant 5 fire at
 * its own position, passing its own pitch/heading words as the spawn rotation.
 *
 * Bowser's position goes through v[] first (volatile, so the reads keep their
 * order). Yaw is the unsigned half of mAngleY. Frame count is the
 * dExtFrameCtrl_c inside mModelAnim. */
void daKpaFire_c::func_ov060_0211747c() {
    daKpa_c *koopa;
    int n;
    int yaw;
    int i;
    u16 *t;
    int s1;
    int s0;
    volatile int v[3];
    int side;
    int fwd;
    int *kpaPos;

    koopa = (daKpa_c *)dActor_c::FindWithID(this->mKpaUniqueID);
    if (koopa == 0) {
        this->MarkForDestruction();
        return;
    }

    if (koopa->mState != 0xf)
        return;

    n = koopa->func_ov060_02111c68();
    if (n < 0)
        return;

    yaw = *(u16 *)&koopa->mAngleY;
    i = (yaw >> 4) * 2;
    kpaPos = &koopa->mPosX;
    v[0] = kpaPos[0];
    v[1] = kpaPos[1];
    s1 = data_02082214[i + 1];
    s0 = data_02082214[i];
    v[2] = kpaPos[2];
    t = data_ov060_02119364;

    if (n == _ZNK15dExtFrameCtrl_c13GetFrameCountEv(
            static_cast<dExtFrameCtrl_c *>(&koopa->mModelAnim)))
        n = 0;

    t += n * 5;

    if (this->mSoundID != SOUND_ID_MOUTH)
        this->mSoundHandle = 0;
    this->mSoundID = SOUND_ID_MOUTH;
    this->mSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(this->mSoundHandle, 3, this->mSoundID,
                                                           &this->mCamSpacePosX, 0);

    side = t[0];
    fwd = t[2] + 0x14;
    this->mPosX = v[0] + (fwd * s0 + side * s1);
    this->mPosY = v[1] + ((t[1] - 0x5a) << 12);
    this->mPosZ = v[2] + (fwd * s1 - side * s0);
    this->mPrevAngleX = (short)(t[4] + 0x818);
    this->mPrevAngleY = (short)(t[3] + yaw - 0x4700);

    if (n & 1)
        return;

    dActor_c::Spawn(ACTOR_KOOPAFIRE, FIRE_PARAM(5, 0), this->mPos, &this->mPrevAngle, this->mAreaId,
                    -1);
}

// @symbol _ZN11daKpaFire_c19func_ov060_021172e0Ev
/* End of a variant 6/7 fire, called once its mFireScale has burnt down to 0 or
 * below: plays the one-shot particle PARTICLE_END_PUFF at the fire's position
 * and destroys the fire. Then, with a 2 in 10 chance (random >> 16, % 10 < 2),
 * it leaves something behind:
 *   - if the nearest player's param1 is 3 (not decoded; daFPknBall_c and
 *     daJango_c make the same test), a daKpa_c exists and its mCapActorAlive
 *     is 0: it spawns a Mario-cap object (param = character << 8 | 0xb, the
 *     character drawn at random among those of the first three that are
 *     unlocked), sets the daKpa_c's mCapActorAlive to 1 if the spawn worked,
 *     and stops -- this branch returns whether or not the spawn worked;
 *   - otherwise it spawns a coin and zeroes the coin's three velocity words. */
void daKpaFire_c::func_ov060_021172e0() {
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(PARTICLE_END_PUFF, *(Fix12i *)&this->mPosX,
                                                   *(Fix12i *)&this->mPosY,
                                                   *(Fix12i *)&this->mPosZ);
    this->MarkForDestruction();

    if ((((u32)RandomIntInternal(&data_0209e650[0]) >> 0x10) % 10) >= 2) {
        return;
    }

    {
        Player *player = this->ClosestPlayer();
        daKpa_c* koopa;
        if (player != 0 && player->param1 == 3 &&
            (koopa = (daKpa_c*)dActor_c::FindWithActorID(ACTOR_KOOPA, 0)) != 0 &&
            koopa->mCapActorAlive == 0) {
            int chr;
            int idx;
            int mask = 0;
            for (chr = 0; chr < 3; chr++) {
                if (SaveData::IsCharacterUnlocked(chr) != 0) {
                    mask = (mask | (1 << chr)) & 0xff;
                }
            }
            do {
                idx = ((u32)RandomIntInternal(&data_0209e650[0]) >> 0x10) % 3;
            } while ((mask & (1 << idx)) == 0);
            {
                dActor_c *spawned = dActor_c::Spawn(ACTOR_OBJ_MARIO_CAP, (idx << 8) | 0xb,
                                                    this->mPos, &this->mAngle, this->mAreaId, -1);
                if (spawned != 0) {
                    koopa->mCapActorAlive = 1;
                }
            }
            return;
        }
    }

    {
        dActor_c *spawned = dActor_c::Spawn(ACTOR_COIN, 0, this->mPos, 0, this->mAreaId, -1);
        if (spawned != 0) {
            spawned->unk_0a4 = 0;
            spawned->mVertSpeed = 0;
            spawned->unk_0ac = 0;
        }
    }
}

// @symbol _ZN11daKpaFire_c19func_ov060_021172c8Ej
/* 1 if more than n Behavior frames have passed (mFrameCount > n), else 0. */
int daKpaFire_c::func_ov060_021172c8(unsigned int n) {
    return mFrameCount > n;
}

// @symbol _ZN11daKpaFire_c19func_ov060_0211722cEv
/* Variant 6 init: random heading (16 random bits); vertical speed 30.0 units
 * with a 2 in 10 chance, else 10.0; horizontal speed 5.0 (0x5000); gravity
 * -1.0 (-0x1000) per frame per frame; mFireScale 1.0 plus a random 0..0xfff
 * (so from 1.0 to just under 2.0); shadow multiplier 0x10. */
void daKpaFire_c::func_ov060_0211722c() {
    unsigned int r;
    this->mPrevAngleY = (short)((unsigned int)RandomIntInternal(&data_0209e650[0]) >> 0x10);
    r = (unsigned int)RandomIntInternal(&data_0209e650[0]) >> 0x10;
    if (r % 10 < 2)
        this->mVertSpeed = 0x1e000;
    else
        this->mVertSpeed = 0xa000;
    this->mHorzSpeed = 0x5000;
    this->mVertAccel = -0x1000;
    this->mFireScale =
        (int)(((unsigned int)RandomIntInternal(&data_0209e650[0]) >> 16) & 0xfff) + 0x1000;
    this->mShadowRadiusMul = 0x10;
}

// @symbol _ZN11daKpaFire_c19func_ov060_021171e8Ev
/* Variant 7 init: random heading, vertical speed 10.0 units (0xa000), no
 * horizontal speed, mFireScale 6.0 (0x6000), shadow multiplier 0x10. Gravity
 * is left at the InitResources default. */
void daKpaFire_c::func_ov060_021171e8() {
    unsigned int r = RandomIntInternal(data_0209e650);
    this->mPrevAngleY = r >> 0x10;
    this->mVertSpeed = 0xa000;
    this->mHorzSpeed = 0;
    this->mFireScale = 0x6000;
    this->mShadowRadiusMul = 0x10;
}

// @symbol _ZN11daKpaFire_c19func_ov060_0211712cEv
/* Sideways wobble, called by the variant 2 handler and the variant 6/7
 * handler. Forms a phase angle ((mTickCount + mPhaseOffset) & 0x3f) << 10 --
 * 64 steps of a full turn (0x10000) -- and nudges x and z by products of two
 * sine-table entries each, `<< 2` then `/ 0x1000`:
 *   x += table[2h]     * table[2p]
 *   z += table[2h + 1] * table[2p + 1]
 * with h = mPrevAngleY >> 4 (the heading, read unsigned) and p = phase >> 4. */
void daKpaFire_c::func_ov060_0211712c() {
    short phase = (short)(((this->mTickCount + this->mPhaseOffset) & 0x3f) << 10);
    int p = (unsigned short)phase >> 4;
    int *px = &this->mPosX;
    int *pz;
    *px = *px + ((data_02082214[((int)*(unsigned short *)&this->mPrevAngleY >> 4) * 2] *
                  data_02082214[p * 2])
                 << 2) /
                    0x1000;
    pz = &this->mPosZ;
    *pz = *pz + ((data_02082214[((int)*(unsigned short *)&this->mPrevAngleY >> 4) * 2 + 1] *
                  data_02082214[p * 2 + 1])
                 << 2) /
                    0x1000;
}

// @symbol _ZN11daKpaFire_c19func_ov060_02116f90Ev
/* Variant 6 and 7 Behavior: a flame that falls, lands, burns down and bursts.
 *
 * Every frame it moves (UpdatePos), clamps its vertical speed to at least
 * -4.0 (-0x4000), and destroys itself once its y is below 0.
 *
 * While mLanded is 0 (airborne): keeps the PARTICLE_FALLING particle at the
 * fire's position, 55 units up (0x37000); sets cylinder flags bit 0 (cylinder
 * disabled); runs the wobble. On the first frame the mesh collision reports the
 * ground it sets mLanded, picks mFireScale (6.0 for variant 7, else 6.0 plus
 * a random even number from 0 to 0x1ffe, so up to just under 8.0), zeroes the
 * horizontal speed, vertical speed and gravity, and forgets both particle
 * handles.
 *
 * Once mLanded is set: shows PARTICLE_LANDED_BURN scaled by mFireScale (y
 * offset 12 * scale), clears flags bit 0 (cylinder enabled), and once
 * mFrameCount exceeds mFireScale * 10 / 0x1000 + 5 it shrinks mFireScale by
 * 0x266 (about 0.15) per frame; at 0 or below it ends in func_ov060_021172e0. */
void daKpaFire_c::func_ov060_02116f90() {
    this->UpdatePos(0);
    if (this->mVertSpeed < -0x4000)
        this->mVertSpeed = -0x4000;

    if (this->mLanded == 0) {
        this->mParticleHandle =
            _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
                this->mParticleHandle, PARTICLE_FALLING, this->mPosX, this->mPosY + 0x37000,
                this->mPosZ, 0);
        this->mdCcAc_c.flags |= 1;
        func_ov060_0211712c();
        if (this->mWithMeshClsn.IsOnGround()) {
            this->mLanded++;
            if (this->mVariant == 7) {
                this->mFireScale = 0x6000;
            } else {
                this->mFireScale =
                    ((u32)RandomIntInternal(&data_0209e650[0]) >> 16 & 0xfff) * 2 + 0x6000;
            }
            this->mHorzSpeed = 0;
            this->mVertSpeed = 0;
            this->mVertAccel = 0;
            this->mParticleHandle2 = 0;
            this->mParticleHandle = this->mParticleHandle2;
        }
    } else {
        func_ov060_02116518(PARTICLE_LANDED_BURN, 1, this->mFireScale * 0xc);
        this->mdCcAc_c.flags &= ~1;
        if (this->mFrameCount > this->mFireScale * 0xa / 0x1000 + 5) {
            this->mFireScale -= 0x266;
            if (this->mFireScale <= 0)
                func_ov060_021172e0();
        }
    }
    if (this->mPosY < 0)
        this->MarkForDestruction();
}

// @symbol _ZN11daKpaFire_c19func_ov060_02116f74Ev
/* Variant 5 init: horizontal speed is stored as the raw integer 30 (not
 * 30.0 units; variant 5's handler multiplies it straight into the sine
 * table values), mFireScale 2.0 (8192 = 0x2000), shadow multiplier 16. */
void daKpaFire_c::func_ov060_02116f74() {
    this->mHorzSpeed = 30;
    this->mFireScale = 8192;
    this->mShadowRadiusMul = 16;
}

// @symbol _ZN11daKpaFire_c19func_ov060_02116d78Ev
/* Variant 5 Behavior: a spray flame that is thrown along a pitch and heading
 * (the words spawners leave in 0x92 and 0x94, which dActor_c names
 * mPrevAngleX/Y). It never calls UpdatePos; it moves itself by the three
 * velocity words it writes (the mesh collision step in Behavior is separate).
 *
 * Each frame: mFireScale grows by 0x255 (about 0.146) while below 5.0 (0x5000),
 * and the pitch word (mPrevAngleX, signed) loses 0x200 while above 0x800.
 * The three velocity words come from the speed (mHorzSpeed), the pitch and the
 * heading through the sine table: unk_0a4 = x, mVertSpeed = y (-speed times
 * the pitch's table[2p]), unk_0ac = z; the x and z values are divided by 896
 * (0x380, not decoded). Position moves by those velocities. Shows
 * PARTICLE_SCALED_9A scaled by mFireScale (y offset 12 * scale). When the mesh
 * collision reports ground it spawns a variant 6 fire with a 16 in 17 chance
 * (random >> 16, % 17 != 0) and destroys this one; after 60 frames
 * (mFrameCount >= 0x3c) it destroys itself as well. */
void daKpaFire_c::func_ov060_02116d78() {
    if (this->mFireScale < 0x5000)
        this->mFireScale += 0x255;
    if (this->mPrevAngleX > 0x800)
        this->mPrevAngleX -= 0x200;

    {
        int i0 = (*(unsigned short *)&this->mPrevAngleX) >> 4;
        int i1 = (*(unsigned short *)&this->mPrevAngleY) >> 4;
        i0 = i0 * 2;
        i1 = i1 * 2 + 1;
        int s0 = data_02082214[i0];
        int speed = this->mHorzSpeed;
        int s1 = data_02082214[i1];
        this->unk_0a4 = (s1 * (speed * s0)) / 896;
    }
    {
        int speed = this->mHorzSpeed;
        int s0 = data_02082214[((*(unsigned short *)&this->mPrevAngleX) >> 4) * 2];
        this->mVertSpeed = (-speed) * s0;
    }
    {
        int s0 = data_02082214[((*(unsigned short *)&this->mPrevAngleX) >> 4) * 2];
        int s1 = data_02082214[((*(unsigned short *)&this->mPrevAngleY) >> 4) * 2];
        int speed = this->mHorzSpeed;
        this->unk_0ac = (s1 * ((-speed) * s0)) / 896;
    }
    this->mPosX += this->unk_0a4;
    this->mPosY += this->mVertSpeed;
    this->mPosZ += this->unk_0ac;

    func_ov060_02116518(PARTICLE_SCALED_9A, 1, this->mFireScale * 0xc);
    if (this->mWithMeshClsn.IsOnGround() != 0) {
        int rnd = RandomIntInternal(&data_0209e650[0]);
        unsigned int roll = (unsigned int)rnd >> 16;
        if (roll % 17 != 0)
            dActor_c::Spawn(ACTOR_KOOPAFIRE, FIRE_PARAM(6, 0), this->mPos, 0, this->mAreaId, -1);
        this->MarkForDestruction();
    }
    if (this->mFrameCount < 0x3c)
        return;
    this->MarkForDestruction();
}

// @symbol _ZN11daKpaFire_c19func_ov060_02116c68Ev
/* Variant 2 init: random heading (16 random bits) and phase; horizontal speed
 * random (r % 10 in 0.5-unit steps, up to 4.5, when mSubVariant is non-zero;
 * in 3.5-unit steps (0x3800), up to 31.5, when it is 0); vertical speed random 0..9
 * whole units; gravity -1.0 (-0x1000); terminal velocity from
 * data_ov060_0211934c[mSubVariant] (-8.0, -6.0, -6.0 for sub-variants 0..2);
 * mFireScale 2.0 (0x2000); shadow multiplier 0x10. */
void daKpaFire_c::func_ov060_02116c68() {
    unsigned int r;
    this->mPrevAngleY = (short)(((unsigned int)RandomIntInternal(&data_0209e650[0])) >> 0x10);
    if (this->mSubVariant != 0) {
        r = ((unsigned int)RandomIntInternal(&data_0209e650[0])) >> 0x10;
        this->mHorzSpeed = (int)((r % 10) << 0xc) >> 1;
    } else {
        r = ((unsigned int)RandomIntInternal(&data_0209e650[0])) >> 0x10;
        this->mHorzSpeed = (int)((r % 10) * 0x7000) >> 1;
    }

    r = ((unsigned int)RandomIntInternal(&data_0209e650[0])) >> 0x10;
    this->mVertSpeed = (r % 10) << 0xc;
    this->mVertAccel = -0x1000;

    this->mPhaseOffset = (short)(((unsigned int)RandomIntInternal(&data_0209e650[0])) >> 0x10);
    this->mTerminalVelocity = data_ov060_0211934c[this->mSubVariant];
    this->mFireScale = 0x2000;
    this->mShadowRadiusMul = 0x10;
}

// @symbol _ZN11daKpaFire_c19func_ov060_02116b68Ev
/* Variant 2 Behavior: a flame in flight. Shows the particle id that
 * data_ov060_02119358[mSubVariant] picks (0xa2, 0xa4, 0xa4 for sub-variants 0..2)
 * 70 units up (0x46000), unscaled; wobbles; destroys itself once mFrameCount
 * exceeds 450 (0x1c2); moves (UpdatePos). When the mesh collision reports
 * ground it spawns a variant 7 fire (mSubVariant 0) or a variant 3 fire
 * (otherwise) at its position and destroys itself; while still airborne it
 * destroys itself once its y is below 0. */
void daKpaFire_c::func_ov060_02116b68() {
    func_ov060_02116518(data_ov060_02119358[this->mSubVariant], 0, 0x46000);
    func_ov060_0211712c();
    if (this->func_ov060_021172c8(0x1c2))
        this->MarkForDestruction();
    this->UpdatePos(0);
    if (this->mWithMeshClsn.IsOnGround()) {
        if (this->mSubVariant == 0)
            dActor_c::Spawn(ACTOR_KOOPAFIRE, FIRE_PARAM(7, 0), this->mPos, 0, this->mAreaId, -1);
        else
            dActor_c::Spawn(ACTOR_KOOPAFIRE, FIRE_PARAM(3, 0), this->mPos, 0, this->mAreaId, -1);
        this->MarkForDestruction();
    } else {
        if (this->mPosY >= 0)
            return;
        this->MarkForDestruction();
    }
}

// @symbol _ZN11daKpaFire_c19func_ov060_02116b18Ev
/* Variant 1 init: vertical speed 7.0 units (0x7000), horizontal speed 17.5
 * (0x11800), gravity +1.0 (0x1000, upward -- unlike the InitResources
 * default), mFireScale 2.0, random phase, shadow multiplier 0x10. */
void daKpaFire_c::func_ov060_02116b18() {
    this->mVertSpeed = 0x7000;
    this->mHorzSpeed = 0x11800;
    this->mFireScale = 0x2000;
    this->mVertAccel = 0x1000;
    this->mPhaseOffset = (unsigned int)RandomIntInternal(data_0209e650) >> 0x10;
    this->mShadowRadiusMul = 0x10;
}

// @symbol _ZN11daKpaFire_c19func_ov060_021169f8Ev
/* Variant 1 Behavior: grows, then splits and vanishes. mFireScale grows by
 * 0x200 per frame while below 5.0 (0x5000); it moves (UpdatePos) and shows
 * PARTICLE_SCALED_9A at a 120 unit (0x78000) y offset. Once mFrameCount is
 * above 20 (0x14): with mSubVariant 0 it spawns three variant 2 fires
 * (param 2), each with mFireScale set to 5.0; otherwise two, with sub-variants 1
 * and 2 (params 0x12 and 0x22), each with mFireScale 8.0 (0x8000). The
 * children get this fire's pitch/heading words as their spawn rotation and
 * their mFireScale is set after the spawn returns. Then it destroys itself.
 * Before frame 21 it only grows and moves. */
void daKpaFire_c::func_ov060_021169f8() {
    int i;
    daKpaFire_c* child;
    if (this->mFireScale < 0x5000) {
        this->mFireScale += 0x200;
    }
    this->UpdatePos(0);
    func_ov060_02116518(PARTICLE_SCALED_9A, 1, 0x78000);
    if (this->mFrameCount <= 0x14)
        return;
    if (this->mSubVariant == 0) {
        for (i = 0; i < 3; i++) {
            child = (daKpaFire_c *)dActor_c::Spawn(ACTOR_KOOPAFIRE, FIRE_PARAM(2, 0), this->mPos,
                                                   &this->mPrevAngle, this->mAreaId, -1);
            child->mFireScale = 0x5000;
        }
    } else {
        child = (daKpaFire_c *)dActor_c::Spawn(ACTOR_KOOPAFIRE, FIRE_PARAM(2, 1), this->mPos,
                                               &this->mPrevAngle, this->mAreaId, -1);
        child->mFireScale = 0x8000;
        child = (daKpaFire_c *)dActor_c::Spawn(ACTOR_KOOPAFIRE, FIRE_PARAM(2, 2), this->mPos,
                                               &this->mPrevAngle, this->mAreaId, -1);
        child->mFireScale = 0x8000;
    }
    this->MarkForDestruction();
}

// @symbol _ZN11daKpaFire_c19func_ov060_021169b0Ev
/* Variant 4 init. Vertical speed 30.0, horizontal speed 15.0, random
 * phase, SetLimMovFlag on the mesh collision, shadow multiplier 0x10. */
void daKpaFire_c::func_ov060_021169b0() {
    this->mVertSpeed = 0x1e000;
    this->mHorzSpeed = 0xf000;
    this->mPhaseOffset = (unsigned int)RandomIntInternal(&data_0209e650[0]) >> 16;
    this->mWithMeshClsn.SetLimMovFlag();
    this->mShadowRadiusMul = 0x10;
}

// @symbol _ZN11daKpaFire_c19func_ov060_021168c4Ev
/* Variant 4 Behavior: a bouncing flame that is called off near Bowser.
 *
 * On its first frame it finds the daKpa_c by actor id and remembers its
 * uniqueID in mKpaUniqueID; later frames look it up by that id. Whenever the
 * mesh collision reports it has just hit the ground its vertical speed is
 * set to 30.0 units (0x1e000) -- the bounce. Moves (UpdatePos), shows
 * PARTICLE_VARIANT_4 at a 50 unit (0x32000) y offset, destroys itself once
 * mFrameCount exceeds 150 (0x96). It also destroys itself when Bowser is
 * still there, his hold state is idle, and he is within 150 units.
 */
void daKpaFire_c::func_ov060_021168c4() {
    daKpa_c* koopa;
    if (this->mFrameCount == 0) {
        dActor_c* found = dActor_c::FindWithActorID(ACTOR_KOOPA, 0);
        koopa = (daKpa_c*)found;
        this->mKpaUniqueID = koopa->uniqueID;
    } else {
        koopa = (daKpa_c *)dActor_c::FindWithID(this->mKpaUniqueID);
    }
    if (this->mWithMeshClsn.JustHitGround() != 0) {
        this->mVertSpeed = 0x1e000;
    }
    this->UpdatePos(0);
    func_ov060_02116518(PARTICLE_VARIANT_4, 0, 0x32000);
    if (this->func_ov060_021172c8(0x96) != 0) {
        this->MarkForDestruction();
    }
    if (koopa == 0) return;
    if (koopa->mHoldState != 0) return;
    if (Vec3_HorzDist(&this->mPos, &koopa->mPos) >= 0x96000)
        return;
    this->MarkForDestruction();
}

// @symbol _ZN11daKpaFire_c19func_ov060_021167ecEv
/* Variant 3 Behavior: acts on its first frame only (mFrameCount 0; after that
 * it does nothing, and it also does nothing if there is no player). Aims at
 * the nearest player (heading = Vec3_HorzAngle to the player), sets mFireScale
 * to 5.0 (0x5000), spawns three variant 4 fires at its position, each with
 * heading = that heading + 0, 0x5555, 0xaaaa (a third of a turn apart, 0x10000
 * being a full turn) and mFireScale copied from this one, then destroys itself.
 * The children are spawned with no rotation and have their pitch/heading words
 * written afterwards. */
void daKpaFire_c::func_ov060_021167ec() {
    dActor_c* player;
    if (this->mFrameCount != 0)
        return;
    player = this->ClosestPlayer();
    if (player == 0) return;
    this->mPrevAngleY = Vec3_HorzAngle(&this->mPos, &player->mPos);
    this->mFireScale = 0x5000;
    {
        int i = 0;
        int ang = 0;
        do {
            daKpaFire_c *child = (daKpaFire_c *)dActor_c::Spawn(ACTOR_KOOPAFIRE, FIRE_PARAM(4, 0),
                                                                this->mPos, 0, this->mAreaId, -1);
            short heading = this->mPrevAngleY + ang;
            child->mPrevAngleX = 0;
            child->mPrevAngleY = heading;
            child->mPrevAngleZ = 0;
            child->mFireScale = this->mFireScale;
            i++;
            ang += 0x5555;
        } while (i < 3);
    }
    this->MarkForDestruction();
}

// @symbol _ZN11daKpaFire_c19func_ov060_021167c8Ev
/* Init shared by variants 0 and 3: cylinder flags bit 0 set (cylinder
 * disabled), mFireScale 2.0 (0x2000), shadow multiplier 0x10. */
void daKpaFire_c::func_ov060_021167c8() {
    this->mdCcAc_c.flags |= 1;
    this->mFireScale = 0x2000;
    this->mShadowRadiusMul = 0x10;
}

// @symbol func_ov060_02116740
/* Burn the player the fire's cylinder touched. Reads mdCcAc_c.otherOwner (the
 * dCc_c field Clear zeroes; here it is used as an actor uniqueID, 0 for none)
 * and looks that actor up; if it is the Player (actor id 0xbf = ACTOR_PLAYER)
 * and neither Player::mIsMetal nor Player::mIsVanish is set it calls
 * Player::Burn.
 * Keeps the char * parameter decl_common.h declares. */
extern "C" {
void func_ov060_02116740(char* c)
{
    unsigned int id = ((daKpaFire_c *)c)->mdCcAc_c.otherOwner;
    if (id == 0) return;
    Player* player = (Player *)dActor_c::FindWithID(id);
    if (player == 0) return;
    /* Measured: folding this temp into `if (player->actorID != ACTOR_PLAYER)`
       makes the function 12 bytes shorter than the ROM size, 0x88. */
    int isPlayer = (int)(player->actorID == ACTOR_PLAYER);
    if (isPlayer == 0) return;
    if (player->mIsMetal != 0) return;
    if (player->mIsVanish != 0) return;
    player->Burn();
}
}

// @symbol _ZN11daKpaFire_c19func_ov060_02116518Ejii
/* Particle helper. Keeps two particles alive at the fire's position, y raised
 * by yOffset (Fix12): `kind` in mParticleHandle and `kind + 1` in
 * mParticleHandle2 (each handle is passed back in, so a live particle is
 * presumably refreshed rather than duplicated). Kinds 0xa2 and 0xa4 start the
 * first one through the NewUnkCallback818 form of Particle::System::New; the rest use
 * the plain New.
 *
 * With resize non-zero each live particle is resized. callbackScale becomes
 * mFireScale as a halfword (0x7fff once mFireScale reaches 0x8000). Kind
 * 0x9a also writes the unnamed word at +0x44 and callbackVelocity from
 * mFireScale * 0x2800 and mFireScale * 0xa66.
 */
void daKpaFire_c::func_ov060_02116518(u32 kind, int resize, int yOffset) {
    Particle::System *sys;

    if (kind == 0xa2 || kind == 0xa4) {
        this->mParticleHandle =
            _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
                this->mParticleHandle, kind, this->mPosX, this->mPosY + yOffset, this->mPosZ, 0);
    } else {
        this->mParticleHandle =
            _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                this->mParticleHandle, kind, this->mPosX, this->mPosY + yOffset, this->mPosZ, 0, 0);
    }

    if (this->mParticleHandle != 0 && resize != 0) {
        sys = Particle::System::FromUniqueID(this->mParticleHandle);
        if (this->mFireScale >= 0x8000) {
            sys->callbackScale = 0x7fff;
        } else {
            sys->callbackScale = (short)this->mFireScale;
        }

        if (kind == PARTICLE_SCALED_9A) {
            *(int *)((char *)sys + 0x44) = FX_MUL((this->mFireScale), 0x2800);
            sys->callbackVelocity = (short)FX_MUL((this->mFireScale), 0xa66);
        }
    }

    this->mParticleHandle2 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        this->mParticleHandle2, kind + 1, this->mPosX, this->mPosY + yOffset, this->mPosZ, 0, 0);
    if (this->mParticleHandle2 == 0)
        return;
    if (resize == 0)
        return;

    sys = Particle::System::FromUniqueID(this->mParticleHandle2);
    if (this->mFireScale >= 0x8000) {
        sys->callbackScale = 0x7fff;
    } else {
        sys->callbackScale = (short)this->mFireScale;
    }

    if (kind != PARTICLE_SCALED_9A)
        return;

    *(int *)((char *)sys + 0x44) = FX_MUL((this->mFireScale), 0x2800);
    sys->callbackVelocity = (short)FX_MUL((this->mFireScale), 0xa66);
}

/* ---- the static initializer's objects. __sinit_d_a_kpa_fire.cpp, emitted from
 * these definitions, copies the 16 .data PMF descriptors at 0x0211a734..
 * 0x0211a7ac into the two 8-slot .bss tables. A member-pointer initializer is
 * not static here, so mwccarm emits a runtime copy for each slot; the
 * descriptors keep their ROM bytes. First word is the handler, second the
 * pointer-adjustor (0). The descriptor order and the copy order are the ROM's
 * own. The handler named beside each descriptor is its ov060 relocs.txt
 * entry. */
extern PMF data_ov060_0211a734;   /* func_ov060_021169b0 (variant 4 init) */
extern PMF data_ov060_0211a73c;   /* func_ov060_02116c68 (variant 2 init) */
extern PMF data_ov060_0211a744;   /* func_ov060_02116f90 (variant 6 Behavior) */
extern PMF data_ov060_0211a74c;   /* func_ov060_021167c8 (variant 3 init) */
extern PMF data_ov060_0211a754;   /* func_ov060_02116b18 (variant 1 init) */
extern PMF data_ov060_0211a75c;   /* func_ov060_021167c8 (variant 0 init) */
extern PMF data_ov060_0211a764;   /* func_ov060_021168c4 (variant 4 Behavior) */
extern PMF data_ov060_0211a76c;   /* func_ov060_02116b68 (variant 2 Behavior) */
extern PMF data_ov060_0211a774;   /* func_ov060_02116d78 (variant 5 Behavior) */
extern PMF data_ov060_0211a77c;   /* func_ov060_021167ec (variant 3 Behavior) */
extern PMF data_ov060_0211a784;   /* func_ov060_02116f90 (variant 7 Behavior) */
extern PMF data_ov060_0211a78c;   /* func_ov060_021169f8 (variant 1 Behavior) */
extern PMF data_ov060_0211a794;   /* func_ov060_0211747c (variant 0 Behavior) */
extern PMF data_ov060_0211a79c;   /* func_ov060_021171e8 (variant 7 init) */
extern PMF data_ov060_0211a7a4;   /* func_ov060_0211722c (variant 6 init) */
extern PMF data_ov060_0211a7ac;   /* func_ov060_02116f74 (variant 5 init) */

// @symbol _ZN11daKpaFire_cD1Ev
// @symbol _ZN11daKpaFire_cD0Ev
/* One definition emits D1, then D0, then a D2 the cartridge does not keep.
 * The PMF declarations above stay between the last member function and the
 * pragma: with a function body directly before it, the pragma takes effect
 * early and mwccarm emits every deferred function ahead of the destructor,
 * so D1/D0 no longer come first in .text as they do in the ROM. */
#pragma defer_codegen off
daKpaFire_c::~daKpaFire_c() {}
#pragma defer_codegen on

/* Behavior handler table (mVariant 0..7); one Entry{PMF} per slot. */
extern "C" Entry data_ov060_0211afb4[8] = {
    {data_ov060_0211a794}, {data_ov060_0211a78c}, {data_ov060_0211a76c}, {data_ov060_0211a77c},
    {data_ov060_0211a764}, {data_ov060_0211a774}, {data_ov060_0211a744}, {data_ov060_0211a784}
};
/* InitResources handler table (mVariant 0..7); one PMF per slot. */
extern "C" PMF data_ov060_0211af74[8] = {
    data_ov060_0211a75c, data_ov060_0211a754, data_ov060_0211a73c, data_ov060_0211a74c,
    data_ov060_0211a734, data_ov060_0211a7ac, data_ov060_0211a7a4, data_ov060_0211a79c
};
