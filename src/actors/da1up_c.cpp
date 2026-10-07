//cpp
/* da1up_c -- the mushroom, in all fourteen of the ways it can behave.
 *
 * One class covers the 1-Up and the Mega Mushroom and the ways they are placed:
 * waiting in the open, hidden until a condition is met, moving off, spinning in
 * place, falling to the ground. mMushroomType picks one of the 14
 * behaviours out of a dispatch array and Behavior calls it every frame; the
 * class also answers to Yoshi (OnYoshiTryEat) and to being turned into an egg
 * (OnTurnIntoEgg).
 *
 * The TU is the contiguous linker run 0x020aee40..0x020b0530, ROM ordinals
 * 0..35, 36 functions; config/tu_manifest.d/ov002/da1up_c.json names each one.
 *
 * IDENTITY IS THE CARTRIDGE'S, NOT THE TREE'S. ov002 file offset 0x5ad10 ==
 * address 0x02108370 holds `7da1up_c\0`, the length-prefixed Itanium type-name
 * string, and _ZTI7da1up_c at 0x0210837c is the matching __si_class_type_info
 * whose +8 word reaches _ZTI12dEnemyBase_c at 0x021081c0. The tree's former
 * spelling `OneUpMushroom` is in no image in any encoding tested; the fact
 * file kept at notes/data/class-facts/OneUpMushroom.json records the result.
 *
 * SOURCE ORDER IS ROM-ASCENDING AND `#pragma defer_codegen off` IS
 * LOAD-BEARING; they are ONE decision, exactly as on ov006/dScMgPanel_c. With
 * codegen deferred (the default) mwccarm 2004/b56 emits one .text section per
 * function in the REVERSE of source order, which would demand a forward
 * declaration for all 36 members and force one canonical spelling on every
 * shared helper. Generating at parse time emits in source order instead, so
 * every callee but two is already defined above its caller and each member's
 * independently recovered view of a helper survives untouched.
 *
 * THE DESTRUCTOR STAYS OUT OF LINE AND IS DECLARED FIRST, SO THIS TU OWNS THE
 * CLASS'S KEY FUNCTION. The cartridge orders D1 (0x020aee40, 0x48) BELOW D0
 * (0x020aee88, 0x5c) and carries no D2; out-of-line + `defer_codegen off` emits
 * D1, D0, D2, which is that order with a homeless D2 trailing where it is
 * deadstripped. Ordinals 0 and 1 are two shards of ONE C++ definition, so only
 * one destructor body is written below. Owning the key function is also why
 * this TU emits the whole chain's vtable and typeinfo as vague-linkage
 * passengers -- see the manifest's compiler_only_output block.
 *
 * THE MEMBER BODIES CANNOT HOLD THE LOCAL `extern` VIEWS. mwccarm 2004/b56
 * gives a block-scope declaration inside a member body C++ linkage and
 * rejects `extern "C"` at block scope outright, so the recovered
 * per-callsite spellings each shard declared inside its (then `extern
 * "C"`) free function cannot stay -- a bare `extern` there mangles the
 * reference and the ROM link fails. The external FUNCTION declarations
 * the members call are hoisted to the file-scope `extern "C"` region
 * above the destructor instead, one spelling per symbol; the views
 * differed only in pointer types, so marshalling and bytes are unchanged.
 * func_ov002_020aefa4's int-returning view is the exception: a file-scope
 * `int` declaration is `illegal function overloading` against its `void`
 * definition, so func_ov002_020af218 tail-returns it through a
 * function-pointer cast, `((int (*)(void *))func_ov002_020aefa4)(...)`,
 * which mwccarm folds to the same direct `bl`. External DATA declarations
 * stay in the bodies: mwccarm leaves a variable's name unmangled in C++.
 *
 * `decl_common.h` IS DELIBERATELY NOT INCLUDED. Its claims on this TU
 * reduce to `func_ov002_020aefa4(char *)` -- redundant with this file's
 * own definition -- and `func_ov002_020d0d2c(void *)` and `Vec3_Asr`,
 * both already in the decl region above; the header's other ~1900
 * catch-all spellings would only add overloading-conflict surface
 * against them. ov002/Player and ov006/dScMgPanel_c, the two largest
 * promoted TUs, exclude it for the same reason.
 *
 * 35 OF THE 38 SYMBOLS ARE MEMBERS, written as 34 `da1up_c::` definitions
 * (ordinals 0 and 1 are two shards of the one destructor). Nine are the
 * class's own vtable slots: ordinals 0/1 the destructor pair (slots 16/17),
 * 9 OnTurnIntoEgg (19), 10 OnYoshiTryEat (18), 31 CleanupResources (3),
 * 32 OnPendingDestroy (12), 33 Render (9), 34 Behavior (6) and 35
 * InitResources (0). The other 26 members are the former free helpers:
 * fourteen are ROM-proven non-static members -- the {function pointer, 0}
 * descriptors at 0x02108300..0x02108370 are pointer-to-member-function
 * objects with a zero `this` adjustment, and __sinit_ov002_02100adc copies
 * them into the 14-element dispatch array at 0x0210dc00 that Behavior
 * indexes by mMushroomType; the rest each took the actor as arg0 in a
 * `char *`/`void *`/`da1up_c *` spelling and recast it. Their member-ness
 * and index are proven; their original NAMES are not, so they keep
 * address-derived spellings. The 14 are, by index:
 * 0 020aff10, 1 020afe4c, 2 020afd10,
 * 3 020afc44, 4 020afbb4, 5 020afa98, 6 020afa6c, 7 020af950, 8 020af924,
 * 9 020af838, 10 020af7cc, 11 020afa50, 12 020af908, 13 020af724.
 * Behavior dispatches them as `void (da1up_c::*)()`; the opaque shadow
 * class the shard used for the PMF type is gone (the member-pointer call
 * shape is byte-identical under the real class).
 * Leftover fold adds the two classInit factories at 0x020b0530/0x020b0580,
 * so the licensed run is 38 functions through 0x020b05d0.
 *
 * deslop leftovers:
 * - func_ov002_020aefa4 stays a free `void` function: MEASURED, declaring
 *   it `int` so func_ov002_020af218 could tail-return it costs four of its
 *   five words; af218 instead tail-returns it through an
 *   `int (*)(void *)` cast of its name (folds to the same `bl`).
 * - dBgCh_Actr::Init / dCcAc_c::Init / DropShadowRadHeight / ReflectAngle 6az
 *   (Fix12i mangles as i; ROM is Fix12<int> -- method form Undefined)
 * - Particle::System::New / NewSimple: no method declaration in include/
 * - Behavior 0x100: named ++mStateTimer size-DIFF vs unsigned-short launder
 *   (the u16 casts of dEnemyBase_c::mStateTimer at 0x100 stay everywhere they
 *   appear. The u16 at 0x38c is a distinct field, mStateFrames.)
 * - func_ov002_020af0c0: reading the player's position directly instead of
 *   through the laundered int* changes the bytes, so the launder stays.
 * - SharedFilePtr has no recovered fields; handles stay data_ov002_*
 * - decl_common.h stays out (its surviving claims are redundant with the
 *   decl region and this file's own definition)
 *
 * Readability pass: the da1up_c fields at 0x378..0x394 are named, the mushroom
 * types are the da1up_MushroomType enum, and actor, sound, cylinder-flag and
 * particle ids are enums. Leftover:
 * - The da1up_MushroomType names describe what each type does when it runs;
 *   they are not recovered original names (nor are the address-named
 *   handlers).
 * - unk_0a4 / unk_0ac are dActor_c's velocity x/z words (UpdatePosWithHorzSpeedAndAng
 *   writes them from mHorzSpeed and mPrevAngleY); they stay unnamed here because
 *   naming them is a shared dActor_c.h change.
 * - The SND3_* names and the effect ids are known by use only (0x68 is played
 *   as a mushroom launches, 0x69 once when type 13 starts).
 * - The vulnFlags/hitFlags bit names come from the best-effort table in
 *   include/dCc_c.h.
 * - The PlayBank3 / IsPlayerInRange / ReflectAngle / DropShadowRadHeight /
 *   Vec3_* / Matrix4x3_* / cstd / Particle::System::New externs are the
 *   spelled-out declarations in the file-scope `extern "C"` region (the
 *   Fix12/undeclared walls above are why real method/header forms are not
 *   used); member bodies cannot carry per-site views.
 */

#pragma defer_codegen off

#include "types.h"
#include "common.h"
#include "da1up_c.h"
#include "dBgCh_Actr.h"
#include "dBgCh_Gnd.h"
#include "Player.h"
#include "Sound.h"
#include "SharedFilePtr.h"

bool ApproachLinear(short &value, short target, short step);

/* Actor IDs, from the profile ids in symbols/actor_debug_names.tsv: 276 and
   277 are this class's two profiles, 331 is the 1UP logo it spawns. */
enum {
    ACTOR_ONEUPKINOKO     = 276,
    ACTOR_SCALEUP_KINOKO  = 277,
    ACTOR_OBJ_1UPLOGO     = 331
};

/* Sound::PlayBank3 ids. SND3_COIN / SND3_COIN_UNDERWATER are the pair
   dActor_c::GivePlayerCoins plays for a coin (the second when the player is
   underwater); SND3_GIVE_LIFE is played alongside GiveLives(1) here and in
   daObjMarioCap_c; SND3_LAUNCH is played as a mushroom pops out; SND3_UNK_69
   is played once when type 13 starts. Only their use is known. */
enum {
    SND3_COIN            = 0x11,
    SND3_COIN_UNDERWATER = 0x12,
    SND3_LAUNCH          = 0x68,
    SND3_UNK_69          = 0x69,
    SND3_GIVE_LIFE       = 0x6e
};

/* Bit values of the cylinder and actor flag words used below. dCc_c::flags bit
   0 disables the cylinder (dCc_c::Update bails on it); the vulnFlags and
   hitFlags bits are read from the best-effort table in include/dCc_c.h; the
   mFlags bit is the clip-test enable from the table in include/dActor_c.h. */
enum {
    CC_FLAGS_DISABLED    = 0x1,
    CC_VULN_YOSHI_TONGUE = 0x8000,
    CC_HIT_PLAYER        = 0x400000,
    ACTOR_FLAG_CLIP_TEST = 0x1,
    ACTOR_FLAG_OFF_SCREEN = 0x8
};

/* Particle effect ids handed to Particle::System::New / NewSimple. */
enum {
    PTCL_SCALEUP_KINOKO_TRAIL = 0x108, /* the effect func_ov002_020aeee4 starts for actorID 277 ("trail" is this file's word for it) */
    PTCL_TYPE_11_12_CLEANUP   = 0xd2   /* started by CleanupResources for types 11 and 12 */
};

/* The only two intra-TU calls that run UPWARD in ROM address order, so the only
   two that ROM-ascending source order cannot satisfy from the definition above:
   ordinal 21 (0x020afa50) calls ordinal 22 (0x020afa6c), and ordinal 18
   (0x020af908) calls ordinal 19 (0x020af924). Both spellings are the
   definitions' own, so nothing below has to be adapted to them. */

/* File-scope extern "C" region covering the external functions the member
   bodies below call. mwccarm 2004/b56 gives a block-scope `extern` declaration
   inside a member body C++ linkage (and rejects `extern "C"` at block scope
   outright), so the recovered per-callsite views these shards each declared
   locally cannot stay where they were; one spelling per symbol is hoisted here.
   The views differed only in pointer types, so marshalling -- and the emitted
   bytes -- are unchanged. func_ov002_020aefa4 needs no entry: its definition
   below is already file-scope C linkage. */
struct Vec3;
extern "C" {
int Vec3_HorzLen(const Vector3*);
short Vec3_HorzAngle(const Vector3*, const Vector3*);
int func_ov002_020d0d2c(void*);
void Vec3_Asr(struct Vec3*, struct Vec3*, int);
void Matrix4x3_FromRotationY(void*, int);
void Matrix4x3_FromTranslation(void*, int, int, int);
void GiveCoins(int, int);
short _ZN4cstd5atan2E5Fix12IiES1_(int, int);
int _ZN4cstd4fdivEii(int, int);
short _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void*, int, int, short);
void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(void*, void*, void*, int, int, unsigned char);
int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int, unsigned int, int, int, int, const struct Vec3*, void*);
}
/*                         _ZN7da1up_cD0Ev, 0x020aee88, size 0x5c              */
// @symbol _ZN7da1up_cD1Ev
// @symbol _ZN7da1up_cD0Ev
/* ONE definition, both variants. The complete-object destructor (D1) tears the
   four members down in exact reverse of the factories' construction order --
   dExtShadowModel_c at 0x350, Model at 0x300, dBgCh_Actr at 0x144, dCcAc_c at 0x110,
   then ~dEnemyBase_c -- and every one of those is a typed member of this class,
   so the body is empty and the compiler writes the chain. The deleting
   destructor (D0) inlines that same teardown and then calls
   Memory::Deallocate(this, GAME_HEAP_PTR) through dEnemyBase_c's own inline
   `operator delete`, which is why nothing below mentions a heap. Under
   `#pragma defer_codegen off` the variants come out D1, D0, D2; the trailing D2
   is homeless and is deadstripped. */
da1up_c::~da1up_c()
{
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020aeee4Ev
/* Trail effect: asks Particle::System::New for one effect at the mushroom's
   position, 30 units (0x1e000) above it. The effect id is 0x108 for the Mega
   Mushroom (actorID 277) and 0 for the 1-Up. The previous frame's handle is read
   back through a volatile pointer and the new one stored in mParticleID. While
   the actor is off screen (mFlags bit 3) it only runs when CURRENT_GAMEMODE
   (data_0209f2d8) is 1. */
void da1up_c::func_ov002_020aeee4() {
    extern unsigned char data_0209f2d8;

    int t1 = (actorID == ACTOR_SCALEUP_KINOKO);
    unsigned int effectID = 0;
    if (t1 != false) effectID = PTCL_SCALEUP_KINOKO_TRAIL;

    int t2 = ((mFlags & ACTOR_FLAG_OFF_SCREEN) != 0);
    if (t2 != false) {
        int t3 = (data_0209f2d8 == 1);
        if (t3 == false) return;
    }

    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x1e000;
    volatile Vector3* vp = &pos;
    mParticleID = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(volatile unsigned int*)&mParticleID, effectID, vp->x, vp->y, pos.z, 0, 0);
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020aefa4
/* Sets bit 0x8000 in the cylinder's vulnFlags -- the "yoshi tongue" bit of the
   table in include/dCc_c.h (a best-effort reading there) -- so the mushroom can
   be eaten. func_ov002_020af218 calls it while the player is in range.
   MEASURED: this definition must stay `void`. Declaring it `int` -- so that
   ordinal 7's tail-return would type-check against a file-scope declaration
   -- costs four of this function's five words, and a file-scope `int`
   declaration beside this `void` definition is illegal function overloading
   anyway, so ordinal 7 tail-returns it through an `int (*)(void *)` cast of
   its name instead. */
extern "C" {
void func_ov002_020aefa4(char *raw)
{
    da1up_c *self = (da1up_c*)raw;
    self->mdCcAc_c.vulnFlags |= CC_VULN_YOSHI_TONGUE;
}
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020aefb8Ev
/* The shared per-frame physics step. Moves by mHorzSpeed along mPrevAngleY
   (UpdatePosWithHorzSpeedAndAng). While on the ground it adds ten times the
   floor normal's x and z to the words at 0xa4 / 0xac (the x and z of a velocity
   whose y is mVertSpeed: func_ov002_020afd10 saves and restores them as that
   triple), sets mVertSpeed to -0.4 times its value on the frame it lands and to
   0 otherwise, and raises mHorzSpeed to the horizontal length of that vector
   when that is larger, capped at 15 units (0xf000). Then it integrates position
   (UpdatePosWithOnlySpeed), runs the wall/floor collision update, and on a wall
   hit reflects mPrevAngleY about the wall normal.
   The shard carried shadow `dActor_c`/`dEnemyBase_c` tags to name three
   non-virtual methods. The merged TU has both real classes complete through
   da1up_c.h, so the shadows are gone and the calls go through the real types --
   which mangle identically, the class name being the whole of the difference.
   _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s stays spelt out. Its reconstructed
   symbol encodes by-value Fix12<int> parameters; the recorded typed-call
   experiment changed the bytes. This bridge remains pending further
   signature/codegen work. */
void da1up_c::func_ov002_020aefb8() {

    UpdatePosWithHorzSpeedAndAng();
    if (mWithMeshClsn.IsOnGround()) {
        unk_0a4 += mFloorNormalX * 0xa;
        unk_0ac += mFloorNormalZ * 0xa;
        if (mWithMeshClsn.JustHitGround()) {
            mVertSpeed = -(mVertSpeed << 2) / 10;
        } else {
            mVertSpeed = 0;
        }
        if (Vec3_HorzLen((Vector3*)&unk_0a4) > mHorzSpeed) {
            mHorzSpeed = Vec3_HorzLen((Vector3*)&unk_0a4);
            if (mHorzSpeed >= 0xf000) mHorzSpeed = 0xf000;
        }
    }
    UpdatePosWithOnlySpeed((dCc_c*)&mdCcAc_c);
    UpdateWMClsn(mWithMeshClsn, 0);
    if (!mWithMeshClsn.IsOnWall()) return;
    mPrevAngleY = _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(this, mWallNormalX, mWallNormalZ, mPrevAngleY);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020af0c0Ev
/* Steering toward the player, called from state 1 of type 7. Takes the closest
   player; if there is one, builds the vector to it -- the aim point is the
   player's y lowered by 80 units (0x50000) when func_ov002_020d0d2c says the
   player is in one of two Player states and raised by 120 units (0x78000)
   otherwise -- and steps mPrevAngleY toward the horizontal bearing and
   mPrevAngleX toward the pitch (cstd::atan2(horizontal length, height
   difference)), at most 0x1000 (22.5 degrees) per frame each. mVertSpeed and
   mHorzSpeed are then 30 units (0x1e) times the two s16 words of
   data_02082214's entry for mPrevAngleX >> 4 -- the sin/cos table, 0x1000 = 1.0
   -- mVertSpeed from word 1 and mHorzSpeed from word 0. Always finishes with
   func_ov002_020af3a8. */
void da1up_c::func_ov002_020af0c0() {
    extern short data_02082214[];
    /* Forward: ordinal 11 sits above this one in ROM order. */

    Player* p = ClosestPlayer();
    if(p != 0){
        Vector3 diff;
        Vector3 ppos;
        /* The player's position is read through an int* laundered via
           (void*)(int); reading p->mPosX/Y/Z directly here changes the bytes
           (tried: it did not byte-match), so the launder stays. */
        int* s = (int*)((void*)(int)&p->mPosX);
        ppos.x = s[0];
        ppos.y = s[1];
        ppos.z = s[2];
        diff.x = ppos.x - mPosX;
        if(func_ov002_020d0d2c(p) != 0)
            diff.y = ppos.y - mPosY - 0x50000;
        else
            diff.y = ppos.y - mPosY + 0x78000;
        diff.z = ppos.z - mPosZ;
        int len = Vec3_HorzLen(&diff);
        short pitch = _ZN4cstd5atan2E5Fix12IiES1_(len, diff.y);
        short yaw = Vec3_HorzAngle((Vector3*)&mPosX, &ppos);
        ApproachLinear(mPrevAngleY, yaw, 0x1000);
        ApproachLinear(mPrevAngleX, pitch, 0x1000);
        mVertSpeed = (short)data_02082214[(*(unsigned short*)&mPrevAngleX >> 4)*2+1] * (short)0x1e;
        mHorzSpeed = (short)data_02082214[(*(unsigned short*)&mPrevAngleX >> 4)*2] * (short)0x1e;
    }
    func_ov002_020af3a8();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020af1dcEv
/* The player touching the mushroom, or 0. mdCcAc_c.otherOwner is the unique ID
   of the other cylinder's owner (cleared by dCc_c::Clear, which Behavior calls
   every frame); it is looked up with dActor_c::FindWithID and returned only
   when hitFlags bit 0x400000 is set -- the "player" bit of the table in
   include/dCc_c.h, a best-effort reading there that the callers' use of the
   result as a Player agrees with. */
int da1up_c::func_ov002_020af1dc() {
  Player* r=0;
  unsigned int id=mdCcAc_c.otherOwner;
  if(id && (r=(Player*)dActor_c::FindWithID(id)) && (mdCcAc_c.hitFlags&CC_HIT_PLAYER))
    return (int)r;
  return 0;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020af218Ei
/* Stores in mShown whether the player is within `range` whole units
   (IsPlayerInRange shifts it into 20.12), and when it is calls
   func_ov002_020aefa4 to open the mushroom to Yoshi. Returns 0 when out of range,
   otherwise that call's result. Every caller in this TU passes 0xbb8 (3000
   units).
   The second parameter is FORWARDED, not merely declared. The callers
   below pass 0xbb8 in r1 and this body hands that same word to
   _ZN8dActor_c15IsPlayerInRangeEi, whose ROM name mangles as
   dActor_c::IsPlayerInRange(int): `this` in r0 and one `int` in r1, which is
   exactly the member call below. The ROM emits no `mov` before the `bl`
   because r1 still holds the incoming range, so naming the argument is
   byte-neutral here and stops the call from handing the callee whatever r1
   happens to hold on a host ABI. */
int da1up_c::func_ov002_020af218(int range) {
  mShown=(char)IsPlayerInRange(range);
  unsigned char v=mShown;
  if(v==0) return v;
  return ((int (*)(void*))func_ov002_020aefa4)((void*)this);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020af248Ei
/* The expiry countdown, called with n = 30 from state 2 of the behaviours that
   have one. Nothing happens while mStateFrames < n; for the next 40 frames
   (n <= mStateFrames < n + 0x28) mBlinkOn follows the low bit of mStateFrames
   (visible on odd frames); from mStateFrames == n + 0x28 on the actor is removed
   (KillAndTrackInDeathTable) and 1 is returned. Returns 0 otherwise. */
int da1up_c::func_ov002_020af248(int n) {
  int v = mStateFrames;
  if(v < n) return 0;
  if(v < n + 0x28){
    mBlinkOn = (v & 1) != 0;
  } else {
    KillAndTrackInDeathTable();
    return 1;
  }
  return 0;
}

/* The first of the two file-scope `extern "C"` regions. Ordinal 9 is a class
   member function, so it cannot sit in one and a declaration written in its body
   would mangle; these three have to be here. The shared actor egg-turn hook
   and its forwarding helpers return void. Promotion had changed ordinal 14
   from void to int to agree with the old hook declaration despite its void
   terminal call. Ordinals 9, 19 and 22 now use this one void declaration;
   ordinals 19 and 22 retain the player lookup result and its null test. */
extern "C" {
void GiveLives(int count);
void func_ov002_020bdf8c(Player* player);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c13OnTurnIntoEggER6Player
/* Vtable slot 19, verified against config/arm9/overlays/ov002/relocs.txt:
   _ZTV7da1up_c (0x021083c8) + 0x4c relocates to 0x020af2b0, this address.
   Collects the mushroom for `player` (the hook for a mushroom being turned
   into an egg, per its slot). Types 11 and 12 go through
   func_ov002_020af684 with target 5 and 7, the same hand-off their touch
   handlers make. Otherwise a 1-Up (actorID 276) plays sound 0x6e, gives one
   life, spawns the 1UP logo actor (331) 180 units (0xb4000) above itself and
   removes itself; any other actor ID calls Player::func_ov002_020bdf8c on
   `player` and removes itself. */
void da1up_c::OnTurnIntoEgg(Player &player)
{
    if (mMushroomType == MUSHROOM_SPIN_TRIGGER_FOR_5) {
        return func_ov002_020af684(MUSHROOM_HIDDEN_FLEE, &player);
    }
    if (mMushroomType == MUSHROOM_SPIN_TRIGGER_FOR_7) {
        return func_ov002_020af684(MUSHROOM_HIDDEN_CHASE, &player);
    }
    unsigned isMatch = (actorID == ACTOR_ONEUPKINOKO);
    if (isMatch) {
        Vector3 vec;
        Sound::PlayBank3(SND3_GIVE_LIFE, *(Vector3 *)&mCamSpacePosX);
        GiveLives(1);
        vec.x = mPosX;
        vec.y = mPosY;
        vec.z = mPosZ;
        vec.y += 0xb4000;
        Spawn(ACTOR_OBJ_1UPLOGO, 8, vec, 0, mAreaId, -1);
        KillAndTrackInDeathTable();
    } else {
        ((Player *)(&player))->func_ov002_020bdf8c();
        KillAndTrackInDeathTable();
    }
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c13OnYoshiTryEatEv
/* Vtable slot 18. Two instructions: mov r0,#4; bx lr. */
s32 da1up_c::OnYoshiTryEat()
{
    return 4;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020af3a8Ev
/* The touch-collection step, called from every behaviour except types 6, 8, 9,
   10, 11 and 12. Does nothing unless func_ov002_020af1dc finds a player touching
   the cylinder. Then: the Mega Mushroom (actorID 277) calls
   Player::func_ov002_020bdf8c on that player; the 1-Up (actorID 276) plays sound
   0x6e, gives one life and spawns the 1UP logo actor (331) 180 units (0xb4000)
   above itself; either way the mushroom then removes itself. */
void da1up_c::func_ov002_020af3a8() {

    Player* r = (Player*)func_ov002_020af1dc();
    if (r == 0)
        return;

    unsigned short h = actorID;
    unsigned is115 = (h == ACTOR_SCALEUP_KINOKO);
    if (is115) {
        r->func_ov002_020bdf8c();
    } else {
        unsigned is114 = (h == ACTOR_ONEUPKINOKO);
        if (is114) {
            struct Vector3 vec;
            Sound::PlayBank3(SND3_GIVE_LIFE, *(Vector3*)&mCamSpacePosX);
            GiveLives(1);
            vec.x = mPosX;
            vec.y = mPosY;
            vec.z = mPosZ;
            vec.y += 0xb4000;
            dActor_c::Spawn(
                ACTOR_OBJ_1UPLOGO, 8, vec, 0, mAreaId, -1);
        }
    }
    KillAndTrackInDeathTable();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020af474Ev
/* The launch arc of the types that pop out (0, 1, 5 and 7). For the first five
   frames of the state (mStateTimer < 5) it only sets mVertSpeed to 40 units
   (0x28000). After that, every frame it lowers mPrevAngleX by 0x1000 (22.5
   degrees) and reads the s16 pair of data_02082214 (the sin/cos table, 0x1000 =
   1.0) at index mPrevAngleX >> 4: mVertSpeed = 30 units (0x1e) times word 1 plus
   2 units (0x2000); mHorzSpeed = -30 units times word 0. */
void da1up_c::func_ov002_020af474() {
    extern s16 data_02082214[];
    int a;

    if (*(u16*)&mStateTimer < 5) {
        mVertSpeed = 0x28000;
        return;
    }

    {
        s16* p = &mPrevAngleX;
        *p = *p - 0x1000;
    }

    a = (int)*(u16*)&mPrevAngleX >> 4;
    mVertSpeed = (s16)data_02082214[a * 2 + 1] * (s16)0x1e + 0x2000;

    a = (int)*(u16*)&mPrevAngleX >> 4;
    mHorzSpeed = (s16)data_02082214[a * 2] * (s16)-0x1e;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020af4ecEv
/* Per-frame model matrix and drop shadow, run at the end of every Behavior,
   including the frames UpdateYoshiEat returns nonzero. mModel's matrix gets
     types 11 and 12: a Y rotation by mAngleY and translation mPos >> 3;
     other types:     translation mPos >> 3 only (through Vec3_Asr).
   Nothing more happens while mShown is 0. Otherwise the shadow size is chosen and
   handed to dActor_c::DropShadowRadHeight (shadow, matrix, radius, depth,
   opacity) with opacity word 0xf; the local named `radius` is passed in the
   callee's radius slot and `depth` in its depth slot:
     types 11 and 12: radius = depth = 80 units (0x50000);
     airborne:        dBgCh_Gnd probes the floor from 40 units (0x28000) above
                      the actor; depth = the actor's height over the hit (1 unit
                      at least), radius = twice (cylinder radius - 10 units)
                      minus depth * 0x180 / 0x1000 (a Fix12 multiply by 0.09375),
                      10 units at least, and then depth += 60 units (0x3c000);
     on the ground:   depth = 60 units, radius = twice (cylinder radius - 10 units). */
void da1up_c::func_ov002_020af4ec() {

    int depth;
    int radius;
    struct Vector3 v2;
    struct Vector3 v1;

    if ((unsigned)(mMushroomType - 0xb) <= 1) {
        Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
        mModel.mat4x3.m[9] = mPosX >> 3;
        mModel.mat4x3.m[10] = mPosY >> 3;
        mModel.mat4x3.m[11] = mPosZ >> 3;
    } else {
        Vec3_Asr((struct Vec3*)&v1, (struct Vec3*)&mPosX, 3);
        Matrix4x3_FromTranslation(&mModel.mat4x3, v1.x, v1.y, v1.z);
    }

    if (mShown == 0) return;

    if ((unsigned)(mMushroomType - 0xb) <= 1) {
        radius = 0x50000;
        depth = 0x50000;
    } else if (!mWithMeshClsn.IsOnGround()) {
        int y = mPosY;
        int z = mPosZ;
        int adjustedY;
        int x = mPosX;
        adjustedY = y + 0x28000;
        v2.x = x;
        v2.y = adjustedY;
        v2.z = z;
        dBgCh_Gnd rg;
        rg.SetObjAndPos(v2, 0);
        depth = v2.y;
        if (rg.DetectClsn()) {
            depth = rg.clsnY;
        }
        depth = mPosY - depth;
        if (depth <= 0x1000) depth = 0x1000;
        radius = (mdCcAc_c.radius - 0xa000) * 2 - (int)(((long long)depth * 0x180 + 0x800) >> 12);
        if (radius < 0xa000) radius = 0xa000;
        depth += 0x3c000;
    } else {
        depth = 0x3c000;
        radius = (mdCcAc_c.radius - 0xa000) * 2;
    }

    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(this, &mShadowModel, &mModel.mat4x3, radius, depth, 0xf);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020af684EiP6Player
/* Shared with the egg-turn hook and dispatch indices 8 and 6. This helper
   finishes by killing the actor and returns no value. Its callers use the same
   void contract; the lookup result in ordinals 19 and 22 is still needed as
   the player argument. This reconstructs a consistent interface, not an
   original return type recovered from an unused register.
   What it does: walks the actors that share this mushroom's actorID
   (FindWithActorID) and, on the first one whose mMushroomType equals `target`,
   decrements its mUnlockCount. When this mushroom is type 11 or 12 it also gives
   the player one coin (GiveCoins with the player number) and Heal(0x100),
   playing sound 0x12 when the player is underwater and 0x11 otherwise -- the
   same calls and sounds dActor_c::GivePlayerCoins makes for a single coin.
   Always ends by removing the mushroom. */
void da1up_c::func_ov002_020af684(int target, Player* player) {
    Player* p = player;
    dActor_c* found = 0;
    for (;;) {
        found = dActor_c::FindWithActorID(actorID, found);
        if (found == 0)
            break;
        if (target == ((da1up_c*)found)->mMushroomType) {
            ((da1up_c*)found)->mUnlockCount--;
            break;
        }
    }
    if ((unsigned int)(mMushroomType - 0xb) <= 1) {
        GiveCoins(p->mPlayerNo, 1);
        p->Heal(0x100);
        if (p->mIsUnderwater)
            Sound::PlayBank3(SND3_COIN_UNDERWATER, *(Vector3*)&mCamSpacePosX);
        else
            Sound::PlayBank3(SND3_COIN, *(Vector3*)&mCamSpacePosX);
    }
    KillAndTrackInDeathTable();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020af724Ev
/* Dispatch-table index 13 (MUSHROOM_FALL_THEN_WAIT). Physics runs every frame.
   State 0 plays sound 0x69 and moves on; state 1 waits until the actor's floor
   collision (mWithMeshClsn) reports ground contact, then enables the cylinder
   (clears bit 0 of its flags, which disables it while set) and moves on; state 2
   runs the touch check. Then the range check (3000 units) and the trail effect. */
void da1up_c::func_ov002_020af724() {

    func_ov002_020aefb8();
    switch (mState) {
    case 0:
        Sound::PlayBank3(SND3_UNK_69, *(Vector3*)&mCamSpacePosX);
        mState += 1;
        break;
    case 1:
        if (mWithMeshClsn.IsOnGround() != 0) {
            mdCcAc_c.flags &= ~CC_FLAGS_DISABLED;
            mState += 1;
        }
        break;
    case 2:
        func_ov002_020af3a8();
        break;
    }
    func_ov002_020af218(0xbb8);
    func_ov002_020aeee4();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020af7ccEv
/* Dispatch-table index 10 (MUSHROOM_RISE_THEN_POP_OUT). Marks the mushroom
   shown and, while it is below 100 units (0x64000) above mSpawnPosY, raises it 5
   units (0x5000) per frame. On the frame it reaches that height it is clamped
   to it and turned into type 0 in state 0, with mStateTimer and mStateFrames set
   to 0xffff so the increments Behavior makes right after the call wrap them to
   0. */
void da1up_c::func_ov002_020af7cc() {
    mShown = 1;
    if (mPosY >= mSpawnPosY + 0x64000) return;
    mPosY += 0x5000;
    if (mPosY < mSpawnPosY + 0x64000) return;
    mPosY = mSpawnPosY + 0x64000;
    mMushroomType = 0;
    mState = 0;
    *(unsigned short*)&mStateTimer = 0xffff;
    mStateFrames = 0xffff;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020af838Ev
/* Dispatch-table index 9 (MUSHROOM_SPAWNER). Spawns three mushrooms of the same
   actor ID around mSpawnPos and then removes itself: spawn param 0x25 (type 5
   with mUnlockCount 2) 50 units (0x32000) above the spawn point, and two of
   param 0xb (type 11) at the spawn height, 500 units (0x1f4000) to the -x and +x
   side. */
void da1up_c::func_ov002_020af838() {
    struct Vector3 vec;

    vec.x = mSpawnPosX;
    vec.y = mSpawnPosY;
    vec.z = mSpawnPosZ;
    vec.y = mSpawnPosY + 0x32000;
    dActor_c::Spawn(
        actorID, 0x25, vec, 0, mAreaId, -1);

    vec.y = mSpawnPosY;
    vec.x = mSpawnPosX - 0x1f4000;
    dActor_c::Spawn(
        actorID, 0xb, vec, 0, mAreaId, -1);

    vec.x = mSpawnPosX + 0x1f4000;
    dActor_c::Spawn(
        actorID, 0xb, vec, 0, mAreaId, -1);

    KillAndTrackInDeathTable();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020af908Ev
/* Dispatch-table index 12 (MUSHROOM_SPIN_TRIGGER_FOR_7). Turns mAngleY by 0xc00
   (16.875 degrees) a frame, then runs index 8's handler. */
void da1up_c::func_ov002_020af908() {
    mAngleY += 0xc00;
    func_ov002_020af924();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020af924Ev
/* Dispatch-table index 8 (MUSHROOM_TRIGGER_FOR_7). If a player is touching it,
   hands off to func_ov002_020af684 with target 7 (a waiting type 7 loses one
   from its mUnlockCount). */
void da1up_c::func_ov002_020af924() {
  Player* r=(Player*)func_ov002_020af1dc();
  if(!r) return;
  func_ov002_020af684(7, r);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020af950Ev
/* Dispatch-table index 7 (MUSHROOM_HIDDEN_CHASE). State 0: hidden (mShown 0)
   until mUnlockCount is 0; then it appears -- mVertSpeed 40 units (0x28000),
   state 3, shown, opened to Yoshi (func_ov002_020aefa4), sound 0x68, mFlags bit
   0 cleared. State 3 is the launch arc (func_ov002_020af474) under physics, with
   the trail effect once mStateTimer is past 17 (0x11); at mStateTimer 0x25 (37)
   it enables the cylinder (clears bit 0 of its flags), goes to state 1, sets
   mVertAccel to 0 and mHorzSpeed to 10 units (0xa000). State 1 steers toward the
   player (func_ov002_020af0c0) under physics. */
void da1up_c::func_ov002_020af950() {

  switch (mState)
  {
    case 0:
      mShown = 0;
      if (mUnlockCount != 0)
        return;

      mVertSpeed = 0x28000;
      mState = 3;
      mShown = 1;
      func_ov002_020aefa4((char*)this);
      Sound::PlayBank3(SND3_LAUNCH, *(Vector3*)&mCamSpacePosX);
      mFlags &= ~ACTOR_FLAG_CLIP_TEST;
      return;

    case 1:
      func_ov002_020af0c0();
      func_ov002_020aefb8();
      return;

    case 3:
      func_ov002_020aefb8();
      if (*(unsigned short *)&mStateTimer > 0x11)
        func_ov002_020aeee4();
      func_ov002_020af474();
      if (*(unsigned short *)&mStateTimer != 0x25)
        return;

      mdCcAc_c.flags &= ~CC_FLAGS_DISABLED;
      mState = 1;
      mVertAccel = 0;
      mHorzSpeed = 0xa000;
      return;
  }
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020afa50Ev
/* Dispatch-table index 11 (MUSHROOM_SPIN_TRIGGER_FOR_5). Turns mAngleY by 0xc00
   (16.875 degrees) a frame, then runs index 6's handler. */
void da1up_c::func_ov002_020afa50() {
    mAngleY += 0xc00;
    func_ov002_020afa6c();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020afa6cEv
/* Dispatch-table index 6 (MUSHROOM_TRIGGER_FOR_5). If a player is touching it,
   hands off to func_ov002_020af684 with target 5 (a waiting type 5 loses one
   from its mUnlockCount). */
void da1up_c::func_ov002_020afa6c() {
  Player* r=(Player*)func_ov002_020af1dc();
  if(!r) return;
  func_ov002_020af684(5, r);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020afa98Ev
/* Dispatch-table index 5 (MUSHROOM_HIDDEN_FLEE). State 0: hidden until
   mUnlockCount is 0, then it appears exactly as index 7 does (state 3, sound
   0x68, and so on). State 3 is the launch arc under physics with the trail
   effect once mStateTimer is past 17; at mStateTimer 0x25 (37) it enables the
   cylinder, goes to state 1 and sets mHorzSpeed to 8 units (0x8000). State 1
   walks away from the player (func_ov002_020afde4) under physics with the trail
   effect; state 2 is physics, the touch check and the expiry countdown (30 frames, then 40 blinking, then removed)
   (func_ov002_020af248). */
void da1up_c::func_ov002_020afa98() {

    switch (mState) {
    case 0:
        mShown = 0;
        if (mUnlockCount != 0)
            return;
        mVertSpeed = 0x28000;
        mState = 3;
        mShown = 1;
        func_ov002_020aefa4((char*)this);
        Sound::PlayBank3(SND3_LAUNCH, *(Vector3*)&mCamSpacePosX);
        mFlags &= ~ACTOR_FLAG_CLIP_TEST;
        return;
    case 1:
        func_ov002_020aefb8();
        func_ov002_020afde4();
        func_ov002_020aeee4();
        return;
    case 2:
        func_ov002_020aefb8();
        func_ov002_020af3a8();
        func_ov002_020af248(0x1e);
        return;
    case 3:
        func_ov002_020aefb8();
        if (*(u16 *)&mStateTimer > 0x11) {
            func_ov002_020aeee4();
        }
        func_ov002_020af474();
        if (*(u16 *)&mStateTimer != 0x25)
            return;
        mdCcAc_c.flags &= ~CC_FLAGS_DISABLED;
        mState = 1;
        mHorzSpeed = 0x8000;
        return;
    }
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020afbb4Ev
/* Dispatch-table index 4 (MUSHROOM_WAIT_THEN_FACE_AWAY). State 0 waits for the
   player to come within 1000 units (0x3e8), then sets mVertSpeed to 40 units
   (0x28000) and goes to state 1. State 1 turns away from the player
   (func_ov002_020afde4) with the trail effect; unlike types 1 and 5 it sets no
   horizontal speed here; state 2 is the touch check plus
   the expiry countdown (30 frames, then 40 blinking, then removed). The 3000-unit range check runs every frame. The two
   `func_ov002_020aefb8()` calls really do read as
   no-argument calls -- as member calls r0 already carries `this`. */
void da1up_c::func_ov002_020afbb4() {

    switch (mState) {
    case 0:
        if (IsPlayerInRange(0x3e8)) {
            mVertSpeed = 0x28000;
            mState = 1;
        }
        break;
    case 1:
        func_ov002_020aefb8();
        func_ov002_020afde4();
        func_ov002_020aeee4();
        break;
    case 2:
        func_ov002_020aefb8();
        func_ov002_020af3a8();
        func_ov002_020af248(0x1e);
        break;
    }
    func_ov002_020af218(0xbb8);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020afc44Ev
/* Dispatch-table index 3 (MUSHROOM_STATIONARY). Only the touch check and the
   3000-unit range check; no physics and no state. */
int da1up_c::func_ov002_020afc44() {
  func_ov002_020af3a8();
  return func_ov002_020af218(0xbb8);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020afc68Ev
/* State 1 of index 2 (MUSHROOM_WAIT_THEN_ACCELERATE). On the ground it adds 25
   units (0x19000) to mHorzSpeed and zeroes mVertSpeed. In the air it multiplies
   mHorzSpeed by 0xfae / 0x1000 (about 0.98) and passes the product through
   cstd::fdiv(t, 0x1000). mHorzSpeed is capped at 40 units (0x28000). When the
   player is not within 5000 units (0x1388) it goes to state 2. */
void da1up_c::func_ov002_020afc68() {

    if (mWithMeshClsn.IsOnGround() != 0) {
        mHorzSpeed += 0x19000;
        mVertSpeed = 0;
    } else {
        int t = (int)(((s64)mHorzSpeed * 0xfae + 0x800) >> 12);
        mHorzSpeed = _ZN4cstd4fdivEii(t, 0x1000);
    }
    if (mHorzSpeed > 0x28000) {
        mHorzSpeed = 0x28000;
    }
    if (IsPlayerInRange(0x1388) == 0) {
        mState = 2;
    }
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020afd10Ev
/* Dispatch-table index 2 (MUSHROOM_WAIT_THEN_ACCELERATE). From state 1 on, the
   words at 0xa4 / 0xa8 / 0xac are saved before the physics step and the first
   and last restored afterwards. State 0: range check (3000 units), and when the
   player comes within 1000 units (0x3e8) it sets mVertAccel to -4 units
   (-0x4000) and goes to state 1. State 1: func_ov002_020afc68. State 2: the
   expiry countdown (30 frames, then 40 blinking, then removed). Every frame ends with the touch check and the trail effect. */
void da1up_c::func_ov002_020afd10() {

    volatile Fix12i v[3];

    if (mState != 0) {
        v[0] = unk_0a4;
        v[1] = mVertSpeed;
        v[2] = unk_0ac;
        func_ov002_020aefb8();
        unk_0a4 = v[0];
        unk_0ac = v[2];
    }

    switch (mState) {
    case 0:
        func_ov002_020af218(0xbb8);
        if (IsPlayerInRange(0x3e8)) {
            mVertAccel = -0x4000;
            mState = 1;
        }
        break;
    case 1:
        func_ov002_020afc68();
        break;
    case 2:
        func_ov002_020af248(0x1e);
        break;
    }

    func_ov002_020af3a8();
    func_ov002_020aeee4();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020afde4Ev
/* Walking away from the player, called from state 1 of types 1, 4 and 5. Sets
   mPrevAngleY to the bearing from the mushroom to the closest player plus half
   a turn (0x8000), runs the touch check, and goes to state 2 when the floor
   collision reports a wall or the player is not within 3000 units. */
void da1up_c::func_ov002_020afde4() {
  Player* p = ClosestPlayer();
  if(p){
    mPrevAngleY = Vec3_HorzAngle((Vector3*)&mPosX, (Vector3*)&p->mPosX) + 0x8000;
  }
  func_ov002_020af3a8();
  if(mWithMeshClsn.IsOnWall()) mState=2;
  if(IsPlayerInRange(0xbb8)==0) mState=2;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020afe4cEv
/* Dispatch-table index 1 (MUSHROOM_POP_OUT_FLEE). Physics every frame. State 0:
   sound 0x68 on the first frame and the launch arc; at mStateTimer 0x25 (37) it
   enables the cylinder, goes to state 1 and sets mHorzSpeed to 8 units (0x8000).
   State 1 walks away from the player (func_ov002_020afde4). State 2 is the
   expiry countdown (30 frames, then 40 blinking, then removed) and the touch check. Every frame ends with the range check
   and the trail effect. */
void da1up_c::func_ov002_020afe4c() {

    func_ov002_020aefb8();
    switch (mState) {
    case 0:
        if (*(unsigned short*)&mStateTimer == 0) {
            Sound::PlayBank3(SND3_LAUNCH, *(Vector3*)&mCamSpacePosX);
        }
        func_ov002_020af474();
        if (*(unsigned short*)&mStateTimer == 0x25) {
            mdCcAc_c.flags &= ~CC_FLAGS_DISABLED;
            mState = 1;
            mHorzSpeed = 0x8000;
        }
        break;
    case 1:
        func_ov002_020afde4();
        break;
    case 2:
        func_ov002_020af248(0x1e);
        func_ov002_020af3a8();
        break;
    }
    func_ov002_020af218(0xbb8);
    func_ov002_020aeee4();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c19func_ov002_020aff10Ev
/* Dispatch-table index 0 (MUSHROOM_POP_OUT_DRIFT). Physics every frame.
   State 0: sound 0x68 on the first frame and the launch arc; at mStateTimer
   0x25 (37) it enables the cylinder, goes to state 1 and sets mHorzSpeed to
   2 units (0x2000), which this handler never clears. State 1: touch check, and
   state 2 once mStateTimer exceeds 300 (0x12c). State 2: the expiry countdown
   (30 frames, then 40 blinking, then removed) and the touch check. Every frame ends with the range check and the trail effect. */
void da1up_c::func_ov002_020aff10() {

  func_ov002_020aefb8();
  switch(mState){
  case 0:
    if(*(unsigned short*)&mStateTimer == 0) Sound::PlayBank3(SND3_LAUNCH, *(Vector3*)&mCamSpacePosX);
    func_ov002_020af474();
    if(*(unsigned short*)&mStateTimer != 0x25) break;
    mdCcAc_c.flags &= ~CC_FLAGS_DISABLED;
    mState = 1;
    mHorzSpeed = 0x2000;
    break;
  case 1:
    if(*(unsigned short*)&mStateTimer > 0x12c) mState = 2;
    func_ov002_020af3a8();
    break;
  case 2:
    func_ov002_020af248(0x1e);
    func_ov002_020af3a8();
    break;
  }
  func_ov002_020af218(0xbb8);
  func_ov002_020aeee4();
}

/* File-scope `extern "C"` region for the members below. Every one of these
   is spelt as the shard that uses it recovered it; nothing above declares
   any of them, so nothing here overrides a recovered view. */
extern "C" {
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* thiz, dActor_c* a, int r, int h, unsigned int e, unsigned int g);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* thiz, dActor_c* a, int r, int h, Vector3_16* p, int q);
int IsStarCollectedInCurLevel(int a);
}

/* Ordinal 34 dispatches through this pointer-to-member-function type: the
   fourteen descriptors at 0x02108300..0x02108370 are {function pointer, 0}
   records on da1up_c, copied into the 14-element array at 0x0210dc00 by
   __sinit_ov002_02100adc. */
typedef void (da1up_c::*PMF)();

/* Ordinal 35's view of data_ov002_0210d9b8: a cached model handle whose second
   word is the BMD file pointer. */
struct ModelCache { int pad0; BMD_File* file; };

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c16CleanupResourcesEv
/* Vtable slot 3. Releases the model file InitResources loaded -- the 1-Up's
   (data_ov002_0210d9d8) for actorID 276, the other's (data_ov002_0210da30)
   otherwise -- except for types 11 and 12, which use the shared model
   data_ov002_0210d9b8 and release nothing here. For types 11 and 12 it also
   starts particle effect 0xd2 through Particle::System::NewSimple at the
   mushroom's position, 40 units (0x28000) above it. */
int da1up_c::CleanupResources()
{
  extern SharedFilePtr data_ov002_0210d9d8;
  extern SharedFilePtr data_ov002_0210da30;

  int s = mMushroomType;
  if (s != 0xb && s != 0xc){
    int b = (actorID == ACTOR_ONEUPKINOKO);
    if (b != 0) data_ov002_0210d9d8.Release();
    else data_ov002_0210da30.Release();
  }
  if ((unsigned int)(mMushroomType - 0xb) <= 1)
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(PTCL_TYPE_11_12_CLEANUP, mPosX, mPosY + 0x28000, mPosZ);
  return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c16OnPendingDestroyEv
/* Vtable slot 12. One instruction: bx lr. */
void da1up_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c6RenderEv
/* Vtable slot 9. Draws mModel unless mShown or mBlinkOn is 0 or mFlags bit
   0x40000 -- one of the yoshi-mouth states named in dActor_c.h -- is set.
   Returns 1 either way. */
int da1up_c::Render()
{
    if (mShown == 0 || mBlinkOn == 0)
        return 1;
    {
        int b = (mFlags & 0x40000) ? 1 : 0;
        if (b)
            return 1;
    }
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c8BehaviorEv
/* Vtable slot 6, and the only literal-pool reference to the 14-element dispatch array at
   0x0210dc00 in the arm9/overlay images besides __sinit_ov002_02100adc, which fills it
   from the 14 descriptors at 0x02108300..0x02108370. mMushroomType is the
   index. The array stays `extern`: this TU claims .text only, so the sinit, the
   descriptors and the array itself remain their own shards.
   When UpdateYoshiEat returns nonzero only the matrix and shadow
   (func_ov002_020af4ec) and the cylinder Clear run. Otherwise it zeroes
   mEatingPlayer, calls the type's handler, advances mStateTimer and
   mStateFrames by one, and if the handler changed mState zeroes both. */
int da1up_c::Behavior()
{
  extern PMF data_ov002_0210dc00[];

  if(UpdateYoshiEat(mWithMeshClsn) != 0){
    func_ov002_020af4ec();
    mdCcAc_c.Clear();
    return 1;
  }
  mEatingPlayer = 0;
  {
    int old = mState;
    (this->*data_ov002_0210dc00[mMushroomType])();
    /* Named ++mStateTimer / mStateTimer = 0 size-DIFF vs this recovered
       unsigned-short launder; keep MATCH form. */
    ++*(unsigned short*)&mStateTimer;
    ++mStateFrames;
    if(old != mState){
      *(unsigned short*)&mStateTimer = 0;
      mStateFrames = 0;
    }
  }
  mdCcAc_c.Clear();
  mdCcAc_c.Update();
  func_ov002_020af4ec();
  return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN7da1up_c13InitResourcesEv
/* Vtable slot 0, the largest member in the TU, and -- because the destructor is
   declared out of line above it -- NOT this class's key function.
   Order of work: mMushroomType from param1 bits 0..3; the model (the shared
   data_ov002_0210d9b8 for types 11 and 12, otherwise the 1-Up's or the other
   actor's file by actorID) and the shadow cylinder; the dCcAc_c cylinder
   (types 6, 8, 11 and 12: radius 100 units, height 64 units, and 11 / 12 also
   get the yoshi-tongue bit in vulnFlags; otherwise 65 / 65 units for actorID 277
   and 50 / 50 for the other), with Init flags 0x100002 and vulnFlags 0; mState 0;
   two 14-byte per-type tables -- data_ov002_020ff040, where a 0 sets bit 0 of
   the cylinder's flags (disabled until a handler clears it; types 0, 1, 5, 7, 9,
   10 and 13), and data_ov002_020ff050, where a 0 clears mFlags bit 0 (the
   clip-test bit; every type except 3, 6 and 8); mShown 1 for types 11 and 12 and
   0 for the rest; mBlinkOn 1; mUnlockCount from param1 bits 4..7; the spawn
   point; gravity -2 units (-0x2000) and terminal velocity -50 units (-0x32000);
   the dBgCh_Actr (50 / 50 units) with its limited-movement flag. Finally, when
   LEVEL_ID (data_0209f2f8) is 7 and the actor is at y = 3500 units (0xdac000),
   z = 0, and either STAR_ID (data_0209f220) is 1 or star 1 is not collected in
   the current level, it calls MarkForDestruction and returns 0. */
int da1up_c::InitResources()
{
    extern ModelCache data_ov002_0210d9b8;
    extern SharedFilePtr data_ov002_0210d9d8;
    extern SharedFilePtr data_ov002_0210da30;
    extern signed char data_0209f2f8;
    extern unsigned char data_0209f220;
    extern unsigned char data_ov002_020ff040[];
    extern unsigned char data_ov002_020ff050[];

    BMD_File* f;
    int isOneUp, isMega;

    mMushroomType = param1 & 0xf;

    isOneUp = (actorID == ACTOR_ONEUPKINOKO);
    if (isOneUp) {
        if ((unsigned int)(mMushroomType - 0xb) <= 1) {
            if (mModel.SetFile(data_ov002_0210d9b8.file, 1, 1) == 0)
                return 0;
        } else {
            f = (BMD_File*)Model::LoadFile(data_ov002_0210d9d8);
            if (mModel.SetFile(f, 1, 1) == 0)
                return 0;
        }
    } else {
        if ((unsigned int)(mMushroomType - 0xb) <= 1) {
            if (mModel.SetFile(data_ov002_0210d9b8.file, 1, 1) == 0)
                return 0;
        } else {
            f = (BMD_File*)Model::LoadFile(data_ov002_0210da30);
            if (mModel.SetFile(f, 1, 1) == 0)
                return 0;
        }
    }

    if (mShadowModel.InitCylinder() == 0)
        return 0;

    if (mMushroomType == 6 || mMushroomType == 8 || (unsigned int)(mMushroomType - 0xb) <= 1) {
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, (dActor_c*)this, 0x64000, 0x40000, 0x100002, 0);
        if ((unsigned int)(mMushroomType - 0xb) <= 1) {
            mdCcAc_c.vulnFlags |= CC_VULN_YOSHI_TONGUE;
        }
    } else {
        isMega = (actorID == ACTOR_SCALEUP_KINOKO);
        if (isMega) {
            _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, (dActor_c*)this, 0x41000, 0x41000, 0x100002, 0);
        } else {
            _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, (dActor_c*)this, 0x32000, 0x32000, 0x100002, 0);
        }
    }

    mState = 0;
    if (data_ov002_020ff040[mMushroomType] == 0) {
        mdCcAc_c.flags |= CC_FLAGS_DISABLED;
    }
    if (data_ov002_020ff050[mMushroomType] == 0) {
        mFlags &= ~ACTOR_FLAG_CLIP_TEST;
    }
    if ((unsigned int)(mMushroomType - 0xb) <= 1) {
        mShown = 1;
    } else {
        mShown = 0;
    }
    mBlinkOn = 1;
    mUnlockCount = ((unsigned int)param1 >> 4) & 0xf;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x32000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, (dActor_c*)this, 0x32000, 0x32000, 0, 0);
    mWithMeshClsn.SetLimMovFlag();
    mParticleID = 0;

    if (data_0209f2f8 == 7 && mPosY == 0xdac000 && mPosZ == 0
        && (data_0209f220 == 1 || IsStarCollectedInCurLevel(1) == 0)) {
        MarkForDestruction();
        return 0;
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* MEGA_MUSHROOM (277 / SCALEUP_KINOKO). Leaf operator new routes to
   fBase_c::operator new; the implicit constructor inlines the dEnemyBase_c
   base step, vptr store, and the four member constructors. */
// @symbol da1up_c_classInit_SCALEUP_KINOKO
extern "C" da1up_c *da1up_c_classInit_SCALEUP_KINOKO()
{
    return new da1up_c();
}

/* -------------------------------------------------------------------------- */
/* ONE_UP_MUSHROOM (276 / ONEUPKINOKO). Same class, second profile. */
// @symbol da1up_c_classInit_ONEUPKINOKO
extern "C" da1up_c *da1up_c_classInit_ONEUPKINOKO()
{
    return new da1up_c();
}
