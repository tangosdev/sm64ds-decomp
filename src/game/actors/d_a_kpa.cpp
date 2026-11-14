//cpp
/* daKpa_c + daKpaTail_c -- Bowser and his tail, the ov060 boss-fight actors.
 * ROM span 0x02111900..0x02116484, 81 functions: both destructor pairs, the
 * fight helpers, both classes' resource/load/behavior/render methods, both
 * InitResources, and the two registry factories daKpaTail_c_classInit and
 * daKpa_c_classInit (plain C++ definitions, emitted as
 * `_Z21daKpaTail_c_classInitv` and `_Z17daKpa_c_classInitv`).
 *
 * _ZTS7daKpa_c at 0x0211a5c0, _ZTI7daKpa_c at 0x0211a5cc, _ZTV7daKpa_c at
 * 0x0211a6b8; _ZTS11daKpaTail_c at 0x0211a5e4, _ZTI11daKpaTail_c at
 * 0x0211a5d8, _ZTV11daKpaTail_c at 0x0211a634. The type-name bytes are what
 * renamed the tree's old Bowser / BowserTail classes. Both destructors are
 * the key functions, so the TU emits the two RTTI triples and both vtables;
 * those emitted copies are deadstripped and the cartridge copies above own
 * the addresses. D1/D0 at 0x02111900/0x02111950 (daKpa_c) and
 * 0x021119b4/0x021119e4 (daKpaTail_c); the cartridge carries no D2 for
 * either class.
 *
 * Codegen is deferred, so .text comes out in reverse source order. The two
 * destructor bodies are the eager window at the top: under `defer_codegen off`
 * each emits its D1,D0 pair at the definition, and they must stay first or the
 * second class's pair rides the deferred flush to the end of the object.
 *
 * In comments a Fix12 value reads 0x1000 = 1.0 and an angle 0x10000 = a full
 * turn.
 *
 * Leftovers:
 *  - Measured spellings: the mOpacity += / -= 0x14 form; the angle-copy
 *    pointer in func_ov060_02115d68; the overwritten first pos[] stores in
 *    RunIntroTalk; the volatile walk pointer in ActTiltLavaPlatform; the
 *    gotos in func_ov060_02115c1c; the data_ov060_02119294/6/8 + offset
 *    reads; and the `obj` copy of `this` in Bowser_IsAnimAtLastFrame.
 *  - Address-named callees with a known body but no header: func_020092c4 eases
 *    a camera vector toward a target (d_camera.cpp); func_0200fa04 spawns the
 *    boss landing dust (d_actor.cpp); func_02038408 is the
 *    UpdateContinuousExceptSlot0 veneer; func_02011d50 / func_02011cfc are the
 *    Sound.cpp init/cleanup veneers; func_ov002_020c56f0 is a daPly_c state
 *    transition. The unnamed arguments of daPly_c::Hurt and
 *    Sound::ChangeMusicVolume and the KOOPAFIRE spawn-parameter bits are still
 *    numbers where no name is proven.
 *  - Animation indices stay numbers (only the idle pose is named);
 *    Bowser_Anim lists where each one is used.
 *  - unk_420 and unk_422 are written here and never read, so they keep
 *    placeholder names.
 *  - BgchInitAt_ and the three d_a_kpa*Hold prototypes are unused, but their
 *    named parameters pin mwcc's per-TU @NNN counter, which the manifest's
 *    compiler temporaries are keyed to; deleting them renumbers those
 *    temporaries unless the manifest is re-keyed.
 */

/* common.h must precede the class headers: both Matrix4x3 spellings are real,
 * Matrix4x3_ApplyInPlace* wants `m[i]` while the model headers want `.r`/`.t`,
 * and whichever is seen first wins for the TU. */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum daKpa_c_AssetId {
    ASSET_KOOPA_DAMAGE_BCA = 0x035f,
    ASSET_KOOPA_DAMAGE2_ED_BCA = 0x0360,
    ASSET_KOOPA_DAMAGE2_ED_BTP = 0x0361,
    ASSET_KOOPA_DAMAGE2_ST_BCA = 0x0362,
    ASSET_KOOPA_DAMAGE2_ST_BTP = 0x0363,
    ASSET_KOOPA_DEAD_BCA = 0x0364,
    ASSET_KOOPA_DEAD_BTP = 0x0365,
    ASSET_KOOPA_DEAD_ED_BCA = 0x0366,
    ASSET_KOOPA_DEAD_ST_BCA = 0x0367,
    ASSET_KOOPA_DUSH_BCA = 0x0368,
    ASSET_KOOPA_DUSH_ED_BCA = 0x0369,
    ASSET_KOOPA_DUSH_ST_BCA = 0x036a,
    ASSET_KOOPA_FIRE_BCA = 0x036b,
    ASSET_KOOPA_FIRE2_BCA = 0x036c,
    ASSET_KOOPA_FIRE3_BCA = 0x036d,
    ASSET_KOOPA_FIRE3_ED_BCA = 0x036e,
    ASSET_KOOPA_FIRE3_ST_BCA = 0x036f,
    ASSET_KOOPA_FIRE_ED_BCA = 0x0370,
    ASSET_KOOPA_FIRE_ST_BCA = 0x0371,
    ASSET_KOOPA_HOLD_BCA = 0x0372,
    ASSET_KOOPA_HOLD_BTP = 0x0373,
    ASSET_KOOPA_HOLD_ST_BCA = 0x0374,
    ASSET_KOOPA_JUMP1_BCA = 0x0375,
    ASSET_KOOPA_JUMP2_BCA = 0x0376,
    ASSET_KOOPA_MODEL_BMD = 0x0377,
    ASSET_KOOPA_STAND_UP_BCA = 0x0378,
    ASSET_KOOPA_STRETCH_BCA = 0x0379,
    ASSET_KOOPA_THROW_BCA = 0x037a,
    ASSET_KOOPA_THROW_BTP = 0x037b,
    ASSET_KOOPA_VEXING_BCA = 0x037c,
    ASSET_KOOPA_WAIT_BTP = 0x037d,
    ASSET_KOOPA_WAIT1_BCA = 0x037e,
    ASSET_KOOPA_WALK_BCA = 0x037f,
    ASSET_KOOPA_WALK_ED_BCA = 0x0380,
    ASSET_KOOPA_WALK_ST_BCA = 0x0381
};

#include "common.h"
#include "daKpa_c.h"
#include "daKpaTail_c.h"
#include "daKpaFire_c.h"
#include "daKpa2Bg_c.h"
#include "daKirai_c.h"
#include "daPly_c.h"
#include "daObjKey_c.h"
#include "types.h"
#include "decl_common.h"
#include "dCamera_c.h"
#include "dBgCh_Gnd.h"
#include "dActor_c.h"
#include "dBgCh_Actr.h"
#include "SharedFilePtr.h"
#include "dExtAnmTexPat_c.h"
#include "dExtFrameCtrl_c.h"
#include "Sound.h"
#include "Message.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "math/Approach.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "LevelRng.h"
#include "nitro/fx/fx_cp.h"
#include "math/MtxStack.h"

/* Names for the numbers this file uses. A name describes what the value does
 * at its use sites here; it is not a recovered symbol. */

/* daKpa_c::mState, the index into the state table at data_ov060_0211aed4. The
 * handler for each state is named on its own header below. A state with no
 * writer in this file (0x12) is reached only through the table. */
enum Bowser_State {
    BOWSER_STATE_IDLE = 0, /* stand, then pick the next state (variant decides how) */
    BOWSER_STATE_THROWN =
        1, /* set when the tail grabs him and again when he is released; handler runs after release */
    BOWSER_STATE_RECOVER =
        2, /* entered 1000 units below the arena: hidden, then launched back up toward the centre */
    BOWSER_STATE_PLAY_ANIM_0F = 3, /* animation 0xf, then idle */
    BOWSER_STATE_DEFEATED = 4,     /* set when mHealth reaches 0; the multi-step defeat sequence */
    BOWSER_STATE_INTRO_WAIT = 5,  /* initial state; stands idle until daPly_c::StartTalk succeeds */
    BOWSER_STATE_INTRO = 6,       /* animations, then the intro message */
    BOWSER_STATE_CHARGE = 7,      /* run at the target (animations 0x18..0x1a) */
    BOWSER_STATE_FIRE_BREATH = 8, /* spawns KOOPAFIRE (actor 0x118) every 5th frame */
    BOWSER_STATE_FIREBALLS = 9,   /* mFireballShots (1..3) fireballs */
    BOWSER_STATE_PLAY_ANIM_1B =
        0xa, /* animation 0x1b, then BOWSER_STATE_TURN_IN_PLACE; entered when a charge leaves the floor */
    BOWSER_STATE_TURN_IN_PLACE = 0xb, /* mAngleY += 0x200 per frame until mTimer reaches 0x3e */
    BOWSER_STATE_HURT_HOP =
        0xc, /* knocked back; set after a spike bomb hit that leaves mHealth above 0 */
    BOWSER_STATE_JUMP =
        0xd, /* jump and land; the landing effect SpawnShockwave shakes the ground (variant 2) */
    BOWSER_STATE_TURN_TO_TARGET =
        0xe, /* turn mAngleY toward mAngleToTarget while animations 0x13, 0x11, 0x12 play */
    BOWSER_STATE_ANIM_CHAIN = 0xf,   /* animations 8, 6, 7 in turn, then idle */
    BOWSER_STATE_VANISH_DASH = 0x10, /* fade out, dash invisibly at the target, fade in */
    BOWSER_STATE_HOP =
        0x11, /* hop forward at mHorzSpeed 25.0; the variant-0 idle pick chooses it when he faces a target 1500 or more units away */
    BOWSER_STATE_WAIT_ANIM_END = 0x12, /* wait for the current animation to end, then idle */
    BOWSER_STATE_ARENA_TILT =
        0x13 /* tilts the KOOPA2BG arena (actor 0xa6) for a few frames, see ActTiltLavaPlatform */
};

/* daKpa_c::mHoldState, the index into the table at data_ov060_0211aeb4. */
enum Bowser_HoldState {
    BOWSER_HOLD_NONE = 0,        /* normal: run the state table */
    BOWSER_HOLD_HELD = 1,        /* a daPly_c has grabbed the tail and is swinging Bowser by it */
    BOWSER_HOLD_RELEASE_NOW = 2, /* the tail's BOWSER_TAIL_RELEASED flag was set: let go at once */
    BOWSER_HOLD_RELEASE_AFTER_ANIM =
        3 /* the daPly_c no longer holds an object or the hold countdown ran out: let go once the animation ends */
};

/* daKpa_c::mCondFlags. The low byte is recomputed every frame by
 * BowserLoop; the two high bits are set and cleared by the handlers. */
enum Bowser_CondFlag {
    BOWSER_COND_FACING_TARGET = 0x2,  /* |mAngleY - mAngleToTarget| < 0x2000 (45 degrees) */
    BOWSER_COND_FACING_CENTER = 0x4,  /* |mAngleY - mAngleToCenter| < 0x3800 (about 79 degrees) */
    BOWSER_COND_TARGET_NEAR = 0x8,    /* mDistToTarget < 0x352000 (850 units) */
    BOWSER_COND_NEAR_CENTER = 0x10,   /* mDistToCenter < 0x3e8000 (1000 units) */
    BOWSER_COND_RECOVERING = 0x10000, /* set each frame of BOWSER_STATE_RECOVER; the jump handlers read it for variant 2 */
    BOWSER_COND_BREATHING = 0x20000   /* set each frame of the fire-breath handler; cleared when it ends, when he is held, or by the turn handler after five loops */
};

/* daKpaTail_c::mState: the index into the table at data_ov060_0211ae9c. */
enum BowserTail_State {
    BOWSER_TAIL_FREE = 0, /* waiting for a daPly_c to grab it */
    BOWSER_TAIL_COOLDOWN =
        1,                  /* after a release: back to FREE once mTimer passes 0x1e (30 frames) */
    BOWSER_TAIL_HOLDING = 2 /* a daPly_c is holding it (mHeldPlayer) */
};

/* dActor_c::mFlags bit daPly_c sets on the actor it releases
 * (d_a_ply_methods.cpp func_ov002_020daa74); the tail tests it to release
 * Bowser at once. */
enum BowserTail_Flag { BOWSER_TAIL_RELEASED = 0x400 };

/* Actor IDs, from symbols/actor_debug_names.tsv. */
enum Bowser_ActorID {
    BOWSER_ACTOR_KOOPA2BG = 0xa6,
    BOWSER_ACTOR_PLAYER = 0xbf,
    BOWSER_ACTOR_OBJ_MARIO_CAP = 0x10d,
    BOWSER_ACTOR_KOOPATAIL = 0x116,
    BOWSER_ACTOR_KOOPAFIRE = 0x118,
    BOWSER_ACTOR_FIRERING = 0x119,
    BOWSER_ACTOR_OBJ_KEY = 0x11a,
    BOWSER_ACTOR_LAST_STAR = 0x11b,
    BOWSER_ACTOR_KIRAI = 0x11c          /* the spike bomb (daKirai_c) */
};

/* Sound IDs, named by where this file plays them. */
enum Bowser_Sound {
    BOWSER_SND_FOOTFALL = 0xb0, /* Sound::Play(3, ..) in the footfall window */
    BOWSER_SND_LEAP = 0xb1,     /* hop, jump, knockback, rise out of a fall */
    BOWSER_SND_GRABBED = 0xb2,  /* a daPly_c grabs the tail and starts swinging Bowser */
    BOWSER_SND_B5 =
        0xb5, /* when the fire breath starts its second animation, and at frame 8 of animation 8 in the animation chain */
    BOWSER_SND_LAND = 0xb6,        /* touching down after a jump */
    BOWSER_SND_TALK_START = 0xb7,  /* the intro message appears */
    BOWSER_SND_VANISH = 0xb9,      /* start of the vanish-and-dash fade */
    BOWSER_SND_SHRINK_LOOP = 0xba, /* PlayLong while he shrinks in the defeat sequence */
    BOWSER_SND_DEFEATED = 0xbb,    /* the key is spawned after the defeat sequence */
    BOWSER_SND_LAND_SOFT = 0xbd,   /* landing dust in BounceEffects */
    BOWSER_SND_FIREBALL = 0x122,   /* a fireball is spawned (ActSpitFireOntoFloor) */
    BOWSER_SND_FIRE_LOOP = 0x180   /* PlayLong while he breathes fire */
};

/* dCc_c::flags bits this file touches (see include/dCc_c.h). */
enum Bowser_CcFlag {
    BOWSER_CC_DISABLED = 0x1          /* set: the cylinder is switched off */
};

/* Animation indices for InitAnimation (a 0x1c-entry table). Only the idle
 * pose is named; every other index is passed as a number. Where each is used:
 *   0 defeat launch              1, 2 hurt hop              3 looped after the defeat landing
 *   4 defeat conversation ends   5 defeat landing           6, 7, 8 animation chain
 *   9 thrown flight / swing      0xa grabbed                0xb jump wind-up and recover rise
 *   0xc jump landing             0xd landing after the throw   0xe release
 *   0xf PLAY_ANIM_0F             0x10 idle                  0x11, 0x12, 0x13 turn / intro sequence
 *   0x14 fireballs               0x15, 0x16, 0x17 fire breath
 *   0x18, 0x19, 0x1a charge      0x1b PLAY_ANIM_1B */
enum Bowser_Anim {
    BOWSER_ANIM_IDLE = 0x10
};

/* The state tables: daKpa_c's hold and state tables at
 * data_ov060_0211aeb4 / 0211aed4 use daKpa_c member pointers; the tail's at
 * data_ov060_0211ae9c uses KpaTailState / daKpaTail_c member pointers. */
typedef void (daKpa_c::*PMF)();

/* One row of those tables: a single member-function pointer. */
struct PmfEnt { PMF pmf; };

/* The static-init file handles: the model file constructs through
 * func_02017acc and destroys through func_02017ab4, animation handles through
 * SharedFilePtr::Construct/SharedFilePtr_Destruct_Anim, and texture-sequence
 * handles through SharedFilePtr_Construct_TexSeq/SharedFilePtr_Destruct_TexSeq.
 * Each wrapper's ctor and dtor stay undefined here; the manifest aliases them
 * onto those ROM veneers. */
struct KpaModelFilePtr {
    union {
        SharedFilePtr base;
        struct {
            u16 fileID;
            u8 numRefs;
            u8 pad_;
            BMD_File *filePtr;
        } bmd;
    };
    KpaModelFilePtr(u32 fileID);
    ~KpaModelFilePtr();
};
struct KpaAnimationFileHandle : SharedFilePtr {
    KpaAnimationFileHandle(u32 fileID);
    ~KpaAnimationFileHandle();
};
struct KpaTexSequenceFilePtr {
    u16 fileID;
    u8 numRefs;
    u8 pad_;
    BTP_File *filePtr;
    KpaTexSequenceFilePtr(u32 fileID);
    ~KpaTexSequenceFilePtr();
};

/* Tail's own PMF: data_ov060_0211ae9c is a daKpaTail_c state table, unlike
 * PmfEnt's daKpa_c one. */
typedef void (daKpaTail_c::*KpaTailState)();
extern KpaTailState data_ov060_0211ae9c[];

/* C linkage for the external symbols this TU reaches without a header: the
 * data objects below have to stay unmangled, and the address-named callees are
 * C functions. Declarations only; no definition lives in this block. */
extern "C" {
/* The dCamera_c; Bowser publishes himself into its mFramedActor. */
extern dCamera_c *gCamera;
extern int Vec3_HorzDist(const struct Vector3*, const struct Vector3*);
extern short Vec3_HorzAngle(const struct Vector3*, const struct Vector3*);
/* Eases a camera vector toward a target; 1 when it arrives (d_camera.cpp). */
extern int func_020092c4(void*, void*, void*);
extern KpaAnimationFileHandle data_ov060_0211acd0;
extern unsigned char data_ov060_02119264[];
extern PmfEnt data_ov060_0211aeb4[];
/* Sound::Play(3, id, pos) (sound/Sound.cpp). The legacy shards passed a third
 * argument the callee does not take; dropped rather than widening the
 * signature. */
extern void func_02012694(unsigned int id, const Vector3 *v);
/* Veneer to dBgCh_Actr::UpdateContinuousExceptSlot0. */
void func_02038408(dBgCh_Actr *self);
extern PmfEnt data_ov060_0211aed4[];
extern unsigned char NumStars(void);
/* Unused; the named parameters pin this TU's @NNN counter (see the header). */
extern void *d_a_kpaNewHold(unsigned a, unsigned b, int x, int y, int z, void *v, void *cb);
extern void d_a_kpaChangeMusicVolumeHold(unsigned int a, int b);
extern KpaAnimationFileHandle data_ov060_0211ac88;
extern KpaAnimationFileHandle data_ov060_0211ac60;
extern int Vec3_HorzLen(const Vector3* v);
extern int RandomIntInternal(int* seed);
extern KpaAnimationFileHandle data_ov060_0211abe0;
extern KpaAnimationFileHandle data_ov060_0211acd8;
extern KpaAnimationFileHandle data_ov060_0211acf0;
extern KpaAnimationFileHandle data_ov060_0211ac20;
extern KpaAnimationFileHandle data_ov060_0211ac68;
extern KpaAnimationFileHandle data_ov060_0211ac70;
extern s16 data_ov060_0211a4e0[];
/* local extern: the Vector3 spelling math/CalcLib.h declares. */
extern void Vec3_Asr(Vector3 *d, Vector3 *s, int sh);
extern int *Vec3_LslInPlace(void *v, int sh);
extern Matrix4x3 data_020a0e68;
/* Unused; the named parameters pin this TU's @NNN counter (see the header). */
void d_a_kpaEarthquakeHold(void *self, struct Vector3 *v, int f);
/* Sound.cpp 0xc-byte cleanup veneer. */
extern void func_02011cfc(void);
/* One declaration per resource handle, typed from how InitResources and
 * CleanupResources use it:
 *
 *   data_ov060_021192dc / _0211927c  tables of handle pointers (the 0x1c
 *                                    animations / six texture sequences),
 *                                    indexed by i: LoadFile, then ->Release()
 *   data_ov089_02132c50              a model handle in ov089
 *   data_ov060_0211b208              a model handle (SharedFilePtr)
 *   data_ov060_0211ac78              Bowser's model handle (KpaModelFilePtr) */
extern SharedFilePtr *data_ov060_021192dc[];
extern SharedFilePtr *data_ov060_0211927c[];
extern SharedFilePtr data_ov089_02132c50;
/* daFRing_c defines this handle. decl_common.h no longer declares it. */
extern SharedFilePtr data_ov060_0211b208;
extern KpaModelFilePtr data_ov060_0211ac78;
extern KpaTexSequenceFilePtr data_ov060_0211ac28;
/* The other texture-pattern handles InitAnimation picks from. */
extern KpaTexSequenceFilePtr data_ov060_0211ac40;
extern KpaTexSequenceFilePtr data_ov060_0211acb8;
extern KpaTexSequenceFilePtr data_ov060_0211ac10;
extern KpaTexSequenceFilePtr data_ov060_0211abf0;
extern KpaTexSequenceFilePtr data_ov060_0211ac30;
/* Animation handles 0x11, 0x12, 0x18, 0x19, 0xf and 0xd, the ones the footfall
 * windows in UpdateFootfalls compare against. */
extern KpaAnimationFileHandle data_ov060_0211acb0;
extern KpaAnimationFileHandle data_ov060_0211ac00;
extern KpaAnimationFileHandle data_ov060_0211ace0;
extern KpaAnimationFileHandle data_ov060_0211aca0;
extern KpaAnimationFileHandle data_ov060_0211abf8;
extern KpaAnimationFileHandle data_ov060_0211ac90;
/* The arena-tilt table walked by ActTiltLavaPlatform: rows of three halfwords
 * (sign, scale, end frame); each column is read through its own symbol. */
extern short data_ov060_02119294[];
extern short data_ov060_02119296[];
extern short data_ov060_02119298[];
/* Unused; the named parameters pin this TU's @NNN counter (see the header). */
typedef void BgchInitAt_(void *self, void *act, int a, int b, void *d1, void *d2);

}

/* The two destructor bodies are defined first so their D1/D0 groups lead the
 * object: the eager window is the only one before the deferred queue fills. */
#pragma defer_codegen off

/* Empty body: the member/base teardown and the D1/D0 pair are compiler-
 * generated. */
#ifndef _MSC_VER
daKpa_c::~daKpa_c()
{
}
#endif

/* Empty body, as for Bowser's. */
#ifndef _MSC_VER
daKpaTail_c::~daKpaTail_c()
{
}
#endif

#pragma defer_codegen on

/* KOOPA registry factory. `new daKpa_c` is the sequence the old shard spelled
 * by hand: fBase_c::operator new(0x454), the dActor_c base constructor, the
 * vptr store, then the dExtAnmModel_c, dExtAnmTexPat_c, dBgCh_Actr,
 * dExtShadowModel_c and dCcAcPos_c member constructors. */
daKpa_c *daKpa_c_classInit(void) {
    return new daKpa_c;
}

/* KOOPATAIL registry factory: operator new(0x118), the dActor_c base
 * constructor, the vptr store and the dCcAc_c member constructor. */
daKpaTail_c *daKpaTail_c_classInit(void) {
    return new daKpaTail_c;
}

/*
Tail setup: a single cylinder collider (dCcAc_c::Init) with radius 0x32000 (50
units), height 0x50000 (80 units), flags 0x800000 and vulnFlags 0x1000, which is
the grab bit of dCc_c's hit-flag table (that table is a best-effort reading, see
include/dCc_c.h). Always returns 1. */
int daKpaTail_c::InitResources()
{
    mdCcAc_c.Init(this, 0x32000, 0x50000, 0x800000, 0x1000);
    return 1;
}

/* Bowser's setup, and with it the whole fight's resources. Load the model and
 * hand it to dExtModel_c::SetFile, load the 0x1c animations and the six
 * dExtAnmTexPat_c files, then two more dExtSimpleModel_c files; shadow
 * cylinder; idle animation (0x10) and its texture pattern; body cylinder
 * (radius 0x78000 = 120 units, height 0x118000 = 280, flags 0x200004); spawn
 * position into mHomePos; gravity -0x2000 (-2.0) and terminal velocity
 * -0x3c000 (-60.0); floor collider (0x50000 = 80 for both Fix12 arguments);
 * param1 (bits 0-1 = variant, bit 2 = jump-only); InitFightState for the rest;
 * then spawn KOOPAFIRE and KOOPATAIL at his position and exchange uniqueIDs.
 * Returns 0 only if the shadow cylinder fails, otherwise 1.
 *
 * This is the other half of CleanupResources: every handle loaded here is one
 * that cleanup releases, in the same order and with the same counts (one
 * single, a 0x1c table, a six table, then two singles, the last in ov089).
 * That pairing is why the tail's and the fire's CleanupResources are bare
 * `return 1`s. The loops stay loops because 0x1c and 6 are the counts the ROM
 * compares against. */
int daKpa_c::InitResources()
{
    int i;
    Vector3 pos;
    daKpaFire_c *fire;
    daKpaTail_c *tail;

    this->mAnmModel.SetFile(dExtSimpleModel_c::LoadFile(data_ov060_0211ac78.base), 1, 0x16);

    for (i = 0; i < 0x1c; i++)
        dExtFrameCtrl_c::LoadFile(*data_ov060_021192dc[i]);

    for (i = 0; i < 6; i++)
        dExtAnmTexPat_c::LoadFile(*data_ov060_0211927c[i]);

    dExtSimpleModel_c::LoadFile(data_ov060_0211b208);
    dExtSimpleModel_c::LoadFile(data_ov089_02132c50);

    if (this->mShadowModel.InitCylinder() == 0)
        return 0;

    InitAnimation(BOWSER_ANIM_IDLE, 0);

    dExtAnmTexPat_c::Prepare(*data_ov060_0211ac78.bmd.filePtr, *data_ov060_0211ac28.filePtr);

    this->mTextureSequence.SetFile(*data_ov060_0211ac28.filePtr, 0, 0x1000, 0);

    this->mTextureSequence.SetFlags(0x40000000);

    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    this->mdCcAcPos_c.Init(this, pos, 0x78000, 0x118000, 0x200004, 0);

    this->mHomePosX = this->mPosX;
    this->mHomePosY = this->mPosY;
    this->mHomePosZ = this->mPosZ;
    /* Fix12 gravity and terminal velocity. */
    this->mVertAccel = -0x2000;
    this->mTerminalVelocity = -0x3c000;
    this->mTargetPlayer = 0;
    this->mWithMeshClsn.Init(this, 0x50000, 0x50000, 0, 0);
    this->mWithMeshClsn.SetLimMovFlag();

    this->mState = BOWSER_STATE_IDLE;
    this->mVariantID = (char)(this->param1 & 3);
    this->mChooseJumpOnly = (char)((this->param1 >> 2) & 1);
    this->mTimer = 0;
    this->mStep = 0;
    this->mDropsShadow = 1;
    this->mBounceOnLand = 0;
    this->mScaleX = 0x1000;
    this->mScaleY = 0x1000;
    this->mScaleZ = 0x1000;
    this->mAnimSpeed = 0x1000;
    this->mSkipIdleRoll = 1;
    InitFightState();

    this->mTalkStep = 0;
    this->mCutsceneStep = 0;

    /* Spawn takes the three position words as one Vector3. dActor_c stores them as
     * mPosX/Y/Z at 0x5c, so the argument is the vector, not a pointer to the first
     * scalar. */
    fire =
        (daKpaFire_c *)dActor_c::Spawn(BOWSER_ACTOR_KOOPAFIRE, 0, this->mPos, 0, this->mAreaId, -1);
    fire->mKpaUniqueID = this->uniqueID;

    tail =
        (daKpaTail_c *)dActor_c::Spawn(BOWSER_ACTOR_KOOPATAIL, 0, this->mPos, 0, this->mAreaId, -1);
    this->mTailUniqueID = tail->uniqueID;
    tail->mBowserUniqueID = this->uniqueID;
    this->mVanishChance = 5;
    this->mCapActorAlive = 0;
    this->mParticleHandle = 0;
    this->mFootfallLatch = 0;
    this->mSoundHandle = 0;
    this->mSoundID = 0;
    func_02011d50();
    return 1;
}

/*
Per frame: find Bowser by mBowserUniqueID (return 1 if he is gone), put the tail
0x8c (140) units behind him -- his position plus 0x8c times the table entry for
mPrevAngleY + 0x8000, Y copied as is -- run func_ov060_02115b84, return 1.
   The copy goes through a held `const Vector3 &`; `pos = a->mPos` directly is
   a word short (0xb4 vs 0xb8). */
int daKpaTail_c::Behavior()
{
    dActor_c* a = dActor_c::FindWithID(mBowserUniqueID);
    if (!a) return 1;

    s16 ang = a->mPrevAngleY + 0x8000;
    Vector3 pos;
    const Vector3 &p = a->mPos;
    pos = p;
    mPosX = pos.x + FX_SinIdx((u16)ang >> 4) * 0x8c;
    mPosY = pos.y;
    mPosZ = pos.z + FX_CosIdx((u16)ang >> 4) * 0x8c;

    func_ov060_02115b84();
    return 1;
}

/* One frame of the fight: advance the random state once (result unused), pick
 * the closest player and record the angle and distance to him (none: his own
 * facing and 0x7fffffff), run BowserLoop and UpdateFootfalls, copy mAngleY to
 * mPrevAngleY, advance the model animation at mAnimSpeed, place the model and
 * shadow (UpdateModelMatrix), publish `this` into the camera's mFramedActor,
 * then rebuild the body cylinder 0x50000 in front of the actor. Always 1.
 *
 * mdCcAcPos_c inherits dCc_c through dCcAcPos_c/dCcAc_c, so Clear and Update
 * are its own. The animation speed (0x130) is the dExtFrameCtrl_c base's
 * `speed` inside mAnmModel (+0x50, +0x0c), so advancing it advances the
 * model. */
int daKpa_c::Behavior()
{
    RandomIntInternal(&data_0209e650.value);
    mTargetPlayer = ClosestPlayer();
    if (mTargetPlayer != 0) {
        mAngleToTarget = Vec3_HorzAngle(&mPos, &mTargetPlayer->mPos);
        mDistToTarget = Vec3_HorzDist(&mPos, &mTargetPlayer->mPos);
    } else {
        mAngleToTarget = mAngleY;
        mDistToTarget = ~0x80000000;
    }
    BowserLoop();
    UpdateFootfalls();
    mPrevAngleY = mAngleY;
    mAnmModel.speed = mAnimSpeed;
    mAnmModel.Advance();
    UpdateModelMatrix();
    gCamera->mFramedActor = this;
    mdCcAcPos_c.Clear();
    Vector3 v;
    v.z = 0x50000;
    v.x = 0;
    v.y = 0;
    mdCcAcPos_c.SetPosRelativeToActor(v);
    mdCcAcPos_c.Update();
    if (mCapActorAlive != 0) {
        dActor_c* cap = dActor_c::FindWithActorID(BOWSER_ACTOR_OBJ_MARIO_CAP, 0);
        if (cap == 0) mCapActorAlive = 0;
    }
    return 1;
}

/*
Draws nothing; returns 1. */
int daKpaTail_c::Render()
{
    return 1;
}

/*
Draw Bowser. Skipped while mOpacity < 8, where the opacity handed to the model
(mOpacity >> 3, see UpdateModelMatrix) would be 0. Otherwise step the texture
pattern, apply it to the model and render the model with the actor's scale. */
int daKpa_c::Render()
{
    if (mOpacity < 8) return 1;
    mTextureSequence.Advance();
    mTextureSequence.Update(mAnmModel.data);
    /* dExtAnmModel_c's slot-5 Render(const Vector3 *) takes the scale vector;
     * daKpa2Bg_c::Render makes the same call on its mModel2. */
    mAnmModel.Render(&mScale);
    return 1;
}

/*
Empty override: the ROM body is a single `bx lr`. */
void daKpa_c::OnPendingDestroy()
{
}

/*
Releases nothing; returns 1. The tail loads no files of its own:
daKpa_c::CleanupResources releases everything the fight loaded. */
int daKpaTail_c::CleanupResources()
{
    return 1;
}

/* Release the whole fight. One single, then the 0x1c-entry and six-entry
 * tables walked by index, then two more singles, the last in ov089. The tables
 * hold POINTERS to handles; the singles are handles. The loops stay loops:
 * 0x1c and 6 are the counts the ROM compares against. func_02011cfc is the
 * Sound.cpp cleanup veneer. Always 1. */
int daKpa_c::CleanupResources()
{
    int i;
    data_ov060_0211ac78.base.Release();
    for (i = 0; i < 0x1c; i++)
        data_ov060_021192dc[i]->Release();
    for (i = 0; i < 6; i++)
        data_ov060_0211927c[i]->Release();
    data_ov060_0211b208.Release();
    data_ov089_02132c50.Release();
    func_02011cfc();
    return 1;
}

/*
Tail state BOWSER_TAIL_FREE (0), per frame: nobody holds the tail. Switch the
tail's own collider on (clear its disabled bit). If Bowser is in
BOWSER_STATE_ARENA_TILT, switch his body collider on and stop; if he is defeated,
stop. Otherwise switch his body collider on as well (SetTangible with 1)
and look for a grab: the tail's collider must report an owner and the grab bit
(hitFlags & 0x1000), that actor must not be collecting a cap, and
daPly_c::TryGrab(tail) must succeed. On a grab the daPly_c is given Bowser's three
angles (current and previous), the tail goes to BOWSER_TAIL_HOLDING and records
the daPly_c, Bowser's body collider is switched off, Bowser goes to
BOWSER_HOLD_HELD with the daPly_c in mGrabbedPlayer, and the hold countdown starts
at 0x96 (150 frames). */
void daKpaTail_c::func_ov060_02115d68(){
    daKpa_c *bowser = (daKpa_c *)dActor_c::FindWithID(this->mBowserUniqueID);
    this->mdCcAc_c.flags &= ~BOWSER_CC_DISABLED;
    if (bowser->mState != BOWSER_STATE_ARENA_TILT) goto skip_13;
    bowser->SetTangible(1);
    return;
skip_13:
    if (bowser->mState == BOWSER_STATE_DEFEATED) return;
    bowser->SetTangible(1);
    if (this->mdCcAc_c.otherOwner == 0) return;
    if (!(this->mdCcAc_c.hitFlags & 0x1000)) return;
    daPly_c *player = static_cast<daPly_c *>(dActor_c::FindWithID(this->mdCcAc_c.otherOwner));
    if (!player) return;
    if (player->IsCollectingCap())
        return;
    if (!player->TryGrab(*this))
        return;
    /* Read through one pointer on purpose: copying bowser->mAngleX/Y/Z member
     * by member builds 4 bytes shorter (measured). */
    s16 *ip = &bowser->mAngleX;
    player->mAngleX = ip[0];
    player->mAngleY = ip[1];
    player->mAngleZ = ip[2];
    player->mPrevAngleX = ip[0];
    player->mPrevAngleY = ip[1];
    player->mPrevAngleZ = ip[2];
    this->mState = BOWSER_TAIL_HOLDING;
    this->mHeldPlayer = player;
    bowser->SetTangible(0);
    bowser->mHoldState = BOWSER_HOLD_HELD;
    bowser->mGrabbedPlayer = player;
    this->mHoldCountdown = 0x96;
}

/*
Tail state BOWSER_TAIL_COOLDOWN (1), per frame: once mTimer is above 0x1e (30)
the tail goes back to BOWSER_TAIL_FREE. */
void daKpaTail_c::func_ov060_02115d50(){
    if (this->mTimer > 0x1e)
        this->mState = BOWSER_TAIL_FREE;
}

/*
Tail state BOWSER_TAIL_HOLDING (2), per frame. If Bowser is in
BOWSER_STATE_ARENA_TILT, switch his body collider on and go to BOWSER_TAIL_FREE
(the code then carries on with the checks below). If the tail's own mFlags has
bit 0x400, go to cooldown, forget the daPly_c and tell Bowser to let go at once
(BOWSER_HOLD_RELEASE_NOW); that path returns without touching the tail's
collider. Otherwise, with a daPly_c held: if the daPly_c no longer holds an object
(daPly_c::mHeldObj == 0) he is dropped; else a non-zero daPly_c::mAngleYSpeed
refills mHoldCountdown to 0x96 (150) and a zero one counts it down, dropping the
daPly_c when it reaches 0. A drop (daPly_c::DropActor) puts the tail in cooldown
and Bowser in BOWSER_HOLD_RELEASE_AFTER_ANIM. Every other path ends by switching
the tail's collider off. */
void daKpaTail_c::func_ov060_02115c1c(){
    daKpa_c *bowser;
    daPly_c *held;
    int cond;

    bowser = (daKpa_c *)dActor_c::FindWithID(this->mBowserUniqueID);
    if (bowser->mState == BOWSER_STATE_ARENA_TILT) {
        bowser->SetTangible(1);
        this->mState = BOWSER_TAIL_FREE;
    }

    cond = ((this->mFlags & BOWSER_TAIL_RELEASED) != 0);
    if (cond != 0) {
        this->mState = BOWSER_TAIL_COOLDOWN;
        this->mHeldPlayer = 0;
        bowser->OnTailReleased();
        bowser->mHoldState = BOWSER_HOLD_RELEASE_NOW;
        return;
    }

    held = this->mHeldPlayer;
    if (held == 0) goto tail;

    cond = (held->mHeldObj != 0);
    if (cond == 0) goto do_drop;

    if (held->mAngleYSpeed != 0)
        goto set_default;
    if (this->mHoldCountdown == 0) goto tail;
    this->mHoldCountdown -= 1;
    if (this->mHoldCountdown != 0) goto tail;

do_drop:
    this->mState = BOWSER_TAIL_COOLDOWN;
    this->mHeldPlayer->DropActor();
    this->mHeldPlayer = 0;
    bowser->OnTailReleased();
    bowser->mHoldState = BOWSER_HOLD_RELEASE_AFTER_ANIM;
    goto tail;

set_default:
    this->mHoldCountdown = 0x96;

tail:
    this->mdCcAc_c.flags |= BOWSER_CC_DISABLED;
}

/*
The tail's per-frame dispatcher (called from daKpaTail_c::Behavior). Find
Bowser; run the handler for mState through the table at data_ov060_0211ae9c
(state 0 is func_ov060_02115d68, 1 is func_ov060_02115d50, 2 is
func_ov060_02115c1c; the call is a member-function-pointer call on the tail
itself); switch the tail's collider off if Bowser is defeated; count mTimer up
and reset it to 0 when the handler changed mState; clear and refresh the tail's
collider (dCc_c::Clear and Update). */
void daKpaTail_c::func_ov060_02115b84(){
    daKpa_c *bowser = (daKpa_c *)dActor_c::FindWithID(this->mBowserUniqueID);
    int idx = this->mState;
    (this->*data_ov060_0211ae9c[idx])();
    if (bowser->mState == BOWSER_STATE_DEFEATED) {
        this->mdCcAc_c.flags |= BOWSER_CC_DISABLED;
    }
    this->mTimer++;
    if (idx != this->mState) {
        this->mTimer = 0;
    }
    this->mdCcAc_c.Clear();
    this->mdCcAc_c.Update();
}

/*
Landing effect for the variant-2 fight (nothing happens for the other variants):
dActor_c::Earthquake at his position with the Fix12 argument 0x7d0000 (2000.0,
the same value daKirai_c passes when a bomb goes off) and a FIRERING actor
(0x119) spawned at his position. */
void daKpa_c::SpawnShockwave() {
    struct Vector3 v;
    if (this->mVariantID != 2) return;
    v.x = this->mPosX;
    v.y = this->mPosY;
    v.z = this->mPosZ;
    Earthquake(v, 0x7d0000);
    dActor_c::Spawn(BOWSER_ACTOR_FIRERING, 0, mPos, 0, this->mAreaId, -1);
}

/*
Landing-dust helper. On a frame where the floor collider reports a fresh ground
hit (dBgCh_Actr::JustHitGround), add one to the u16 counter passed in (every
caller passes the address of mStepCounter) and, while that counter is still
below 4, spawn landing dust at his position and play sound 0xbd at his
camera-space position. */
void daKpa_c::BounceEffects(char *counter) {
    if (this->mWithMeshClsn.JustHitGround() == 0) return;
    *(unsigned short*)counter = *(unsigned short*)counter + 1;
    if (*(unsigned short*)counter >= 4) return;
    struct Vector3 v;
    v.x = this->mPosX;
    v.y = this->mPosY;
    v.z = this->mPosZ;
    this->LandingDustAt(v, 0);
    func_02012694(BOWSER_SND_LAND_SOFT, &mCamSpacePos);
}

/*
True once the model's animation has reached its last frame or will reach it on
the coming advance (dExtFrameCtrl_c::Finished, or WillHitFrame(frame count - 1)).
Reads mAnmModel through a copy of this: the plain this->mAnmModel form builds
the same code but draws one @NNN fewer, renumbering every pinned @NNN in this
TU (measured). */
bool daKpa_c::Bowser_IsAnimAtLastFrame(){
    daKpa_c *obj = this;
    return obj->mAnmModel.Finished() ||
           obj->mAnmModel.WillHitFrame((unsigned short)(obj->mAnmModel.GetFrameCount() - 1));
}

/*
Per-frame model placement, called from Behavior. 1) On the ground and not held,
set mAngleX from the floor normal mGroundNormal and mAngleY, and mAngleZ from the
normal and mAngleY - 0x4000 (a quarter turn), both through func_02010844.
2) Build the model matrix in data_020a0e68: translation = position >> 3
(Vec3_Asr by 3; the foot points below are shifted left 3 on the way back out),
then rotations about Y, X and Z; copy it into the model and apply opacity
mOpacity >> 3 (0..31). 3) For skeleton nodes 3 and 6 (mAnmModel.data.transforms
[3] and [6]) multiply the node matrix by the model matrix (the product lands
back in data_020a0e68), read its translation (m[9..11]) and shift it left 3 to
full scale: node 3 gives mFootPosB, node 6 gives mFootPosA, each with Y replaced
by his own Y. 4) Unless mDropsShadow is 0, probe the floor with a dBgCh_Gnd from
0x32000 (50 units) above him (floor height clsnY, or his own Y if there is
none), build the shadow matrix there and call DropShadowRadHeight with radius
0x140000 (320) and height 0x64000 (100). */
void daKpa_c::UpdateModelMatrix() {
    char pad[8];
    Matrix4x3 saved;
    Vector3 pos;
    Vector3 v;
    Vector3 v2;
    int zero;

    if (this->mWithMeshClsn.IsOnGround() == 0)
        goto skip_angles;
    if (this->mHoldState != BOWSER_HOLD_NONE)
        goto skip_angles;
    this->mAngleX = func_02010844((Vector3 *)&this->mGroundNormalX, this->mAngleY);
    this->mAngleZ = func_02010844((Vector3 *)&this->mGroundNormalX, (s16)(this->mAngleY - 0x4000));
skip_angles:
    Vec3_Asr(&v, &this->mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, this->mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, this->mAngleX);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, this->mAngleZ);
    this->mAnmModel.mat4x3 = data_020a0e68;
    this->mAnmModel.ApplyOpacity((unsigned char)((int)this->mOpacity >> 3), 1);
    saved = data_020a0e68;
    zero = 0;
    this->mFootPosBX = zero;
    this->mFootPosBY = zero;
    this->mFootPosBZ = zero;
    MTX_Concat43(&this->mAnmModel.data.transforms[3], &data_020a0e68, &data_020a0e68);
    this->mFootPosBX = data_020a0e68.m[9];
    this->mFootPosBY = data_020a0e68.m[10];
    this->mFootPosBZ = data_020a0e68.m[11];
    Vec3_LslInPlace(&this->mFootPosBX, 3);
    this->mFootPosBY = this->mPosY;
    this->mFootPosAX = zero;
    this->mFootPosAY = zero;
    this->mFootPosAZ = zero;
    data_020a0e68 = saved;
    MTX_Concat43(&this->mAnmModel.data.transforms[6], &data_020a0e68, &data_020a0e68);
    this->mFootPosAX = data_020a0e68.m[9];
    this->mFootPosAY = data_020a0e68.m[10];
    this->mFootPosAZ = data_020a0e68.m[11];
    Vec3_LslInPlace(&this->mFootPosAX, 3);
    this->mFootPosAY = this->mPosY;
    if (this->mDropsShadow == 0)
        return;
    {
        dBgCh_Gnd rc;
        pos.x = this->mPosX;
        pos.y = this->mPosY;
        pos.z = this->mPosZ;
        pos.y = pos.y + 0x32000;
        rc.SetObjAndPos(pos, 0);
        if (rc.DetectClsn())
            pos.y = rc.clsnY;
        else
            pos.y = this->mPosY;
        Vec3_Asr(&v2, &pos, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, v2.x, v2.y, v2.z);
        *(Matrix4x3 *)(&this->mShadowMtx) = data_020a0e68;
        DropShadowRadHeight(this->mShadowModel, *(Matrix4x3 *)&this->mShadowMtx, 0x140000, 0x64000,
                            0xf);
    }
}

/*
First animation of the turn / intro sequence: play animation 0x13 once; when it
reaches its last frame, set mHorzSpeed to 0x3000 (3.0) and return 1, otherwise
return 0. The sequence continues with 0x11 looping (SetAnimSlowGait) and ends
with 0x12 (SetAnimLookDownStopWalk); the intro and turn handlers use it. */
int daKpa_c::SetAnimLookUpAndWalk() {
    InitAnimation(0x13, 0x40000000);
    int r = Bowser_IsAnimAtLastFrame();
    if (r != 0) {
        this->mHorzSpeed = 0x3000;
        return 1;
    }
    return 0;
}

/*
Play animation 0x11 looping with mHorzSpeed 0x3000 (3.0); return whether it is at
its last frame. */
int daKpa_c::SetAnimSlowGait() {
    InitAnimation(0x11, 0);
    this->mHorzSpeed = 0x3000;
    return Bowser_IsAnimAtLastFrame();
}

/*
Play animation 0x12 once with mHorzSpeed 0; return whether it is at its last
frame. */
int daKpa_c::SetAnimLookDownStopWalk() {
    InitAnimation(0x12, 0x40000000);
    this->mHorzSpeed = 0;
    return Bowser_IsAnimAtLastFrame();
}

/*
The intro conversation, one step per mTalkStep (nothing happens without a target
daPly_c). Step 0: daPly_c::StartTalk(this, 1); when it succeeds, mState becomes
BOWSER_STATE_INTRO and the step advances. Step 1: once daPly_c::GetTalkState() is
0, show message data_ov060_0211a4e0[mVariantID] (0xcc, 0xce or 0xd0 in the ROM) at
a point 200 units out along mAngleY and 200 units up (the sin/cos table scaled
by 0xc8, Y + 0xc8000); when that succeeds, advance and play sound 0xb7. Step 2:
when GetTalkState() returns -1, go to BOWSER_STATE_JUMP for variant 1 and
BOWSER_STATE_IDLE otherwise, advance, set mCutsceneStep to 4 and run
UpdateIntroCamera once (its step 4 clears bit 0x8 of Camera::mFlags). */
void daKpa_c::RunIntroTalk() {
    daPly_c *player = this->mTargetPlayer;
    if (player == 0)
    {
        return;
    }
    switch (this->mTalkStep)
    {
        case 0:
            if (player->StartTalk(*this, 1) == 0) {
                return;
            }
            this->mState = BOWSER_STATE_INTRO;
            this->mTalkStep++;
            return;

        case 1:
            if (player->GetTalkState() != 0) {
                return;
            }
            {
                int pos[3];
                int x;
                int y;
                int z;
                int y2;
                int zero;
                int scale;
                int msg;
                s16 *msgs;
                u16 ang;
                s16 sx;
                s16 sz;
                /* pos[] is first filled with his position and then overwritten
                 * before use; dropping those first three stores builds 12 bytes
                 * shorter (measured). */
                x = this->mPosX;
                pos[0] = x;
                y = this->mPosY;
                scale = 0xc8;
                pos[1] = y;
                z = this->mPosZ;
                y2 = y + 0xc8000;
                pos[2] = z;
                ang = this->mAngleY;
                zero = 0;
                msgs = data_ov060_0211a4e0;
                sx = FX_SinIdx(ang >> 4);
                pos[0] = sx * scale + x;
                ang = this->mAngleY;
                sz = FX_CosIdx(ang >> 4);
                pos[1] = y2;
                pos[2] = sz * scale + z;
                msg = msgs[this->mVariantID];
                if (this->mTargetPlayer->ShowMessage(
                        *this, msg, reinterpret_cast<const Vector3 *>(pos), zero, zero) == 0) {
                    return;
                }
            }
            this->mTalkStep++;
            func_02012694(BOWSER_SND_TALK_START, &mCamSpacePos);
            return;

        case 2:
            if (player->GetTalkState() != (-1)) {
                return;
            }
            if ((this->mVariantID) == 1)
            {
                this->mState = BOWSER_STATE_JUMP;
            }
            else
            {
                this->mState = BOWSER_STATE_IDLE;
            }
            this->mTalkStep++;
            this->mCutsceneStep = 4;
            UpdateIntroCamera();
            return;
    }
}

/*
State BOWSER_STATE_INTRO_WAIT (5): run the intro camera (UpdateIntroCamera),
stand still on the idle animation (0x10) and run the conversation
(RunIntroTalk), whose step 0 moves him to BOWSER_STATE_INTRO. */
void daKpa_c::ActWait() {
    UpdateIntroCamera();
    this->mHorzSpeed = 0;
    InitAnimation(BOWSER_ANIM_IDLE, 0);
    RunIntroTalk();
}

/*
State BOWSER_STATE_INTRO (6): run the intro camera (UpdateIntroCamera), then by
mStep. Step 0: SetAnimLookUpAndWalk (animation 0x13), advancing when it returns 1.
Steps 1 and 2: SetAnimSlowGait (animation 0x11 looping); each completed loop
advances the step, and variant 2 advances it by two (from step 1 it skips step 2). Step 3:
SetAnimLookDownStopWalk (animation 0x12); when it returns 1, switch to the idle
animation and advance. Later steps: the conversation (RunIntroTalk). */
void daKpa_c::ActIntroWalk() {
    u8 step;
    UpdateIntroCamera();
    step = this->mStep;
    if (step == 0) {
        if (SetAnimLookUpAndWalk() == 0)
            return;
        this->mStep++;
        return;
    }
    if (step <= 2) {
        if (SetAnimSlowGait() == 0)
            return;
        this->mStep++;
        if (this->mVariantID == 2) {
            this->mStep++;
        }
        return;
    }
    if (step == 3) {
        if (SetAnimLookDownStopWalk() == 0)
            return;
        InitAnimation(BOWSER_ANIM_IDLE, 0);
        this->mStep++;
        return;
    }
    RunIntroTalk();
}

/*
Idle pick for variant 0, called from ActDefault. With mPickToggle 0: if
he faces the target (BOWSER_COND_FACING_TARGET), go to BOWSER_STATE_ANIM_CHAIN
when mDistToTarget < 0x5dc000 (1500 units) and BOWSER_STATE_HOP otherwise; if not,
BOWSER_STATE_TURN_TO_TARGET; then set the toggle and reset mAnimSpeed to 0x1000
(1.0). With mPickToggle set: clear it; unless mSkipIdleRoll is set (then clear
that and turn), one draw in ten (random >> 16, modulo 10, equal to 0) picks
BOWSER_STATE_PLAY_ANIM_0F, otherwise BOWSER_STATE_TURN_TO_TARGET. */
void daKpa_c::BitdwActions() {
    if (this->mPickToggle == 0) {
        if (this->mCondFlags & BOWSER_COND_FACING_TARGET) {
            if (this->mDistToTarget < 0x5dc000)
                this->mState = BOWSER_STATE_ANIM_CHAIN;
            else
                this->mState = BOWSER_STATE_HOP;
        } else {
            this->mState = BOWSER_STATE_TURN_TO_TARGET;
        }
        this->mPickToggle++;
        this->mAnimSpeed = 0x1000;
    } else {
        this->mPickToggle = 0;
        if (this->mSkipIdleRoll == 0) {
            unsigned int v = (unsigned int)RandomIntInternal(&data_0209e650.value) >> 0x10;
            if (v % 10 == 0)
                this->mState = BOWSER_STATE_PLAY_ANIM_0F;
            else
                this->mState = BOWSER_STATE_TURN_TO_TARGET;
        } else {
            this->mSkipIdleRoll = 0;
            this->mState = BOWSER_STATE_TURN_TO_TARGET;
        }
    }
}

/*
Idle pick for variant 1, called from ActDefault. With mPickToggle set:
clear it and go to BOWSER_STATE_TURN_TO_TARGET. Otherwise, if he is not facing
the target, BOWSER_STATE_TURN_TO_TARGET; if he is, and DistToCPlayer() <
0x514000 (1300 units), a draw modulo 10 below mVanishChance picks
BOWSER_STATE_VANISH_DASH (mVanishChance then drops to 3 if it was above 3, else
to 1) and anything else picks BOWSER_STATE_FIREBALLS (mVanishChance back to 5);
if the daPly_c is farther away, BOWSER_STATE_CHARGE, upgraded to
BOWSER_STATE_JUMP on a draw below 5 when mDistToCenter is between 0x1f4000 (500
units) and 0x5dc000 (1500 units). Every path except the cleared-toggle one sets
the toggle. */
void daKpa_c::BitfsActions() {
    unsigned int m;

    if (this->mPickToggle != 0)
        goto clear_toggle;

    if ((this->mCondFlags & BOWSER_COND_FACING_TARGET) == 0)
        goto turn;

    if (this->DistToCPlayer() < 0x514000) {
        m = ((unsigned int)RandomIntInternal(&data_0209e650.value) >> 0x10) % 10;
        if (m < this->mVanishChance) {
            this->mState = BOWSER_STATE_VANISH_DASH;
            if (this->mVanishChance > 3)
                this->mVanishChance = 3;
            else
                this->mVanishChance = 1;
        } else {
            this->mState = BOWSER_STATE_FIREBALLS;
            this->mVanishChance = 5;
        }
        goto set_toggle;
    }

    this->mState = BOWSER_STATE_CHARGE;
    if (this->mDistToCenter > 0x1f4000 && this->mDistToCenter < 0x5dc000) {
        m = ((unsigned int)RandomIntInternal(&data_0209e650.value) >> 0x10) % 10;
        if (m < 5)
            this->mState = BOWSER_STATE_JUMP;
    }
    goto set_toggle;

turn:
    this->mState = BOWSER_STATE_TURN_TO_TARGET;
set_toggle:
    this->mPickToggle++;
    return;

clear_toggle:
    this->mPickToggle = 0;
    this->mState = BOWSER_STATE_TURN_TO_TARGET;
}

/*
Choose an attack for variant 2, called from BitsActions. If he faces the
target: with DistToCPlayer() < 0x3e8000 (1000 units), a draw modulo 10 below 4
picks BOWSER_STATE_FIREBALLS, else a second draw below 8 picks
BOWSER_STATE_FIRE_BREATH, else BOWSER_STATE_ANIM_CHAIN (mFireTimer is cleared and
mAnimSpeed set to 0x1000 on all three); with the daPly_c farther away a draw below
5 picks BOWSER_STATE_JUMP, else BOWSER_STATE_CHARGE. If he does not face the
target, BOWSER_STATE_TURN_TO_TARGET. */
void daKpa_c::BitsActionList() {
    dActor_c *a = this;
    if (this->mCondFlags & BOWSER_COND_FACING_TARGET) {
        if (a->DistToCPlayer() < 0x3e8000) {
            if (((unsigned)RandomIntInternal(&data_0209e650.value) >> 0x10) % 10 < 4) {
                this->mState = BOWSER_STATE_FIREBALLS;
            } else if (((unsigned)RandomIntInternal(&data_0209e650.value) >> 0x10) % 10 < 8) {
                this->mState = BOWSER_STATE_FIRE_BREATH;
            } else {
                this->mState = BOWSER_STATE_ANIM_CHAIN;
            }
            this->mFireTimer = 0;
            this->mAnimSpeed = 0x1000;
        } else if (((unsigned)RandomIntInternal(&data_0209e650.value) >> 0x10) % 10 < 5) {
            this->mState = BOWSER_STATE_JUMP;
        } else {
            this->mState = BOWSER_STATE_CHARGE;
        }
    } else {
        this->mState = BOWSER_STATE_TURN_TO_TARGET;
    }
}

/*
Pick BOWSER_STATE_JUMP (the choice for variants with mChooseJumpOnly set). */
void daKpa_c::SetActBigJump() {
    this->mState = BOWSER_STATE_JUMP;
}

/*
Idle pick for variant 2, called from ActDefault. With mPickToggle 0:
BitsActionList, or SetActBigJump when mChooseJumpOnly is set, then set
the toggle. With the toggle set: clear it and go to
BOWSER_STATE_TURN_TO_TARGET. */
void daKpa_c::BitsActions() {
    if (this->mPickToggle == 0)
    {
        if (this->mChooseJumpOnly == 0)
        {
            BitsActionList();
        }
        else
        {
            SetActBigJump();
        }
        (this->mPickToggle)++;
        return;
    }
    this->mPickToggle = 0;
    this->mState = BOWSER_STATE_TURN_TO_TARGET;
}

/*
Fall catch for the variant-2 jumps: if he is moving down (mVertSpeed < 0) and
mPosY is below mHomePosY - 0x12c000 (300 units below the arena level), put him
at X = Z = 0, 0x7d0000 (2000 units) above the arena level, with both speeds
zeroed. */
void daKpa_c::ResetFallenOffStage() {
    int vy = this->mVertSpeed;
    if (vy >= 0) return;
    int thresh = this->mHomePosY - 0x12c000;
    int cur = this->mPosY;
    if (cur >= thresh) return;
    this->mPosZ = 0;
    this->mPosX = this->mPosZ;
    this->mPosY = this->mHomePosY + 0x7d0000;
    this->mVertSpeed = 0;
    this->mHorzSpeed = 0;
}

/*
State BOWSER_STATE_WAIT_ANIM_END (0x12): back to BOWSER_STATE_IDLE once the
animation is at its last frame. */
void daKpa_c::ActIdle() {
    int done = Bowser_IsAnimAtLastFrame();
    if (done) {
        this->mState = BOWSER_STATE_IDLE;
    }
}

/*
State BOWSER_STATE_IDLE (0): clear unk_422, play the idle animation (0x10), zero
mSpinSpeed and both speeds, then let the variant pick the next state:
BitdwActions (variant 0), BitfsActions (variant 1) or
BitsActions (variant 2; variant 3 was folded into 0 at init). */
void daKpa_c::ActDefault() {
    this->unk_422 = 0;
    InitAnimation(BOWSER_ANIM_IDLE, 0);
    this->mSpinSpeed = 0;
    this->mHorzSpeed = 0;
    this->mVertSpeed = 0;
    if (this->mVariantID == 0)
        BitdwActions();
    else if (this->mVariantID == 1)
        BitfsActions();
    else
        BitsActions();
}

/*
State BOWSER_STATE_ANIM_CHAIN (0xf): mHorzSpeed 0. While animation 8 is playing
(data_ov060_0211ac20 is entry 8 of the animation table), sound 0xb5 plays when
frame 8 is crossed. At the last frame of an animation: after animation 8
(data_ov060_0211ac20) start animation 6, after animation 6 (data_ov060_0211ac68)
start animation 7, after animation 7 (data_ov060_0211ac70) go back to
BOWSER_STATE_IDLE, and after any other animation (the one playing when the state
is entered) start animation 8; the three are started with 0x40000000 (play once). */
void daKpa_c::ActBreathFire() {
    this->mHorzSpeed = 0;
    if ((int)this->mAnmModel.file == (int)data_ov060_0211ac20.filePtr) {
        if (this->mAnmModel.WillHitFrame(8)) {
            func_02012694(BOWSER_SND_B5, &mCamSpacePos);
        }
    }
    if (Bowser_IsAnimAtLastFrame() == 0) return;
    if ((int)this->mAnmModel.file == (int)data_ov060_0211ac20.filePtr) {
        InitAnimation(6, 0x40000000);
        return;
    }
    if ((int)this->mAnmModel.file == (int)data_ov060_0211ac68.filePtr) {
        InitAnimation(7, 0x40000000);
        return;
    }
    if ((int)this->mAnmModel.file == (int)data_ov060_0211ac70.filePtr) {
        this->mState = BOWSER_STATE_IDLE;
        return;
    }
    InitAnimation(8, 0x40000000);
}

/*
State BOWSER_STATE_TURN_TO_TARGET (0xe). Each frame turn mAngleY toward
mAngleToTarget by 0x400 (5.6 degrees) for variant 1 and for mHealth above 2, by
0x300 (4.2 degrees) at mHealth 2, by 0x200 (2.8 degrees) otherwise
(ApproachLinear). Then by mStep. Step 0: clear mStepCounter and run
SetAnimLookUpAndWalk (animation 0x13); advance when it returns 1. Steps 1 and 2:
SetAnimSlowGait (animation 0x11 looping); each finished loop counts in
mStepCounter, and if BOWSER_COND_BREATHING is set it is cleared after five loops
and nothing else happens; otherwise, once the angle difference measured before
this frame's turn is under 0x2000 (45 degrees), restart the animation, advance
the step and clear the counter. Later: SetAnimLookDownStopWalk
(animation 0x12), then BOWSER_STATE_IDLE when it returns 1. */
void daKpa_c::ActWalkToMario() {
    int angleDiff;
    short step;

    angleDiff = this->GetSubtraction(this->mAngleY, this->mAngleToTarget);

    if (this->mVariantID == 1) {
        step = 0x400;
    } else {
        int t = this->mHealth;
        if (t > 2) {
            step = 0x400;
        } else if (t == 2) {
            step = 0x300;
        } else {
            step = 0x200;
        }
    }
    ApproachLinear(this->mAngleY, this->mAngleToTarget, step);

    if (this->mStep == 0) {
        this->mStepCounter = 0;
        if (SetAnimLookUpAndWalk() == 0)
            return;
        (this->mStep)++;
        return;
    }
    if (this->mStep <= 2) {
        if (SetAnimSlowGait() == 0)
            return;
        (this->mStepCounter)++;
        if (this->mCondFlags & BOWSER_COND_BREATHING) {
            if (this->mStepCounter < 5) return;
            this->mCondFlags &= ~BOWSER_COND_BREATHING;
            return;
        }
        if (angleDiff >= 0x2000)
            return;
        this->mAnmModel.currFrame = 0;
        (this->mStep)++;
        this->mStepCounter = 0;
        return;
    }
    if (SetAnimLookDownStopWalk() != 0)
        this->mState = BOWSER_STATE_IDLE;
}

/*
State BOWSER_STATE_VANISH_DASH (0x10). The body collider is switched off every
frame. Step 0: target opacity 0, mStepCounter = 0x1e; play sound 0xb9 on the first
frame (mTimer == 0); when mOpacity reaches 0, face the target and advance. Step 1:
dash at mHorzSpeed 0x64000 (100.0) while the counter (decremented each frame)
was still non-zero; the dash ends (step 2, facing the target again) when the
counter runs out, when the angle to the target is over 0x4000 (90 degrees) and
the target is more than 0x1f4000 (500 units) away, or when the floor collider
says he is off the ground (position restored to mLastGroundPos, speed 0). Step 2:
speed 0, target opacity 0xff; when fully visible, return to BOWSER_STATE_IDLE and
switch the body collider back on. */
void daKpa_c::ActTeleport() {
    this->mdCcAcPos_c.flags |= BOWSER_CC_DISABLED;
    switch (this->mStep)
    {
        case 0:
            this->mTargetOpacity = 0;
            this->mStepCounter = 0x1e;
            if ((this->mTimer) == 0)
            {
                func_02012694(BOWSER_SND_VANISH, &mCamSpacePos);
            }
            if ((this->mOpacity) != 0)
            {
                return;
            }
            this->mStep++;
            this->mAngleY = this->mAngleToTarget;
            return;

        case 1:
            {
                int endDash = 0;
                int sub;
                if (this->mStepCounter-- != 0)
                {
                    this->mHorzSpeed = 0x64000;
                }
                else
                {
                    endDash = 1;
                }
                sub = this->GetSubtraction(this->mAngleY, this->mAngleToTarget);
                if (sub > 0x4000)
                {
                    if ((this->mDistToTarget) > 0x1f4000)
                    {
                        endDash = 1;
                    }
                }
                if (this->mWithMeshClsn.IsOnGround() == 0)
                {
                    this->mPosX = this->mLastGroundPosX;
                    this->mPosY = this->mLastGroundPosY;
                    this->mPosZ = this->mLastGroundPosZ;
                    endDash = 1;
                    this->mHorzSpeed = 0;
                }
                if (endDash == 0)
                {
                    return;
                }
                this->mStep = 2;
                this->mAngleY = this->mAngleToTarget;
                return;
            }

        case 2:
            this->mHorzSpeed = 0;
            this->mTargetOpacity = 0xff;
            if ((this->mOpacity) == 0xff)
            {
                this->mState = BOWSER_STATE_IDLE;
                this->mdCcAcPos_c.flags &= ~BOWSER_CC_DISABLED;
            }
            return;

        default:
            return;
    }
}

/*
State BOWSER_STATE_FIRE_BREATH (8). While animation 0x15 plays, or animation 0x16
after frame 0x2a (data_ov060_0211acd8 and data_ov060_0211acf0 are entries 0x15
and 0x16 of the animation table): keep the looping sound 0x180 going, spawn a
KOOPAFIRE every 5th frame of mFireTimer at his position plus 200 units out along
mPrevAngleY and 0xb4000 (180 units) up, with the angles mPrevAngleX/Y/Z as
rotation and spawn parameter (random modulo 10) << 10 | 1, or | 0x11 while the
frame number is 0xf..0x13; mFireTimer counts the frames in that window.
BOWSER_COND_BREATHING is set every call. At the last frame of an animation, by
mStep: 0 starts animation 0x16 (once) with sound 0xb5, 1 starts 0x15 (looping),
2 starts 0x17 (once), 3 goes to BOWSER_STATE_IDLE and clears the flag. */
void daKpa_c::ActSpitFireIntoSky() {
    if (mAnmModel.file == data_ov060_0211acd8.filePtr ||
        (mAnmModel.file == data_ov060_0211acf0.filePtr &&
         (u16)((u32)mAnmModel.currFrame >> 12) > 0x2a)) {
        int frame = (u16)((u32)mAnmModel.currFrame >> 12);
        if (mSoundID != 0x180)
            mSoundHandle = 0;
        mSoundID = BOWSER_SND_FIRE_LOOP;
        mSoundHandle = Sound::PlayLong(mSoundHandle, 3, mSoundID, mCamSpacePos, 0);
        if (mFireTimer % 5 == 0) {
            Vector3 pos = mPos;
            pos.x += FX_SinIdx((u16)mPrevAngleY >> 4) * 0xc8;
            pos.y += 0xb4000;
            pos.z += FX_CosIdx((u16)mPrevAngleY >> 4) * 0xc8;
            if (frame >= 0xf && frame < 0x14) {
                dActor_c::Spawn(BOWSER_ACTOR_KOOPAFIRE,
                                ((u32)RandomIntInternal(&data_0209e650.value) >> 16) % 10 << 10 |
                                    0x11,
                                pos, &mPrevAngle, mAreaId, -1);
            } else {
                dActor_c::Spawn(BOWSER_ACTOR_KOOPAFIRE,
                                ((u32)RandomIntInternal(&data_0209e650.value) >> 16) % 10 << 10 | 1,
                                pos, &mPrevAngle, mAreaId, -1);
            }
        }
        this->mFireTimer++;
    }
    this->mCondFlags |= BOWSER_COND_BREATHING;
    if (Bowser_IsAnimAtLastFrame() == 0)
    {
        return;
    }
    switch (this->mStep)
    {
        case 0:
            InitAnimation(0x16, 0x40000000);
            func_02012694(BOWSER_SND_B5, &mCamSpacePos);
            this->mStep++;
            return;

        case 1:
            InitAnimation(0x15, 0);
            this->mStep++;
            return;

        case 2:
            InitAnimation(0x17, 0x40000000);
            this->mStep++;
            return;

        case 3:
            this->mState = BOWSER_STATE_IDLE;
            this->mCondFlags &= ~BOWSER_COND_BREATHING;
            return;
    }
}

/*
State BOWSER_STATE_HURT_HOP (0xc), the knock-back after a bomb hit. On the first
frame: mHorzSpeed -0x1c000 (-28.0), mVertSpeed 0x50000 (80.0), mAngleY = angle to
the centre + 0x8000 (so the negative speed carries him toward the centre),
unk_422 = 1, mAngleX = -0xc00 and sound 0xb1. On later frames mAngleX drops by
0xc00 (16.9 degrees) per frame, from its unsigned value 0xf400 (-0xc00): 20 drops,
337.5 degrees in all, until that value is 0xc00 or less, then it is set to 0. By
mStep: 0 plays animation 1 once and clears mStepCounter; 1 runs the landing-dust helper on mStepCounter, plays
animation 2 once when the counter reaches 1 and, at 3, zeroes both speeds and
advances; 2 waits for the last frame, then goes to BOWSER_STATE_PLAY_ANIM_0F when
mHealth is 1 and to BOWSER_STATE_IDLE otherwise; unk_422 is cleared on every
frame of step 2. */
void daKpa_c::ActHitMine() {
    if (this->mTimer == 0) {
        this->mHorzSpeed = -0x1c000;
        this->mVertSpeed = 0x50000;
        this->mAngleY = this->mAngleToCenter + 0x8000;
        this->unk_422 = 1;
        this->mAngleX = -0xc00;
        func_02012694(BOWSER_SND_LEAP, &mCamSpacePos);
    } else {
        if ((u16)this->mAngleX > 0xc00) {
            this->mAngleX -= 0xc00;
        } else {
            this->mAngleX = 0;
        }
    }
    {
        u8 st = this->mStep;
        if (st == 0) {
            InitAnimation(1, 0x40000000);
            this->mStep++;
            this->mStepCounter = 0;
            return;
        }
        if (st == 1) {
            BounceEffects((char *)&this->mStepCounter);
            if (this->mStepCounter == 1)
                InitAnimation(2, 0x40000000);
            if (this->mStepCounter < 3) return;
            this->mVertSpeed = 0;
            this->mHorzSpeed = 0;
            this->mStep++;
            return;
        }
        if (st != 2) return;
        if (Bowser_IsAnimAtLastFrame() != 0) {
            if (this->mHealth == 1) this->mState = BOWSER_STATE_PLAY_ANIM_0F;
            else this->mState = BOWSER_STATE_IDLE;
        }
        this->unk_422 = 0;
    }
}

/*
Jump wind-up: play animation 0xb once; return 1 when the animation will cross
frame 0x20 on this advance, otherwise 0. */
int daKpa_c::SetAnimJump() {
    InitAnimation(0xb, 0x40000000);
    int r = this->mAnmModel.WillHitFrame(0x20);
    if (r != 0) return 1;
    return 0;
}

/*
Jump landing check. If the floor collider says he is on the ground: zero both
speeds, call func_0200fa04 with his position, play animation 0xc once, play sound
0xb6; for variant 0 also call func_ov002_020c56f0 with the target daPly_c and
(mDistToTarget >= 0x352000, 850 units) when that daPly_c is not in the air; return
1. Otherwise return 0. */
int daKpa_c::BowserLand() {
    struct Vector3 v;
    int b;
    if(this->mWithMeshClsn.IsOnGround()){
        this->mHorzSpeed = 0;
        this->mVertSpeed = 0;
        v.x = this->mPosX;
        v.y = this->mPosY;
        v.z = this->mPosZ;
        func_0200fa04(&v, 0);
        InitAnimation(0xc, 0x40000000);
        func_02012694(BOWSER_SND_LAND, &mCamSpacePos);
        if(this->mVariantID == 0){
            b = (this->mDistToTarget >= 0x352000);
            if (!this->mTargetPlayer->IsInAir())
                func_ov002_020c56f0((unsigned char *)this->mTargetPlayer, b);
        }
        return 1;
    }
    return 0;
}

/*
Variant 2 only, and only while BOWSER_COND_RECOVERING is set: when mDistToCenter
is above 0x3e8000 (1000 units) set mHorzSpeed to 0x1e000 (30.0). */
void daKpa_c::ShortSecondHop() {
    if (this->mVariantID != 2) return;
    if (!(this->mCondFlags & BOWSER_COND_RECOVERING)) return;
    if (this->mDistToCenter > 0x3e8000) {
        this->mHorzSpeed = 0x1e000;
    }
}

/*
State BOWSER_STATE_JUMP (0xd). Step 0: wind-up (SetAnimJump); then
mVertSpeed = 0x28000 (40.0) for variant 2 while BOWSER_COND_RECOVERING is set,
0x32000 (50.0) otherwise; sound 0xb1; zero mHorzSpeed and mStepCounter, let
ShortSecondHop set a horizontal speed, and if it stays 0 snap X and Z to
mLastGroundPos; advance and reset mAnimSpeed to 0x1000. Step 1: variant 2
recovering runs ResetFallenOffStage first; he lands when BowserLand
reports it, or when mPosY has dropped below mLastGroundPosY (then, if still
moving vertically, the landing effects are replayed here); landing clears
BOWSER_COND_RECOVERING, zeroes both speeds, puts him at mLastGroundPosY, advances,
runs SpawnShockwave and, for variant 1, goes to BOWSER_STATE_ARENA_TILT and
records the KOOPA2BG actor's uniqueID in mArenaBgUniqueID. Step 2: at the last
frame, BOWSER_STATE_IDLE. */
void daKpa_c::ActBigJump() {
    unsigned char st = this->mStep;
    int vec[3];

    if (st == 0) {
        if (SetAnimJump() == 0)
            return;
        if (this->mVariantID == 2 && (this->mCondFlags & BOWSER_COND_RECOVERING)) {
            this->mVertSpeed = 0x28000;
        } else {
            this->mVertSpeed = 0x32000;
        }
        func_02012694(BOWSER_SND_LEAP, &mCamSpacePos);
        this->mStepCounter = 0;
        this->mHorzSpeed = 0;
        ShortSecondHop();
        if (this->mHorzSpeed == 0) {
            this->mPosX = this->mLastGroundPosX;
            this->mPosZ = this->mLastGroundPosZ;
        }
        (this->mStep)++;
        this->mAnimSpeed = 0x1000;
        return;
    }
    if (st == 1) {
        if (this->mVariantID == 2 && (this->mCondFlags & BOWSER_COND_RECOVERING)) {
            ResetFallenOffStage();
        }
        if (BowserLand() == 0) {
            if (this->mPosY >= this->mLastGroundPosY) return;
        }
        if (this->mVertSpeed != 0) {
            vec[0] = this->mPosX;
            vec[1] = this->mPosY;
            vec[2] = this->mPosZ;
            func_0200fa04((Vector3 *)vec, 0);
            InitAnimation(0xc, 0x40000000);
        }
        this->mCondFlags &= ~BOWSER_COND_RECOVERING;
        this->mHorzSpeed = 0;
        this->mVertSpeed = 0;
        this->mPosY = this->mLastGroundPosY;
        (this->mStep)++;
        SpawnShockwave();
        if (this->mVariantID == 1) {
            void* a;
            this->mState = BOWSER_STATE_ARENA_TILT;
            a = dActor_c::FindWithActorID(BOWSER_ACTOR_KOOPA2BG, 0);
            if (a != 0) {
                this->mArenaBgUniqueID = ((dActor_c *)a)->uniqueID;
            }
        }
        return;
    }
    if (Bowser_IsAnimAtLastFrame() != 0) {
        this->mState = BOWSER_STATE_IDLE;
    }
}

/*
State BOWSER_STATE_HOP (0x11). Step 0: wind-up (SetAnimJump); then
mVertSpeed 0x32000 (50.0), mHorzSpeed 0x19000 (25.0), mStepCounter 0, sound 0xb1,
advance. Step 1: advance once BowserLand reports the landing. Step 2:
BOWSER_STATE_IDLE at the last frame. */
void daKpa_c::ActQuickJump() {
    unsigned char k = this->mStep;
    if (k == 0) {
        if (!SetAnimJump())
            return;
        this->mVertSpeed = 0x32000;
        this->mHorzSpeed = 0x19000;
        this->mStepCounter = 0;
        this->mStep++;
        func_02012694(BOWSER_SND_LEAP, &mCamSpacePos);
    } else if (k == 1) {
        if (!BowserLand())
            return;
        this->mStep++;
    } else {
        if (Bowser_IsAnimAtLastFrame()) this->mState = BOWSER_STATE_IDLE;
    }
}

/*
State BOWSER_STATE_PLAY_ANIM_1B (0xa): mHorzSpeed 0, clear mStepCounter on the
first frame, play animation 0x1b once, then BOWSER_STATE_TURN_IN_PLACE at its last
frame. */
void daKpa_c::ActHitEdge() {
    this->mHorzSpeed = 0;
    if (this->mTimer == 0) {
        this->mStepCounter = 0;
    }
    InitAnimation(0x1b, 0x40000000);
    int r = Bowser_IsAnimAtLastFrame();
    if (r != 0) {
        this->mState = BOWSER_STATE_TURN_IN_PLACE;
    }
}

/*
State BOWSER_STATE_FIREBALLS (9). On the first frame choose mFireballShots: 3 if
the target daPly_c has 4 or fewer health (daPly_c::GetHealth), else a random 1 to 3
(also when there is no daPly_c). When animation 0x14 (data_ov060_0211abe0 is entry
0x14 of the animation table) will cross frame 5, spawn a fireball (SpawnFireball,
horizontal speed 0x1e000 = 30.0, second Fix12 argument 0xa000) at his position
plus 0xe8 (232) units out along mAngleY and 0x58000 (88 units) up, with sound
0x122. At the last frame of an animation: if it was animation 0x14, count the
shot in mStep and go to BOWSER_STATE_IDLE once mStep reaches mFireballShots;
otherwise start animation 0x14; then restart the animation from frame 0. */
void daKpa_c::ActSpitFireOntoFloor() {
    if (this->mTimer == 0) {
        daPly_c *player = this->mTargetPlayer;
        if (player == 0 || player->GetHealth() > 4) {
            s32 rnd = RandomIntInternal(&data_0209e650.value);
            u32 hi = ((u32)rnd) >> 16;
            u32 m = hi % 10u;
            this->mFireballShots = (char)((m % 3u) + 1);
        } else {
            this->mFireballShots = 3;
        }
    }
    if ((int)this->mAnmModel.file == (int)data_ov060_0211abe0.filePtr) {
        if (this->mAnmModel.WillHitFrame(5)) {
            Vector3 pos = mPos;
            Vector3_16 dir = mAngle;
            pos.x += FX_SinIdx((u16)dir.y >> 4) * 0xe8;
            pos.y += 0x58000;
            pos.z += FX_CosIdx((u16)dir.y >> 4) * 0xe8;
            dir.x = 0x1000;
            SpawnFireball(pos, &dir, 0x1e000, 0xa000, 0);
            func_02012694(BOWSER_SND_FIREBALL, &mCamSpacePos);
        }
    }
    if (Bowser_IsAnimAtLastFrame() != 0) {
        if ((int)this->mAnmModel.file == (int)data_ov060_0211abe0.filePtr) {
            this->mStep++;
            if (this->mStep >= this->mFireballShots) {
                this->mState = BOWSER_STATE_IDLE;
            }
        } else {
            InitAnimation(0x14, 0);
        }
        this->mAnmModel.currFrame = 0;
    }
}

/*
Turn in place for the caller's frame count: by mStep, play animation 0x13 once,
then 0x12 once (each step ends at the animation's last frame), then the idle
animation; mHorzSpeed 0 and mAngleY += turnStep every call. Returns 1 once mTimer is
at least minFrames (an unsigned halfword), else 0. */
int daKpa_c::TurnOnTimer(unsigned short minFrames, int turnStep) {
    int done;
    unsigned char step = this->mStep;
    if (step == 0) {
        InitAnimation(0x13, 0x40000000);
        if (Bowser_IsAnimAtLastFrame() != 0) {
            this->mStep++;
        }
    } else if (step == 1) {
        InitAnimation(0x12, 0x40000000);
        if (Bowser_IsAnimAtLastFrame() != 0) {
            this->mStep++;
        }
    } else {
        InitAnimation(BOWSER_ANIM_IDLE, 0);
    }
    done = 0;
    this->mHorzSpeed = 0;
    this->mAngleY += turnStep;
    if (this->mTimer >= minFrames)
        done = 1;
    return done;
}

/*
State BOWSER_STATE_TURN_IN_PLACE (0xb): TurnOnTimer with 0x3e frames and
0x200 (2.8 degrees) per frame, about 177 degrees in all (63 calls, mTimer 0 to
0x3e); BOWSER_STATE_IDLE when it returns 1. */
void daKpa_c::ActTurnFromEdge() {
    int finished = TurnOnTimer(0x3e, 0x200);
    if (finished != 0) {
        this->mState = BOWSER_STATE_IDLE;
    }
}

/*
State BOWSER_STATE_CHARGE (7). mHorzSpeed is zeroed on the first frame. By mStep:
0 plays animation 0x18 once, clears mStepCounter and, at its last frame, goes to
step 1. Step 1 plays animation 0x19 looping at mHorzSpeed 0x2a000 (42.0) and
steers mAngleY toward the target by 0x200 per frame; each time that animation
reaches its last frame the counter grows, and the charge ends (step 3) when the
counter exceeds 0xa or, from the second loop on, when the angle to the target is
over 0x2000 (45 degrees) (mParticleHandle is cleared in that second case). Step
3 plays animation 0x1a once, spawns particle 0x101 50 units above him
(Particle::System::New with mParticleHandle) and slows mHorzSpeed to 0 by 0x1000
per frame, going to step 2 once it reaches 0. Step 2 waits for the last frame of that
animation, sets mTimer to 0xa (variant 2) or 0x1e and returns to
BOWSER_STATE_IDLE once mStepCounter has passed it. If the floor collider says he
has left the ground, he goes to BOWSER_STATE_PLAY_ANIM_1B at mLastGroundPos with
speed 0. */
void daKpa_c::ActChargeMario() {
    if (this->mTimer == 0)
        this->mHorzSpeed = 0;

    switch (this->mStep) {
    case 0:
        InitAnimation(0x18, 0x40000000);
        this->mStepCounter = 0;
        if (Bowser_IsAnimAtLastFrame() != 0)
            this->mStep = 1;
        break;
    case 1:
        InitAnimation(0x19, 0);
        this->mHorzSpeed = 0x2a000;
        if (Bowser_IsAnimAtLastFrame() != 0) {
            this->mStepCounter++;
            if (this->mStepCounter > 0xa)
                this->mStep = 3;
            if (this->mStepCounter >= 2) {
                if (this->GetSubtraction(this->mAngleToTarget, this->mAngleY) > 0x2000) {
                    this->mStep = 3;
                    this->mParticleHandle = 0;
                }
            }
        }
        ApproachLinear(this->mAngleY, this->mAngleToTarget, 0x200);
        break;
    case 3:
        this->mStepCounter = 0;
        InitAnimation(0x1a, 0x40000000);
        this->mParticleHandle =
            (unsigned int)Particle::System::New(this->mParticleHandle, kPtclSlideDust, this->mPosX,
                                                this->mPosY + 0x32000, this->mPosZ, 0, 0);
        if (ApproachLinear(this->mHorzSpeed, 0, 0x1000) != 0)
            this->mStep = 2;
        break;
    case 2:
        this->mHorzSpeed = 0;
        if (Bowser_IsAnimAtLastFrame() != 0) {
            if (this->mVariantID == 2)
                this->mTimer = 0xa;
            else
                this->mTimer = 0x1e;
            if (this->mStepCounter > this->mTimer) {
                this->mState = BOWSER_STATE_IDLE;
                this->mAnimSpeed = 0x1000;
            }
            this->mStepCounter++;
        }
        break;
    default:
        break;
    }

    if (this->mWithMeshClsn.IsOnGround())
        return;
    this->mState = BOWSER_STATE_PLAY_ANIM_1B;
    this->mPosX = this->mLastGroundPosX;
    this->mPosY = this->mLastGroundPosY;
    this->mPosZ = this->mLastGroundPosZ;
    this->mHorzSpeed = 0;
}

/*
Bomb hit test: find the closest KIRAI actor (0x11c, daKirai_c); if it reports
that his position is close enough (IsInTriggerRange), set it off
(Explode) and return 1, otherwise return 0. */
int daKpa_c::CheckHitMine() {
    Vector3 v;
    dActor_c *closest = this->ClosestWithActorID(BOWSER_ACTOR_KIRAI);
    if (closest) {
        v.x = this->mPosX;
        v.y = this->mPosY;
        v.z = this->mPosZ;
        if (((daKirai_c *)closest)->IsInTriggerRange(&v)) {
            ((daKirai_c *)closest)->Explode();
            return 1;
        }
    }
    return 0;
}

/*
State BOWSER_STATE_THROWN (1), the handler that runs after the tail lets go.
mStepCounter is cleared for the first two frames. The ground height under him is
found with a dBgCh_Gnd probe from 0x96000 (150 units) above mHomePosY when he is
above mHomePosY (a floor at most 0x64000 = 100 units below mHomePosY counts), else
his own Y. Step 0: play animation 9; when he is more than 0xc8000 (200 units)
above that height or mDistToCenter exceeds 0xdac000 (3500 units), pull
mHorzSpeed down to 0x1e000 by 0x4000 per frame if it is higher; run the
landing-dust helper; once on the ground without a fresh hit, zero the speed,
clear mBounceOnLand, advance and play animation 0xd once. Step 1: BOWSER_STATE_IDLE
at the last frame. Every frame, if CheckHitMine reports a bomb hit, mHealth
drops by one: BOWSER_STATE_DEFEATED at 0 or below, BOWSER_STATE_HURT_HOP
otherwise. */
void daKpa_c::ActThrown() {
    if (this->mTimer < 2) {
        this->mStepCounter = 0;
    }

    int referenceY = this->mPosY;
    if (this->mPosY > this->mHomePosY) {
        Vector3 v;
        dBgCh_Gnd rg;
        int base = this->mHomePosY;
        int zz = this->mPosZ;
        int xx = this->mPosX;
        int yy = base + 0x96000;
        v.x = xx;
        v.y = yy;
        v.z = zz;
        rg.SetObjAndPos(v, this);
        if (rg.DetectClsn() != 0) {
            int hy = rg.clsnY;
            if (hy >= this->mHomePosY - 0x64000)
                referenceY = hy;
        }
    }

    if (this->mStep == 0) {
        InitAnimation(9, 0);

        int dy = this->mPosY - referenceY;
        if (dy > 0xc8000 || this->mDistToCenter > 0xdac000) {
            if (this->mHorzSpeed >= 0x1e000) {
                ApproachLinear(this->mHorzSpeed, 0x1e000, 0x4000);
            }
        }

        BounceEffects((char *)&this->mStepCounter);

        if (this->mWithMeshClsn.IsOnGround() != 0) {
            if (this->mWithMeshClsn.JustHitGround() == 0) {
                this->mHorzSpeed = 0;
                this->mBounceOnLand = 0;
                this->mStep++;
                InitAnimation(0xd, 0x40000000);
            }
        }
    } else {
        if (Bowser_IsAnimAtLastFrame() != 0) {
            this->mState = BOWSER_STATE_IDLE;
        }
    }

    if (CheckHitMine() != 0) {
        this->mHealth--;
        if (this->mHealth <= 0) {
            this->mState = BOWSER_STATE_DEFEATED;
        } else {
            this->mState = BOWSER_STATE_HURT_HOP;
        }
    }
}

/*
Keep him out of sight under the arena during BOWSER_STATE_RECOVER: target
opacity 0; once mOpacity is 0, zero both speeds, put him 0x3e8000 (1000 units)
below mHomePosY and, if he is 0xed8000 (3800 units) or more from the origin,
pull X and Z back to the circle of radius 0xed8 (3800) in his direction from the
origin. */
void daKpa_c::SetGoalInvisible() {
    this->mTargetOpacity = 0;
    if (this->mOpacity != 0) return;
    this->mHorzSpeed = 0;
    this->mVertSpeed = 0;
    this->mPosY = this->mHomePosY - 0x3e8000;
    if (Vec3_HorzLen(&mPos) < 0xed8000)
        return;
    {
        Vector3 zero;
        int a;
        zero.x = 0;
        zero.y = 0;
        zero.z = 0;
        a = (unsigned short)Vec3_HorzAngle(&zero, &mPos) >> 4;
        this->mPosX = (short)FX_SinIdx(a) * (short)0xed8;
        this->mPosZ = (short)FX_CosIdx(a) * (short)0xed8;
    }
}

/*
State BOWSER_STATE_RECOVER (2), entered when he has fallen 1000 units below the
arena (CheckFallenOffStage). Each frame: ground = mHomePosY - 0x7d0000 (2000
units below), unless he is at or above mHomePosY and a dBgCh_Gnd probe from
0x96000 (150 units) above mHomePosY finds a floor, whose height then replaces it;
hit is set if that floor has a collision ID; BOWSER_COND_RECOVERING is set.
Step 0 (also hides him with SetGoalInvisible): zero mAngleX/Z on the first frame, then add 0x800
(11.25 degrees) to both each frame until mAngleX wraps back to 0 (32 frames),
and advance. Step 1: animation 0xb once; from frame 0x20 face the centre, set
mVertSpeed 0x28000 (40.0), sound 0xb1, gravity 0, target opacity 0xff, clear
mStepCounter and advance (before that, keep hiding him). Step 2: while mPosY >=
mHomePosY, gravity -0x1000 (-1.0) and steer mHorzSpeed (toward 0 by 0x5000 when
mDistToCenter is under 0x8fc000 / 2300 units for variant 1 or 0x9c4000 / 2500
units otherwise and the floor is within 0x64000 = 100 units of mHomePosY,
otherwise toward 0x4b000 = 75.0 by 0x2000); on landing (BowserLand)
advance, run SpawnShockwave if the probe found no floor ID, send variant 2
into BOWSER_STATE_JUMP with mAnimSpeed 0x4000 (4.0) and normal gravity -0x2000
when it did, and send variant 1 into BOWSER_STATE_ARENA_TILT (recording the
KOOPA2BG actor); ResetFallenOffStage runs every frame of the step. Step 3: at the
last frame, BOWSER_STATE_IDLE, BOWSER_COND_RECOVERING cleared, gravity -0x2000. */
void daKpa_c::ActJumpOntoStage() {
    Vector3 pos;
    int hit;
    int ground;

    ground = this->mHomePosY - 0x7d0000;
    hit = 0;
    if (this->mPosY >= this->mHomePosY) {
        dBgCh_Gnd rc;
        {
            int pz = this->mPosZ;
            int py = this->mHomePosY + 0x96000;
            int px = this->mPosX;
            pos.x = px;
            pos.y = py;
            pos.z = pz;
        }
        rc.SetObjAndPos(pos, this);
        if (rc.DetectClsn()) {
            ground = rc.clsnY;
            /* GetClsnID reads the hit record, dBgCh_Gnd's dBgPi base at +0x10. */
            if ((int)rc.GetClsnID() != -1)
                hit = 1;
        }
    }

    this->mCondFlags |= BOWSER_COND_RECOVERING;

    switch (this->mStep) {
    case 0:
    {
        if (this->mTimer == 0) {
            this->mAngleZ = 0;
            this->mAngleX = this->mAngleZ;
        }
        this->mAngleX += 0x800;
        this->mAngleZ += 0x800;
        if ((this->mAngleX & 0xffff) == 0)
            (this->mStep)++;
        SetGoalInvisible();
        return;
    }
    case 1:
        InitAnimation(0xb, 0x40000000);
        if ((((u32)this->mAnmModel.currFrame << 4) >> 16) >= 0x20) {
            this->mAngleY = this->mAngleToCenter;
            this->mPrevAngleY = this->mAngleY;
            this->mVertSpeed = 0x28000;
            func_02012694(BOWSER_SND_LEAP, &mCamSpacePos);
            this->mVertAccel = 0;
            this->mTargetOpacity = 0xff;
            this->mStepCounter = 0;
            (this->mStep)++;
            return;
        }
        SetGoalInvisible();
        return;
    case 2:
    {
        int thr;
        if (this->mVariantID == 1) thr = 0x8fc000; else thr = 0x9c4000;
        if (this->mPosY >= this->mHomePosY) {
            this->mVertAccel = -0x1000;
            if (this->mDistToCenter < thr) {
                int d = ground - this->mHomePosY;
                if (d < 0) d = -d;
                if (d < 0x64000)
                    ApproachLinear(this->mHorzSpeed, 0, 0x5000);
                else
                    ApproachLinear(this->mHorzSpeed, 0x4b000, 0x2000);
            } else {
                ApproachLinear(this->mHorzSpeed, 0x4b000, 0x2000);
            }
        }
        if (BowserLand()) {
            (this->mStep)++;
            if (hit == 0) {
                SpawnShockwave();
            } else {
                if (this->mVariantID == 2) {
                    this->mState = BOWSER_STATE_JUMP;
                    this->mAnimSpeed = 0x4000;
                    this->mVertAccel = -0x2000;
                }
            }
            if (this->mVariantID == 1) {
                dActor_c *r;
                this->mState = BOWSER_STATE_ARENA_TILT;
                r = dActor_c::FindWithActorID(BOWSER_ACTOR_KOOPA2BG, 0);
                if (r) this->mArenaBgUniqueID = r->uniqueID;
                this->mVertAccel = -0x2000;
            }
        }
        ResetFallenOffStage();
        return;
    }
    case 3:
        if (Bowser_IsAnimAtLastFrame() != 0) {
            this->mState = BOWSER_STATE_IDLE;
            this->mCondFlags &= ~BOWSER_COND_RECOVERING;
            this->mVertAccel = -0x2000;
        }
        return;
    }
}

/*
State BOWSER_STATE_PLAY_ANIM_0F (3): play animation 0xf looping until its last
frame, then BOWSER_STATE_IDLE. */
void daKpa_c::ActDance() {
    InitAnimation(0xf, 0);
    if (Bowser_IsAnimAtLastFrame() != 0) {
        this->mState = BOWSER_STATE_IDLE;
    }
}

/*
The defeat payoff. Switch the body collider off. Variant 2: spawn the LAST_STAR
actor (0x11b, parameter 7) 0xa0000 (160 units) above him. Other variants:
particles 0xad and 0xae 50 units above him, an OBJ_KEY actor (0x11a, parameter =
variant) at his position that receives his mSpinSpeed (daObjKey_c::mSpinSpeed),
and sound 0xbb. */
void daKpa_c::SpawnCollectable() {
    Vector3 base = mPos;
    base.y += 0x32000;

    mdCcAcPos_c.flags |= BOWSER_CC_DISABLED;

    if (mVariantID == 2) {
        Vector3 pos = mPos;
        pos.y += 0xa0000;
        dActor_c::Spawn(BOWSER_ACTOR_LAST_STAR, 7, pos, 0, mAreaId, -1);
    } else {
        Particle::System::NewSimple(kPtclBowserDefeatA, base.x, base.y, base.z);
        Particle::System::NewSimple(kPtclBowserDefeatB, base.x, base.y, base.z);
        daObjKey_c *key =
            (daObjKey_c *)dActor_c::Spawn(BOWSER_ACTOR_OBJ_KEY, mVariantID, mPos, 0, mAreaId, -1);
        key->mSpinSpeed = mSpinSpeed;
        func_02012694(BOWSER_SND_DEFEATED, &mCamSpacePos);
    }
}

/*
Defeat step 0 (from ActDead): knock-back launch. Animation 0 once,
mHorzSpeed -0x1c000 (-28.0) for variant 2 and -0x19000 (-25.0) otherwise,
mVertSpeed 0x50000 (80.0), gravity -0x2000 (-2.0), mAngleY = angle to the centre
+ 0x8000 (so the negative speed carries him toward the centre), clear
mStepCounter, advance, widen the body collider's radius to 0xb4000 (180 units)
and play sound 0xb1. */
void daKpa_c::FlyBackDead() {
    InitAnimation(0, 0x40000000);
    if (this->mVariantID == 2)
        this->mHorzSpeed = -0x1c000;
    else
        this->mHorzSpeed = -0x19000;
    this->mVertSpeed = 0x50000;
    this->mVertAccel = -0x2000;
    this->mAngleY = (short)(this->mAngleToCenter + 0x8000);
    this->mStepCounter = 0;
    this->mStep += 1;
    this->mdCcAcPos_c.radius = 0xb4000;
    func_02012694(BOWSER_SND_LEAP, &mCamSpacePos);
}

/*
Defeat step 1: airborne after the launch. Sets unk_422, runs the landing-dust
helper on mStepCounter; on a fresh ground hit it starts animation 5 once when the
current animation is animation 0 (data_ov060_0211ac88 is entry 0 of the
animation table) and halves mHorzSpeed; once on the ground without a fresh hit
it zeroes the speed and advances. LoopLayDownAnimation runs every call. */
void daKpa_c::DeadBounce() {
    this->unk_422 = 1;
    BounceEffects((char *)&this->mStepCounter);
    if (this->mWithMeshClsn.JustHitGround()) {
        if ((int)this->mAnmModel.file == (int)data_ov060_0211ac88.filePtr) {
            InitAnimation(5, 0x40000000);
        }
        this->mHorzSpeed >>= 1;
    }
    if (this->mWithMeshClsn.IsOnGround()) {
        if (!this->mWithMeshClsn.JustHitGround()) {
            this->mHorzSpeed = 0;
            this->mStep++;
        }
    }
    LoopLayDownAnimation();
}

/*
Defeat step 2: wait for the daPly_c. LoopLayDownAnimation runs first. Returns 1
when the current animation is animation 3 (data_ov060_0211ac60 is entry 3 of the
animation table), a target daPly_c exists, he is within 0x258000 (600 units)
horizontally and the daPly_c's facing differs from the angle to the daPly_c by
more than 0x6000 (135 degrees), that is, he is looking toward Bowser; otherwise
0. mStepCounter is cleared on every call. */
int daKpa_c::DeadWaitForMario() {
    int r = 0;
    LoopLayDownAnimation();
    if ((int)this->mAnmModel.file == (int)data_ov060_0211ac60.filePtr) {
        char* player = (char *)this->mTargetPlayer;
        if (player != 0) {
            int* targetPos = &this->mTargetPlayer->mPosX;
            struct Vector3 v;
            s16 ang;
            v.x = targetPos[0];
            v.y = targetPos[1];
            v.z = targetPos[2];
            ang = this->mTargetPlayer->mAngleY;
            if (Vec3_HorzDist(&mPos, &v) < 0x258000) {
                if (this->GetSubtraction(ang, this->mAngleToTarget) > 0x6000) r = 1;
            }
        }
    }
    this->mStepCounter = 0;
    return r;
}

/*
Defeat shrink-and-spin for variants 0 and 1, per frame; returns 1 once done.
Particle 0x99 is spawned 50 units above him each frame (mParticleHandle).
Once mScaleX is below 0xccc (about 0.8), mSpinSpeed grows by 0x80 per frame.
While mScaleX is above 0x334 (about 0.2) mScaleX and mScaleZ shrink by 0x52 per
frame; after that mScaleY shrinks by 0x29, he rises at 0xa000 (10.0) and gravity
is 0. The result is 1 once mScaleY is below 0x800 (0.5). mAngleY advances by
mSpinSpeed, mOpacity drops by 2 while above 2, and until done the looping sound
0xba plays (mSoundHandle is reset when the sound ID changes). */
int daKpa_c::DeadTwirlUp() {
    int done = 0;
    Vector3 pos = mPos;
    pos.y += 0x32000;

    mParticleHandle = Particle::System::New(mParticleHandle, 0x99, pos.x, pos.y, pos.z, 0, 0);

    if (this->mScaleX < 0xccc) {
        this->mSpinSpeed += 0x80;
    }

    if (this->mScaleX > 0x334) {
        this->mScaleX -= 0x52;
        this->mScaleZ -= 0x52;
    } else {
        this->mScaleY -= 0x29;
        this->mVertSpeed = 0xa000;
        this->mVertAccel = 0;
    }

    if (this->mScaleY < 0x800)
        done = 1;

    this->mAngleY += this->mSpinSpeed;

    if (this->mOpacity > 2) {
        this->mOpacity -= 2;
    }

    if (done == 0) {
        if (this->mSoundID != BOWSER_SND_SHRINK_LOOP)
            this->mSoundHandle = 0;
        this->mSoundID = BOWSER_SND_SHRINK_LOOP;
        this->mSoundHandle =
            Sound::PlayLong(this->mSoundHandle, 3, this->mSoundID, this->mCamSpacePos, 0);
    }
    return done;
}

/*
Park him after the defeat: X and Z set to 0 (Y kept), scale 0, both speeds and
gravity 0, no shadow, body collider switched off. */
void daKpa_c::DeadHide() {
    int y = this->mPosY;

    this->mPosX = 0;
    this->mPosY = y;
    this->mPosZ = 0;
    this->mScaleX = 0;
    this->mScaleY = 0;
    this->mScaleZ = 0;
    this->mHorzSpeed = 0;
    this->mVertSpeed = 0;
    this->mVertAccel = 0;
    this->mDropsShadow = 0;
    this->mdCcAcPos_c.flags |= BOWSER_CC_DISABLED;
}

/*
Defeat conversation and shrink for variants 0 and 1 (defeat step 3). mStepCounter
is the phase: while it is 0 or 1 the conversation runs by mTalkStep, then
DeadTwirlUp runs each frame and, when it returns 1,
DeadHide and SpawnCollectable run and this returns 1. Talk step 0:
daPly_c::StartTalk(this, 1); on success change the music volume (0x14, 0x15666)
and advance. Step 1: once GetTalkState() is 0, show message 0xcd (variant 0) or
0xcf at his position; advance on success. Step 2: when GetTalkState() returns -1,
move to the next phase, advance, play animation 4 once, stop the loaded music
layer (0x3c) and set the music volume (0x7f, 0x7222). */
int daKpa_c::DeadDefaultStageEnding() {
    int ret = 0;
    u16 outer = this->mStepCounter;

    if (outer <= 1) {
        u8 inner;

        if (outer == 0) {
            this->mStepCounter++;
            this->mTalkStep = 0;
        }

        inner = this->mTalkStep;
        switch (inner) {
        case 0:
            if (this->mTargetPlayer->StartTalk(*this, 1)) {
                Sound::ChangeMusicVolume(0x14, 0x15666);
                this->mTalkStep++;
            }
            break;

        case 1:
            if (this->mTargetPlayer->GetTalkState() == 0) {
                int msg = (this->mVariantID == 0) ? 0xcd : 0xcf;
                if (this->mTargetPlayer->ShowMessage(*this, msg, &mPos, 0, 2))
                    this->mTalkStep++;
            }
            break;

        case 2:
            if (this->mTargetPlayer->GetTalkState() == -1) {
                this->mStepCounter++;
                this->mTalkStep++;
                InitAnimation(4, 0x40000000);
                Sound::StopLoadedMusic_Layer1(0x3c);
                Sound::ChangeMusicVolume(0x7f, 0x7222);
            }
            break;
        }
    } else {
        if (DeadTwirlUp()) {
            DeadHide();
            SpawnCollectable();
            ret = 1;
        }
    }

    return ret;
}

/*
Defeat conversation and fade-out for variant 2 (defeat step 0xa). mStepCounter is
the phase as in DeadDefaultStageEnding. Talk step 0: daPly_c::StartTalk(this, 1).
Step 1: once GetTalkState() is 0, show message 0xd1, or 0xd2 when NumStars()
is 0x96 (150); when that succeeds also call Message::PrepareTalk. Step 2: when
GetTalkState() returns -1, stop the loaded music layer (0x3c), run SpawnCollectable (the star), move
to the next phase, advance and play animation 4 once. After the conversation: while
mOpacity is above 4 it drops by 4 per frame with particle 0x99 50 units above him;
then DeadHide runs and this returns 1. */
int daKpa_c::DeadFinalStageEnding() {
    int ret = 0;
    unsigned short mode = this->mStepCounter;
    Vector3 pos;

    if (mode <= 1) {
        if (mode == 0) {
            this->mStepCounter++;
            this->mTalkStep = 0;
        }

        switch (this->mTalkStep) {
        case 0:
            if (this->mTargetPlayer->StartTalk(*this, 1))
                this->mTalkStep++;
            break;

        case 1:
            if (this->mTargetPlayer->GetTalkState() == 0) {
                unsigned m = (NumStars() != 0x96) ? 0xd1 : 0xd2;
                if (this->mTargetPlayer->ShowMessage(*this, m, &mPos, 0, 2)) {
                    this->mTalkStep++;
                    Message::PrepareTalk();
                }
            }
            break;

        case 2:
            if (this->mTargetPlayer->GetTalkState() == -1) {
                Sound::StopLoadedMusic_Layer1(0x3c);
                SpawnCollectable();
                this->mStepCounter++;
                this->mTalkStep++;
                InitAnimation(4, 0x40000000);
            }
            break;
        }
    } else {
        if (this->mOpacity > 4) {
            this->mOpacity -= 4;
            pos = mPos;
            pos.y += 0x32000;
            mParticleHandle =
                Particle::System::New(mParticleHandle, 0x99, pos.x, pos.y, pos.z, 0, 0);
        } else {
            DeadHide();
            ret = 1;
        }
    }

    return ret;
}

/*
State BOWSER_STATE_DEFEATED (4), by mStep. 0: FlyBackDead (launch). 1:
DeadBounce (airborne until landed). 2: DeadWaitForMario; when it
returns 1, clear mStepCounter and go to step 0xa for variant 2, else step 3. 3:
DeadDefaultStageEnding, advancing when it returns 1. 0xa: DeadFinalStageEnding,
advancing to 0xb when it returns 1. 0xb: nothing. */
void daKpa_c::ActDead() {
    switch (this->mStep) {
    case 0:
        FlyBackDead();
        break;
    case 1:
        DeadBounce();
        break;
    case 2:
        if (DeadWaitForMario() == 0)
            break;
        this->mStepCounter = 0;
        if (this->mVariantID == 2) { this->mStep = 0xa; break; }
        this->mStep++;
        break;
    case 3:
        if (DeadDefaultStageEnding() == 0)
            break;
        this->mStep++;
        break;
    case 10:
        if (DeadFinalStageEnding() != 0)
            this->mStep++;
        break;
    case 11: break;
    }
}

/*
Tilt the arena: find the KOOPA2BG actor by mArenaBgUniqueID (nothing if it is
gone) and set its mAngleXSpeed to tiltSpeed * cos and its mAngleZSpeed to
-(tiltSpeed * sin) of the angle mAngleToCenter + 0x8000 (the opposite of the direction to the
centre), both Fix12 products shifted back down by 12. */
void daKpa_c::TiltPlatform(int tiltSpeed) {
    daKpa2Bg_c *arena = (daKpa2Bg_c *)dActor_c::FindWithID(this->mArenaBgUniqueID);
    if (arena == 0)
        return;
    s16 angle = mAngleToCenter + 0x8000;
    arena->mAngleXSpeed = (tiltSpeed * FX_CosIdx((u16)angle >> 4)) >> 12;
    arena->mAngleZSpeed = (tiltSpeed * -FX_SinIdx((u16)angle >> 4)) >> 12;
}

/*
State BOWSER_STATE_ARENA_TILT (0x13): the arena rocks for 186 frames. mVertSpeed
is held at mTerminalVelocity. If the KOOPA2BG actor is gone, back to
BOWSER_STATE_IDLE. Otherwise the table at data_ov060_02119294 (rows of three
halfwords: sign, scale, end frame; a row with end frame 0 ends it) is walked for
the first row whose end frame is above mTimer; the tilt speed v is scale times a
linear ramp in mTimer (counting down to the row's end for a positive sign, up
from the previous row's end otherwise) and goes to TiltPlatform; on odd
frames with v non-zero the arena plays its tilt sound (PlayTiltSound). When no row is
left, go to BOWSER_STATE_IDLE and zero the arena's three angular speeds and three
angles, and mVertSpeed. */
/* p is volatile so the walk reloads the sentinel for each test -- lifetimes
 * on would keep p[2] in a register and skip the ROM's second load, and the
 * flag and the computed value are split so the flag keeps r6. */
void daKpa_c::ActTiltLavaPlatform() {
    daKpa2Bg_c* found; int i; int flag; volatile short* p;
    this->mVertSpeed = this->mTerminalVelocity;
    found = (daKpa2Bg_c*)dActor_c::FindWithID(this->mArenaBgUniqueID);
    if (found == 0) { this->mState = BOWSER_STATE_IDLE; return; }
    p = data_ov060_02119294;
    i = 0;
    flag = 1;
    while (p[2] != 0) {
        int timer = this->mTimer;
        if (timer < p[2]) {
            int off = i * 6;
            short a = *(short*)((char*)data_ov060_02119294 + off);
            short b = *(short*)((char*)data_ov060_02119296 + off);
            int v;
            if (a > 0) {
                v = (short)(b * (*(short *)((char *)data_ov060_02119298 + off) - 1 - timer));
            } else {
                i -= 1; off = i * 6;
                v = (short)(b * (timer - *(short *)((char *)data_ov060_02119298 + off)));
            }
            TiltPlatform(v);
            if (v != 0 && (this->mTimer & 1)) {
                found->PlayTiltSound();
            }
            flag = 0; break;
        }
        p += 3;
        i += 1;
    }
    if (flag != 0) {
        short* q = &found->mAngleX;
        this->mState = BOWSER_STATE_IDLE;
        found->mAngleXSpeed = 0;
        found->mAngleYSpeed = 0;
        found->mAngleZSpeed = 0;
        q[0] = 0; q[1] = 0; q[2] = 0;
        this->mVertSpeed = 0;
    }
}

/*
True when he has fallen 0x3e8000 (1000 units) below mHomePosY while not already
in BOWSER_STATE_RECOVER or BOWSER_STATE_ARENA_TILT. (The result of the
IsOnGround call is discarded.) */
int daKpa_c::CheckFallenOffStage() {
    int state = this->mState;
    if (state != BOWSER_STATE_RECOVER && state != BOWSER_STATE_ARENA_TILT) {
        if (this->mPosY < this->mHomePosY - 0x3e8000) return 1;
        this->mWithMeshClsn.IsOnGround();
    }
    return 0;
}

/*
Hold state BOWSER_HOLD_NONE (0), the normal per-frame handler, called through the
hold table by BowserLoop. Clears mHoldStep. Unless he is defeated, if the
body collider names an owner that is a daPly_c (actorID 0xbf), hurt him
(daPly_c::Hurt at Bowser's position, arguments 2, 0x8000, 1, 0, 1). Then call the
handler for mState from the table at data_ov060_0211aed4 (each row is a slot and
a packed target; target bit 0 selects a call through the vtable slot, the rest
is the this-adjustment), count mTimer, and on a state change reset mStep and
mTimer. Then: dActor_c::UpdatePos with the body collider; update the floor
collider (variant 1 uses func_02038408, others the continuous update); on the
ground, copy the floor normal into mGroundNormal and remember mLastGroundPos;
with mBounceOnLand set, a fresh ground hit makes mVertSpeed -60 percent of itself,
at most 0x14000 (20.0); otherwise, when the floor normal's Y is non-zero,
mVertSpeed is recomputed from the floor normal and mSpeedX/mSpeedZ, minus 0x8000.
For the states whose byte is 1 in data_ov060_02119268 (0, 3, 7..0xb, 0xe..0x10,
0x12 and 0x13), if he has left the ground he is put back at mLastGroundPos and
moved 8 units toward the centre (the sin/cos table entry shifted left 3), else
mLastGroundPos is refreshed. Finally, CheckFallenOffStage sends him to
BOWSER_STATE_RECOVER. */
void daKpa_c::FreeUpdate() {
    this->mHoldStep = 0;
    u32 id;
    if (this->mState != BOWSER_STATE_DEFEATED && (id = this->mdCcAcPos_c.otherOwner) != 0) {
        daPly_c *f = static_cast<daPly_c *>(dActor_c::FindWithID(id));
        if (f != 0) {
            int b = (((dActor_c *)f)->actorID == BOWSER_ACTOR_PLAYER);
            if (b) {
                Vector3 v;
                v.x = this->mPosX;
                v.y = this->mPosY;
                v.z = this->mPosZ;
                f->Hurt(v, 2, 0x8000, 1, 0, 1);
            }
        }
    }

    s32 idx = this->mState;
    (this->*data_ov060_0211aed4[idx].pmf)();

    this->mTimer++;
    if (this->mState != idx) {
        this->mStep = 0;
        this->mTimer = 0;
    }

    this->UpdatePos(&this->mdCcAcPos_c);

    if (this->mVariantID == 1)
        func_02038408(&this->mWithMeshClsn);
    else
        dBgCh_Actr_UpdateContinuous_Veneer(&this->mWithMeshClsn);

    if (this->mWithMeshClsn.IsOnGround()) {
        void* fr = this->mWithMeshClsn.GetFloorResult();
        ((dBgPi *)fr)->surface.CopyNormalTo(*(Vector3 *)&this->mGroundNormalX);
        this->mLastGroundPosX = this->mPosX;
        this->mLastGroundPosY = this->mPosY;
        this->mLastGroundPosZ = this->mPosZ;
        if (this->mBounceOnLand != 0 && this->mWithMeshClsn.JustHitGround()) {
            this->mVertSpeed = (this->mVertSpeed * -60) / 100;
            if (this->mVertSpeed >= 0x14000)
                this->mVertSpeed = 0x14000;
        } else if (this->mGroundNormalY != 0) {
            this->mVertSpeed = -(FX_Div(FX_MUL(this->mGroundNormalX, this->mSpeedX) +
                                            FX_MUL(this->mGroundNormalZ, this->mSpeedZ),
                                        this->mGroundNormalY) +
                                 0x8000);
        }
    }

    if (data_ov060_02119268[idx] != 0) {
        if (!this->mWithMeshClsn.IsOnGround()) {
            mPosX = mLastGroundPosX;
            mPosY = mLastGroundPosY;
            mPosZ = mLastGroundPosZ;
            mPosX += FX_SinIdx((u16)mAngleToCenter >> 4) << 3;
            mPosZ += FX_CosIdx((u16)mAngleToCenter >> 4) << 3;
        } else {
            this->mLastGroundPosX = this->mPosX;
            this->mLastGroundPosY = this->mPosY;
            this->mLastGroundPosZ = this->mPosZ;
        }
    }

    if (CheckFallenOffStage() == 0)
        return;
    this->mState = BOWSER_STATE_RECOVER;
    this->mStep = 0;
    this->mTimer = 0;
}

/*
Hold state BOWSER_HOLD_HELD (1): a daPly_c has grabbed the tail and swings him
around. BOWSER_COND_BREATHING is cleared. By mHoldStep: 0 switches the body collider
off, plays sound 0xb2, sets BOWSER_STATE_THROWN, starts animation 0xa and resets
mAnimSpeed, advance; 1 starts animation 9 (looping) once animation 0xa reaches
its last frame, and advances; 2 does nothing. Every frame: mSwingSpeed = the
grabbed daPly_c's mAngleYSpeed, mAngleX = -|mSwingSpeed| (used directly as an angle),
mAngleY = the daPly_c's mAngleY, and his position is the daPly_c's plus 0xa0 (160)
units out along the daPly_c's facing (X and Z) and 0x18000 (24 units) up minus
160 * sin(mAngleX) (Y). */
void daKpa_c::HeldUpdate() {
    int v;
    short a;
    this->mCondFlags &= ~BOWSER_COND_BREATHING;

    switch (this->mHoldStep) {
    case 0:
        this->mdCcAcPos_c.flags |= BOWSER_CC_DISABLED;
        func_02012694(BOWSER_SND_GRABBED, &mCamSpacePos);
        this->mState = BOWSER_STATE_THROWN;
        InitAnimation(0xa, 0);
        this->mAnimSpeed = 0x1000;
        (this->mHoldStep)++;
        break;
    case 1:
        if (Bowser_IsAnimAtLastFrame()) {
            InitAnimation(9, 0);
            (this->mHoldStep)++;
        }
        break;
    case 2:
        break;
    }

    this->mSwingSpeed = this->mGrabbedPlayer->mAngleYSpeed;
    v = this->mSwingSpeed;
    a = this->mGrabbedPlayer->mAngleY;
    if (v < 0) {
        v = -v;
    }
    this->mAngleX = -v;
    this->mAngleY = a;

    mPos = mGrabbedPlayer->mPos;
    mPosX += FX_SinIdx((u16)a >> 4) * 0xa0;
    mPosY += 0x18000 - FX_SinIdx((u16)mAngleX >> 4) * 0xa0;
    mPosZ += FX_CosIdx((u16)a >> 4) * 0xa0;
}

/*
Hold states BOWSER_HOLD_RELEASE_NOW (2) and BOWSER_HOLD_RELEASE_AFTER_ANIM (3):
the release. Plays animation 0xe once; state 3 waits for its last frame. Then: clear
mHoldStep and mHoldState, switch the body collider back on, set
BOWSER_STATE_THROWN, and launch him: v = |mSwingSpeed| * 0x46 / 6000, times 2.5
when v is above 0x2d; mHorzSpeed = v * cos(mAngleX) and mVertSpeed =
-v * sin(mAngleX) (table entries at mAngleX >> 4). The tail (by mTailUniqueID) goes
to BOWSER_TAIL_COOLDOWN with mTimer 0; mAngleX, mTimer and mStep are cleared and
the floor collider's ground flag is cleared. */
void daKpa_c::ThrownDroppedUpdate() {
    int v;
    daKpaTail_c *tail;

    InitAnimation(0xe, 0x40000000);
    if (this->mHoldState == BOWSER_HOLD_RELEASE_AFTER_ANIM) {
        if (Bowser_IsAnimAtLastFrame() == 0)
            return;
    }
    this->mHoldStep = 0;
    this->mHoldState = BOWSER_HOLD_NONE;
    this->mdCcAcPos_c.flags &= ~BOWSER_CC_DISABLED;
    this->mState = BOWSER_STATE_THROWN;
    v = this->mSwingSpeed;
    if (v < 0)
        v = -v;
    v = v * 0x46 / 6000;
    if (v > 0x2d)
        v = v * 0x19 / 10;
    this->mHorzSpeed = v * FX_CosIdx((u16)mAngleX >> 4);
    this->mVertSpeed = -v * FX_SinIdx((u16)mAngleX >> 4);
    tail = (daKpaTail_c *)dActor_c::FindWithID(this->mTailUniqueID);
    if (tail != 0) {
        tail->mState = BOWSER_TAIL_COOLDOWN;
        tail->mTimer = 0;
    }
    this->mAngleX = 0;
    this->mTimer = 0;
    this->mStep = 0;
    this->mWithMeshClsn.ClearGroundFlag();
}

/*
Per-frame conditions, hold-state dispatch and opacity fade. Measure the
horizontal distance and angle to the arena centre (the origin) into
mDistToCenter and mAngleToCenter and rebuild the low byte of mCondFlags (see
Bowser_CondFlag) from the four tests. Run the handler for mHoldState through the
table at data_ov060_0211aeb4 (0: FreeUpdate, 1: HeldUpdate, 2
and 3: ThrownDroppedUpdate). Unless he is defeated, move mOpacity toward
mTargetOpacity by 0x14 (20) per frame, clamping at 0xff and 0. */
void daKpa_c::BowserLoop() {
    Vector3 zero;
    zero.x = 0;
    zero.y = 0;
    zero.z = 0;
    this->mDistToCenter = Vec3_HorzDist(&this->mPos, &zero);
    this->mAngleToCenter = Vec3_HorzAngle(&this->mPos, &zero);

    int toTarget = this->GetSubtraction(this->mAngleY, this->mAngleToTarget);
    int toCenter = this->GetSubtraction(this->mAngleY, this->mAngleToCenter);

    this->mCondFlags &= ~0xff;
    if (toTarget < 0x2000)
        this->mCondFlags |= BOWSER_COND_FACING_TARGET;
    if (toCenter < 0x3800)
        this->mCondFlags |= BOWSER_COND_FACING_CENTER;
    if (this->mDistToCenter < 0x3e8000)
        this->mCondFlags |= BOWSER_COND_NEAR_CENTER;
    if (this->mDistToTarget < 0x352000)
        this->mCondFlags |= BOWSER_COND_TARGET_NEAR;

    (this->*data_ov060_0211aeb4[this->mHoldState].pmf)();

    if (this->mState == BOWSER_STATE_DEFEATED) return;

    /* The steps are written `mOpacity += 0x14` / `-= 0x14` rather than storing
     * v: replacing the `+= 0x14` with `= v` builds 28 bytes shorter, and the
     * `-= 0x14` with `= v` 16 bytes shorter (both measured). */
    unsigned char cur = this->mOpacity;
    unsigned char target = this->mTargetOpacity;
    if (target == cur) return;
    if (target > cur) {
        int v = cur + 0x14;
        if (v >= 0xff) {
            this->mOpacity = 0xff;
            return;
        }
        this->mOpacity += 0x14;
        return;
    }
    {
        int v = cur - 0x14;
        if (v <= 0) {
            this->mOpacity = 0;
        } else {
            this->mOpacity -= 0x14;
        }
    }
}

/*
Fight-start values, called from InitResources: mPickToggle 1, fully opaque, variant
3 treated as 0, mHealth from the byte table at data_ov060_02119264 (1, 1, 3 in the
ROM, indexed by variant), state BOWSER_STATE_INTRO_WAIT, no hold, and unk_420 and
unk_422 cleared. */
void daKpa_c::InitFightState() {
    this->mPickToggle = 1;
    this->mOpacity = 0xff;
    this->mTargetOpacity = 0xff;
    if (this->mVariantID == 3) this->mVariantID = 0;
    this->mHealth = data_ov060_02119264[this->mVariantID];
    this->mState = BOWSER_STATE_INTRO_WAIT;
    this->mHoldState = BOWSER_HOLD_NONE;
    this->unk_420 = 0;
    this->unk_422 = 0;
}

/*
Called when the tail lets go of a daPly_c: set mBounceOnLand (FreeUpdate
then bounces him when he next hits the ground) and forget the grabbed daPly_c. */
void daKpa_c::OnTailReleased() {
    this->mBounceOnLand = 1;
    this->mGrabbedPlayer = 0;
}

/*
Body collider switch: a non-zero argument clears the disabled bit (dCc_c flags
bit 0) of mdCcAcPos_c, zero sets it. */
void daKpa_c::SetTangible(int enable) {
    if (enable)
        this->mdCcAcPos_c.flags &= ~BOWSER_CC_DISABLED;
    else
        this->mdCcAcPos_c.flags |= BOWSER_CC_DISABLED;
}

/*
When the current animation has finished and is animation 5 (data_ov060_0211acd0
is entry 5 of the animation table), start animation 3 (looping). */
void daKpa_c::LoopLayDownAnimation() {
    int finished = this->mAnmModel.Finished();
    if (!finished) return;
    if ((int)this->mAnmModel.file != (int)data_ov060_0211acd0.filePtr)
        return;
    InitAnimation(3, 0);
}

/*
The intro camera, one step per mCutsceneStep, run each frame by the two intro
states and once more by the conversation. Returns 1 at once when there is no
daPly_c, and otherwise only when step 3 completes. Step 0: Camera::SetFlag_3; aim
the camera at the daPly_c's position with Y replaced by Bowser's own (kept in
mCamLookAt); place the camera 0.625 times the horizontal distance to that point
away from Bowser, at the angle toward it minus 0x2000 (45 degrees), and 0xc8000
(200 units) above him (kept in mCamPos); Camera::SetPos. Step 1: func_020092c4
on the look-at; count frames the daPly_c is not in the air (the count resets
while he is) and go on once it exceeds 0x1e (30). Step 2: ease the camera
position toward a spot at 0.375 times the horizontal distance to the daPly_c (at least 0x1f4000 = 500 units) at the
angle minus 0x1000 (22.5 degrees) by 0x4000 (4.0) per frame per axis, and the
look-at X and Z toward Bowser's by 0x1e000 (30.0); go on when func_020092c4 on
the look-at returns non-zero. Step 3: ease the look-at, by 0xa000 (10.0) per
axis, to a point 0xc0 (192) units ahead along mAngleY and 0xfa000 (250 units) up;
return 1 when func_020092c4 returns non-zero. Step 4: clear bit 0x8 of
Camera::mFlags. */
int daKpa_c::UpdateIntroCamera() {
    dCamera_c *cam = gCamera;
    daPly_c *player = this->ClosestPlayer();
    struct Vector3 sp;
    struct Vector3* pv;
    int dist;
    int v;
    int k;
    int d;

    if (player == 0)
        return 1;

    switch (this->mCutsceneStep) {
    case 0:
        cam->SetFlag_3();
        pv = &((dActor_c *)player)->mPos;
        sp.x = pv->x;
        sp.y = pv->y;
        sp.z = pv->z;
        sp.y = this->mPosY;
        cam->SetLookAt(sp);
        this->mCamLookAtX = sp.x;
        this->mCamLookAtY = sp.y;
        this->mCamLookAtZ = sp.z;
        dist = Vec3_HorzDist(&mPos, &sp);
        k = (unsigned short)(short)(Vec3_HorzAngle(&mPos, &sp) - 0x2000) >> 4;
        v = FX_MUL(dist, 0xA00);
        this->mCamPosX = this->mPosX + FX_MUL(v, FX_SinIdx(k));
        this->mCamPosY = this->mPosY + 0xc8000;
        this->mCamPosZ = this->mPosZ + FX_MUL(v, FX_CosIdx(k));
        cam->SetPos(*(const Vector3 *)(&this->mCamPosX));
        this->mCutsceneStep++;
        break;
    case 1:
        func_020092c4(cam, &cam->lookAt, &this->mCamLookAtX);
        if (player->IsInAir() != 0)
            this->mCutsceneTimer = 0;
        else
            this->mCutsceneTimer++;
        if (this->mCutsceneTimer > 0x1e)
            this->mCutsceneStep++;
        break;
    case 2:
        pv = &((dActor_c *)player)->mPos;
        sp.x = pv->x;
        sp.y = pv->y;
        sp.z = pv->z;
        dist = Vec3_HorzDist(&mPos, &sp);
        d = FX_MUL(dist, 0x600);
        if (d < 0x1f4000)
            d = 0x1f4000;
        k = (unsigned short)(short)(Vec3_HorzAngle(&mPos, &sp) - 0x1000) >> 4;
        sp.x = this->mPosX + FX_MUL(d, FX_SinIdx(k));
        sp.z = this->mPosZ + FX_MUL(d, FX_CosIdx(k));
        ApproachLinear(this->mCamPosX, sp.x, 0x4000);
        ApproachLinear(this->mCamPosZ, sp.z, 0x4000);
        ApproachLinear(this->mCamLookAtX, this->mPosX, 0x1e000);
        ApproachLinear(this->mCamLookAtZ, this->mPosZ, 0x1e000);
        func_020092c4(cam, &cam->pos, &this->mCamPosX);
        if (func_020092c4(cam, &cam->lookAt, &this->mCamLookAtX) != 0)
            this->mCutsceneStep++;
        break;
    case 3: {
        int ty, tz, tx;
        tz = FX_CosIdx((u16)this->mAngleY >> 4) * 0xc0 + this->mPosZ;
        ty = this->mPosY + 0xfa000;
        tx = FX_SinIdx((u16)this->mAngleY >> 4) * 0xc0 + this->mPosX;
        sp.x = tx;
        sp.y = ty;
        sp.z = tz;
    }
        ApproachLinear(this->mCamLookAtX, sp.x, 0xa000);
        ApproachLinear(this->mCamLookAtY, sp.y, 0xa000);
        ApproachLinear(this->mCamLookAtZ, sp.z, 0xa000);
        if (func_020092c4(cam, &cam->lookAt, &this->mCamLookAtX) != 0)
            return 1;
        break;
    case 4:
        cam->mFlags &= ~8;
        break;
    default:
        break;
    }
    return 0;
}

/*
Switch the model to animation idx (entries 0..0x1b of the table at
data_ov060_021192dc; each handle's filePtr is the file) and pick the texture
pattern that goes with it. The third parameter reaches dExtAnmModel_c::SetAnim
unchanged as its flags word: the local `a` below is never assigned, and the ROM
passes the caller's third argument (still in r2) straight through. Callers pass
0 or 0x40000000; the top two bits of an Animation's flag word choose between wrapping and clamping at the end (see
dExtFrameCtrl_c::WillHitFrame), so 0x40000000 reads as play-once and 0 as looping.
Texture pattern by idx: 1 uses data_ov060_0211ac40, 2 acb8 and 0xa ac30 (all
with texture-sequence flags 0x40000000); 3 and 5 use ac10, 0xe uses abf0, and
every other idx (0 included) uses ac28 (these three with flags 0). */
void daKpa_c::InitAnimation(int idx, int animFlags) {
    int a;  /* deliberately never assigned: it is animFlags, still in r2 (see above) */
    this->mAnmModel.SetAnim((BCA_File *)(data_ov060_021192dc[idx]->filePtr), a, 0x1000, 0);
    switch (idx) {
    case 1:
        dExtAnmTexPat_c::Prepare(*data_ov060_0211ac78.bmd.filePtr, *data_ov060_0211ac40.filePtr);
        this->mTextureSequence.SetFile(*data_ov060_0211ac40.filePtr, 0, 0x1000, 0);
        this->mTextureSequence.SetFlags(0x40000000);
        return;
    case 2:
        dExtAnmTexPat_c::Prepare(*data_ov060_0211ac78.bmd.filePtr, *data_ov060_0211acb8.filePtr);
        this->mTextureSequence.SetFile(*data_ov060_0211acb8.filePtr, 0, 0x1000, 0);
        this->mTextureSequence.SetFlags(0x40000000);
        return;
    case 3:
    case 5:
        dExtAnmTexPat_c::Prepare(*data_ov060_0211ac78.bmd.filePtr, *data_ov060_0211ac10.filePtr);
        this->mTextureSequence.SetFile(*data_ov060_0211ac10.filePtr, 0, 0x1000, 0);
        this->mTextureSequence.SetFlags(0);
        return;
    case 14:
        dExtAnmTexPat_c::Prepare(*data_ov060_0211ac78.bmd.filePtr, *data_ov060_0211abf0.filePtr);
        this->mTextureSequence.SetFile(*data_ov060_0211abf0.filePtr, 0, 0x1000, 0);
        this->mTextureSequence.SetFlags(0);
        return;
    case 10:
        dExtAnmTexPat_c::Prepare(*data_ov060_0211ac78.bmd.filePtr, *data_ov060_0211ac30.filePtr);
        this->mTextureSequence.SetFile(*data_ov060_0211ac30.filePtr, 0, 0x1000, 0);
        this->mTextureSequence.SetFlags(0x40000000);
        return;
    case 0:
    default:
        dExtAnmTexPat_c::Prepare(*data_ov060_0211ac78.bmd.filePtr, *data_ov060_0211ac28.filePtr);
        this->mTextureSequence.SetFile(*data_ov060_0211ac28.filePtr, 0, 0x1000, 0);
        this->mTextureSequence.SetFlags(0);
        return;
    }
}

/*
Row number for the mouth fire in daKpaFire_c, or -1. While animation 8 plays
(data_ov060_0211ac20 is entry 8 of the animation table) the row is the whole
frame number minus 0x31 from frame 0x31 on; while animation 6 plays
(data_ov060_0211ac68) it is the frame number plus 0xb; otherwise -1. */
int daKpa_c::GetMouthFireRow() {
    int anim = (int)this->mAnmModel.file;
    if (anim == (int)data_ov060_0211ac20.filePtr) {
        unsigned int frame = ((unsigned int)this->mAnmModel.currFrame << 4) >> 0x10;
        if (frame >= 0x31) return frame - 0x31;
    }
    if (anim == (int)data_ov060_0211ac68.filePtr) {
        return (((unsigned int)this->mAnmModel.currFrame << 4) >> 0x10) + 0xb;
    }
    return -1;
}

/*
Footfall detection, called from Behavior each frame. For six animations the whole
frame number n is tested against windows that pick a foot point: 1 means
mFootPosA, 2 means mFootPosB. Animation 0x11: n 0x14..0x17 is B, n == 0x29 or
n < 2 is A. 0x12: 0xf..0x12 is B. 0x18: n >= 0x1c is A. 0x19: 6..9 is B, n >= 0xf
is A. 0xf: 0x10..0x13 is A, 0x1e..0x21 is B, 0x2d..0x30 is A. 0xd: 0xa..0xd is A,
0x18..0x1b is B. (The six handles data_ov060_0211acb0, ac00, ace0, aca0, abf8 and
ac90 are entries 0x11, 0x12, 0x18, 0x19, 0xf and 0xd of the animation table.)
mFootfallLatch remembers that the previous frame was in a window, so the effect
fires only on entering one: huge landing dust at the chosen foot point, sound
0xb0 at his camera-space position and dActor_c::Earthquake at his position with
the Fix12 argument 0x320000 (800.0). */
void daKpa_c::UpdateFootfalls() {
    /* (unsigned short)(u32 >> 12) forces ROM prologue load order
       (mAnmModel.currFrame at 0x12c into r0, then mFootfallLatch at 0x446 into r1)
       + lsl#4/lsr#16 extract. */
    int n = (unsigned short)((unsigned)this->mAnmModel.currFrame >> 12);
    int wasInWindow = (this->mFootfallLatch != 0) ? 1 : 0;
    int anim = (int)this->mAnmModel.file;
    int foot = 0;
    this->mFootfallLatch = 0;

    if (anim == (int)data_ov060_0211acb0.filePtr) {
        if (n >= 0x14 && n <= 0x17)
            foot = 2;
        else if (n == 0x29 || n < 2)
            foot = 1;
    } else if (anim == (int)data_ov060_0211ac00.filePtr) {
        if (n >= 0xf && n <= 0x12)
            foot = 2;
    } else if (anim == (int)data_ov060_0211ace0.filePtr) {
        if (n >= 0x1c)
            foot = 1;
    } else if (anim == (int)data_ov060_0211aca0.filePtr) {
        if (n >= 6 && n <= 9)
            foot = 2;
        else if (n >= 0xf)
            foot = 1;
    } else if (anim == (int)data_ov060_0211abf8.filePtr) {
        if (n >= 0x10 && n <= 0x13)
            foot = 1;
        else if (n >= 0x1e && n <= 0x21)
            foot = 2;
        else if (n >= 0x2d && n <= 0x30)
            foot = 1;
    } else if (anim == (int)data_ov060_0211ac90.filePtr) {
        if (n >= 0xa && n <= 0xd)
            foot = 1;
        else if (n >= 0x18 && n <= 0x1b)
            foot = 2;
    }

    if (foot == 0)
        return;
    this->mFootfallLatch = 1;
    if (wasInWindow != 0)
        return;

    if (foot == 1) {
        Vector3 dust;
        dust.x = this->mFootPosAX;
        dust.y = this->mFootPosAY;
        dust.z = this->mFootPosAZ;
        this->HugeLandingDustAt(dust, 0);
    } else {
        Vector3 dust;
        dust.x = this->mFootPosBX;
        dust.y = this->mFootPosBY;
        dust.z = this->mFootPosBZ;
        this->HugeLandingDustAt(dust, 0);
    }
    Sound::Play(3, BOWSER_SND_FOOTFALL, this->mCamSpacePos);
    {
        Vector3 quake;
        quake.x = this->mPosX;
        quake.y = this->mPosY;
        quake.z = this->mPosZ;
        Earthquake(quake, 0x320000);
    }
}

/* The static-init globals: one ctor veneer and destructor registration per
 * file handle (the @NNN nodes and .data descriptors are compiler
 * temporaries), then the three pointer-to-member state tables. */
KpaModelFilePtr data_ov060_0211ac78(ASSET_KOOPA_MODEL_BMD);
KpaAnimationFileHandle data_ov060_0211ac88(ASSET_KOOPA_DAMAGE_BCA);
KpaAnimationFileHandle data_ov060_0211ac50(ASSET_KOOPA_DAMAGE2_ST_BCA);
KpaAnimationFileHandle data_ov060_0211ac58(ASSET_KOOPA_DAMAGE2_ED_BCA);
KpaAnimationFileHandle data_ov060_0211ac60(ASSET_KOOPA_DEAD_BCA);
KpaAnimationFileHandle data_ov060_0211ac18(ASSET_KOOPA_DEAD_ED_BCA);
KpaAnimationFileHandle data_ov060_0211acd0(ASSET_KOOPA_DEAD_ST_BCA);
KpaAnimationFileHandle data_ov060_0211ac68(ASSET_KOOPA_FIRE_BCA);
KpaAnimationFileHandle data_ov060_0211ac70(ASSET_KOOPA_FIRE_ED_BCA);
KpaAnimationFileHandle data_ov060_0211ac20(ASSET_KOOPA_FIRE_ST_BCA);
KpaAnimationFileHandle data_ov060_0211ac80(ASSET_KOOPA_HOLD_BCA);
KpaAnimationFileHandle data_ov060_0211abe8(ASSET_KOOPA_HOLD_ST_BCA);
KpaAnimationFileHandle data_ov060_0211ac48(ASSET_KOOPA_JUMP1_BCA);
KpaAnimationFileHandle data_ov060_0211ac38(ASSET_KOOPA_JUMP2_BCA);
KpaAnimationFileHandle data_ov060_0211ac90(ASSET_KOOPA_STAND_UP_BCA);
KpaAnimationFileHandle data_ov060_0211ac98(ASSET_KOOPA_THROW_BCA);
KpaAnimationFileHandle data_ov060_0211abf8(ASSET_KOOPA_VEXING_BCA);
KpaAnimationFileHandle data_ov060_0211aca8(ASSET_KOOPA_WAIT1_BCA);
KpaAnimationFileHandle data_ov060_0211acb0(ASSET_KOOPA_WALK_BCA);
KpaAnimationFileHandle data_ov060_0211ac00(ASSET_KOOPA_WALK_ED_BCA);
KpaAnimationFileHandle data_ov060_0211ac08(ASSET_KOOPA_WALK_ST_BCA);
KpaAnimationFileHandle data_ov060_0211abe0(ASSET_KOOPA_FIRE2_BCA);
KpaAnimationFileHandle data_ov060_0211acd8(ASSET_KOOPA_FIRE3_BCA);
KpaAnimationFileHandle data_ov060_0211acf0(ASSET_KOOPA_FIRE3_ST_BCA);
KpaAnimationFileHandle data_ov060_0211ace8(ASSET_KOOPA_FIRE3_ED_BCA);
KpaAnimationFileHandle data_ov060_0211ace0(ASSET_KOOPA_DUSH_ST_BCA);
KpaAnimationFileHandle data_ov060_0211aca0(ASSET_KOOPA_DUSH_BCA);
KpaAnimationFileHandle data_ov060_0211acc8(ASSET_KOOPA_DUSH_ED_BCA);
KpaAnimationFileHandle data_ov060_0211acc0(ASSET_KOOPA_STRETCH_BCA);
KpaTexSequenceFilePtr data_ov060_0211ac40(ASSET_KOOPA_DAMAGE2_ST_BTP);
KpaTexSequenceFilePtr data_ov060_0211acb8(ASSET_KOOPA_DAMAGE2_ED_BTP);
KpaTexSequenceFilePtr data_ov060_0211ac10(ASSET_KOOPA_DEAD_BTP);
KpaTexSequenceFilePtr data_ov060_0211abf0(ASSET_KOOPA_THROW_BTP);
KpaTexSequenceFilePtr data_ov060_0211ac28(ASSET_KOOPA_WAIT_BTP);
KpaTexSequenceFilePtr data_ov060_0211ac30(ASSET_KOOPA_HOLD_BTP);

/* Indexed by mHoldState (Bowser_HoldState). */
PmfEnt data_ov060_0211aeb4[4] = {
    {&daKpa_c::FreeUpdate},          /* BOWSER_HOLD_NONE */
    {&daKpa_c::HeldUpdate},          /* BOWSER_HOLD_HELD */
    {&daKpa_c::ThrownDroppedUpdate}, /* BOWSER_HOLD_RELEASE_NOW */
    {&daKpa_c::ThrownDroppedUpdate}  /* BOWSER_HOLD_RELEASE_AFTER_ANIM */
};
/* Indexed by mState (Bowser_State). */
PmfEnt data_ov060_0211aed4[20] = {
    {&daKpa_c::ActDefault},           /* BOWSER_STATE_IDLE */
    {&daKpa_c::ActThrown},            /* BOWSER_STATE_THROWN */
    {&daKpa_c::ActJumpOntoStage},     /* BOWSER_STATE_RECOVER */
    {&daKpa_c::ActDance},             /* BOWSER_STATE_PLAY_ANIM_0F */
    {&daKpa_c::ActDead},              /* BOWSER_STATE_DEFEATED */
    {&daKpa_c::ActWait},              /* BOWSER_STATE_INTRO_WAIT */
    {&daKpa_c::ActIntroWalk},         /* BOWSER_STATE_INTRO */
    {&daKpa_c::ActChargeMario},       /* BOWSER_STATE_CHARGE */
    {&daKpa_c::ActSpitFireIntoSky},   /* BOWSER_STATE_FIRE_BREATH */
    {&daKpa_c::ActSpitFireOntoFloor}, /* BOWSER_STATE_FIREBALLS */
    {&daKpa_c::ActHitEdge},           /* BOWSER_STATE_PLAY_ANIM_1B */
    {&daKpa_c::ActTurnFromEdge},      /* BOWSER_STATE_TURN_IN_PLACE */
    {&daKpa_c::ActHitMine},           /* BOWSER_STATE_HURT_HOP */
    {&daKpa_c::ActBigJump},           /* BOWSER_STATE_JUMP */
    {&daKpa_c::ActWalkToMario},       /* BOWSER_STATE_TURN_TO_TARGET */
    {&daKpa_c::ActBreathFire},        /* BOWSER_STATE_ANIM_CHAIN */
    {&daKpa_c::ActTeleport},          /* BOWSER_STATE_VANISH_DASH */
    {&daKpa_c::ActQuickJump},         /* BOWSER_STATE_HOP */
    {&daKpa_c::ActIdle},              /* BOWSER_STATE_WAIT_ANIM_END */
    {&daKpa_c::ActTiltLavaPlatform}   /* BOWSER_STATE_ARENA_TILT */
};
/* Indexed by the tail's mState (BowserTail_State). */
KpaTailState data_ov060_0211ae9c[3] = {
    &daKpaTail_c::func_ov060_02115d68,  /* BOWSER_TAIL_FREE */
    &daKpaTail_c::func_ov060_02115d50,  /* BOWSER_TAIL_COOLDOWN */
    &daKpaTail_c::func_ov060_02115c1c   /* BOWSER_TAIL_HOLDING */
};
