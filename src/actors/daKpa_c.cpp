//cpp
/* daKpa_c + daKpaTail_c -- Bowser and his tail, the ov060 boss-fight actors.
 * ROM span 0x02111900..0x021163f0, 79 functions: both destructor pairs,
 * the shared fight helpers, both classes' resource/load/behavior/render
 * methods, and both InitResources. The class names are the cartridge's
 * own RTTI spellings: ex-coined Bowser/BowserTail, renamed with the
 * aliased-vtable evidence (shared _ZTV addresses, typeinfo slots and
 * ROM-spelled classInit factories), the same S35 shape as daKpaFire_c.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Folded from the 79 legacy one-function sources in ROM order (ordinals
 * 0-78, 0x02111900..0x021163f0), retired at promotion; per-symbol credit
 * survives in attribution.json path#symbol overrides.
 *
 * READABILITY PASS (byte-neutral). Every function below has a comment saying what
 * it does and, where this file shows it, where it is called from; the numbers that name a thing are enums
 * (Bowser_State, Bowser_HoldState, BowserTail_State, Bowser_CondFlag,
 * Bowser_ActorID, Bowser_Sound, Bowser_CcFlag, Bowser_Anim); and the fields are
 * reached through the real members of daKpa_c / daKpaTail_c. In the comments a
 * Fix12 value is read as 0x1000 = 1.0 and an angle as 0x10000 = a full turn.
 *
 * deslop leftovers:
 *  - the legacy func_ov060_* helpers are now daKpa_c / daKpaTail_c members;
 *    their names keep the ROM addresses for tooling, the state tables' PMF
 *    and vtable-slot records resolve through symbols.txt.
 *  - Spellings kept because the bytes depend on them (most are noted at the
 *    function): the daKpaTail_c::Behavior pointer bump to +0x5c and its volatile
 *    array; the `u16 *h = &mTimer; *h = *h + 1` increments; the mOpacity
 *    += / -= 0x14 form; the `+ 0x3fe` write-back pointer in func_ov060_02114b60;
 *    the (unsigned short) and *(u16 *)& angle reads; the new_var / new_var2 temps
 *    and the `dp != 0 && kpa != 0` barrier in func_ov060_021140c0 and
 *    func_ov060_02114858; the volatile locals in func_ov060_021135fc,
 *    func_ov060_021132a4, func_ov060_02112ee0 and func_ov060_02112bfc; the gotos
 *    in func_ov060_02115c1c; the data_ov060_02119294/6/8 + offset reads; and the
 *    opaque Obj view in Bowser_IsAnimAtLastFrame.
 *  - Behavior stores `this` at +0x114 of the object at data_0209f318 (a dCamera_c);
 *    dCamera_c.h has no member there, so it stays a byte offset.
 *  - Callees known only by address, whose purpose this file does not establish:
 *    func_020092c4, func_0200fa04, func_ov002_020c56f0, func_ov060_02117a3c,
 *    func_02038408, func_02011d50, func_02011cfc. Likewise the trailing arguments
 *    of Player::Hurt and Sound::ChangeMusicVolume, the KOOPAFIRE spawn-parameter
 *    bits and bit 0x400 of the tail's mFlags are given as numbers.
 *  - Animation indices stay numbers (only the idle pose, 0x10, is named);
 *    Bowser_Anim lists where each one is used.
 *  - unk_420 and unk_422 of daKpa_c and the dActor_c words at 0xa4 / 0xac are
 *    unnamed: the first two are written here and never read.
 *  - The legacy TUBUILD CONFLICT blocks and shard declarations are left as the
 *    fold wrote them. */

/* TUBUILD NOTE -- codegen is deferred, so .text emits in reverse source
 * order and the function bodies below are ROM-descending. The exception is
 * the destructor window directly under the declaration block: bracketed by
 * `defer_codegen off/on` they generate eagerly in source order, and placed
 * before any deferred function both D1,D0 groups lead the object as the
 * ROM requires (left deferred the variants come out D2,D0,D1, and in a
 * later window the second class's group rides the deferred flush to the
 * end of the object). Same shape as the promoted daKpaFire_c. The legacy
 * func_ov060_02112bfc shard carried `#pragma opt_common_subs off` /
 * `opt_lifetimes off`; under a merged TU that state is file-global
 * last-wins at codegen, so it is spelled out in source instead: the walk
 * pointer is volatile and the flag/value locals are split. The
 * opt_common_subs off on func_ov060_021125f0's shard was not needed --
 * verified without it. */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
/* common.h must come first. It carries Matrix4x3 behind a guard, and both
 * spellings are 0x30 bytes and both are real -- Matrix4x3_ApplyInPlace* wants
 * `m[i]` while the model headers want `.r`/`.t`. Whichever is seen first in a TU
 * stands. The class headers reach Model.h, so including them ahead of common.h
 * hands the TU the `.r`/`.t` form and the helpers below stop compiling. Same
 * ordering constraint daKpa2Bg_c documents. */
#include "common.h"
#include "daKpa_c.h"
#include "daKpaTail_c.h"
#include "daKpaFire_c.h"
#include "daKpa2Bg_c.h"
#include "daKirai_c.h"
#include "Player.h"
#include "daObjKey_c.h"
#include "types.h"
#include "decl_common.h"
#include "dCamera_c.h"
#include "dBgCh_Gnd.h"
#include "dActor_c.h"
#include "dBgCh_Actr.h"
#include "SharedFilePtr.h"
#include "TextureSequence.h"
#include "dExtFrameCtrl_c.h"

/* -------------------------------------------------------------------------- */
/* Names for the numbers this file uses. The values are the ROM's; the names are
 * what each value does at its use sites in this file -- nothing here comes from
 * a symbol table, so a name that only describes an animation or a call is spelled
 * that way on purpose. */

/* daKpa_c::mState, the index into the state table at data_ov060_0211aed4. The
 * handler for each state is named on its own header below. A state with no
 * writer in this file (0x12) is reached only through the table. */
enum Bowser_State {
    BOWSER_STATE_IDLE = 0,            /* stand, then pick the next state (variant decides how) */
    BOWSER_STATE_THROWN = 1,          /* set when the tail grabs him and again when he is released; handler runs after release */
    BOWSER_STATE_RECOVER = 2,         /* entered 1000 units below the arena: hidden, then launched back up toward the centre */
    BOWSER_STATE_PLAY_ANIM_0F = 3,    /* animation 0xf, then idle */
    BOWSER_STATE_DEFEATED = 4,        /* set when mHealth reaches 0; the multi-step defeat sequence */
    BOWSER_STATE_INTRO_WAIT = 5,      /* initial state; stands idle until Player::StartTalk succeeds */
    BOWSER_STATE_INTRO = 6,           /* animations, then the intro message */
    BOWSER_STATE_CHARGE = 7,          /* run at the target (animations 0x18..0x1a) */
    BOWSER_STATE_FIRE_BREATH = 8,     /* spawns KOOPAFIRE (actor 0x118) every 5th frame */
    BOWSER_STATE_FIREBALLS = 9,       /* mFireballShots (1..3) fireballs */
    BOWSER_STATE_PLAY_ANIM_1B = 0xa,  /* animation 0x1b, then BOWSER_STATE_TURN_IN_PLACE; entered when a charge leaves the floor */
    BOWSER_STATE_TURN_IN_PLACE = 0xb, /* mAngleY += 0x200 per frame until mTimer reaches 0x3e */
    BOWSER_STATE_HURT_HOP = 0xc,      /* knocked back; set after a spike bomb hit that leaves mHealth above 0 */
    BOWSER_STATE_JUMP = 0xd,          /* jump and land; the landing effect func_ov060_02115b0c shakes the ground (variant 2) */
    BOWSER_STATE_TURN_TO_TARGET = 0xe, /* turn mAngleY toward mAngleToTarget while animations 0x13, 0x11, 0x12 play */
    BOWSER_STATE_ANIM_CHAIN = 0xf,    /* animations 8, 6, 7 in turn, then idle */
    BOWSER_STATE_VANISH_DASH = 0x10,  /* fade out, dash invisibly at the target, fade in */
    BOWSER_STATE_HOP = 0x11,          /* hop forward at mHorzSpeed 25.0; the variant-0 idle pick chooses it when he faces a target 1500 or more units away */
    BOWSER_STATE_WAIT_ANIM_END = 0x12, /* wait for the current animation to end, then idle */
    BOWSER_STATE_ARENA_TILT = 0x13    /* tilts the KOOPA2BG arena (actor 0xa6) for a few frames, see func_ov060_02112bfc */
};

/* daKpa_c::mHoldState, the index into the table at data_ov060_0211aeb4. */
enum Bowser_HoldState {
    BOWSER_HOLD_NONE = 0,             /* normal: run the state table */
    BOWSER_HOLD_HELD = 1,             /* a Player has grabbed the tail and is swinging Bowser by it */
    BOWSER_HOLD_RELEASE_NOW = 2,      /* the tail's flag bit 0x400 was set: let go at once */
    BOWSER_HOLD_RELEASE_AFTER_ANIM = 3 /* the Player no longer holds an object or the hold countdown ran out: let go once the animation ends */
};

/* daKpa_c::mCondFlags. The low byte is recomputed every frame by
 * func_ov060_02112434; the two high bits are set and cleared by the handlers. */
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
    BOWSER_TAIL_FREE = 0,             /* waiting for a Player to grab it */
    BOWSER_TAIL_COOLDOWN = 1,         /* after a release: back to FREE once mTimer passes 0x1e (30 frames) */
    BOWSER_TAIL_HOLDING = 2           /* a Player is holding it (mHeldPlayer) */
};

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
    BOWSER_SND_FOOTFALL = 0xb0,       /* Sound::Play(3, ..) in the footfall window */
    BOWSER_SND_LEAP = 0xb1,           /* hop, jump, knockback, rise out of a fall */
    BOWSER_SND_GRABBED = 0xb2,        /* a Player grabs the tail and starts swinging Bowser */
    BOWSER_SND_B5 = 0xb5,             /* when the fire breath starts its second animation, and at frame 8 of animation 8 in the animation chain */
    BOWSER_SND_LAND = 0xb6,           /* touching down after a jump */
    BOWSER_SND_TALK_START = 0xb7,     /* the intro message appears */
    BOWSER_SND_VANISH = 0xb9,         /* start of the vanish-and-dash fade */
    BOWSER_SND_SHRINK_LOOP = 0xba,    /* PlayLong while he shrinks in the defeat sequence */
    BOWSER_SND_DEFEATED = 0xbb,       /* the key is spawned after the defeat sequence */
    BOWSER_SND_LAND_SOFT = 0xbd,      /* landing dust in func_ov060_02115a84 */
    BOWSER_SND_FIREBALL = 0x122,      /* a fireball is spawned (func_ov060_021140c0) */
    BOWSER_SND_FIRE_LOOP = 0x180      /* PlayLong while he breathes fire */
};

/* dCc_c::flags bits this file touches (see include/dCc_c.h). */
enum Bowser_CcFlag {
    BOWSER_CC_DISABLED = 0x1          /* set: the cylinder is switched off */
};

/* Animation indices for func_ov060_02111cc0 (a 0x1c-entry table). Only the idle
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

/* The opaque view of the object that Bowser_IsAnimAtLastFrame walks: the
 * Animation lives at 0x124. Animation itself is the real class (include/
 * Animation.h) rather than the local stub this shard carried. */
struct Obj {
    char pad[0x124];
    dExtFrameCtrl_c anim;
};

/* Local shadow declarations carried from the legacy files verbatim.
 * NOT reconciled against real project headers -- check include/*.h for
 * each of these before compiling; a real header should usually win. */
/* The state table at data_ov060_0211ae9c holds pointers-to-member of the actor
 * itself, so PMF is a member pointer of daKpa_c, not of the opaque 4-byte `C`
 * this shard invented. The real class is what the ROM dispatches through. */
typedef void (daKpa_c::*PMF)();

/* shadow struct 'PmfEnt' */
struct PmfEnt { PMF pmf; };

/* Two unrelated two-word tables share this name in different shards:
 *
 *   data_ov060_0211aed4  the state table. Its second word is a packed target
 *                        (bit 0 selects "call through the vtable", the rest is
 *                        either the slot byte offset or a direct target), so
 *                        both words are read here.
 *   data_ov060_0211acd8  a plain two-int compare pair; only the SECOND word is
 *      _0211acf0        ever compared against, so it is named to say so.
 *
 * One struct cannot serve both, so the state table gets its own. */
struct TabEnt { int slot; int target; };

/* Compare pair: second word only. */
struct TabCmp { int unused; int match; };

/* shadow struct 'D0211ac88' */
struct D0211ac88 { int a, b; };

/* shadow typedef 'Vec3' */
typedef struct Vec3 { int x, y, z; } Vec3;

/* shadow class 'dActor_c' */
class dActor_c;

/* shadow class 'Player' */
class Player;

/* TUBUILD CONFLICT -- alternate body of typedef 'Vector3', from the legacy file for func_ov060_021132a4, NOT applied:
typedef struct { int x, y, z; } Vector3;
*/

/* TUBUILD CONFLICT -- alternate body of typedef 'Vector3', from the legacy file for func_ov060_021135fc, NOT applied:
typedef struct Vector3 { int x, y, z; } Vector3;
*/

/* TUBUILD CONFLICT -- alternate body of typedef 'Vector3', from the legacy file for func_ov060_02113a94, NOT applied:
typedef struct Vector3 { int x, y, z; } Vector3;
*/

/* TUBUILD CONFLICT -- alternate body of typedef 's16', from the legacy file for func_ov060_021140c0, NOT applied:
typedef signed short s16;
*/

/* TUBUILD CONFLICT -- alternate body of typedef 'Vector3', from the legacy file for func_ov060_021140c0, NOT applied:
typedef struct { s32 x, y, z; } Vector3;
*/

/* TUBUILD CONFLICT -- alternate body of typedef 's32', from the legacy file for func_ov060_02114858, NOT applied:
typedef int s32;
*/

/* TUBUILD CONFLICT -- alternate body of typedef 'Vector3', from the legacy file for func_ov060_02114858, NOT applied:
typedef struct
{
  int x;
  int y;
  int z;
} Vector3;
*/

/* TUBUILD CONFLICT -- alternate body of struct 'TabEnt', from the legacy file for func_ov060_02114858, NOT applied:
struct TabEnt
{
  int a;
  int b;
};
*/

/* TUBUILD CONFLICT -- alternate body of typedef 'dBgCh_Gnd', from the legacy file for func_ov060_0211577c, NOT applied:
typedef struct dBgCh_Gnd { char filler[0x44]; int clsnY; char rest[0x8]; } dBgCh_Gnd;
*/

/* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the legacy file for _ZN7daKpa_c6RenderEv, NOT applied:
struct Obj { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(void*); };
*/

#define LAUND(p) ((void *)(p))
#define M(p) (p)
/* TUBUILD CONFLICT -- alternate #define of LAUND, from the legacy file for func_ov060_021130c0, NOT applied: #define LAUND(p) ((void*)(p)) */

extern "C" {
/* dCamera_c base pointer; Bowser publishes itself into the target slot at +0x114,
 * so the offset is a byte offset from this base, not a field of a known class. */
extern char *data_0209f318;
extern short data_02082214[];
extern void* _ZN8dActor_c13ClosestPlayerEv(void *thiz);
extern int Vec3_HorzDist(const struct Vector3*, const struct Vector3*);
extern short Vec3_HorzAngle(const struct Vector3*, const struct Vector3*);
extern int func_020092c4(void*, void*, void*);
extern int _ZN6Player7IsInAirEv(void*);
extern int _Z14ApproachLinearRiii(int*, int, int);
/* local extern: the call passes an untyped this, so it cannot use the header method. */
extern int _ZN15dExtFrameCtrl_c8FinishedEv(void*);
/* Switch the model to animation `idx`; `animFlags` is forwarded to
 * ModelAnim::SetAnim (see the definition). The shards disagreed on the arity --
 * some declared 2, some 3, and the return type was spelled both int and void --
 * which C++ rejects as illegal overloading, so this is the widest form actually
 * called, which all ~30 call sites agree with. The helpers below take this same
 * actor; the shards held it as char* or void*, which C++ will not reconcile, so
 * they take daKpa_c*. */

extern int data_ov060_0211acd0[];
extern unsigned char data_ov060_02119264[];
extern PmfEnt data_ov060_0211aeb4[];
int _ZN8dActor_c14GetSubtractionEss(void* self, short a, short b);
extern dActor_c *_ZN8dActor_c10FindWithIDEj(unsigned int id);
extern int _ZN10dBgCh_Actr15ClearGroundFlagEv(char *c);
/* func_02012694 takes (soundId, position) -- see src/engine/sound/Sound.cpp, whose
 * body is a single Sound::Play(3, id, pos) forward. Two shards here passed a
 * third argument the callee does not take (a counter byte, and a value stored
 * on the line above); dropped here rather than widening the signature. */
extern void func_02012694(unsigned int id, const Vector3 *v);
void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* self, const Vector3& v, u32 a, Fix12i b, u32 c, u32 d, u32 e);
void _ZN8dActor_c9UpdatePosEP5dCc_c(void* self, void* cc);
void func_02038408(void* p);
void dBgCh_Actr_UpdateContinuous_Veneer(void* p);
int _ZNK10dBgCh_Actr10IsOnGroundEv(void* self);
void* _ZNK10dBgCh_Actr14GetFloorResultEv(void* self);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* self, Vector3* out);
int _ZNK10dBgCh_Actr13JustHitGroundEv(void* self);
int _ZN4cstd4fdivEii(int a, int b);

extern TabEnt data_ov060_0211aed4[];
extern s16 data_02082214[];

extern int _ZN6Player9StartTalkER7fBase_cb(void *pl, void *a, int b);
extern int _ZN6Player12GetTalkStateEv(void *pl);
extern unsigned char NumStars(void);
extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
void *pl, void *a, unsigned m, void *v, unsigned d, unsigned e);
extern void _ZN7Message11PrepareTalkEv(void);
extern void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned a);
extern void func_ov060_021135fc(daKpa_c *kpa);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
unsigned a, unsigned b, int x, int y, int z, void *v, void *cb);
extern void func_ov060_02113260(daKpa_c *kpa);
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int a, int b);
extern int func_ov060_021132a4(daKpa_c *kpa);
/* Sound::PlayLong is declared in include/Sound.h as
 * (u32 handle, u32, u32, const Vector3 &pos, s16). The mangled name encodes that
 * order, so the bridge is spelled to match rather than through void*. */
extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int c, const Vector3 *v, short d);
/* Landing-dust helper; the second argument is the counter it bumps (every call
 * site passes &mStepCounter). */
extern void func_ov060_02115a84(daKpa_c *kpa, char* p);
extern void func_ov060_02112350(daKpa_c *kpa);
extern struct D0211ac88 data_ov060_0211ac88;
/* The mangled name encodes the real signature: (u32, u32, Vector3*,
 * const Vector3_16*, int, int). Spelled to match so the two call sites that pass
 * a real Vector3_16* at c+0x92 bind without a cast. */
extern void* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int kind, unsigned int b, Vector3 *pos, const Vector3_16 *rot, int e, int f);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int kind, int x, int y, int z);
extern void *_ZN9dBgCh_GndC1Ev(dBgCh_Gnd *self);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd *self, Vec3 *pos, void *actor);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd *self);
extern int _ZNK5dBgPi9GetClsnIDEv(void *self);
extern void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd *self);
extern void func_ov060_02113a94(daKpa_c *kpa);
/* Defined below. */
extern int func_ov060_021145d4(daKpa_c *kpa);
/* Defined further down; called from daKpaTail_c::Behavior above it. */
extern void func_ov060_02115b84(daKpaTail_c *c);
extern void func_ov060_0211577c(daKpa_c *kpa);
extern void func_ov060_02115b0c(daKpa_c *kpa);
extern void *_ZN8dActor_c15FindWithActorIDEjPS_(u32 id, void *p);
extern void func_ov060_02115018(daKpa_c *kpa);
extern int Vec3_HorzLen(const Vector3* v);
extern int func_ov060_02113d20(dActor_c *self);
/* Defined below; the second parameter is a frame count, read as an unsigned
 * halfword (the one caller passes 0x3e). */
extern int func_ov060_02113ff4(daKpa_c *kpa, unsigned short a, int b);
/* local extern: the call passes an untyped this, so it cannot use the header method. */
extern int _ZNK15dExtFrameCtrl_c12WillHitFrameEi(void* anim, int frame);
extern void _ZN8dActor_c13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j( void* self, const void* pos, const void* rot, int speed, int gravity, u32 flags);
extern int _ZN6Player9GetHealthEv(void* player);
extern int RandomIntInternal(int* seed);
extern int data_0209e650;
extern int data_ov060_0211abe0[];
extern int func_ov060_0211469c(daKpa_c *kpa);
extern void func_ov060_021145a8(daKpa_c *kpa);
extern void func_0200fa04(void* c, void* v, int a);
extern struct TabCmp data_ov060_0211acd8;
extern struct TabCmp data_ov060_0211acf0;
extern int _Z14ApproachLinearRsss(short *value, short target, short step);
extern int func_ov060_02115744(daKpa_c *kpa);
extern int func_ov060_02115718(daKpa_c *kpa);
extern int func_ov060_021156ec(daKpa_c *kpa);
extern int data_ov060_0211ac20[];
extern int data_ov060_0211ac68[];
extern int data_ov060_0211ac70[];
/* Both take this same actor. */
extern void func_ov060_021150d0(daKpa_c *kpa);
extern void func_ov060_021150c4(daKpa_c *kpa);
extern int _ZN8dActor_c13DistToCPlayerEv(void *self);
extern int func_ov060_02111f08(daKpa_c *kpa);
extern void func_ov060_02115518(daKpa_c *kpa);
extern s16 data_ov060_0211a4e0[];
extern s16 func_02010844(void *unused, void *v, s16 angle);
extern void Vec3_Asr(Vec3 *d, Vec3 *s, int sh);
extern void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *m, s16 angY);
extern void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *m, s16 angX);
extern void Matrix4x3_ApplyInPlaceToRotationZ(Matrix4x3 *m, s16 angZ);
extern void _ZN9ModelBase12ApplyOpacityEjj(void *self, unsigned int opacity, unsigned int unused);
extern void MulMat4x3Mat4x3(void *dst, void *a, void *b);
extern void Vec3_LslInPlace(void *v, int sh);
extern void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(void *c, void *sm, void *mtx, int rad, int h, unsigned int flags);
extern Matrix4x3 data_020a0e68;
extern void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void* self, struct Vector3* v, int f);
extern void func_ov060_021123a0(daKpa_c *kpa, int b);
extern void func_ov060_021123c8(daKpa_c *kpa);
extern void _ZN6Player9DropActorEv(void *p);
extern "C" int _ZN6Player15IsCollectingCapEv(Player *p);
extern "C" int _ZN6Player7TryGrabER8dActor_c(Player *p, dActor_c &a);
extern "C" void func_02011cfc(void);
/* One declaration per resource handle, typed from how this TU actually uses it.
 * The shards disagreed: the resource-loader shard passed them straight to
 * Model::LoadFile(SharedFilePtr) and called ->Release(), while the state shard
 * read the same addresses as flat int tables. One object cannot be both, and
 * C++ rejects the pair. Typed here from the loader, which is the real consumer.
 *
 *   data_ov060_021192dc / _0211927c  indexed by i, ->Release()  -> SharedFilePtr[]
 *   data_ov060_0211b208 is SharedFilePtr here, but decl_common.h owns an
 *   int[] decl for daFRing_c -- cast through it at the use sites.
 *   data_ov060_0211ac78              indexed [1] and .w[1]       -> SharedFilePtr[] */
extern SharedFilePtr *data_ov060_021192dc[];
extern SharedFilePtr *data_ov060_0211927c[];
extern SharedFilePtr data_ov089_02132c50;
extern SharedFilePtr *data_ov060_0211ac78[];
extern SharedFilePtr *data_ov060_0211ac28[];
/* local extern: the call passes an untyped this, so it cannot use the header method. */
extern void _ZN15dExtFrameCtrl_c7AdvanceEv(void* a);
extern void _ZN15TextureSequence6UpdateER15ModelComponents(void* a, void* b);
/* Model::LoadFile is declared in include/Model.h as taking SharedFilePtr&.
 * The legacy shard bridged it as `void *`, which only worked while the handles
 * were untyped. Now that they are the SharedFilePtr they really are, the bridge
 * takes the reference the ROM symbol encodes, and the call sites pass the
 * handles themselves rather than casting to void*. */
extern void *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
/* local extern: the call passes an untyped this, so it cannot use the header method. */
extern void _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(void *f);
extern void _ZN15TextureSequence8LoadFileER13SharedFilePtr(void *f);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, void *btp, int a, int b, unsigned int d);
/* local extern: the call passes an untyped this, so it cannot use the header method. */
extern void _ZN15dExtFrameCtrl_c8SetFlagsEi(void *self, int flags);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, void *act, void *pos, int c3, int d, unsigned int e, unsigned int f);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *act, int a, int b, void *d1, void *d2);
extern void _ZN10dBgCh_Actr13SetLimMovFlagEv(void *self);
extern void func_ov060_021123dc(void *c);
extern void func_02011d50(void *a);
int _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *actor, int fix, int t, unsigned int e, unsigned int f);
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzDist, from the legacy file for func_ov060_02112434, NOT applied: int Vec3_HorzDist(const Vector3* a, const Vector3* b); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for func_ov060_02112434, NOT applied: short Vec3_HorzAngle(const Vector3* a, const Vector3* b); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111cc0, from the legacy file for func_ov060_021125f0, NOT applied: extern int func_ov060_02111cc0(char *c, int a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111cc0, from the legacy file for func_ov060_02112724, NOT applied: extern void func_ov060_02111cc0(char *c, int idx, int a); */
/* TUBUILD CONFLICT -- alternate declaration of Bowser_IsAnimAtLastFrame, from the legacy file for func_ov060_02112724, NOT applied: extern int Bowser_IsAnimAtLastFrame(char *o); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c10FindWithIDEj, from the legacy file for func_ov060_021128c0, NOT applied: void* _ZN8dActor_c10FindWithIDEj(u32 id); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c10FindWithIDEj, from the legacy file for func_ov060_02112d48, NOT applied: extern void *_ZN8dActor_c10FindWithIDEj(unsigned int id); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111cc0, from the legacy file for func_ov060_02112ee0, NOT applied: extern void func_ov060_02111cc0(void *c, int a, int b, int d); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN6Player9StartTalkER7fBase_cb, from the legacy file for func_ov060_021130c0, NOT applied: extern int _ZN6Player9StartTalkER7fBase_cb(void* player, void* actorBase, int isTalk); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN6Player12GetTalkStateEv, from the legacy file for func_ov060_021130c0, NOT applied: extern int _ZN6Player12GetTalkStateEv(void* player); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh, from the legacy file for func_ov060_021130c0, NOT applied: extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void* player, void* actorBase, unsigned int msgId, const void* pos, unsigned int a, unsigned int b); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111cc0, from the legacy file for func_ov060_021130c0, NOT applied: extern void func_ov060_02111cc0(void* c, int a, int b, int d); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN5Sound22StopLoadedMusic_Layer1Ej, from the legacy file for func_ov060_021130c0, NOT applied: extern void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int a); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02113260, from the legacy file for func_ov060_021130c0, NOT applied: extern void func_ov060_02113260(void* c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_021135fc, from the legacy file for func_ov060_021130c0, NOT applied: extern void func_ov060_021135fc(void* c); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE, from the legacy file for func_ov060_021132a4, NOT applied: extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int a, unsigned int b, int fix, int t1, int t2, void *v, void *cb); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzDist, from the legacy file for func_ov060_02113404, NOT applied: extern int Vec3_HorzDist(const struct Vector3* a, const struct Vector3* b); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c14GetSubtractionEss, from the legacy file for func_ov060_02113404, NOT applied: extern int _ZN8dActor_c14GetSubtractionEss(void* self, short a, short b); */
/* TUBUILD CONFLICT -- alternate declaration of _ZNK10dBgCh_Actr13JustHitGroundEv, from the legacy file for func_ov060_021134ac, NOT applied: extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void* clsn); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111cc0, from the legacy file for func_ov060_021134ac, NOT applied: extern void func_ov060_02111cc0(void* c, int a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of _ZNK10dBgCh_Actr10IsOnGroundEv, from the legacy file for func_ov060_021134ac, NOT applied: extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* clsn); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111cc0, from the legacy file for func_ov060_02113564, NOT applied: extern void func_ov060_02111cc0(char *c, int a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012694, from the legacy file for func_ov060_02113564, NOT applied: extern void func_02012694(int a, void *p, int b); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111cc0, from the legacy file for func_ov060_02113710, NOT applied: extern void func_ov060_02111cc0(void *c, int a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of Bowser_IsAnimAtLastFrame, from the legacy file for func_ov060_02113710, NOT applied: extern int Bowser_IsAnimAtLastFrame(void *c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111cc0, from the legacy file for func_ov060_02113740, NOT applied: extern void func_ov060_02111cc0(void *c, int a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012694, from the legacy file for func_ov060_02113740, NOT applied: extern void func_02012694(int a, void *p); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov060_02113740, NOT applied: extern void _Z14ApproachLinearRiii(int *v, int target, int step); */
/* TUBUILD CONFLICT -- alternate declaration of Bowser_IsAnimAtLastFrame, from the legacy file for func_ov060_02113740, NOT applied: extern int Bowser_IsAnimAtLastFrame(void *c); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for func_ov060_02113a94, NOT applied: extern s16 Vec3_HorzAngle(const Vector3* v0, const Vector3* v1); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111cc0, from the legacy file for func_ov060_02113b5c, NOT applied: void func_ov060_02111cc0(char* c, int a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of _Z14ApproachLinearRiii, from the legacy file for func_ov060_02113b5c, NOT applied: void _Z14ApproachLinearRiii(int* v, int a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02115a84, from the legacy file for func_ov060_02113b5c, NOT applied: void func_ov060_02115a84(char* c, char* arg); */
/* TUBUILD CONFLICT -- alternate declaration of Bowser_IsAnimAtLastFrame, from the legacy file for func_ov060_02113b5c, NOT applied: int Bowser_IsAnimAtLastFrame(char* c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111cc0, from the legacy file for func_ov060_02113ff4, NOT applied: extern void func_ov060_02111cc0(void *c, int a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of Bowser_IsAnimAtLastFrame, from the legacy file for func_ov060_02113ff4, NOT applied: extern int Bowser_IsAnimAtLastFrame(void *c); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012694, from the legacy file for func_ov060_021140c0, NOT applied: extern void func_02012694(int id, void* pos); */
/* TUBUILD CONFLICT -- alternate declaration of Bowser_IsAnimAtLastFrame, from the legacy file for func_ov060_021140c0, NOT applied: extern int Bowser_IsAnimAtLastFrame(void* self); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111cc0, from the legacy file for func_ov060_021140c0, NOT applied: extern void func_ov060_02111cc0(void* self, int a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111cc0, from the legacy file for func_ov060_021142b4, NOT applied: extern void func_ov060_02111cc0(void *c, int a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of Bowser_IsAnimAtLastFrame, from the legacy file for func_ov060_021142b4, NOT applied: extern int Bowser_IsAnimAtLastFrame(void *c); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012694, from the legacy file for func_ov060_02114300, NOT applied: extern void func_02012694(int a, char *b, int cnt); */
/* TUBUILD CONFLICT -- alternate declaration of Bowser_IsAnimAtLastFrame, from the legacy file for func_ov060_02114300, NOT applied: extern int Bowser_IsAnimAtLastFrame(void *c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_0211469c, from the legacy file for func_ov060_021143b8, NOT applied: extern int func_ov060_0211469c(void* c); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012694, from the legacy file for func_ov060_021143b8, NOT applied: extern void func_02012694(int a, void* b); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02115018, from the legacy file for func_ov060_021143b8, NOT applied: extern void func_ov060_02115018(void* c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_021145d4, from the legacy file for func_ov060_021143b8, NOT applied: extern int func_ov060_021145d4(void* c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111cc0, from the legacy file for func_ov060_021143b8, NOT applied: extern void func_ov060_02111cc0(char* c, int idx, int b); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02115b0c, from the legacy file for func_ov060_021143b8, NOT applied: extern void func_ov060_02115b0c(char* c); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c15FindWithActorIDEjPS_, from the legacy file for func_ov060_021143b8, NOT applied: extern void* _ZN8dActor_c15FindWithActorIDEjPS_(unsigned int id, void* p); */
/* TUBUILD CONFLICT -- alternate declaration of Bowser_IsAnimAtLastFrame, from the legacy file for func_ov060_021143b8, NOT applied: extern int Bowser_IsAnimAtLastFrame(void* o); */
/* TUBUILD CONFLICT -- alternate declaration of _ZNK10dBgCh_Actr10IsOnGroundEv, from the legacy file for func_ov060_021145d4, NOT applied: extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *c); */
/* TUBUILD CONFLICT -- alternate declaration of func_0200fa04, from the legacy file for func_ov060_021145d4, NOT applied: extern void func_0200fa04(void *c, void *v, int flag); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012694, from the legacy file for func_ov060_021145d4, NOT applied: extern void func_02012694(int a, void *b); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN6Player7IsInAirEv, from the legacy file for func_ov060_021145d4, NOT applied: extern int _ZN6Player7IsInAirEv(void *p); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111cc0, from the legacy file for func_ov060_0211469c, NOT applied: extern "C" void func_ov060_02111cc0(void *c, int a1, int a2); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012694, from the legacy file for func_ov060_021146d0, NOT applied: extern void func_02012694(int a, void *b); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111cc0, from the legacy file for func_ov060_021146d0, NOT applied: extern void func_ov060_02111cc0(char *c, int idx, int m); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02115a84, from the legacy file for func_ov060_021146d0, NOT applied: extern void func_ov060_02115a84(char *c, char *arg); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN5Sound8PlayLongEjjjRK7Vector3s, from the legacy file for func_ov060_02114858, NOT applied: extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 a, u32 b, u32 cc, const Vector3 *v, u32 e); */
/* TUBUILD CONFLICT -- alternate declaration of RandomIntInternal, from the legacy file for func_ov060_02114858, NOT applied: extern int RandomIntInternal(int *seed); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as, from the legacy file for func_ov060_02114858, NOT applied: extern void _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 id, u32 f, const Vector3 *v, const Vector3_16 *r, int a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of Bowser_IsAnimAtLastFrame, from the legacy file for func_ov060_02114858, NOT applied: extern int Bowser_IsAnimAtLastFrame(void *o); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111cc0, from the legacy file for func_ov060_02114858, NOT applied: extern void func_ov060_02111cc0(char *c, int idx, int a); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012694, from the legacy file for func_ov060_02114858, NOT applied: extern void func_02012694(int a, void *p); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012694, from the legacy file for func_ov060_02114b60, NOT applied: extern void func_02012694(int a, void *b); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c14GetSubtractionEss, from the legacy file for func_ov060_02114b60, NOT applied: extern int _ZN8dActor_c14GetSubtractionEss(char *self, s16 a, s16 b); */
/* TUBUILD CONFLICT -- alternate declaration of _ZNK10dBgCh_Actr10IsOnGroundEv, from the legacy file for func_ov060_02114b60, NOT applied: extern int _ZNK10dBgCh_Actr10IsOnGroundEv(char *self); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c14GetSubtractionEss, from the legacy file for func_ov060_02114d08, NOT applied: extern int _ZN8dActor_c14GetSubtractionEss(void* actor, s16 a, s16 b); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012694, from the legacy file for func_ov060_02114e9c, NOT applied: extern "C" int func_02012694(int a, int* b); */
/* TUBUILD CONFLICT -- alternate declaration of Bowser_IsAnimAtLastFrame, from the legacy file for func_ov060_02114e9c, NOT applied: extern "C" int Bowser_IsAnimAtLastFrame(void* o); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111cc0, from the legacy file for func_ov060_02114e9c, NOT applied: extern "C" void func_ov060_02111cc0(void* o, int a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of _ZNK15dExtFrameCtrl_c12WillHitFrameEi, from the legacy file for func_ov060_02114e9c, NOT applied: extern "C" int _ZNK15dExtFrameCtrl_c12WillHitFrameEi(void* self, int frame); */
/* TUBUILD CONFLICT -- alternate declaration of Bowser_IsAnimAtLastFrame, from the legacy file for func_ov060_02114ff8, NOT applied: extern int Bowser_IsAnimAtLastFrame(void *); */
/* TUBUILD CONFLICT -- alternate declaration of RandomIntInternal, from the legacy file for func_ov060_021151d4, NOT applied: extern int RandomIntInternal(int *seed); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02115744, from the legacy file for func_ov060_021153f8, NOT applied: extern int func_ov060_02115744(void *c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02115718, from the legacy file for func_ov060_021153f8, NOT applied: extern int func_ov060_02115718(void *c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_021156ec, from the legacy file for func_ov060_021153f8, NOT applied: extern int func_ov060_021156ec(void *c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111cc0, from the legacy file for func_ov060_021153f8, NOT applied: extern void func_ov060_02111cc0(char *c, int idx, int extra); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111f08, from the legacy file for func_ov060_021154e8, NOT applied: extern int func_ov060_02111f08(void *c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111cc0, from the legacy file for func_ov060_021154e8, NOT applied: extern int func_ov060_02111cc0(void *c, int a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02115518, from the legacy file for func_ov060_021154e8, NOT applied: extern int func_ov060_02115518(void *c); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN6Player9StartTalkER7fBase_cb, from the legacy file for func_ov060_02115518, NOT applied: extern int _ZN6Player9StartTalkER7fBase_cb(void *player, void *actor, int flag); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN6Player12GetTalkStateEv, from the legacy file for func_ov060_02115518, NOT applied: extern int _ZN6Player12GetTalkStateEv(void *player); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh, from the legacy file for func_ov060_02115518, NOT applied: extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void *player, void *actor, unsigned int msg, void *pos, unsigned int a, unsigned int b); */
/* TUBUILD CONFLICT -- alternate declaration of func_02012694, from the legacy file for func_ov060_02115518, NOT applied: extern void func_02012694(int a, void *v); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111f08, from the legacy file for func_ov060_02115518, NOT applied: extern int func_ov060_02111f08(void *self); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111cc0, from the legacy file for func_ov060_021156ec, NOT applied: extern void func_ov060_02111cc0(void *c, int a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of Bowser_IsAnimAtLastFrame, from the legacy file for func_ov060_021156ec, NOT applied: extern int Bowser_IsAnimAtLastFrame(void *c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111cc0, from the legacy file for func_ov060_02115718, NOT applied: extern void func_ov060_02111cc0(void *c, int a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of Bowser_IsAnimAtLastFrame, from the legacy file for func_ov060_02115718, NOT applied: extern int Bowser_IsAnimAtLastFrame(void *c); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111cc0, from the legacy file for func_ov060_02115744, NOT applied: extern void func_ov060_02111cc0(void *c, int a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of Bowser_IsAnimAtLastFrame, from the legacy file for func_ov060_02115744, NOT applied: extern int Bowser_IsAnimAtLastFrame(void *c); */
/* TUBUILD CONFLICT -- alternate declaration of _ZNK10dBgCh_Actr10IsOnGroundEv, from the legacy file for func_ov060_0211577c, NOT applied: extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *self); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as, from the legacy file for func_ov060_02115b0c, NOT applied: extern void* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, struct Vector3* pos, void* rot, int e, int f); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c10FindWithIDEj, from the legacy file for func_ov060_02115c1c, NOT applied: extern void *_ZN8dActor_c10FindWithIDEj(unsigned int id); */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c10FindWithIDEj, from the legacy file for func_ov060_02115d68, NOT applied: extern "C" dActor_c *_ZN8dActor_c10FindWithIDEj(u32 id); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_021123a0, from the legacy file for func_ov060_02115d68, NOT applied: extern "C" void func_ov060_021123a0(void *a, int b); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzAngle, from the legacy file for _ZN7daKpa_c8BehaviorEv, NOT applied: extern s16 Vec3_HorzAngle(const Vector3* a, const Vector3* b); */
/* TUBUILD CONFLICT -- alternate declaration of Vec3_HorzDist, from the legacy file for _ZN7daKpa_c8BehaviorEv, NOT applied: extern Fix12i Vec3_HorzDist(const Vector3* a, const Vector3* b); */
/* TUBUILD CONFLICT -- alternate declaration of data_0209f318, from the legacy file for _ZN7daKpa_c8BehaviorEv, NOT applied: extern char* data_0209f318; */
/* TUBUILD CONFLICT -- alternate declaration of _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as, from the legacy file for _ZN7daKpa_c13InitResourcesEv, NOT applied: extern void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, void *pos, void *dir, int e, int f); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov060_02111cc0, from the legacy file for _ZN7daKpa_c13InitResourcesEv, NOT applied: extern void func_ov060_02111cc0(void *c, int a, int b); */
}

/* The two destructor bodies are defined first so their D1/D0 groups lead the
 * object in ROM order. Under defer_codegen off each out-of-line destructor
 * emits its D1, D0 and a homeless D2 at the definition. With the deferred
 * queue still empty here both groups land before every deferred function;
 * placed later, the second class's group instead rides the deferred flush and
 * ends the object. */
#pragma defer_codegen off

/* -------------------------------------------------------------------------- */
/* ROM ordinals 0-1 -- _ZN7daKpa_cD1Ev, 0x02111900 / _ZN7daKpa_cD0Ev, 0x02111950 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_cD1Ev
// @symbol _ZN7daKpa_cD0Ev
/*
Bowser's destructors. The body is empty: the member and base teardown and the
D1/D0 pair are compiler-generated. */
#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~daKpa_c() it emits,
 * so compiling the definition below as well would define that symbol
 * twice. This arm spells out, in terms of it, what the deleting destructor
 * does: the D1 body, called qualified so it is a direct call even where a
 * header declares the destructor virtual, then the class-specific
 * operator delete. Nothing here reaches mwccarm: it builds the #else arm
 * and emits the ROM bytes it always emitted. */
extern "C" daKpa_c *_ZN7daKpa_cD0Ev(daKpa_c *thiz)
{
    thiz->daKpa_c::~daKpa_c();        /* the D1 body, through the one host symbol */
    daKpa_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
daKpa_c::~daKpa_c()
{
}
#endif

/* -------------------------------------------------------------------------- */
/* ROM ordinals 2-3 -- _ZN11daKpaTail_cD1Ev, 0x021119b4 / _ZN11daKpaTail_cD0Ev, 0x021119e4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daKpaTail_cD1Ev
// @symbol _ZN11daKpaTail_cD0Ev
/*
The tail's destructors. The body is empty, as for Bowser's. */
#ifdef _MSC_VER
/* The host needs the ROM's flat D0 name; see the note on _ZN7daKpa_cD0Ev. */
extern "C" daKpaTail_c *_ZN11daKpaTail_cD0Ev(daKpaTail_c *thiz)
{
    thiz->daKpaTail_c::~daKpaTail_c();    /* the D1 body, through the one host symbol */
    daKpaTail_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
daKpaTail_c::~daKpaTail_c()
{
}
#endif

#pragma defer_codegen on

/* -------------------------------------------------------------------------- */
/* ROM ordinal 78 -- _ZN11daKpaTail_c13InitResourcesEv, 0x021163b4, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daKpaTail_c13InitResourcesEv
/*
Tail setup: a single cylinder collider (dCcAc_c::Init) with radius 0x32000 (50
units), height 0x50000 (80 units), flags 0x800000 and vulnFlags 0x1000, which is
the grab bit of dCc_c's hit-flag table (that table is a best-effort reading, see
include/dCc_c.h). Always returns 1. */
/* recovered: named members + shared header, real C++ method */
int daKpaTail_c::InitResources()
{
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x32000, 0x50000, 0x800000, 0x1000);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 77 -- _ZN7daKpa_c13InitResourcesEv, 0x02116130, size 0x284 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c13InitResourcesEv
/*
Bowser's setup, in order: load the model file and hand it to ModelBase::SetFile,
load the 0x1c Animation files and the six TextureSequence files, then two more
Model files (CleanupResources releases the same set); shadow cylinder; start the
idle animation (0x10) and its texture pattern; body collider (cylinder, radius 0x78000 = 120 units, height
0x118000 = 280, flags 0x200004); remember the spawn position in mHomePos; gravity
-0x2000 (-2.0) and terminal velocity -0x3c000 (-60.0); floor collider (0x50000 =
80 for both of its Fix12 arguments); decode param1 (bits 0-1 = variant, bit 2 =
jump-only); reset the working fields (func_ov060_021123dc finishes that); then
spawn KOOPAFIRE and KOOPATAIL at his position and give each his uniqueID.
Returns 0 only if the shadow cylinder fails, otherwise 1. */
/* recovered: named members + shared header, real C++ method
 *
 * The other half of daKpa_c::CleanupResources. Every handle this loads is one
 * the cleanup releases, in the same order and with the same counts -- one
 * single, a 0x1c-entry table, a six-entry table, then two more singles, the
 * last of which (data_ov089_02132c50) lives in ov089 rather than this overlay.
 * That pairing is why the siblings' CleanupResources are bare `return 1`s:
 * daKpa_c loads the whole fight's resources, so daKpa_c frees them.
 *
 * The two loops are reproduced rather than unrolled, for the same reason as in
 * the cleanup: 0x1c and 6 are the counts the ROM's own comparisons test.
 *
 * `Vector3 pos` was a local shadow typedef; it is the real types.h Vector3
 * here, which is layout-identical (Fix12i is s32) and costs nothing.
 *
 * The fields this used to spell as unk_ are the base classes' and are named now:
 * mVertAccel / mTerminalVelocity are dActor_c::mVertAccel and dActor_c::mTerminalVelocity -- and the
 * values written here, -0x2000 and -0x3c000, are fix12 gravity and terminal
 * velocity, which is the same evidence dActor_c.h cites from BooCage and daPiano_c.
 * mParam is fBase_c::param1, uniqueID is fBase_c::uniqueID, and mAreaId is
 * dActor_c::mAreaId -- which is why it is read as a signed char and handed straight
 * to dActor_c::Spawn's areaID parameter.
 *
 * The early `return 0` when dExtShadowModel_c::InitCylinder fails is the ROM's -- the
 * only failure path in the function.
 */
int daKpa_c::InitResources()
{
    int i;
    Vector3 pos;
    daKpaFire_c *a1;
    daKpaTail_c *a2;

    _ZN9ModelBase7SetFileEP8BMD_Fileii(&this->mModelAnim,
        _ZN5Model8LoadFileER13SharedFilePtr(*(SharedFilePtr *)data_ov060_0211ac78), 1, 0x16);

    for (i = 0; i < 0x1c; i++)
        _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr((void *)data_ov060_021192dc[i]);

    for (i = 0; i < 6; i++)
        _ZN15TextureSequence8LoadFileER13SharedFilePtr((void *)data_ov060_0211927c[i]);

    _ZN5Model8LoadFileER13SharedFilePtr(*(SharedFilePtr *)data_ov060_0211b208);
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov089_02132c50);

    if (this->mShadowModel.InitCylinder() == 0)
        return 0;

    /* Same object as the other ~30 call sites, which hand it the base pointer as
     * char*. Inside a member function `this` is daKpa_c*, so cast it the same way
     * rather than widening the helper's signature for one caller. */
    func_ov060_02111cc0(BOWSER_ANIM_IDLE, 0);

    TextureSequence::Prepare(*(BMD_File *)data_ov060_0211ac78[1],
                             *(BTP_File *)data_ov060_0211ac28[1]);

    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        &this->mTextureSequence, (void *)data_ov060_0211ac28[1], 0, 0x1000, 0);

    _ZN15dExtFrameCtrl_c8SetFlagsEi(&this->mTextureSequence, 0x40000000);

    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &this->mdCcAcPos_c, this, &pos, 0x78000, 0x118000, 0x200004, 0);

    this->mHomePosX = this->mPosX;
    this->mHomePosY = this->mPosY;
    this->mHomePosZ = this->mPosZ;
    this->mVertAccel = -0x2000;
    this->mTerminalVelocity = -0x3c000;
    this->mTargetPlayer = 0;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &this->mWithMeshClsn, this, 0x50000, 0x50000, 0, 0);
    _ZN10dBgCh_Actr13SetLimMovFlagEv(&this->mWithMeshClsn);

    this->mState = BOWSER_STATE_IDLE;
    this->mVariantID = (char)(this->param1 & 3);
    this->mChooseJumpOnly = (char)(((unsigned int)this->param1 >> 2) & 1);
    this->mTimer = 0;
    this->mStep = 0;
    this->mDropsShadow = 1;
    this->mBounceOnLand = 0;
    this->mScaleX = 0x1000;
    this->mScaleY = 0x1000;
    this->mScaleZ = 0x1000;
    this->mAnimSpeed = 0x1000;
    this->mSkipIdleRoll = 1;
    func_ov060_021123dc();

    this->mTalkStep = 0;
    this->mCutsceneStep = 0;

    /* Spawn takes the three position words as one Vector3. dActor_c stores them as
     * mPosX/Y/Z at 0x5c, so the argument is the vector, not a pointer to the first
     * scalar. (dActor_c::Pos() would say this directly, but that accessor is not on
     * this branch yet.) */
    a1 = (daKpaFire_c *)_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(BOWSER_ACTOR_KOOPAFIRE, 0, (Vector3 *)&this->mPosX, 0, this->mAreaId, -1);
    a1->mKpaUniqueID = this->uniqueID;

    a2 = (daKpaTail_c *)_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(BOWSER_ACTOR_KOOPATAIL, 0, (Vector3 *)&this->mPosX, 0, this->mAreaId, -1);
    this->mTailUniqueID = a2->uniqueID;
    a2->mBowserUniqueID = this->uniqueID;
    this->mVanishChance = 5;
    this->mCapActorAlive = 0;
    this->mParticleHandle = 0;
    this->mFootfallLatch = 0;
    this->mSoundHandle = 0;
    this->mSoundID = 0;
    func_02011d50(a2);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 76 -- _ZN11daKpaTail_c8BehaviorEv, 0x02116078, size 0xb8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daKpaTail_c8BehaviorEv
/*
Per frame: find Bowser by mBowserUniqueID (return 1 if he is gone), put the tail
0x8c (140) units behind him -- his position plus 0x8c times the table entry for
mPrevAngleY + 0x8000, Y copied as is -- run func_ov060_02115b84, return 1. */
/* recovered: named members + shared header, real C++ method, declarations from a shared header
 *
 * The tail follows daKpa_c: find him by the uniqueID stashed in mBowserUniqueID, then park
 * this actor 0x8c units from his position in the direction of his previous facing angle
 * plus 0x8000.
 * data_02082214 is a sin/cos table indexed by angle>>4, two shorts per entry.
 *
 * The one-line `struct dActor_c { static dActor_c* FindWithID(unsigned int); };` stand-in
 * this file used to carry is gone -- dActor_c.h already declared FindWithID, so it was
 * never needed.
 *
 * THE POINTER BUMP AND THE volatile ARE LOAD-BEARING, both measured. Reading
 * daKpa_c's fields the obvious way -- `bowser->mPrevAngleY`, `bowser->mPosX` and so
 * on, which the real dActor_c now makes possible -- compiles and does not reproduce
 * the ROM. The bump to +0x5c and the three loads off it are what the original
 * source did, and the offsets are dActor_c's: 0x94 is mPrevAngleY, 0x5c..0x64 are
 * mPosX/mPosY/mPosZ.
 */
int daKpaTail_c::Behavior()
{
    dActor_c* a = dActor_c::FindWithID(mBowserUniqueID);
    if (!a) return 1;

    int ang = *(short*)((char*)a + 0x94);        /* dActor_c::mPrevAngleY */
    a = (dActor_c*)((int)a + 0x5c);                 /* dActor_c::mPosX */
    int x = *(int*)a;
    volatile int v[3];
    v[0] = x;
    v[1] = ((int*)a)[1];                         /* mPosY */
    v[2] = ((int*)a)[2];                         /* mPosZ */
    int j = 2 * (((unsigned short)(short)(ang + 0x8000)) >> 4);
    mPosX = (short)data_02082214[j] * 0x8c + x;
    mPosY = v[1];
    mPosZ = (short)data_02082214[j + 1] * 0x8c + v[2];

    func_ov060_02115b84();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 75 -- _ZN7daKpa_c8BehaviorEv, 0x02115f64, size 0x114 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c8BehaviorEv
/*
One frame of Bowser, in order: advance the random state once (result unused);
find the closest Player and record the angle and the horizontal distance to him
(no Player: angle = his own facing, distance 0x7fffffff); func_ov060_02112434
(condition flags, hold-state handler, opacity fade); func_ov060_02111a28
(footfall effects); copy mAngleY to mPrevAngleY; advance the model animation at
mAnimSpeed; func_ov060_0211577c (model matrix, foot points, shadow); store this
actor at offset 0x114 of the camera object; clear and refresh the body collider,
offset (0, 0, 0x50000) = 80 units on Z relative to the actor; and drop
mCapActorAlive once no cap actor exists any more. Always returns 1. */
/* recovered: named members + shared header, real C++ method, declarations from a shared header
 *
 * One frame of the fight: pick the closest player and record the angle and distance
 * to him, run the two state workers, advance the animation, publish `this` into the
 * global at data_0209f318+0x114, then rebuild the body cylinder 0x50000 in front of
 * the actor.
 *
 * FOUR STAND-IN STRUCTS ARE GONE -- `dActor_c`, `Animation`, `dCc_c` and
 * `dCcAcPos_c`, each declared here with just the one or two methods
 * this file called, then "defined" again below with a set of bodyless declarations
 * that existed only to stop the compiler mangling them differently. All four are
 * the real classes now, and the casts that reached them go with them:
 * mdCcAcPos_c IS a dCc_c by inheritance
 * (dCcAcPos_c -> dCcAc_c -> dCc_c), so Clear() and
 * Update() are called directly.
 *
 * The two fields this used to spell as its own were the ModelAnim's: `mAnimation`
 * at 0x124 is the Animation base inside mModelAnim at +0x50, and `unk_130` at 0x130
 * is that base's `speed` at +0x0c. Advancing "the animation" is advancing the model.
 */
int daKpa_c::Behavior()
{
    RandomIntInternal(&data_0209e650);
    mTargetPlayer = (dActor_c *)ClosestPlayer();
    if (mTargetPlayer != 0) {
        mAngleToTarget = Vec3_HorzAngle((Vector3 *)&mPosX, (Vector3*)&mTargetPlayer->mPosX);
        mDistToTarget = Vec3_HorzDist((Vector3 *)&mPosX, (Vector3*)&mTargetPlayer->mPosX);
    } else {
        mAngleToTarget = mAngleY;
        mDistToTarget = ~0x80000000;
    }
    func_ov060_02112434();
    func_ov060_02111a28();
    mPrevAngleY = mAngleY;
    mModelAnim.speed = mAnimSpeed;
    mModelAnim.Advance();
    func_ov060_0211577c();
    *(char**)(data_0209f318 + 0x114) = (char *)this;
    mdCcAcPos_c.Clear();
    Vector3 v;
    v.z = 0x50000;
    v.x = 0;
    v.y = 0;
    mdCcAcPos_c.SetPosRelativeToActor(v);
    mdCcAcPos_c.Update();
    if (mCapActorAlive != 0) {
        dActor_c* f = dActor_c::FindWithActorID(BOWSER_ACTOR_OBJ_MARIO_CAP, 0);
        if (f == 0) mCapActorAlive = 0;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 74 -- _ZN11daKpaTail_c6RenderEv, 0x02115f5c, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daKpaTail_c6RenderEv
/*
Draws nothing; returns 1. */
/* recovered: shared header, real C++ method
 *
 * `return 1` and nothing else.
 */
int daKpaTail_c::Render()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 73 -- _ZN7daKpa_c6RenderEv, 0x02115f0c, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c6RenderEv
/*
Draw Bowser. Skipped while mOpacity < 8, where the opacity handed to the model
(mOpacity >> 3, see func_ov060_0211577c) would be 0. Otherwise step the texture
pattern, apply it to the model and render the model with the actor's scale. */
/* recovered: named members + shared header, real C++ method */
int daKpa_c::Render()
{
  if(mOpacity < 8) return 1;
  _ZN15dExtFrameCtrl_c7AdvanceEv(&mTextureSequence);
  _ZN15TextureSequence6UpdateER15ModelComponents(&mTextureSequence, &mModelAnim.data);
  /* The old shard declared a local `struct Obj` with a method `m` purely so this
   * call would compile; it is ModelAnim's real slot-5 Render(const Vector3*), which
   * takes the scale vector. daKpa2Bg_c::Render makes the same call as mModel2.Render. */
  mModelAnim.Render((const Vector3 *)&mScaleX);
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 72 -- _ZN7daKpa_c16OnPendingDestroyEv, 0x02115f08, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c16OnPendingDestroyEv
/*
Empty override: the ROM body is a single `bx lr`. */
/* recovered: shared header, real C++ method
 *
 * Empty -- the ROM body is a single `bx lr`.
 */
void daKpa_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 71 -- _ZN11daKpaTail_c16CleanupResourcesEv, 0x02115f00, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daKpaTail_c16CleanupResourcesEv
/*
Releases nothing; returns 1. */
/* recovered: shared header, real C++ method
 *
 * `return 1`, no releases. The tail shares Bowser's translation unit and his
 * files -- tu_map puts both classes in one TU at 0x2111900..0x2116484 -- so
 * Bowser::CleanupResources frees everything and the tail takes no reference of
 * its own.
 */
int daKpaTail_c::CleanupResources()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 70 -- _ZN7daKpa_c16CleanupResourcesEv, 0x02115e80, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c16CleanupResourcesEv
/*
Release every handle InitResources loaded (the model, the 0x1c animations, the six
texture patterns, then two more Model files), call func_02011cfc (no arguments;
its purpose is not established here) and return 1. */
/* recovered: shared header, real C++ method
 *
 * daKpa_c frees the whole fight. One single, then a 0x1c-entry table and a
 * six-entry table walked by index, then two more singles -- one of which
 * (data_ov089_02132c50) lives in ov089, not this overlay.
 *
 * The two loops are reproduced rather than unrolled: 0x1c and 6 are the counts
 * the ROM's own comparisons test against, and the tables are arrays of
 * POINTERS to handles, unlike the singles which are handles themselves.
 *
 * That is why his siblings release almost nothing -- daKpaFire_c and daKpaTail_c
 * hold no reference at all, and daFRing_c shares 0211b208 with him.
 */
int daKpa_c::CleanupResources()
{
    int i;
    ((SharedFilePtr *)(&data_ov060_0211ac78))->Release();
    for (i = 0; i < 0x1c; i++)
        data_ov060_021192dc[i]->Release();
    for (i = 0; i < 6; i++)
        data_ov060_0211927c[i]->Release();
    ((SharedFilePtr *)(&data_ov060_0211b208))->Release();
    ((SharedFilePtr *)(&data_ov089_02132c50))->Release();
    func_02011cfc();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 69 -- func_ov060_02115d68, 0x02115d68, size 0x118 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daKpaTail_c19func_ov060_02115d68Ev
/*
Tail state BOWSER_TAIL_FREE (0), per frame: nobody holds the tail. Switch the
tail's own collider on (clear its disabled bit). If Bowser is in
BOWSER_STATE_ARENA_TILT, switch his body collider on and stop; if he is defeated,
stop. Otherwise switch his body collider on as well (func_ov060_021123a0 with 1)
and look for a grab: the tail's collider must report an owner and the grab bit
(hitFlags & 0x1000), that actor must not be collecting a cap, and
Player::TryGrab(tail) must succeed. On a grab the Player is given Bowser's three
angles (current and previous), the tail goes to BOWSER_TAIL_HOLDING and records
the Player, Bowser's body collider is switched off, Bowser goes to
BOWSER_HOLD_HELD with the Player in mGrabbedPlayer, and the hold countdown starts
at 0x96 (150 frames). */
void daKpaTail_c::func_ov060_02115d68(){
    u32 *p = &this->mdCcAc_c.flags;
    daKpa_c *bowser = (daKpa_c *)_ZN8dActor_c10FindWithIDEj(this->mBowserUniqueID);
    *p &= ~BOWSER_CC_DISABLED;
    if (bowser->mState != BOWSER_STATE_ARENA_TILT) goto skip_13;
    bowser->func_ov060_021123a0(1);
    return;
skip_13:
    if (bowser->mState == BOWSER_STATE_DEFEATED) return;
    bowser->func_ov060_021123a0(1);
    if (this->mdCcAc_c.otherOwner == 0) return;
    if (!(this->mdCcAc_c.hitFlags & 0x1000)) return;
    dActor_c *player = _ZN8dActor_c10FindWithIDEj(this->mdCcAc_c.otherOwner);
    if (!player) return;
    if (_ZN6Player15IsCollectingCapEv((Player *)player)) return;
    if (!_ZN6Player7TryGrabER8dActor_c((Player *)player, *this)) return;
    s16 *ip = &bowser->mAngleX;
    player->mAngleX = ip[0];
    player->mAngleY = ip[1];
    player->mAngleZ = ip[2];
    player->mPrevAngleX = ip[0];
    player->mPrevAngleY = ip[1];
    player->mPrevAngleZ = ip[2];
    this->mState = BOWSER_TAIL_HOLDING;
    this->mHeldPlayer = player;
    bowser->func_ov060_021123a0(0);
    bowser->mHoldState = BOWSER_HOLD_HELD;
    bowser->mGrabbedPlayer = player;
    this->mHoldCountdown = 0x96;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 68 -- func_ov060_02115d50, 0x02115d50, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daKpaTail_c19func_ov060_02115d50Ev
/*
Tail state BOWSER_TAIL_COOLDOWN (1), per frame: once mTimer is above 0x1e (30)
the tail goes back to BOWSER_TAIL_FREE. */
void daKpaTail_c::func_ov060_02115d50(){
  if (this->mTimer > 0x1e)
    this->mState = BOWSER_TAIL_FREE;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 67 -- func_ov060_02115c1c, 0x02115c1c, size 0x134 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daKpaTail_c19func_ov060_02115c1cEv
/*
Tail state BOWSER_TAIL_HOLDING (2), per frame. If Bowser is in
BOWSER_STATE_ARENA_TILT, switch his body collider on and go to BOWSER_TAIL_FREE
(the code then carries on with the checks below). If the tail's own mFlags has
bit 0x400, go to cooldown, forget the Player and tell Bowser to let go at once
(BOWSER_HOLD_RELEASE_NOW); that path returns without touching the tail's
collider. Otherwise, with a Player held: if the Player no longer holds an object
(Player::mHeldObj == 0) he is dropped; else a non-zero Player::mAngleYSpeed
refills mHoldCountdown to 0x96 (150) and a zero one counts it down, dropping the
Player when it reaches 0. A drop (Player::DropActor) puts the tail in cooldown
and Bowser in BOWSER_HOLD_RELEASE_AFTER_ANIM. Every other path ends by switching
the tail's collider off. */
void daKpaTail_c::func_ov060_02115c1c(){
    daKpa_c *bowser;
    dActor_c *held;
    int cond;

    bowser = (daKpa_c *)_ZN8dActor_c10FindWithIDEj(this->mBowserUniqueID);
    if (bowser->mState == BOWSER_STATE_ARENA_TILT) {
        bowser->func_ov060_021123a0(1);
        this->mState = BOWSER_TAIL_FREE;
    }

    cond = (int)((this->mFlags & 0x400) != 0);
    if (cond != 0) {
        this->mState = BOWSER_TAIL_COOLDOWN;
        this->mHeldPlayer = 0;
        bowser->func_ov060_021123c8();
        bowser->mHoldState = BOWSER_HOLD_RELEASE_NOW;
        return;
    }

    held = this->mHeldPlayer;
    if (held == 0) goto tail;

    cond = (int)(((Player *)held)->mHeldObj != 0);
    if (cond == 0) goto do_drop;

    if (((Player *)held)->mAngleYSpeed != 0) goto set_default;
    if (this->mHoldCountdown == 0) goto tail;
    this->mHoldCountdown -= 1;
    if (this->mHoldCountdown != 0) goto tail;

do_drop:
    this->mState = BOWSER_TAIL_COOLDOWN;
    _ZN6Player9DropActorEv(this->mHeldPlayer);
    this->mHeldPlayer = 0;
    bowser->func_ov060_021123c8();
    bowser->mHoldState = BOWSER_HOLD_RELEASE_AFTER_ANIM;
    goto tail;

set_default:
    this->mHoldCountdown = 0x96;

tail:
    this->mdCcAc_c.flags |= BOWSER_CC_DISABLED;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 66 -- func_ov060_02115b84, 0x02115b84, size 0x98 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daKpaTail_c19func_ov060_02115b84Ev
/*
The tail's per-frame dispatcher (called from daKpaTail_c::Behavior). Find
Bowser; run the handler for mState through the table at data_ov060_0211ae9c
(state 0 is func_ov060_02115d68, 1 is func_ov060_02115d50, 2 is
func_ov060_02115c1c; the call is a member-function-pointer call on the tail
itself); switch the tail's collider off if Bowser is defeated; count mTimer up
and reset it to 0 when the handler changed mState; clear and refresh the tail's
collider (dCc_c::Clear and Update). */
/* This shard re-declared `struct C; typedef void (C::*PMF)();` against its own
 * forward declaration of C, which collided with the PMF already defined at the
 * top of this TU (on the real C). One typedef serves both; only the opaque
 * padding view of the object is local to this shard. */
extern PMF data_ov060_0211ae9c[];
extern "C" {
extern void _ZN5dCc_c5ClearEv(void* cc);
extern void _ZN5dCc_c6UpdateEv(void* cc);
}
struct C_func15b84 { char pad[0x800]; };
void daKpaTail_c::func_ov060_02115b84(){
  daKpa_c *bowser = (daKpa_c *)_ZN8dActor_c10FindWithIDEj(this->mBowserUniqueID);
  int idx = this->mState;
  (((daKpa_c *)this)->*data_ov060_0211ae9c[idx])();
  if (bowser->mState == BOWSER_STATE_DEFEATED) {
    this->mdCcAc_c.flags |= BOWSER_CC_DISABLED;
  }
  {
    u16 *h = &this->mTimer;
    *h = *h + 1;
  }
  if (idx != this->mState) {
    this->mTimer = 0;
  }
  _ZN5dCc_c5ClearEv(&this->mdCcAc_c);
  _ZN5dCc_c6UpdateEv(&this->mdCcAc_c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 65 -- func_ov060_02115b0c, 0x02115b0c, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02115b0cEv
/*
Landing effect for the variant-2 fight (nothing happens for the other variants):
dActor_c::Earthquake at his position with the Fix12 argument 0x7d0000 (2000.0,
the same value daKirai_c passes when a bomb goes off) and a FIRERING actor
(0x119) spawned at his position. */
/* recovered: shared common types */
void daKpa_c::func_ov060_02115b0c(){
    struct Vector3 v;
    if (this->mVariantID != 2) return;
    v.x = this->mPosX;
    v.y = this->mPosY;
    v.z = this->mPosZ;
    _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(this, &v, 0x7d0000);
    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(BOWSER_ACTOR_FIRERING, 0, (struct Vector3*)(&this->mPosX), 0, this->mAreaId, -1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 64 -- func_ov060_02115a84, 0x02115a84, size 0x88 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02115a84EPc
// @symbol _ZN7daKpa_c19func_ov060_02115a84EPc
/*
Landing-dust helper. On a frame where the floor collider reports a fresh ground
hit (dBgCh_Actr::JustHitGround), add one to the u16 counter passed in (every
caller passes the address of mStepCounter) and, while that counter is still
below 4, spawn landing dust at his position and play sound 0xbd at his
camera-space position. */
/* recovered: shared common types */
#include "common.h"
extern "C" {
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void* p);
extern void _ZN8dActor_c13LandingDustAtER7Vector3b(void* a, struct Vector3* v, int b);
}
void daKpa_c::func_ov060_02115a84(char* arg){
  if(_ZNK10dBgCh_Actr13JustHitGroundEv(&this->mWithMeshClsn)==0) return;
  *(unsigned short*)arg = *(unsigned short*)arg + 1;
  if(*(unsigned short*)arg >= 4) return;
  struct Vector3 v;
  v.x = this->mPosX;
  v.y = this->mPosY;
  v.z = this->mPosZ;
  _ZN8dActor_c13LandingDustAtER7Vector3b(this, &v, 0);
  func_02012694(BOWSER_SND_LAND_SOFT, (const Vector3 *)(&this->mCamSpacePosX));
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 63 -- Bowser_IsAnimAtLastFrame, 0x02115a30, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c24Bowser_IsAnimAtLastFrameEv
/*
True once the model's animation has reached its last frame or will reach it on
the coming advance (dExtFrameCtrl_c::Finished, or WillHitFrame(frame count - 1)).
Takes the actor as the opaque Obj view; the Animation sits at +0x124. */
// Bowser_IsAnimAtLastFrame at 0x02115a30 -- matched byte-for-byte with mwccarm 1.2/sp2p3 (ov060).
bool daKpa_c::Bowser_IsAnimAtLastFrame(){
    Obj *obj = (Obj *)this;
    return obj->anim.Finished() || obj->anim.WillHitFrame((unsigned short)(obj->anim.GetFrameCount() - 1));
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 62 -- func_ov060_0211577c, 0x0211577c, size 0x2b4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_0211577cEv
/*
Per-frame model placement, called from Behavior. 1) On the ground and not held,
set mAngleX from the floor normal mGroundNormal and mAngleY, and mAngleZ from the
normal and mAngleY - 0x4000 (a quarter turn), both through func_02010844.
2) Build the model matrix in data_020a0e68: translation = position >> 3
(Vec3_Asr by 3; the foot points below are shifted left 3 on the way back out),
then rotations about Y, X and Z; copy it into the model and apply opacity
mOpacity >> 3 (0..31). 3) For skeleton nodes 3 and 6 (mModelAnim.data.transforms
[3] and [6]) multiply the node matrix by the model matrix (the product lands
back in data_020a0e68), read its translation (m[9..11]) and shift it left 3 to
full scale: node 3 gives mFootPosB, node 6 gives mFootPosA, each with Y replaced
by his own Y. 4) Unless mDropsShadow is 0, probe the floor with a dBgCh_Gnd from
0x32000 (50 units) above him (floor height clsnY, or his own Y if there is
none), build the shadow matrix there and call DropShadowRadHeight with radius
0x140000 (320) and height 0x64000 (100). */
void daKpa_c::func_ov060_0211577c(){
    char pad[8];
    Matrix4x3 saved;
    Vec3 pos;
    Vec3 v;
    Vec3 v2;
    int zero;

    if (_ZNK10dBgCh_Actr10IsOnGroundEv(&this->mWithMeshClsn) == 0)
        goto skip_angles;
    if (this->mHoldState != BOWSER_HOLD_NONE)
        goto skip_angles;
    this->mAngleX = func_02010844(this, &this->mGroundNormalX, this->mAngleY);
    this->mAngleZ = func_02010844(this, &this->mGroundNormalX, (s16)(this->mAngleY - 0x4000));
skip_angles:
    Vec3_Asr(&v, (Vec3 *)(&this->mPosX), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, this->mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, this->mAngleX);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, this->mAngleZ);
    this->mModelAnim.mat4x3 = data_020a0e68;
    _ZN9ModelBase12ApplyOpacityEjj(&this->mModelAnim, (unsigned char)((int)this->mOpacity >> 3), 1);
    saved = data_020a0e68;
    zero = 0;
    this->mFootPosBX = zero;
    this->mFootPosBY = zero;
    this->mFootPosBZ = zero;
    MulMat4x3Mat4x3((char *)&this->mModelAnim.data.transforms[3], &data_020a0e68, &data_020a0e68);
    this->mFootPosBX = data_020a0e68.m[9];
    this->mFootPosBY = data_020a0e68.m[10];
    this->mFootPosBZ = data_020a0e68.m[11];
    Vec3_LslInPlace(&this->mFootPosBX, 3);
    this->mFootPosBY = this->mPosY;
    this->mFootPosAX = zero;
    this->mFootPosAY = zero;
    this->mFootPosAZ = zero;
    data_020a0e68 = saved;
    MulMat4x3Mat4x3((char *)&this->mModelAnim.data.transforms[6], &data_020a0e68, &data_020a0e68);
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
        rc.SetObjAndPos(*(Vector3 *)&pos, 0);
        if (rc.DetectClsn())
            pos.y = rc.clsnY;
        else
            pos.y = this->mPosY;
        Vec3_Asr(&v2, &pos, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, v2.x, v2.y, v2.z);
        *(Matrix4x3 *)(&this->mShadowMtx) = data_020a0e68;
        _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(this, &this->mShadowModel, &this->mShadowMtx, 0x140000, 0x64000, 0xf);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 61 -- func_ov060_02115744, 0x02115744, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02115744Ev
/*
First animation of the turn / intro sequence: play animation 0x13 once; when it
reaches its last frame, set mHorzSpeed to 0x3000 (3.0) and return 1, otherwise
return 0. The sequence continues with 0x11 looping (func_ov060_02115718) and ends
with 0x12 (func_ov060_021156ec); the intro and turn handlers use it. */
int daKpa_c::func_ov060_02115744(){
    /* Both callees take the actor's base pointer as char* (Obj is the opaque
     * view of it); this shard holds it as void*, so the cast belongs at the call
     * rather than on the declaration. */
    func_ov060_02111cc0(0x13, 0x40000000);
    int r = Bowser_IsAnimAtLastFrame();
    if (r != 0) {
        this->mHorzSpeed = 0x3000;
        return 1;
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 60 -- func_ov060_02115718, 0x02115718, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02115718Ev
/*
Play animation 0x11 looping with mHorzSpeed 0x3000 (3.0); return whether it is at
its last frame. */
int daKpa_c::func_ov060_02115718(){
    func_ov060_02111cc0(0x11, 0x0);
    this->mHorzSpeed = 12288;
    return Bowser_IsAnimAtLastFrame();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 59 -- func_ov060_021156ec, 0x021156ec, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_021156ecEv
/*
Play animation 0x12 once with mHorzSpeed 0; return whether it is at its last
frame. */
int daKpa_c::func_ov060_021156ec(){
    func_ov060_02111cc0(0x12, 0x40000000);
    this->mHorzSpeed = 0;
    return Bowser_IsAnimAtLastFrame();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 58 -- func_ov060_02115518, 0x02115518, size 0x1d4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02115518Ev
/*
The intro conversation, one step per mTalkStep (nothing happens without a target
Player). Step 0: Player::StartTalk(this, 1); when it succeeds, mState becomes
BOWSER_STATE_INTRO and the step advances. Step 1: once Player::GetTalkState() is
0, show message data_ov060_0211a4e0[mVariantID] (0xcc, 0xce or 0xd0 in the ROM) at
a point 200 units out along mAngleY and 200 units up (the sin/cos table scaled
by 0xc8, Y + 0xc8000); when that succeeds, advance and play sound 0xb7. Step 2:
when GetTalkState() returns -1, go to BOWSER_STATE_JUMP for variant 1 and
BOWSER_STATE_IDLE otherwise, advance, set mCutsceneStep to 4 and run
func_ov060_02111f08 once (its step 4 clears bit 0x8 of Camera::mFlags). */
void daKpa_c::func_ov060_02115518(){
  void *player = this->mTargetPlayer;
  if (player == 0)
  {
    return;
  }
  switch (this->mTalkStep)
  {
    case 0:
      if (_ZN6Player9StartTalkER7fBase_cb(player, this, 1) == 0)
    {
      return;
    }
      this->mState = BOWSER_STATE_INTRO;
    {
      u8 *p = &this->mTalkStep;
      *p = (*p) + 1;
    }
      return;

    case 1:
      if (_ZN6Player12GetTalkStateEv(player) != 0)
    {
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
      s16 *tbl;
      s16 *msgs;
      u16 ang;
      s16 sx;
      s16 sz;
      x = this->mPosX;
      tbl = data_02082214;
      pos[0] = x;
      y = this->mPosY;
      scale = 0xc8;
      pos[1] = (scale) ? (y) : (y);
      z = this->mPosZ;
      y2 = y + 0xc8000;
      pos[2] = z;
      tbl = (s16 *) ((void *) data_02082214);
      ang = this->mAngleY;
      zero = 0;
      msgs = data_ov060_0211a4e0;
      sx = (s16) tbl[(ang >> 4) * 2];
      pos[0] = (((s16) sx) * ((s16) scale)) + x;
      tbl = data_02082214;
      ang = this->mAngleY;
      sz = (s16) tbl[((ang >> 4) * 2) + 1];
      pos[1] = y2;
      pos[2] = (((s16) sz) * ((s16) scale)) + z;
      msg = (s16) msgs[this->mVariantID];
      if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(this->mTargetPlayer, this, msg, pos, zero, zero) == 0)
      {
        return;
      }
    }
    {
      u8 *p = &this->mTalkStep;
      *p = (*p) + 1;
    }
      func_02012694(BOWSER_SND_TALK_START, (const Vector3 *)(&this->mCamSpacePosX));
      return;

    case 2:
      if (_ZN6Player12GetTalkStateEv(player) != (-1))
    {
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
    {
      u8 *p = &this->mTalkStep;
      *p = (*p) + 1;
    }
      this->mCutsceneStep = 4;
      func_ov060_02111f08();
      return;

  }

}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 57 -- func_ov060_021154e8, 0x021154e8, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_021154e8Ev
/*
State BOWSER_STATE_INTRO_WAIT (5): run the intro camera (func_ov060_02111f08),
stand still on the idle animation (0x10) and run the conversation
(func_ov060_02115518), whose step 0 moves him to BOWSER_STATE_INTRO. */
void daKpa_c::func_ov060_021154e8(){
    func_ov060_02111f08();
    this->mHorzSpeed = 0;
    func_ov060_02111cc0(BOWSER_ANIM_IDLE, 0);
    func_ov060_02115518();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 56 -- func_ov060_021153f8, 0x021153f8, size 0xf0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_021153f8Ev
/*
State BOWSER_STATE_INTRO (6): run the intro camera (func_ov060_02111f08), then by
mStep. Step 0: func_ov060_02115744 (animation 0x13), advancing when it returns 1.
Steps 1 and 2: func_ov060_02115718 (animation 0x11 looping); each completed loop
advances the step, and variant 2 advances it by two (from step 1 it skips step 2). Step 3:
func_ov060_021156ec (animation 0x12); when it returns 1, switch to the idle
animation and advance. Later steps: the conversation (func_ov060_02115518). */
void daKpa_c::func_ov060_021153f8(){
    u8 state;
    func_ov060_02111f08();
    state = this->mStep;
    if (state == 0) {
        if (func_ov060_02115744() == 0) return;
        {
            u8 *p = &this->mStep;
            *p = *p + 1;
        }
        return;
    }
    if (state <= 2) {
        if (func_ov060_02115718() == 0) return;
        {
            u8 *p = &this->mStep;
            *p = *p + 1;
            if (this->mVariantID == 2) {
                *p = *p + 1;
            }
        }
        return;
    }
    if (state == 3) {
        if (func_ov060_021156ec() == 0) return;
        func_ov060_02111cc0(BOWSER_ANIM_IDLE, 0);
        {
            u8 *p = &this->mStep;
            *p = *p + 1;
        }
        return;
    }
    func_ov060_02115518();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 55 -- func_ov060_02115314, 0x02115314, size 0xe4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02115314Ev
/*
Idle pick for variant 0, called from func_ov060_02114f88. With mPickToggle 0: if
he faces the target (BOWSER_COND_FACING_TARGET), go to BOWSER_STATE_ANIM_CHAIN
when mDistToTarget < 0x5dc000 (1500 units) and BOWSER_STATE_HOP otherwise; if not,
BOWSER_STATE_TURN_TO_TARGET; then set the toggle and reset mAnimSpeed to 0x1000
(1.0). With mPickToggle set: clear it; unless mSkipIdleRoll is set (then clear
that and turn), one draw in ten (random >> 16, modulo 10, equal to 0) picks
BOWSER_STATE_PLAY_ANIM_0F, otherwise BOWSER_STATE_TURN_TO_TARGET. */
void daKpa_c::func_ov060_02115314(){
    if (this->mPickToggle == 0) {
        if (this->mCondFlags & BOWSER_COND_FACING_TARGET) {
            if (this->mDistToTarget < 0x5dc000)
                this->mState = BOWSER_STATE_ANIM_CHAIN;
            else
                this->mState = BOWSER_STATE_HOP;
        } else {
            this->mState = BOWSER_STATE_TURN_TO_TARGET;
        }
        unsigned char* q = &this->mPickToggle;
        *q = *q + 1;
        this->mAnimSpeed = 0x1000;
    } else {
        this->mPickToggle = 0;
        if (this->mSkipIdleRoll == 0) {
            unsigned int v = (unsigned int)RandomIntInternal(&data_0209e650) >> 0x10;
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 54 -- func_ov060_021151d4, 0x021151d4, size 0x140 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_021151d4Ev
/*
Idle pick for variant 1, called from func_ov060_02114f88. With mPickToggle set:
clear it and go to BOWSER_STATE_TURN_TO_TARGET. Otherwise, if he is not facing
the target, BOWSER_STATE_TURN_TO_TARGET; if he is, and DistToCPlayer() <
0x514000 (1300 units), a draw modulo 10 below mVanishChance picks
BOWSER_STATE_VANISH_DASH (mVanishChance then drops to 3 if it was above 3, else
to 1) and anything else picks BOWSER_STATE_FIREBALLS (mVanishChance back to 5);
if the Player is farther away, BOWSER_STATE_CHARGE, upgraded to
BOWSER_STATE_JUMP on a draw below 5 when mDistToCenter is between 0x1f4000 (500
units) and 0x5dc000 (1500 units). Every path except the cleared-toggle one sets
the toggle. */
void daKpa_c::func_ov060_021151d4(){
    unsigned int m;

    if (this->mPickToggle != 0)
        goto cold;

    if ((this->mCondFlags & BOWSER_COND_FACING_TARGET) == 0)
        goto setE;

    if (_ZN8dActor_c13DistToCPlayerEv(this) < 0x514000) {
        m = ((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) % 10;
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
        goto tail;
    }

    this->mState = BOWSER_STATE_CHARGE;
    if (this->mDistToCenter > 0x1f4000 && this->mDistToCenter < 0x5dc000) {
        m = ((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) % 10;
        if (m < 5)
            this->mState = BOWSER_STATE_JUMP;
    }
    goto tail;

setE:
    this->mState = BOWSER_STATE_TURN_TO_TARGET;
tail:
    {
        unsigned char *p = &this->mPickToggle;
        *p = *p + 1;
    }
    return;

cold:
    this->mPickToggle = 0;
    this->mState = BOWSER_STATE_TURN_TO_TARGET;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 53 -- func_ov060_021150d0, 0x021150d0, size 0x104 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_021150d0Ev
/*
Choose an attack for variant 2, called from func_ov060_02115060. If he faces the
target: with DistToCPlayer() < 0x3e8000 (1000 units), a draw modulo 10 below 4
picks BOWSER_STATE_FIREBALLS, else a second draw below 8 picks
BOWSER_STATE_FIRE_BREATH, else BOWSER_STATE_ANIM_CHAIN (mFireTimer is cleared and
mAnimSpeed set to 0x1000 on all three); with the Player farther away a draw below
5 picks BOWSER_STATE_JUMP, else BOWSER_STATE_CHARGE. If he does not face the
target, BOWSER_STATE_TURN_TO_TARGET. */
#include "dActor_c.h"
extern "C" {
extern int RandomIntInternal(int* seed);
extern int data_0209e650;
}
void daKpa_c::func_ov060_021150d0(){
    dActor_c *a = this;
    if (this->mCondFlags & BOWSER_COND_FACING_TARGET) {
        if (a->DistToCPlayer() < 0x3e8000) {
            if (((unsigned)RandomIntInternal(&data_0209e650) >> 0x10) % 10 < 4) {
                this->mState = BOWSER_STATE_FIREBALLS;
            } else if (((unsigned)RandomIntInternal(&data_0209e650) >> 0x10) % 10 < 8) {
                this->mState = BOWSER_STATE_FIRE_BREATH;
            } else {
                this->mState = BOWSER_STATE_ANIM_CHAIN;
            }
            this->mFireTimer = 0;
            this->mAnimSpeed = 0x1000;
        } else if (((unsigned)RandomIntInternal(&data_0209e650) >> 0x10) % 10 < 5) {
            this->mState = BOWSER_STATE_JUMP;
        } else {
            this->mState = BOWSER_STATE_CHARGE;
        }
    } else {
        this->mState = BOWSER_STATE_TURN_TO_TARGET;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 52 -- func_ov060_021150c4, 0x021150c4, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_021150c4Ev
/*
Pick BOWSER_STATE_JUMP (the choice for variants with mChooseJumpOnly set). */
void daKpa_c::func_ov060_021150c4(){
    this->mState = BOWSER_STATE_JUMP;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 51 -- func_ov060_02115060, 0x02115060, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02115060Ev
/*
Idle pick for variant 2, called from func_ov060_02114f88. With mPickToggle 0:
func_ov060_021150d0, or func_ov060_021150c4 when mChooseJumpOnly is set, then set
the toggle. With the toggle set: clear it and go to
BOWSER_STATE_TURN_TO_TARGET. */
void daKpa_c::func_ov060_02115060(){
  if (this->mPickToggle == 0)
  {
    if (this->mChooseJumpOnly == 0)
    {
      func_ov060_021150d0();
    }
    else
    {
      func_ov060_021150c4();
    }
    (this->mPickToggle)++;
    return;
  }
  this->mPickToggle = 0;
  this->mState = BOWSER_STATE_TURN_TO_TARGET;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 50 -- func_ov060_02115018, 0x02115018, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02115018Ev
/*
Fall catch for the variant-2 jumps: if he is moving down (mVertSpeed < 0) and
mPosY is below mHomePosY - 0x12c000 (300 units below the arena level), put him
at X = Z = 0, 0x7d0000 (2000 units) above the arena level, with both speeds
zeroed. */
void daKpa_c::func_ov060_02115018(){
    int r1 = this->mVertSpeed;
    if (r1 >= 0) return;
    int thresh = this->mHomePosY - 0x12c000;
    int cur = this->mPosY;
    if (cur >= thresh) return;
    this->mPosZ = 0;
    this->mPosX = this->mPosZ;
    this->mPosY = this->mHomePosY + 0x7d0000;
    this->mVertSpeed = 0;
    this->mHorzSpeed = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 49 -- func_ov060_02114ff8, 0x02114ff8, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02114ff8Ev
/*
State BOWSER_STATE_WAIT_ANIM_END (0x12): back to BOWSER_STATE_IDLE once the
animation is at its last frame. */
void daKpa_c::func_ov060_02114ff8(){
    int r = Bowser_IsAnimAtLastFrame();
    if (r) {
        this->mState = BOWSER_STATE_IDLE;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 48 -- func_ov060_02114f88, 0x02114f88, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02114f88Ev
/*
State BOWSER_STATE_IDLE (0): clear unk_422, play the idle animation (0x10), zero
mSpinSpeed and both speeds, then let the variant pick the next state:
func_ov060_02115314 (variant 0), func_ov060_021151d4 (variant 1) or
func_ov060_02115060 (variant 2; variant 3 was folded into 0 at init). */
void daKpa_c::func_ov060_02114f88(){
  this->unk_422 = 0;
  func_ov060_02111cc0(BOWSER_ANIM_IDLE, 0);
  this->mSpinSpeed = 0;
  this->mHorzSpeed = 0;
  this->mVertSpeed = 0;
  if(this->mVariantID == 0) func_ov060_02115314();
  else if(this->mVariantID == 1) func_ov060_021151d4();
  else func_ov060_02115060();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 47 -- func_ov060_02114e9c, 0x02114e9c, size 0xec */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02114e9cEv
/*
State BOWSER_STATE_ANIM_CHAIN (0xf): mHorzSpeed 0. While animation 8 is playing
(data_ov060_0211ac20 is entry 8 of the animation table), sound 0xb5 plays when
frame 8 is crossed. At the last frame of an animation: after animation 8
(data_ov060_0211ac20) start animation 6, after animation 6 (data_ov060_0211ac68)
start animation 7, after animation 7 (data_ov060_0211ac70) go back to
BOWSER_STATE_IDLE, and after any other animation (the one playing when the state
is entered) start animation 8; the three are started with 0x40000000 (play once). */
void daKpa_c::func_ov060_02114e9c(){
    this->mHorzSpeed = 0;
    if ((int)this->mModelAnim.file == data_ov060_0211ac20[1]) {
        if (this->mModelAnim.WillHitFrame(8)) {
            func_02012694(BOWSER_SND_B5, (const Vector3 *)(&this->mCamSpacePosX));
        }
    }
    if (Bowser_IsAnimAtLastFrame() == 0) return;
    if (*(int *)&this->mModelAnim.file == data_ov060_0211ac20[1]) {
        func_ov060_02111cc0(6, 0x40000000);
        return;
    }
    if (*(int *)&this->mModelAnim.file == data_ov060_0211ac68[1]) {
        func_ov060_02111cc0(7, 0x40000000);
        return;
    }
    if (*(int *)&this->mModelAnim.file == data_ov060_0211ac70[1]) {
        this->mState = BOWSER_STATE_IDLE;
        return;
    }
    func_ov060_02111cc0(8, 0x40000000);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 46 -- func_ov060_02114d08, 0x02114d08, size 0x194 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02114d08Ev
/*
State BOWSER_STATE_TURN_TO_TARGET (0xe). Each frame turn mAngleY toward
mAngleToTarget by 0x400 (5.6 degrees) for variant 1 and for mHealth above 2, by
0x300 (4.2 degrees) at mHealth 2, by 0x200 (2.8 degrees) otherwise
(ApproachLinear). Then by mStep. Step 0: clear mStepCounter and run
func_ov060_02115744 (animation 0x13); advance when it returns 1. Steps 1 and 2:
func_ov060_02115718 (animation 0x11 looping); each finished loop counts in
mStepCounter, and if BOWSER_COND_BREATHING is set it is cleared after five loops
and nothing else happens; otherwise, once the angle difference measured before
this frame's turn is under 0x2000 (45 degrees), restart the animation, advance
the step and clear the counter. Later: func_ov060_021156ec
(animation 0x12), then BOWSER_STATE_IDLE when it returns 1. */
void daKpa_c::func_ov060_02114d08(){
    int r4;
    short step;

    r4 = _ZN8dActor_c14GetSubtractionEss(this, this->mAngleY, this->mAngleToTarget);

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
    _Z14ApproachLinearRsss((s16*)(&this->mAngleY), this->mAngleToTarget, step);

    if (this->mStep == 0) {
        this->mStepCounter = 0;
        if (func_ov060_02115744() == 0) return;
        (this->mStep)++;
        return;
    }
    if (this->mStep <= 2) {
        if (func_ov060_02115718() == 0) return;
        (this->mStepCounter)++;
        if (this->mCondFlags & BOWSER_COND_BREATHING) {
            if (this->mStepCounter < 5) return;
            this->mCondFlags &= ~BOWSER_COND_BREATHING;
            return;
        }
        if (r4 >= 0x2000) return;
        this->mModelAnim.currFrame = 0;
        (this->mStep)++;
        this->mStepCounter = 0;
        return;
    }
    if (func_ov060_021156ec() != 0)
        this->mState = BOWSER_STATE_IDLE;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 45 -- func_ov060_02114b60, 0x02114b60, size 0x1a8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02114b60Ev
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
void daKpa_c::func_ov060_02114b60(){
  int *pflag = (int *)&this->mdCcAcPos_c.flags;
  *pflag |= BOWSER_CC_DISABLED;
  switch (this->mStep)
  {
    case 0:
      this->mTargetOpacity = 0;
      this->mStepCounter = 0x1e;
      if ((this->mTimer) == 0)
    {
      func_02012694(BOWSER_SND_VANISH, (const Vector3 *)(&this->mCamSpacePosX));
    }
      if ((this->mOpacity) != 0)
    {
      return;
    }
    {
      u8 *ps = &this->mStep;
      *ps = (*ps) + 1;
    }
      this->mAngleY = this->mAngleToTarget;
      return;

    case 1:
    {
      int r4 = 0;
      u16 *pd = (u16 *) ((((int) this) + 0x3fe));
      u16 *base3 = &this->mStepCounter;
      int sub;
      u16 h = *pd;
      u16 h2 = *base3;
      *((u16 *) ((((int) this) + 0x3fe))) = h - 1;
      if (h2 != 0)
      {
        this->mHorzSpeed = 0x64000;
      }
      else
      {
        r4 = 1;
      }
      sub = _ZN8dActor_c14GetSubtractionEss(this, this->mAngleY, this->mAngleToTarget);
      if (sub > 0x4000)
      {
        if ((this->mDistToTarget) > 0x1f4000)
        {
          r4 = 1;
        }
      }
      if (_ZNK10dBgCh_Actr10IsOnGroundEv(&this->mWithMeshClsn) == 0)
      {
        this->mPosX = this->mLastGroundPosX;
        this->mPosY = this->mLastGroundPosY;
        this->mPosZ = this->mLastGroundPosZ;
        r4 = 1;
        this->mHorzSpeed = 0;
      }
      if (r4 == 0)
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
      *pflag &= ~BOWSER_CC_DISABLED;
    }
      return;

    default:
      return;

  }

}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 44 -- func_ov060_02114858, 0x02114858, size 0x308 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02114858Ev
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
void daKpa_c::func_ov060_02114858(){
  int new_var;
  int new_var2;
  s32 v134 = (s32)this->mModelAnim.file;
  if ((v134 == data_ov060_0211acd8.match) || ((v134 == data_ov060_0211acf0.match) && ((((u32) (((u32) (this->mModelAnim.currFrame)) << 4)) >> 16) > 0x2a)))
  {
    s32 v12c = this->mModelAnim.currFrame;
    s32 v450 = *(volatile s32 *)&this->mSoundID;
    s32 r4 = ((u32) (v12c << 4)) >> 16;
    if (v450 != 0x180)
    {
      this->mSoundHandle = 0;
    }
    this->mSoundID = BOWSER_SND_FIRE_LOOP;
    this->mSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(this->mSoundHandle, 3, this->mSoundID, (const Vector3 *) (&this->mCamSpacePosX), 0);
    u16 t = this->mFireTimer;
    if ((t % 5) == 0)
    {
      s32 pos[3];
      pos[0] = this->mPosX;
      pos[1] = this->mPosY;
      pos[2] = this->mPosZ;
      {
        s32 y = pos[1];
        u16 ang = this->mPrevAngleY;
        s16 *tbl = data_02082214;
        s16 sx = tbl[(ang >> 4) * 2];
        new_var2 = (sx * 0xc8) + pos[0];
        pos[1] = y + 0xb4000;
        pos[0] = new_var2;
        ang = this->mPrevAngleY;
        pos[2] = (tbl[((ang >> 4) * 2) + 1] * 0xc8) + pos[2];
      }
      if ((r4 >= 0xf) && (r4 < 0x14))
      {
        s32 rnd = RandomIntInternal(&data_0209e650);
        u32 hi = ((u32) rnd) >> 16;
        u32 m = hi % 10u;
        _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(BOWSER_ACTOR_KOOPAFIRE, (m << 10) | 0x11, (Vector3 *) pos, (const Vector3_16 *) (&this->mPrevAngleX), (int)this->mAreaId, -1);
      }
      else
      {
        s32 rnd = RandomIntInternal(&data_0209e650);
        u32 hi = ((u32) rnd) >> 16;
        u32 m = hi % 10u;
        _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(BOWSER_ACTOR_KOOPAFIRE, (m << 10) | 1, (Vector3 *) pos, (const Vector3_16 *) (&this->mPrevAngleX), (int)this->mAreaId, -1);
      }
    }
    {
      u16 *pt = &this->mFireTimer;
      *pt = (*pt) + 1;
    }
  }
  {
    int *p = (int *)&this->mCondFlags;
    *p |= BOWSER_COND_BREATHING;
  }
  if (Bowser_IsAnimAtLastFrame() == 0)
  {
    return;
  }
  switch (this->mStep)
  {
    case 0:
      func_ov060_02111cc0(0x16, 0x40000000);
      func_02012694(BOWSER_SND_B5, (const Vector3 *)(&this->mCamSpacePosX));
    {
      u8 *ps = &this->mStep;
      *ps = (*ps) + 1;
    }
      return;

    case 1:
      func_ov060_02111cc0(0x15, 0);
    {
      u8 *ps = &this->mStep;
      *ps = (*ps) + 1;
    }
      return;

    case 2:
      func_ov060_02111cc0(0x17, 0x40000000);
    {
      u8 *ps = &this->mStep;
      *ps = (*ps) + 1;
    }
      return;

    case 3:
      this->mState = BOWSER_STATE_IDLE;
    {
      int *p = (int *) (((int)&this->mCondFlags));
      *p &= ~BOWSER_COND_BREATHING;
    }
      return;

  }

}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 43 -- func_ov060_021146d0, 0x021146d0, size 0x188 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_021146d0Ev
/*
State BOWSER_STATE_HURT_HOP (0xc), the knock-back after a bomb hit. On the first
frame: mHorzSpeed -0x1c000 (-28.0), mVertSpeed 0x50000 (80.0), mAngleY = angle to
the centre + 0x8000 (so the negative speed carries him toward the centre),
unk_422 = 1, mAngleX = -0xc00 and sound 0xb1. On later frames mAngleX drops by
0xc00 (16.9 degrees) per frame, from its unsigned value 0xf400 (-0xc00): 20 drops,
337.5 degrees in all, until that value is 0xc00 or less, then it is set to 0. By mStep: 0 plays animation 1 once
and clears mStepCounter; 1 runs the landing-dust helper on mStepCounter, plays
animation 2 once when the counter reaches 1 and, at 3, zeroes both speeds and
advances; 2 waits for the last frame, then goes to BOWSER_STATE_PLAY_ANIM_0F when
mHealth is 1 and to BOWSER_STATE_IDLE otherwise; unk_422 is cleared on every
frame of step 2. */
/* func_ov060_021146d0 at 0x021146d0 (ov060), size 0x188
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
void daKpa_c::func_ov060_021146d0(){
    if (this->mTimer == 0) {
        this->mHorzSpeed = -0x1c000;
        this->mVertSpeed = 0x50000;
        this->mAngleY = this->mAngleToCenter + 0x8000;
        this->unk_422 = 1;
        this->mAngleX = -0xc00;
        func_02012694(BOWSER_SND_LEAP, (const Vector3 *)(&this->mCamSpacePosX));
    } else {
        if ((u16)this->mAngleX > 0xc00) {
            s16 *p8c = &this->mAngleX;
            *p8c = *p8c - 0xc00;
        } else {
            this->mAngleX = 0;
        }
    }
    {
        u8 st = this->mStep;
        if (st == 0) {
            func_ov060_02111cc0(1, 0x40000000);
            {
                u8 *p = &this->mStep;
                *p = *p + 1;
            }
            this->mStepCounter = 0;
            return;
        }
        if (st == 1) {
            func_ov060_02115a84((char *)&this->mStepCounter);
            if (this->mStepCounter == 1)
                func_ov060_02111cc0(2, 0x40000000);
            if (this->mStepCounter < 3) return;
            this->mVertSpeed = 0;
            this->mHorzSpeed = 0;
            {
                u8 *p = &this->mStep;
                *p = *p + 1;
            }
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 42 -- func_ov060_0211469c, 0x0211469c, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_0211469cEv
/*
Jump wind-up: play animation 0xb once; return 1 when the animation will cross
frame 0x20 on this advance, otherwise 0. */
int daKpa_c::func_ov060_0211469c(){
    func_ov060_02111cc0(0xb, 0x40000000);
    int r = this->mModelAnim.WillHitFrame(0x20);
    if (r != 0) return 1;
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 41 -- func_ov060_021145d4, 0x021145d4, size 0xc8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_021145d4Ev
/*
Jump landing check. If the floor collider says he is on the ground: zero both
speeds, call func_0200fa04 with his position, play animation 0xc once, play sound
0xb6; for variant 0 also call func_ov002_020c56f0 with the target Player and
(mDistToTarget >= 0x352000, 850 units) when that Player is not in the air; return
1. Otherwise return 0. */
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
int daKpa_c::func_ov060_021145d4(){
  struct Vector3 v;
  int b;
  if(_ZNK10dBgCh_Actr10IsOnGroundEv(&this->mWithMeshClsn)){
    this->mHorzSpeed = 0;
    this->mVertSpeed = 0;
    v.x = this->mPosX;
    v.y = this->mPosY;
    v.z = this->mPosZ;
    func_0200fa04(this, &v, 0);
    func_ov060_02111cc0(0xc, 0x40000000);
    func_02012694(BOWSER_SND_LAND, (const Vector3 *)(&this->mCamSpacePosX));
    if(this->mVariantID == 0){
      b = (this->mDistToTarget >= 0x352000);
      if(!_ZN6Player7IsInAirEv(this->mTargetPlayer))
        func_ov002_020c56f0((unsigned char *)this->mTargetPlayer, b);
    }
    return 1;
  }
  return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 40 -- func_ov060_021145a8, 0x021145a8, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_021145a8Ev
/*
Variant 2 only, and only while BOWSER_COND_RECOVERING is set: when mDistToCenter
is above 0x3e8000 (1000 units) set mHorzSpeed to 0x1e000 (30.0). */
void daKpa_c::func_ov060_021145a8(){
    if (this->mVariantID != 2) return;
    if (!(this->mCondFlags & BOWSER_COND_RECOVERING)) return;
    if (this->mDistToCenter > 0x3e8000) {
        this->mHorzSpeed = 0x1e000;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 39 -- func_ov060_021143b8, 0x021143b8, size 0x1f0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_021143b8Ev
/*
State BOWSER_STATE_JUMP (0xd). Step 0: wind-up (func_ov060_0211469c); then
mVertSpeed = 0x28000 (40.0) for variant 2 while BOWSER_COND_RECOVERING is set,
0x32000 (50.0) otherwise; sound 0xb1; zero mHorzSpeed and mStepCounter, let
func_ov060_021145a8 set a horizontal speed, and if it stays 0 snap X and Z to
mLastGroundPos; advance and reset mAnimSpeed to 0x1000. Step 1: variant 2
recovering runs func_ov060_02115018 first; he lands when func_ov060_021145d4
reports it, or when mPosY has dropped below mLastGroundPosY (then, if still
moving vertically, the landing effects are replayed here); landing clears
BOWSER_COND_RECOVERING, zeroes both speeds, puts him at mLastGroundPosY, advances,
runs func_ov060_02115b0c and, for variant 1, goes to BOWSER_STATE_ARENA_TILT and
records the KOOPA2BG actor's uniqueID in mArenaBgUniqueID. Step 2: at the last
frame, BOWSER_STATE_IDLE. */
void daKpa_c::func_ov060_021143b8(){
    unsigned char st = this->mStep;
    int vec[3];

    if (st == 0) {
        if (func_ov060_0211469c() == 0) return;
        if (this->mVariantID == 2 && (this->mCondFlags & BOWSER_COND_RECOVERING)) {
            this->mVertSpeed = 0x28000;
        } else {
            this->mVertSpeed = 0x32000;
        }
        func_02012694(BOWSER_SND_LEAP, (const Vector3 *)(&this->mCamSpacePosX));
        this->mStepCounter = 0;
        this->mHorzSpeed = 0;
        func_ov060_021145a8();
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
            func_ov060_02115018();
        }
        if (func_ov060_021145d4() == 0) {
            if (this->mPosY >= this->mLastGroundPosY) return;
        }
        if (this->mVertSpeed != 0) {
            vec[0] = this->mPosX;
            vec[1] = this->mPosY;
            vec[2] = this->mPosZ;
            func_0200fa04(this, vec, 0);
            func_ov060_02111cc0(0xc, 0x40000000);
        }
        this->mCondFlags &= ~BOWSER_COND_RECOVERING;
        this->mHorzSpeed = 0;
        this->mVertSpeed = 0;
        this->mPosY = this->mLastGroundPosY;
        (this->mStep)++;
        func_ov060_02115b0c();
        if (this->mVariantID == 1) {
            void* a;
            this->mState = BOWSER_STATE_ARENA_TILT;
            a = _ZN8dActor_c15FindWithActorIDEjPS_(BOWSER_ACTOR_KOOPA2BG, 0);
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 38 -- func_ov060_02114300, 0x02114300, size 0xb8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02114300Ev
/*
State BOWSER_STATE_HOP (0x11). Step 0: wind-up (func_ov060_0211469c); then
mVertSpeed 0x32000 (50.0), mHorzSpeed 0x19000 (25.0), mStepCounter 0, sound 0xb1,
advance. Step 1: advance once func_ov060_021145d4 reports the landing. Step 2:
BOWSER_STATE_IDLE at the last frame. */
void daKpa_c::func_ov060_02114300(){
    unsigned char k = this->mStep;
    if (k == 0) {
        if (!func_ov060_0211469c()) return;
        {
            u8 *p;
            this->mVertSpeed = 0x32000;
            this->mHorzSpeed = 0x19000;
            this->mStepCounter = 0;
            p = &this->mStep;
            *p = *p + 1;
            func_02012694(BOWSER_SND_LEAP, (const Vector3 *)(&this->mCamSpacePosX));
        }
    } else if (k == 1) {
        if (!func_ov060_021145d4()) return;
        {
            unsigned char *p = &this->mStep;
            *p = *p + 1;
        }
    } else {
        if (Bowser_IsAnimAtLastFrame()) this->mState = BOWSER_STATE_IDLE;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 37 -- func_ov060_021142b4, 0x021142b4, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_021142b4Ev
/*
State BOWSER_STATE_PLAY_ANIM_1B (0xa): mHorzSpeed 0, clear mStepCounter on the
first frame, play animation 0x1b once, then BOWSER_STATE_TURN_IN_PLACE at its last
frame. */
void daKpa_c::func_ov060_021142b4(){
    this->mHorzSpeed = 0;
    unsigned short* p = &this->mTimer;
    if (*p == 0) {
        this->mStepCounter = 0;
    }
    func_ov060_02111cc0(0x1b, 0x40000000);
    int r = Bowser_IsAnimAtLastFrame();
    if (r != 0) {
        this->mState = BOWSER_STATE_TURN_IN_PLACE;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 36 -- func_ov060_021140c0, 0x021140c0, size 0x1f4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_021140c0Ev
/*
State BOWSER_STATE_FIREBALLS (9). On the first frame choose mFireballShots: 3 if
the target Player has 4 or fewer health (Player::GetHealth), else a random 1 to 3
(also when there is no Player). When animation 0x14 (data_ov060_0211abe0 is entry
0x14 of the animation table) will cross frame 5, spawn a fireball (SpawnFireball,
horizontal speed 0x1e000 = 30.0, second Fix12 argument 0xa000) at his position
plus 0xe8 (232) units out along mAngleY and 0x58000 (88 units) up, with sound
0x122. At the last frame of an animation: if it was animation 0x14, count the
shot in mStep and go to BOWSER_STATE_IDLE once mStep reaches mFireballShots;
otherwise start animation 0x14; then restart the animation from frame 0. */
void daKpa_c::func_ov060_021140c0(){
    int new_var;
    if (this->mTimer == 0) {
        void* player = this->mTargetPlayer;
        if (player == 0 || _ZN6Player9GetHealthEv(player) > 4) {
            s32 rnd = RandomIntInternal(&data_0209e650);
            u32 hi = ((u32)rnd) >> 16;
            u32 m = hi % 10u;
            this->mFireballShots = (char)((m % 3u) + 1);
        } else {
            this->mFireballShots = 3;
        }
    }
    if ((int)this->mModelAnim.file == data_ov060_0211abe0[1]) {
        if (this->mModelAnim.WillHitFrame(5)) {
            Vector3 pos;
            u16 dir[3];
            u16 ax, ay, az;
            s16* tbl;
            s16 scale;
            int grav;
            /* `dp` must be a live pointer here, not a folded `&dir` at the call: the
               address has to be materialised before the two smlabb steps so it holds
               r2 across them, which is what pushes pos.x to r3 and `scale` to ip. */
            u16* dp;

            pos.x = this->mPosX;
            tbl = data_02082214;
            pos.y = this->mPosY;
            scale = 0xe8;
            pos.z = this->mPosZ;
            ax = *(u16 *)&this->mAngleX;
            ay = *(u16 *)&this->mAngleY;
            dir[1] = ay;
            dir[0] = ax;
            az = *(u16 *)&this->mAngleZ;
            dir[2] = az;
            pos.x = (tbl[(dir[1] >> 4) * 2] * scale) + pos.x;
            new_var = pos.y + 0x58000;
            dp = dir;
            pos.z = (tbl[((dp[1] >> 4) * 2) + 1] * scale) + pos.z;
            /* Scheduling barrier, not dead code. Both operands are provably non-null,
               so this changes nothing at runtime, but the short-circuit `&&` splits the
               block and stops the scheduler hoisting the 0xa000 constant into the slot
               the ROM gives to `dp`. Without it this function is 11 words off. */
            if (dp != 0 && this != 0) {
            }
            pos.y = new_var;
            dir[0] = 0x1000;
            grav = 0xa000;
            _ZN8dActor_c13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(
                this, &pos, dp, 0x1e000, grav, 0);
            func_02012694(BOWSER_SND_FIREBALL, (const Vector3 *)(&this->mCamSpacePosX));
        }
    }
    if (Bowser_IsAnimAtLastFrame() != 0) {
        if ((int)this->mModelAnim.file == data_ov060_0211abe0[1]) {
            unsigned char* p = &this->mStep;
            *p = (*p) + 1;
            if (this->mStep >= this->mFireballShots) {
                this->mState = BOWSER_STATE_IDLE;
            }
        } else {
            func_ov060_02111cc0(0x14, 0);
        }
        this->mModelAnim.currFrame = 0;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 35 -- func_ov060_02113ff4, 0x02113ff4, size 0xcc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02113ff4Eti
/*
Turn in place for the caller's frame count: by mStep, play animation 0x13 once,
then 0x12 once (each step ends at the animation's last frame), then the idle
animation; mHorzSpeed 0 and mAngleY += arg2 every call. Returns 1 once mTimer is
at least arg1 (an unsigned halfword), else 0. */
int daKpa_c::func_ov060_02113ff4(unsigned short arg1, int arg2){
    int r;
    unsigned char f = this->mStep;
    if (f == 0) {
        func_ov060_02111cc0(0x13, 0x40000000);
        if (Bowser_IsAnimAtLastFrame() != 0) {
            unsigned char *p = &this->mStep;
            *p = *p + 1;
        }
    } else if (f == 1) {
        func_ov060_02111cc0(0x12, 0x40000000);
        if (Bowser_IsAnimAtLastFrame() != 0) {
            unsigned char *p = &this->mStep;
            *p = *p + 1;
        }
    } else {
        func_ov060_02111cc0(BOWSER_ANIM_IDLE, 0);
    }
    r = 0;
    this->mHorzSpeed = 0;
    {
        short *q = &this->mAngleY;
        *q += arg2;
    }
    if (this->mTimer >= arg1)
        r = 1;
    return r;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 34 -- func_ov060_02113fcc, 0x02113fcc, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02113fccEv
/*
State BOWSER_STATE_TURN_IN_PLACE (0xb): func_ov060_02113ff4 with 0x3e frames and
0x200 (2.8 degrees) per frame, about 177 degrees in all (63 calls, mTimer 0 to 0x3e); BOWSER_STATE_IDLE
when it returns 1. */
void daKpa_c::func_ov060_02113fcc(){
    int r0 = func_ov060_02113ff4(0x3e, 0x200);
    if (r0 != 0) {
        this->mState = BOWSER_STATE_IDLE;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- func_ov060_02113d8c, 0x02113d8c, size 0x240 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02113d8cEv
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
bool ApproachLinear(short &value, short target, short step);

/* This shard carried a one-method `struct dActor_c { GetSubtraction(...); }`
 * stand-in. dActor_c is the real class (include/dActor_c.h, already included
 * above) and GetSubtraction is declared there, so the local copy is dropped. */

extern "C" {
int _Z14ApproachLinearRiii(int *dst, int target, int step);
}

void daKpa_c::func_ov060_02113d8c(){
    if (this->mTimer == 0)
        this->mHorzSpeed = 0;

    switch (this->mStep) {
    case 0:
        func_ov060_02111cc0(0x18, 0x40000000);
        this->mStepCounter = 0;
        if (Bowser_IsAnimAtLastFrame() != 0)
            this->mStep = 1;
        break;
    case 1:
        func_ov060_02111cc0(0x19, 0);
        this->mHorzSpeed = 0x2a000;
        if (Bowser_IsAnimAtLastFrame() != 0) {
            u16 *p = &this->mStepCounter;
            *p = *p + 1;
            if (this->mStepCounter > 0xa)
                this->mStep = 3;
            if (this->mStepCounter >= 2) {
                if (((dActor_c *)this)->GetSubtraction(this->mAngleToTarget, this->mAngleY) > 0x2000) {
                    this->mStep = 3;
                    this->mParticleHandle = 0;
                }
            }
        }
        ApproachLinear(this->mAngleY, this->mAngleToTarget, 0x200);
        break;
    case 3:
        this->mStepCounter = 0;
        func_ov060_02111cc0(0x1a, 0x40000000);
        this->mParticleHandle = (unsigned int)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            this->mParticleHandle, 0x101, this->mPosX, this->mPosY + 0x32000, this->mPosZ, 0, 0);
        if (_Z14ApproachLinearRiii(&this->mHorzSpeed, 0, 0x1000) != 0)
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
            {
                u16 *p = &this->mStepCounter;
                *p = *p + 1;
            }
        }
        break;
    default:
        break;
    }

    if (((dBgCh_Actr *)(&this->mWithMeshClsn))->IsOnGround())
        return;
    this->mState = BOWSER_STATE_PLAY_ANIM_1B;
    this->mPosX = this->mLastGroundPosX;
    this->mPosY = this->mLastGroundPosY;
    this->mPosZ = this->mLastGroundPosZ;
    this->mHorzSpeed = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- func_ov060_02113d20, 0x02113d20, size 0x6c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02113d20Ev
/*
Bomb hit test: find the closest KIRAI actor (0x11c, daKirai_c); if it reports
that his position is close enough (func_ov060_02118544), set it off
(func_ov060_021185c4) and return 1, otherwise return 0. */
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
// func_ov060_02113d20 at 0x02113d20
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov060).
int daKpa_c::func_ov060_02113d20(){
    Vector3 v;
    dActor_c *closest = this->ClosestWithActorID(BOWSER_ACTOR_KIRAI);
    if (closest) {
        v.x = this->mPosX;
        v.y = this->mPosY;
        v.z = this->mPosZ;
        if (((daKirai_c *)closest)->func_ov060_02118544(&v)) {
            ((daKirai_c *)closest)->func_ov060_021185c4();
            return 1;
        }
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- func_ov060_02113b5c, 0x02113b5c, size 0x1c4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02113b5cEv
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
at the last frame. Every frame, if func_ov060_02113d20 reports a bomb hit, mHealth
drops by one: BOWSER_STATE_DEFEATED at 0 or below, BOWSER_STATE_HURT_HOP
otherwise. */
/* recovered: shared common types */
void daKpa_c::func_ov060_02113b5c(){
    if (this->mTimer < 2) {
        this->mStepCounter = 0;
    }

    int r4 = this->mPosY;
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
        rg.SetObjAndPos(v, (dActor_c*)this);
        if (rg.DetectClsn() != 0) {
            int hy = rg.clsnY;
            if (hy >= this->mHomePosY - 0x64000) r4 = hy;
        }
    }

    if (this->mStep == 0) {
        func_ov060_02111cc0(9, 0);

        int dy = this->mPosY - r4;
        if (dy > 0xc8000 || this->mDistToCenter > 0xdac000) {
            if (this->mHorzSpeed >= 0x1e000) {
                _Z14ApproachLinearRiii(&this->mHorzSpeed, 0x1e000, 0x4000);
            }
        }

        func_ov060_02115a84((char *)&this->mStepCounter);

        if (_ZNK10dBgCh_Actr10IsOnGroundEv(&this->mWithMeshClsn) != 0) {
            if (_ZNK10dBgCh_Actr13JustHitGroundEv(&this->mWithMeshClsn) == 0) {
                this->mHorzSpeed = 0;
                this->mBounceOnLand = 0;
                u8* p = &this->mStep;
                *p = *p + 1;
                func_ov060_02111cc0(0xd, 0x40000000);
            }
        }
    } else {
        if (Bowser_IsAnimAtLastFrame() != 0) {
            this->mState = BOWSER_STATE_IDLE;
        }
    }

    if (func_ov060_02113d20() != 0) {
        signed char* q = &this->mHealth;
        *q = *q - 1;
        if (this->mHealth <= 0) {
            this->mState = BOWSER_STATE_DEFEATED;
        } else {
            this->mState = BOWSER_STATE_HURT_HOP;
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- func_ov060_02113a94, 0x02113a94, size 0xc8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02113a94Ev
/*
Keep him out of sight under the arena during BOWSER_STATE_RECOVER: target
opacity 0; once mOpacity is 0, zero both speeds, put him 0x3e8000 (1000 units)
below mHomePosY and, if he is 0xed8000 (3800 units) or more from the origin,
pull X and Z back to the circle of radius 0xed8 (3800) in his direction from the
origin. */
void daKpa_c::func_ov060_02113a94(){
    this->mTargetOpacity = 0;
    if (this->mOpacity != 0) return;
    this->mHorzSpeed = 0;
    this->mVertSpeed = 0;
    this->mPosY = this->mHomePosY - 0x3e8000;
    if (Vec3_HorzLen((Vector3*)(&this->mPosX)) < 0xed8000) return;
    {
        Vector3 zero;
        int a;
        zero.x = 0;
        zero.y = 0;
        zero.z = 0;
        a = (int)(unsigned short)Vec3_HorzAngle(&zero, (Vector3*)(&this->mPosX)) >> 4;
        this->mPosX = (short)data_02082214[a * 2] * (short)0xed8;
        this->mPosZ = (short)data_02082214[a * 2 + 1] * (short)0xed8;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- func_ov060_02113740, 0x02113740, size 0x354 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02113740Ev
/*
State BOWSER_STATE_RECOVER (2), entered when he has fallen 1000 units below the
arena (func_ov060_02112ba8). Each frame: ground = mHomePosY - 0x7d0000 (2000
units below), unless he is at or above mHomePosY and a dBgCh_Gnd probe from
0x96000 (150 units) above mHomePosY finds a floor, whose height then replaces it;
hit is set if that floor has a collision ID; BOWSER_COND_RECOVERING is set. Step 0 (also hides
him with func_ov060_02113a94): zero mAngleX/Z on the first frame, then add 0x800
(11.25 degrees) to both each frame until mAngleX wraps back to 0 (32 frames),
and advance. Step 1: animation 0xb once; from frame 0x20 face the centre, set
mVertSpeed 0x28000 (40.0), sound 0xb1, gravity 0, target opacity 0xff, clear
mStepCounter and advance (before that, keep hiding him). Step 2: while mPosY >= mHomePosY,
gravity -0x1000 (-1.0) and steer mHorzSpeed (toward 0 by 0x5000 when
mDistToCenter is under 0x8fc000 / 2300 units for variant 1 or 0x9c4000 / 2500
units otherwise and the floor is within 0x64000 = 100 units of mHomePosY,
otherwise toward 0x4b000 = 75.0 by 0x2000); on landing (func_ov060_021145d4)
advance, run func_ov060_02115b0c if the probe found no floor ID, send variant 2
into BOWSER_STATE_JUMP with mAnimSpeed 0x4000 (4.0) and normal gravity -0x2000
when it did, and send variant 1 into BOWSER_STATE_ARENA_TILT (recording the
KOOPA2BG actor); func_ov060_02115018 runs every frame of the step. Step 3: at the
last frame, BOWSER_STATE_IDLE, BOWSER_COND_RECOVERING cleared, gravity -0x2000. */
void daKpa_c::func_ov060_02113740(){
    Vec3 pos;
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
        rc.SetObjAndPos(*(Vector3 *)&pos, (dActor_c *)this);
        if (rc.DetectClsn()) {
            ground = rc.clsnY;
            /* The original shard read the collision id through a hand-rolled struct whose
             * `char result[0x34]` sat at offset 0x10, so `&rc.result` was simply rc + 0x10 --
             * a probe-state field, not a member of dBgCh_Gnd. The real class does not name
             * it, so the offset is spelled directly. */
            if ((int)((dBgPi *)&rc)->GetClsnID() != -1)
                hit = 1;
        }
    }

    this->mCondFlags |= BOWSER_COND_RECOVERING;

    switch (this->mStep) {
    case 0:
    {
        s16 *p8c = (s16*)(&this->mAngleX);
        s16 *p90 = (s16*)(&this->mAngleZ);
        if (this->mTimer == 0) {
            this->mAngleZ = 0;
            this->mAngleX = this->mAngleZ;
        }
        *p8c += 0x800;
        *p90 += 0x800;
        if ((this->mAngleX & 0xffff) == 0)
            (this->mStep)++;
        func_ov060_02113a94();
        return;
    }
    case 1:
        func_ov060_02111cc0(0xb, 0x40000000);
        if ((((u32)this->mModelAnim.currFrame << 4) >> 16) >= 0x20) {
            this->mAngleY = this->mAngleToCenter;
            this->mPrevAngleY = this->mAngleY;
            this->mVertSpeed = 0x28000;
            func_02012694(BOWSER_SND_LEAP, (const Vector3 *)(&this->mCamSpacePosX));
            this->mVertAccel = 0;
            this->mTargetOpacity = 0xff;
            this->mStepCounter = 0;
            (this->mStep)++;
            return;
        }
        func_ov060_02113a94();
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
                    _Z14ApproachLinearRiii(&this->mHorzSpeed, 0, 0x5000);
                else
                    _Z14ApproachLinearRiii(&this->mHorzSpeed, 0x4b000, 0x2000);
            } else {
                _Z14ApproachLinearRiii(&this->mHorzSpeed, 0x4b000, 0x2000);
            }
        }
        if (func_ov060_021145d4()) {
            (this->mStep)++;
            if (hit == 0) {
                func_ov060_02115b0c();
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
                r = (dActor_c *)_ZN8dActor_c15FindWithActorIDEjPS_(BOWSER_ACTOR_KOOPA2BG, 0);
                if (r) this->mArenaBgUniqueID = r->uniqueID;
                this->mVertAccel = -0x2000;
            }
        }
        func_ov060_02115018();
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- func_ov060_02113710, 0x02113710, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02113710Ev
/*
State BOWSER_STATE_PLAY_ANIM_0F (3): play animation 0xf looping until its last
frame, then BOWSER_STATE_IDLE. */
void daKpa_c::func_ov060_02113710(){
    func_ov060_02111cc0(0xf, 0);
    if (Bowser_IsAnimAtLastFrame() != 0) {
        this->mState = BOWSER_STATE_IDLE;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov060_021135fc, 0x021135fc, size 0x114 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_021135fcEv
/*
The defeat payoff. Switch the body collider off. Variant 2: spawn the LAST_STAR
actor (0x11b, parameter 7) 0xa0000 (160 units) above him. Other variants:
particles 0xad and 0xae 50 units above him, an OBJ_KEY actor (0x11a, parameter =
variant) at his position that receives his mSpinSpeed (daObjKey_c::mSpinSpeed),
and sound 0xbb. */
void daKpa_c::func_ov060_021135fc(){
    volatile Vector3 base;
    int px, py, pz;
    px = this->mPosX;
    base.x = px;
    py = this->mPosY;
    base.y = py;
    pz = this->mPosZ;
    base.z = pz;
    py = py + 0x32000;
    base.y = py;

    this->mdCcAcPos_c.flags |= BOWSER_CC_DISABLED;

    if (this->mVariantID == 2) {
        Vector3 pos;
        pos.x = this->mPosX;
        pos.y = this->mPosY;
        pos.z = this->mPosZ;
        pos.y = pos.y + 0xa0000;
        _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(BOWSER_ACTOR_LAST_STAR, 7, &pos, 0, this->mAreaId, -1);
    } else {
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xad, base.x, base.y, base.z);
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xae, base.x, base.y, base.z);
        void *spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(BOWSER_ACTOR_OBJ_KEY, this->mVariantID, (Vector3*)(&this->mPosX), 0, this->mAreaId, -1);
        ((daObjKey_c *)spawned)->mSpinSpeed = this->mSpinSpeed;
        func_02012694(BOWSER_SND_DEFEATED, (Vector3*)(&this->mCamSpacePosX));
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov060_02113564, 0x02113564, size 0x98 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02113564Ev
/*
Defeat step 0 (from func_ov060_02112ddc): knock-back launch. Animation 0 once,
mHorzSpeed -0x1c000 (-28.0) for variant 2 and -0x19000 (-25.0) otherwise,
mVertSpeed 0x50000 (80.0), gravity -0x2000 (-2.0), mAngleY = angle to the centre
+ 0x8000 (so the negative speed carries him toward the centre), clear
mStepCounter, advance, widen the body collider's radius to 0xb4000 (180 units)
and play sound 0xb1. */
void daKpa_c::func_ov060_02113564(){
    func_ov060_02111cc0(0, 0x40000000);
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
    func_02012694(BOWSER_SND_LEAP, (const Vector3 *)(&this->mCamSpacePosX));
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov060_021134ac, 0x021134ac, size 0xb8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_021134acEv
/*
Defeat step 1: airborne after the launch. Sets unk_422, runs the landing-dust
helper on mStepCounter; on a fresh ground hit it starts animation 5 once when the
current animation is animation 0 (data_ov060_0211ac88 is entry 0 of the
animation table) and halves mHorzSpeed; once on the ground without a fresh hit
it zeroes the speed and advances. func_ov060_02112350 runs every call. */
void daKpa_c::func_ov060_021134ac(){
    this->unk_422 = 1;
    func_ov060_02115a84((char *)&this->mStepCounter);
    if (_ZNK10dBgCh_Actr13JustHitGroundEv(&this->mWithMeshClsn)) {
        if ((int)this->mModelAnim.file == data_ov060_0211ac88.b) {
            func_ov060_02111cc0(5, 0x40000000);
        }
        {
            this->mHorzSpeed = this->mHorzSpeed >> 1;
        }
    }
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(&this->mWithMeshClsn)) {
        if (!_ZNK10dBgCh_Actr13JustHitGroundEv(&this->mWithMeshClsn)) {
            this->mHorzSpeed = 0;
            {
                unsigned char* p = &this->mStep;
                *p = *p + 1;
            }
        }
    }
    func_ov060_02112350();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov060_02113404, 0x02113404, size 0xa8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02113404Ev
/*
Defeat step 2: wait for the Player. func_ov060_02112350 runs first. Returns 1
when the current animation is animation 3 (data_ov060_0211ac60 is entry 3 of the
animation table), a target Player exists, he is within 0x258000 (600 units)
horizontally and the Player's facing differs from the angle to the Player by
more than 0x6000 (135 degrees), that is, he is looking toward Bowser; otherwise
0. mStepCounter is cleared on every call. */
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
int daKpa_c::func_ov060_02113404(){
  int r = 0;
  func_ov060_02112350();
  if ((int)this->mModelAnim.file == *(int*)(data_ov060_0211ac60+4)) {
    char* base = (char *)this->mTargetPlayer;
    if (base != 0) {
      int* o = &this->mTargetPlayer->mPosX;
      struct Vector3 v;
      s16 ang;
      v.x = o[0];
      v.y = o[1];
      v.z = o[2];
      ang = this->mTargetPlayer->mAngleY;
      if (Vec3_HorzDist((struct Vector3*)(&this->mPosX), &v) < 0x258000) {
        if (_ZN8dActor_c14GetSubtractionEss(this, ang, this->mAngleToTarget) > 0x6000) r = 1;
      }
    }
  }
  this->mStepCounter = 0;
  return r;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov060_021132a4, 0x021132a4, size 0x160 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_021132a4Ev
/*
Defeat shrink-and-spin for variants 0 and 1, per frame; returns 1 once done.
Particle 0x99 is spawned 50 units above him each frame (mParticleHandle).
Once mScaleX is below 0xccc (about 0.8), mSpinSpeed grows by 0x80 per frame.
While mScaleX is above 0x334 (about 0.2) mScaleX and mScaleZ shrink by 0x52 per
frame; after that mScaleY shrinks by 0x29, he rises at 0xa000 (10.0) and gravity
is 0. The result is 1 once mScaleY is below 0x800 (0.5). mAngleY advances by
mSpinSpeed, mOpacity drops by 2 while above 2, and until done the looping sound
0xba plays (mSoundHandle is reset when the sound ID changes). */
int daKpa_c::func_ov060_021132a4(){
    int r4 = 0;
    volatile Vector3 pos;
    int ytmp;
    int z;

    pos.x = this->mPosX;
    ytmp = this->mPosY;
    pos.y = ytmp;
    z = this->mPosZ;
    pos.z = z;
    pos.y = ytmp + 0x32000;

    this->mParticleHandle = (int)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(volatile int *)&this->mParticleHandle, 0x99, pos.x, pos.y, z, 0, 0);

    if (this->mScaleX < 0xccc) {
        short *p402 = (short *)(int)M(&this->mSpinSpeed);
        *p402 = (short)(*p402 + 0x80);
    }

    if (this->mScaleX > 0x334) {
        int *p80 = (int *)(int)M(&this->mScaleX);
        int *p88 = (int *)(int)M(&this->mScaleZ);
        *p80 = *p80 - 0x52;
        *p88 = *p88 - 0x52;
    } else {
        int *p84 = (int *)(int)M(&this->mScaleY);
        *p84 = *p84 - 0x29;
        this->mVertSpeed = 0xa000;
        this->mVertAccel = 0;
    }

    if (this->mScaleY < 0x800)
        r4 = 1;

    {
        short *p8e = (short *)(int)M(&this->mAngleY);
        *p8e = (short)(*p8e + this->mSpinSpeed);
    }

    if (this->mOpacity > 2) {
        unsigned char *p41c = (unsigned char *)(int)M(&this->mOpacity);
        *p41c = (unsigned char)(*p41c - 2);
    }

    if (r4 == 0) {
        if (this->mSoundID != BOWSER_SND_SHRINK_LOOP)
            this->mSoundHandle = 0;
        this->mSoundID = BOWSER_SND_SHRINK_LOOP;
        this->mSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(
            this->mSoundHandle, 3, this->mSoundID, (const Vector3 *)(&this->mCamSpacePosX), 0);
    }
    return r4;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov060_02113260, 0x02113260, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02113260Ev
/*
Park him after the defeat: X and Z set to 0 (Y kept), scale 0, both speeds and
gravity 0, no shadow, body collider switched off. */
void daKpa_c::func_ov060_02113260(){
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov060_021130c0, 0x021130c0, size 0x1a0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_021130c0Ev
/*
Defeat conversation and shrink for variants 0 and 1 (defeat step 3). mStepCounter
is the phase: while it is 0 or 1 the conversation runs by mTalkStep, then
func_ov060_021132a4 runs each frame and, when it returns 1,
func_ov060_02113260 and func_ov060_021135fc run and this returns 1. Talk step 0:
Player::StartTalk(this, 1); on success change the music volume (0x14, 0x15666)
and advance. Step 1: once GetTalkState() is 0, show message 0xcd (variant 0) or
0xcf at his position; advance on success. Step 2: when GetTalkState() returns -1,
move to the next phase, advance, play animation 4 once, stop the loaded music
layer (0x3c) and set the music volume (0x7f, 0x7222). */
int daKpa_c::func_ov060_021130c0(){
    int ret = 0;
    u16 outer = this->mStepCounter;

    if (outer <= 1) {
        u8 inner;

        if (outer == 0) {
            u16* op = (u16*)LAUND(&this->mStepCounter);
            *op = *op + 1;
            this->mTalkStep = 0;
        }

        inner = this->mTalkStep;
        switch (inner) {
        case 0:
            if (_ZN6Player9StartTalkER7fBase_cb(this->mTargetPlayer, this, 1)) {
                _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x14, 0x15666);
                {
                    u8* p = (u8*)LAUND(&this->mTalkStep);
                    *p = *p + 1;
                }
            }
            break;

        case 1:
            if (_ZN6Player12GetTalkStateEv(this->mTargetPlayer) == 0) {
                int msg = (this->mVariantID == 0) ? 0xcd : 0xcf;
                if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
                        this->mTargetPlayer, this, msg, &this->mPosX, 0, 2)) {
                    u8* p = (u8*)LAUND(&this->mTalkStep);
                    *p = *p + 1;
                }
            }
            break;

        case 2:
            if (_ZN6Player12GetTalkStateEv(this->mTargetPlayer) == -1) {
                u16* op = (u16*)LAUND(&this->mStepCounter);
                u8* ip = (u8*)LAUND(&this->mTalkStep);
                int v;
                *op = *op + 1;
                v = *ip + 1;
                *ip = v;
                func_ov060_02111cc0(4, 0x40000000);
                _ZN5Sound22StopLoadedMusic_Layer1Ej(0x3c);
                _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x7222);
            }
            break;
        }
    } else {
        if (func_ov060_021132a4()) {
            func_ov060_02113260();
            func_ov060_021135fc();
            ret = 1;
        }
    }

    return ret;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov060_02112ee0, 0x02112ee0, size 0x1e0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02112ee0Ev
/*
Defeat conversation and fade-out for variant 2 (defeat step 0xa). mStepCounter is
the phase as in func_ov060_021130c0. Talk step 0: Player::StartTalk(this, 1).
Step 1: once GetTalkState() is 0, show message 0xd1, or 0xd2 when NumStars()
is 0x96 (150); when that succeeds also call Message::PrepareTalk. Step 2: when GetTalkState() returns
-1, stop the loaded music layer (0x3c), run func_ov060_021135fc (the star), move
to the next phase, advance and play animation 4 once. After the conversation: while
mOpacity is above 4 it drops by 4 per frame with particle 0x99 50 units above him;
then func_ov060_02113260 runs and this returns 1. */
int daKpa_c::func_ov060_02112ee0(){
    int ret = 0;
    unsigned short mode =
        this->mStepCounter;
    volatile int v[4];

    if (mode <= 1) {
        if (mode == 0) {
            unsigned short *op = (unsigned short *)LAUND(&this->mStepCounter);
            *op = *op + 1;
            this->mTalkStep = 0;
        }

        switch (this->mTalkStep) {
        case 0:
            if (_ZN6Player9StartTalkER7fBase_cb(
                    this->mTargetPlayer, this, 1)) {
                unsigned char *p =
                    (unsigned char *)LAUND(&this->mTalkStep);
                *p = *p + 1;
            }
            break;

        case 1:
            if (_ZN6Player12GetTalkStateEv(this->mTargetPlayer) == 0) {
                unsigned m = (NumStars() != 0x96) ? 0xd1 : 0xd2;
                if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
                        this->mTargetPlayer, this, m, &this->mPosX, 0, 2)) {
                    unsigned char *p =
                        (unsigned char *)LAUND(&this->mTalkStep);
                    *p = *p + 1;
                    _ZN7Message11PrepareTalkEv();
                }
            }
            break;

        case 2:
            if (_ZN6Player12GetTalkStateEv(this->mTargetPlayer) == -1) {
                _ZN5Sound22StopLoadedMusic_Layer1Ej(0x3c);
                func_ov060_021135fc();

                unsigned short *op =
                    (unsigned short *)LAUND(&this->mStepCounter);
                unsigned char *ip =
                    (unsigned char *)LAUND(&this->mTalkStep);
                *op = *op + 1;
                int t = *ip + 1;
                *ip = t;
                func_ov060_02111cc0(4, 0x40000000);
            }
            break;
        }
    } else {
        if (this->mOpacity > 4) {
            *(unsigned char *)LAUND(&this->mOpacity) -= 4;
            int y, z;
            v[0] = this->mPosX;
            v[1] = y = this->mPosY;
            v[2] = z = this->mPosZ;
            v[1] = y + 0x32000;

            *(void **)(&this->mParticleHandle) =
                _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                    *(volatile unsigned *)&this->mParticleHandle, 0x99,
                    v[0], v[1], z, 0, 0);
        } else {
            func_ov060_02113260();
            ret = 1;
        }
    }

    return ret;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov060_02112ddc, 0x02112ddc, size 0x104 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02112ddcEv
/*
State BOWSER_STATE_DEFEATED (4), by mStep. 0: func_ov060_02113564 (launch). 1:
func_ov060_021134ac (airborne until landed). 2: func_ov060_02113404; when it
returns 1, clear mStepCounter and go to step 0xa for variant 2, else step 3. 3:
func_ov060_021130c0, advancing when it returns 1. 0xa: func_ov060_02112ee0,
advancing to 0xb when it returns 1. 0xb: nothing. */
void daKpa_c::func_ov060_02112ddc(){
    unsigned char *p;
    switch (this->mStep) {
    case 0: func_ov060_02113564(); break;
    case 1: func_ov060_021134ac(); break;
    case 2:
        if (func_ov060_02113404() == 0) break;
        this->mStepCounter = 0;
        if (this->mVariantID == 2) { this->mStep = 0xa; break; }
        p = &this->mStep;
        *p += 1;
        break;
    case 3:
        if (func_ov060_021130c0() == 0) break;
        p = &this->mStep;
        *p += 1;
        break;
    case 10:
        if (func_ov060_02112ee0() != 0) { p = &this->mStep; *p += 1; }
        break;
    case 11: break;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov060_02112d48, 0x02112d48, size 0x94 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02112d48Ei
/*
Tilt the arena: find the KOOPA2BG actor by mArenaBgUniqueID (nothing if it is
gone) and set its mAngleXSpeed to arg * cos and its mAngleZSpeed to -(arg * sin)
of the angle mAngleToCenter + 0x8000 (the opposite of the direction to the
centre), both Fix12 products shifted back down by 12. */
void daKpa_c::func_ov060_02112d48(int arg){
    daKpa2Bg_c *o = (daKpa2Bg_c *)_ZN8dActor_c10FindWithIDEj(this->mArenaBgUniqueID);
    if (o == 0)
        return;
    short angle = this->mAngleToCenter;
    unsigned int idx = (unsigned short)(short)(angle + 0x8000) >> 4;
    int k = idx * 2;
    o->mAngleXSpeed = (short)((arg * data_02082214[k + 1]) >> 12);
    o->mAngleZSpeed = (short)((arg * -data_02082214[k]) >> 12);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov060_02112bfc, 0x02112bfc, size 0x14c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02112bfcEv
/*
State BOWSER_STATE_ARENA_TILT (0x13): the arena rocks for 186 frames. mVertSpeed
is held at mTerminalVelocity. If the KOOPA2BG actor is gone, back to
BOWSER_STATE_IDLE. Otherwise the table at data_ov060_02119294 (rows of three
halfwords: sign, scale, end frame; a row with end frame 0 ends it) is walked for
the first row whose end frame is above mTimer; the tilt speed v is scale times a
linear ramp in mTimer (counting down to the row's end for a positive sign, up
from the previous row's end otherwise) and goes to func_ov060_02112d48; on odd
frames with v non-zero func_ov060_02117a3c is called on the arena. When no row is
left, go to BOWSER_STATE_IDLE and zero the arena's three angular speeds and three
angles, and mVertSpeed. */
extern "C" {
extern short data_ov060_02119294[];
extern short data_ov060_02119296[];
extern short data_ov060_02119298[];
/* p is volatile so the walk reloads the sentinel for each test -- lifetimes
 * on would keep p[2] in a register and skip the ROM's second load, and the
 * flag and the computed value are split so the flag keeps r6. */
}
void daKpa_c::func_ov060_02112bfc(){
    daKpa2Bg_c* found; int i; int flag; volatile short* p;
    this->mVertSpeed = this->mTerminalVelocity;
    found = (daKpa2Bg_c*)_ZN8dActor_c10FindWithIDEj(this->mArenaBgUniqueID);
    if (found == 0) { this->mState = BOWSER_STATE_IDLE; return; }
    p = data_ov060_02119294;
    i = 0;
    flag = 1;
    while (p[2] != 0) {
        int r1 = this->mTimer;
        if (r1 < p[2]) {
            int off = i * 6;
            short a = *(short*)((char*)data_ov060_02119294 + off);
            short b = *(short*)((char*)data_ov060_02119296 + off);
            int v;
            if (a > 0) {
                v = (short)(b * (*(short*)((char*)data_ov060_02119298 + off) - 1 - r1));
            } else {
                i -= 1; off = i * 6;
                v = (short)(b * (r1 - *(short*)((char*)data_ov060_02119298 + off)));
            }
            func_ov060_02112d48(v);
            if (v != 0 && (this->mTimer & 1)) { found->func_ov060_02117a3c(); }
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov060_02112ba8, 0x02112ba8, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02112ba8Ev
/*
True when he has fallen 0x3e8000 (1000 units) below mHomePosY while not already
in BOWSER_STATE_RECOVER or BOWSER_STATE_ARENA_TILT. (The result of the
IsOnGround call is discarded.) */
extern "C" {
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);
}
int daKpa_c::func_ov060_02112ba8(){
  int s = this->mState;
  if(s != BOWSER_STATE_RECOVER && s != BOWSER_STATE_ARENA_TILT){
    if(this->mPosY < this->mHomePosY - 0x3e8000) return 1;
    _ZNK10dBgCh_Actr10IsOnGroundEv(&this->mWithMeshClsn);
  }
  return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov060_021128c0, 0x021128c0, size 0x2e8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_021128c0Ev
/*
Hold state BOWSER_HOLD_NONE (0), the normal per-frame handler, called through the
hold table by func_ov060_02112434. Clears mHoldStep. Unless he is defeated, if the
body collider names an owner that is a Player (actorID 0xbf), hurt him
(Player::Hurt at Bowser's position, arguments 2, 0x8000, 1, 0, 1). Then call the
handler for mState from the table at data_ov060_0211aed4 (each row is a slot and
a packed target; target bit 0 selects a call through the vtable slot, the rest
is the this-adjustment), count mTimer, and on a state change reset mStep and
mTimer. Then: dActor_c::UpdatePos with the body collider; update the floor
collider (variant 1 uses func_02038408, others the continuous update); on the
ground, copy the floor normal into mGroundNormal and remember mLastGroundPos;
with mBounceOnLand set, a fresh ground hit makes mVertSpeed -60 percent of itself,
at most 0x14000 (20.0); otherwise, when the floor normal's Y is non-zero, mVertSpeed is recomputed from
the floor normal and the unnamed dActor_c words at 0xa4 and 0xac, minus 0x8000.
For the states whose byte is 1 in data_ov060_02119268 (0, 3, 7..0xb, 0xe..0x10,
0x12 and 0x13), if he has left the ground he is put back at mLastGroundPos and
moved 8 units toward the centre (the sin/cos table entry shifted left 3), else
mLastGroundPos is refreshed. Finally, func_ov060_02112ba8 sends him to
BOWSER_STATE_RECOVER. */
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
void daKpa_c::func_ov060_021128c0(){
    this->mHoldStep = 0;
    u32 id;
    if (this->mState != BOWSER_STATE_DEFEATED && (id = this->mdCcAcPos_c.otherOwner) != 0) {
        void* f = (void *)_ZN8dActor_c10FindWithIDEj(id);
        if (f != 0) {
            int b = (((dActor_c *)f)->actorID == BOWSER_ACTOR_PLAYER);
            if (b) {
                Vector3 v;
                v.x = this->mPosX;
                v.y = this->mPosY;
                v.z = this->mPosZ;
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(f, v, 2, 0x8000, 1, 0, 1);
            }
        }
    }

    s32 idx = this->mState;
    {
        TabEnt* e = &data_ov060_0211aed4[idx];
        int off = e->target;
        void* base = (void*)((char *)this + (off >> 1));
        void (*fn)(void*);
        if (off & 1)
            fn = (void (*)(void*))*(void**)((char*)(*(void***)base) + e->slot);
        else
            fn = (void (*)(void*))e->slot;
        fn(base);
    }

    {
        u16 *h = &this->mTimer;
        *h = *h + 1;
    }
    if (this->mState != idx) {
        this->mStep = 0;
        this->mTimer = 0;
    }

    _ZN8dActor_c9UpdatePosEP5dCc_c(this, &this->mdCcAcPos_c);

    if (this->mVariantID == 1)
        func_02038408(&this->mWithMeshClsn);
    else
        dBgCh_Actr_UpdateContinuous_Veneer(&this->mWithMeshClsn);

    if (_ZNK10dBgCh_Actr10IsOnGroundEv(&this->mWithMeshClsn)) {
        void* fr = _ZNK10dBgCh_Actr14GetFloorResultEv(&this->mWithMeshClsn);
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)fr + 4, (Vector3*)(&this->mGroundNormalX));
        this->mLastGroundPosX = this->mPosX;
        this->mLastGroundPosY = this->mPosY;
        this->mLastGroundPosZ = this->mPosZ;
        if (this->mBounceOnLand != 0 && _ZNK10dBgCh_Actr13JustHitGroundEv(&this->mWithMeshClsn)) {
            this->mVertSpeed = (this->mVertSpeed * -60) / 100;
            if (this->mVertSpeed >= 0x14000)
                this->mVertSpeed = 0x14000;
        } else if (this->mGroundNormalY != 0) {
            this->mVertSpeed = -(_ZN4cstd4fdivEii(
                (int)(((s64)this->mGroundNormalX * this->unk_0a4 + 0x800) >> 12)
              + (int)(((s64)this->mGroundNormalZ * this->unk_0ac + 0x800) >> 12),
                this->mGroundNormalY) + 0x8000);
        }
    }

    if (data_ov060_02119268[idx] != 0) {
        if (!_ZNK10dBgCh_Actr10IsOnGroundEv(&this->mWithMeshClsn)) {
            s32* px = &this->mPosX;
            s32* pz = &this->mPosZ;
            s16* tab = data_02082214;
            this->mPosX = this->mLastGroundPosX;
            this->mPosY = this->mLastGroundPosY;
            this->mPosZ = this->mLastGroundPosZ;
            {
                s32 a = (*(u16*)&this->mAngleToCenter >> 4);
                *px = *px + ((s32)tab[a * 2] << 3);
            }
            {
                s32 a = (*(u16*)&this->mAngleToCenter >> 4);
                *pz = *pz + ((s32)tab[a * 2 + 1] << 3);
            }
        } else {
            this->mLastGroundPosX = this->mPosX;
            this->mLastGroundPosY = this->mPosY;
            this->mLastGroundPosZ = this->mPosZ;
        }
    }

    if (func_ov060_02112ba8() == 0)
        return;
    this->mState = BOWSER_STATE_RECOVER;
    this->mStep = 0;
    this->mTimer = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov060_02112724, 0x02112724, size 0x19c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02112724Ev
/*
Hold state BOWSER_HOLD_HELD (1): a Player has grabbed the tail and swings him
around. BOWSER_COND_BREATHING is cleared. By mHoldStep: 0 switches the body collider
off, plays sound 0xb2, sets BOWSER_STATE_THROWN, starts animation 0xa and resets
mAnimSpeed, advance; 1 starts animation 9 (looping) once animation 0xa
reaches its last frame, and advances; 2 does nothing. Every frame: mSwingSpeed = the grabbed
Player's mAngleYSpeed, mAngleX = -|mSwingSpeed| (used directly as an angle),
mAngleY = the Player's mAngleY, and his position is the Player's plus 0xa0 (160)
units out along the Player's facing (X and Z) and 0x18000 (24 units) up minus
160 * sin(mAngleX) (Y). */
void daKpa_c::func_ov060_02112724(){
    int v;
    short a;
    int i;
    int j;
    Vector3 *s;

    this->mCondFlags &= ~BOWSER_COND_BREATHING;

    switch (this->mHoldStep) {
    case 0:
        this->mdCcAcPos_c.flags |= BOWSER_CC_DISABLED;
        func_02012694(BOWSER_SND_GRABBED, (const Vector3 *)(&this->mCamSpacePosX));
        this->mState = BOWSER_STATE_THROWN;
        func_ov060_02111cc0(0xa, 0);
        this->mAnimSpeed = 0x1000;
        (this->mHoldStep)++;
        break;
    case 1:
        if (Bowser_IsAnimAtLastFrame()) {
            func_ov060_02111cc0(9, 0);
            (this->mHoldStep)++;
        }
        break;
    case 2:
        break;
    }

    this->mSwingSpeed = ((Player *)this->mGrabbedPlayer)->mAngleYSpeed;
    v = this->mSwingSpeed;
    a = this->mGrabbedPlayer->mAngleY;
    if (v < 0) {
        v = -v;
    }
    this->mAngleX = -v;
    this->mAngleY = a;

    s = (Vector3 *)&this->mGrabbedPlayer->mPosX;
    this->mPosX = s->x;
    this->mPosY = s->y;
    this->mPosZ = s->z;

    i = ((unsigned short)a >> 4) * 2;

    this->mPosX += data_02082214[i] * 0xa0;
    j = ((unsigned short)this->mAngleX >> 4) * 2;
    this->mPosY += 0x18000 - data_02082214[j] * 0xa0;
    this->mPosZ += data_02082214[i + 1] * 0xa0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov060_021125f0, 0x021125f0, size 0x134 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_021125f0Ev
/*
Hold states BOWSER_HOLD_RELEASE_NOW (2) and BOWSER_HOLD_RELEASE_AFTER_ANIM (3):
the release. Plays animation 0xe once; state 3 waits for its last frame. Then: clear
mHoldStep and mHoldState, switch the body collider back on, set
BOWSER_STATE_THROWN, and launch him: v = |mSwingSpeed| * 0x46 / 6000, times 2.5
when v is above 0x2d; mHorzSpeed = v * cos(mAngleX) and mVertSpeed =
-v * sin(mAngleX) (table entries at mAngleX >> 4). The tail (by mTailUniqueID) goes
to BOWSER_TAIL_COOLDOWN with mTimer 0; mAngleX, mTimer and mStep are cleared and
the floor collider's ground flag is cleared. */
void daKpa_c::func_ov060_021125f0(){
    int v, nv;
    daKpaTail_c *a;

    func_ov060_02111cc0(0xe, 0x40000000);
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
    nv = -v;
    this->mHorzSpeed = v * data_02082214[((unsigned short)this->mAngleX >> 4 << 1) + 1];
    this->mVertSpeed = nv * data_02082214[(unsigned short)this->mAngleX >> 4 << 1];
    a = (daKpaTail_c *)_ZN8dActor_c10FindWithIDEj(this->mTailUniqueID);
    if (a != 0) {
        a->mState = BOWSER_TAIL_COOLDOWN;
        a->mTimer = 0;
    }
    this->mAngleX = 0;
    this->mTimer = 0;
    {
        this->mStep = 0;
        _ZN10dBgCh_Actr15ClearGroundFlagEv((char *)&this->mWithMeshClsn);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov060_02112434, 0x02112434, size 0x1bc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02112434Ev
/*
Per-frame conditions, hold-state dispatch and opacity fade. Measure the
horizontal distance and angle to the arena centre (the origin) into
mDistToCenter and mAngleToCenter and rebuild the low byte of mCondFlags (see
Bowser_CondFlag) from the four tests. Run the handler for mHoldState through the
table at data_ov060_0211aeb4 (0: func_ov060_021128c0, 1: func_ov060_02112724, 2
and 3: func_ov060_021125f0). Unless he is defeated, move mOpacity toward
mTargetOpacity by 0x14 (20) per frame, clamping at 0xff and 0. */
/* recovered: shared common types */
void daKpa_c::func_ov060_02112434(){
    Vector3 zero;
    zero.x = 0;
    zero.y = 0;
    zero.z = 0;
    this->mDistToCenter = Vec3_HorzDist((Vector3*)&this->mPosX, &zero);
    this->mAngleToCenter = Vec3_HorzAngle((Vector3*)&this->mPosX, &zero);

    int s0 = _ZN8dActor_c14GetSubtractionEss(this, this->mAngleY, this->mAngleToTarget);
    int s1 = _ZN8dActor_c14GetSubtractionEss(this, this->mAngleY, this->mAngleToCenter);

    this->mCondFlags &= ~0xff;
    if (s0 < 0x2000)
        this->mCondFlags |= BOWSER_COND_FACING_TARGET;
    if (s1 < 0x3800)
        this->mCondFlags |= BOWSER_COND_FACING_CENTER;
    if (this->mDistToCenter < 0x3e8000)
        this->mCondFlags |= BOWSER_COND_NEAR_CENTER;
    if (this->mDistToTarget < 0x352000)
        this->mCondFlags |= BOWSER_COND_TARGET_NEAR;

    (this->*data_ov060_0211aeb4[this->mHoldState].pmf)();

    if (this->mState == BOWSER_STATE_DEFEATED) return;

    unsigned char lo = this->mOpacity;
    unsigned char hi = this->mTargetOpacity;
    if (hi == lo) return;
    if (hi > lo) {
        int v = lo + 0x14;
        if (v >= 0xff) {
            this->mOpacity = 0xff;
            return;
        }
        this->mOpacity += 0x14;
        return;
    }
    {
        int v = lo - 0x14;
        if (v <= 0) {
            this->mOpacity = 0;
        } else {
            this->mOpacity -= 0x14;
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov060_021123dc, 0x021123dc, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_021123dcEv
/*
Fight-start values, called from InitResources: mPickToggle 1, fully opaque, variant
3 treated as 0, mHealth from the byte table at data_ov060_02119264 (1, 1, 3 in the
ROM, indexed by variant), state BOWSER_STATE_INTRO_WAIT, no hold, and unk_420 and
unk_422 cleared. */
void daKpa_c::func_ov060_021123dc(){
  this->mPickToggle=1;
  this->mOpacity=0xff;
  this->mTargetOpacity=0xff;
  if(this->mVariantID==3) this->mVariantID=0;
  this->mHealth=data_ov060_02119264[this->mVariantID];
  this->mState=BOWSER_STATE_INTRO_WAIT;
  this->mHoldState=BOWSER_HOLD_NONE;
  this->unk_420=0;
  this->unk_422=0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov060_021123c8, 0x021123c8, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_021123c8Ev
/*
Called when the tail lets go of a Player: set mBounceOnLand (func_ov060_021128c0
then bounces him when he next hits the ground) and forget the grabbed Player. */
void daKpa_c::func_ov060_021123c8(){
    this->mBounceOnLand = 1;
    this->mGrabbedPlayer = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov060_021123a0, 0x021123a0, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_021123a0Ei
/*
Body collider switch: a non-zero argument clears the disabled bit (dCc_c flags
bit 0) of mdCcAcPos_c, zero sets it. */
void daKpa_c::func_ov060_021123a0(int f){
    if (f)
        (this->mdCcAcPos_c.flags) &= ~BOWSER_CC_DISABLED;
    else
        (this->mdCcAcPos_c.flags) |= BOWSER_CC_DISABLED;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov060_02112350, 0x02112350, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02112350Ev
/*
When the current animation has finished and is animation 5 (data_ov060_0211acd0
is entry 5 of the animation table), start animation 3 (looping). */
void daKpa_c::func_ov060_02112350(){
  int r=this->mModelAnim.Finished();
  if(!r) return;
  if((int)this->mModelAnim.file != data_ov060_0211acd0[1]) return;
  func_ov060_02111cc0(3,0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov060_02111f08, 0x02111f08, size 0x448 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02111f08Ev
/*
The intro camera, one step per mCutsceneStep, run each frame by the two intro
states and once more by the conversation. Returns 1 at once when there is no
Player, and otherwise only when step 3 completes. Step 0: Camera::SetFlag_3; aim
the camera at the Player's position with Y replaced by Bowser's own (kept in
mCamLookAt); place the camera 0.625 times the horizontal distance to that point
away from Bowser, at the angle toward it minus 0x2000 (45 degrees), and 0xc8000
(200 units) above him (kept in mCamPos); Camera::SetPos. Step 1: func_020092c4 on the look-at;
count frames the Player is not in the air (the count resets while he is) and go
on once it exceeds 0x1e (30). Step 2: ease the camera position toward a spot at 0.375 times
the horizontal distance to the Player (at least 0x1f4000 = 500 units) at the
angle minus 0x1000 (22.5 degrees) by 0x4000 (4.0) per frame per axis, and the
look-at X and Z toward Bowser's by 0x1e000 (30.0); go on when func_020092c4 on
the look-at returns non-zero. Step 3: ease the look-at, by 0xa000 (10.0) per
axis, to a point 0xc0 (192) units ahead along mAngleY and 0xfa000 (250 units) up;
return 1 when func_020092c4 returns non-zero. Step 4: clear bit 0x8 of
Camera::mFlags. */
/* recovered: shared common types */
int daKpa_c::func_ov060_02111f08(){
    dCamera_c* cam = (dCamera_c *)data_0209f318;
    char* player = (char*)_ZN8dActor_c13ClosestPlayerEv(this);
    struct Vector3 sp;
    struct Vector3* pv;
    unsigned char* p;
    int dist;
    int v;
    int k;
    int d;

    if (player == 0)
        return 1;

    switch (this->mCutsceneStep) {
    case 0:
        cam->SetFlag_3();
        pv = (struct Vector3*)&((dActor_c *)player)->mPosX;
        sp.x = pv->x;
        sp.y = pv->y;
        sp.z = pv->z;
        sp.y = this->mPosY;
        cam->SetLookAt(sp);
        this->mCamLookAtX = sp.x;
        this->mCamLookAtY = sp.y;
        this->mCamLookAtZ = sp.z;
        dist = Vec3_HorzDist((struct Vector3*)(&this->mPosX), &sp);
        k = (unsigned short)(short)(Vec3_HorzAngle((struct Vector3*)(&this->mPosX), &sp) - 0x2000) >> 4;
        v = (int)(((long long)dist * 0xA00 + 0x800) >> 12);
        this->mCamPosX = this->mPosX + (int)(((long long)v * data_02082214[k * 2] + 0x800) >> 12);
        this->mCamPosY = this->mPosY + 0xc8000;
        this->mCamPosZ = this->mPosZ + (int)(((long long)v * data_02082214[k * 2 + 1] + 0x800) >> 12);
        cam->SetPos(*(const Vector3 *)(&this->mCamPosX));
        p = (unsigned char*)(&this->mCutsceneStep);
        *p = *p + 1;
        break;
    case 1:
        func_020092c4(cam, &cam->lookAt, &this->mCamLookAtX);
        if (_ZN6Player7IsInAirEv(player) != 0)
            this->mCutsceneTimer = 0;
        else {
            p = (unsigned char*)(&this->mCutsceneTimer);
            *p = *p + 1;
        }
        if (this->mCutsceneTimer > 0x1e) {
            p = (unsigned char*)(&this->mCutsceneStep);
            *p = *p + 1;
        }
        break;
    case 2:
        pv = (struct Vector3*)&((dActor_c *)player)->mPosX;
        sp.x = pv->x;
        sp.y = pv->y;
        sp.z = pv->z;
        dist = Vec3_HorzDist((struct Vector3*)(&this->mPosX), &sp);
        d = (int)(((long long)dist * 0x600 + 0x800) >> 12);
        if (d < 0x1f4000)
            d = 0x1f4000;
        k = (unsigned short)(short)(Vec3_HorzAngle((struct Vector3*)(&this->mPosX), &sp) - 0x1000) >> 4;
        sp.x = this->mPosX + (int)(((long long)d * data_02082214[k * 2] + 0x800) >> 12);
        sp.z = this->mPosZ + (int)(((long long)d * data_02082214[k * 2 + 1] + 0x800) >> 12);
        _Z14ApproachLinearRiii(&this->mCamPosX, sp.x, 0x4000);
        _Z14ApproachLinearRiii(&this->mCamPosZ, sp.z, 0x4000);
        _Z14ApproachLinearRiii(&this->mCamLookAtX, this->mPosX, 0x1e000);
        _Z14ApproachLinearRiii(&this->mCamLookAtZ, this->mPosZ, 0x1e000);
        func_020092c4(cam, &cam->pos, &this->mCamPosX);
        if (func_020092c4(cam, &cam->lookAt, &this->mCamLookAtX) != 0) {
            p = (unsigned char*)(&this->mCutsceneStep);
            *p = *p + 1;
        }
        break;
    case 3: {
        int ty, tz, tx;
        k = (int)((unsigned short)this->mAngleY) >> 4;
        tz = data_02082214[k * 2 + 1] * 0xc0 + this->mPosZ;
        ty = this->mPosY + 0xfa000;
        tx = data_02082214[k * 2] * 0xc0 + this->mPosX;
        sp.x = tx;
        sp.y = ty;
        sp.z = tz;
    }
        _Z14ApproachLinearRiii(&this->mCamLookAtX, sp.x, 0xa000);
        _Z14ApproachLinearRiii(&this->mCamLookAtY, sp.y, 0xa000);
        _Z14ApproachLinearRiii(&this->mCamLookAtZ, sp.z, 0xa000);
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov060_02111cc0, 0x02111cc0, size 0x248 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02111cc0Eii
/*
Switch the model to animation idx (entries 0..0x1b of the table at
data_ov060_021192dc; entry [1] of each handle is the file) and pick the texture
pattern that goes with it. The third parameter reaches ModelAnim::SetAnim
unchanged as its flags word: the local `a` below is never assigned, and the ROM
passes the caller's third argument (still in r2) straight through. Callers pass 0 or 0x40000000; the top two bits of
an Animation's flag word choose between wrapping and clamping at the end (see
dExtFrameCtrl_c::WillHitFrame), so 0x40000000 reads as play-once and 0 as looping.
Texture pattern by idx: 1 uses data_ov060_0211ac40, 2 acb8 and 0xa ac30 (all
with texture-sequence flags 0x40000000); 3 and 5 use ac10, 0xe uses abf0, and
every other idx (0 included) uses ac28 (these three with flags 0). */
#include "TextureSequence.h"
struct BMD_File;
struct BTP_File;
extern "C" {
/* The six BTP_File handles below were declared by this shard through a local
 * `struct E { int w[2]; }` overlay and read as `x.w[1]`, while the neighbouring
 * resource-loader shard declares the same addresses as SharedFilePtr tables and
 * reads them as `x[1]`. One object cannot be both. The loader view is the real
 * one -- these are the SharedFilePtr handles Model::LoadFile populates and the
 * cleanup path releases -- so they are declared once, there, and this shard's
 * `.w[1]` reads are respelled `[1]`. */
extern SharedFilePtr *data_ov060_021192dc[];
extern SharedFilePtr *data_ov060_0211ac78[];
extern SharedFilePtr *data_ov060_0211ac40[];
extern SharedFilePtr *data_ov060_0211acb8[];
extern SharedFilePtr *data_ov060_0211ac10[];
extern SharedFilePtr *data_ov060_0211abf0[];
extern SharedFilePtr *data_ov060_0211ac30[];
extern SharedFilePtr *data_ov060_0211ac28[];
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *anim, void *file, int a, int d, unsigned e);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *ts, void *file, int a, int d, unsigned e);
/* local extern: the call passes an untyped this, so it cannot use the header method. */
extern void _ZN15dExtFrameCtrl_c8SetFlagsEi(void *anim, int flags);
}
void daKpa_c::func_ov060_02111cc0(int idx, int animFlags){
    int a;  /* deliberately never assigned: it is animFlags, still in r2 (see above) */
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&this->mModelAnim, (void *)((int *)data_ov060_021192dc[idx])[1], a, 0x1000, 0);
    switch (idx) {
    case 1:
        TextureSequence::Prepare(*(BMD_File *)data_ov060_0211ac78[1], *(BTP_File *)data_ov060_0211ac40[1]);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&this->mTextureSequence, (void *)data_ov060_0211ac40[1], 0, 0x1000, 0);
        _ZN15dExtFrameCtrl_c8SetFlagsEi(&this->mTextureSequence, 0x40000000);
        return;
    case 2:
        TextureSequence::Prepare(*(BMD_File *)data_ov060_0211ac78[1], *(BTP_File *)data_ov060_0211acb8[1]);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&this->mTextureSequence, (void *)data_ov060_0211acb8[1], 0, 0x1000, 0);
        _ZN15dExtFrameCtrl_c8SetFlagsEi(&this->mTextureSequence, 0x40000000);
        return;
    case 3:
    case 5:
        TextureSequence::Prepare(*(BMD_File *)data_ov060_0211ac78[1], *(BTP_File *)data_ov060_0211ac10[1]);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&this->mTextureSequence, (void *)data_ov060_0211ac10[1], 0, 0x1000, 0);
        _ZN15dExtFrameCtrl_c8SetFlagsEi(&this->mTextureSequence, 0);
        return;
    case 14:
        TextureSequence::Prepare(*(BMD_File *)data_ov060_0211ac78[1], *(BTP_File *)data_ov060_0211abf0[1]);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&this->mTextureSequence, (void *)data_ov060_0211abf0[1], 0, 0x1000, 0);
        _ZN15dExtFrameCtrl_c8SetFlagsEi(&this->mTextureSequence, 0);
        return;
    case 10:
        TextureSequence::Prepare(*(BMD_File *)data_ov060_0211ac78[1], *(BTP_File *)data_ov060_0211ac30[1]);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&this->mTextureSequence, (void *)data_ov060_0211ac30[1], 0, 0x1000, 0);
        _ZN15dExtFrameCtrl_c8SetFlagsEi(&this->mTextureSequence, 0x40000000);
        return;
    case 0:
    default:
        TextureSequence::Prepare(*(BMD_File *)data_ov060_0211ac78[1], *(BTP_File *)data_ov060_0211ac28[1]);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&this->mTextureSequence, (void *)data_ov060_0211ac28[1], 0, 0x1000, 0);
        _ZN15dExtFrameCtrl_c8SetFlagsEi(&this->mTextureSequence, 0);
        return;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov060_02111c68, 0x02111c68, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02111c68Ev
/*
Row number for the mouth fire in daKpaFire_c, or -1. While animation 8 plays
(data_ov060_0211ac20 is entry 8 of the animation table) the row is the whole
frame number minus 0x31 from frame 0x31 on; while animation 6 plays
(data_ov060_0211ac68) it is the frame number plus 0xb; otherwise -1. */
int daKpa_c::func_ov060_02111c68(){
  int v = (int)this->mModelAnim.file;
  if(v == (int)data_ov060_0211ac20[1]){
    unsigned int t = ((unsigned int)this->mModelAnim.currFrame << 4) >> 0x10;
    if(t >= 0x31) return t - 0x31;
  }
  if(v == (int)data_ov060_0211ac68[1]){
    return (((unsigned int)this->mModelAnim.currFrame << 4) >> 0x10) + 0xb;
  }
  return -1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov060_02111a28, 0x02111a28, size 0x240 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daKpa_c19func_ov060_02111a28Ev
// @symbol _ZN7daKpa_c19func_ov060_02111a28Ev
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
/* recovered: shared common types */
#include "common.h"
extern "C" {
/* The six resource handles at 0x1acb0..0x1ac90 are read only through their
 * second word (`x.w[1]`), compared against a value loaded from the actor. Spelled
 * as a two-int array rather than a named struct: mwccarm rejects a class declared
 * between function bodies in this position, reading the type as undeclared. */
extern int data_ov060_0211acb0[];
extern int data_ov060_0211ac00[];
extern int data_ov060_0211ace0[];
extern int data_ov060_0211aca0[];
extern int data_ov060_0211abf8[];
extern int data_ov060_0211ac90[];
extern void _ZN8dActor_c17HugeLandingDustAtER7Vector3b(void *a, void *v, int b);
extern void _ZN5Sound4PlayEjjRK7Vector3(unsigned a, unsigned b, void *v);
}
void daKpa_c::func_ov060_02111a28(){
    /* (unsigned short)(u32 >> 12) forces ROM prologue load order
       (frame@0x12c into r0, then flag@0x446 into r1) + lsl#4/lsr#16 extract. */
    int n = (unsigned short)((unsigned)this->mModelAnim.currFrame >> 12);
    int r3 = (this->mFootfallLatch != 0) ? 1 : 0;
    int v = (int)this->mModelAnim.file;
    int r1 = 0;
    this->mFootfallLatch = 0;

    if (v == data_ov060_0211acb0[1]) {
        if (n >= 0x14 && n <= 0x17)
            r1 = 2;
        else if (n == 0x29 || n < 2)
            r1 = 1;
    } else if (v == data_ov060_0211ac00[1]) {
        if (n >= 0xf && n <= 0x12)
            r1 = 2;
    } else if (v == data_ov060_0211ace0[1]) {
        if (n >= 0x1c)
            r1 = 1;
    } else if (v == data_ov060_0211aca0[1]) {
        if (n >= 6 && n <= 9)
            r1 = 2;
        else if (n >= 0xf)
            r1 = 1;
    } else if (v == data_ov060_0211abf8[1]) {
        if (n >= 0x10 && n <= 0x13)
            r1 = 1;
        else if (n >= 0x1e && n <= 0x21)
            r1 = 2;
        else if (n >= 0x2d && n <= 0x30)
            r1 = 1;
    } else if (v == data_ov060_0211ac90[1]) {
        if (n >= 0xa && n <= 0xd)
            r1 = 1;
        else if (n >= 0x18 && n <= 0x1b)
            r1 = 2;
    }

    if (r1 == 0)
        return;
    this->mFootfallLatch = 1;
    if (r3 != 0)
        return;

    if (r1 == 1) {
        Vector3 dust;
        dust.x = this->mFootPosAX;
        dust.y = this->mFootPosAY;
        dust.z = this->mFootPosAZ;
        _ZN8dActor_c17HugeLandingDustAtER7Vector3b(this, &dust, 0);
    } else {
        Vector3 dust;
        dust.x = this->mFootPosBX;
        dust.y = this->mFootPosBY;
        dust.z = this->mFootPosBZ;
        _ZN8dActor_c17HugeLandingDustAtER7Vector3b(this, &dust, 0);
    }
    _ZN5Sound4PlayEjjRK7Vector3(3, BOWSER_SND_FOOTFALL, &this->mCamSpacePosX);
    {
        Vector3 quake;
        quake.x = this->mPosX;
        quake.y = this->mPosY;
        quake.z = this->mPosZ;
        _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(this, &quake, 0x320000);
    }
}
