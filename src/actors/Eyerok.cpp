//cpp
/* Eyerok, the two-handed pyramid boss (.text 0x0211603c..0x0211a418,
 * 62 functions). ROM RTTI daIwante_c (_ZTS10daIwante_c ov066:0x0211ad30);
 * this tree keeps the coined name.
 *
 * The unit's two destructors (D1 0x02115ee0, D0 0x02115f84) stay in their
 * own files: ~Eyerok is the key function, and defining it here would emit
 * the vtable and RTTI as _ZTS6Eyerok, a name the ROM does not have. The
 * tail folds the three zero-gap neighbours up to the sinit at 0x0211a418:
 * the free helper func_ov066_0211a2e4, its tail-call veneer
 * func_ov066_0211a35c (this unit's dBgW callback), and the registry
 * factory daIwante_c_classInit, which installs this unit's vtable.
 *
 * Source is ROM-ascending under defer_codegen off. Do not reorder.
 *
 * Structure: one class, three live instances. Part 0 (mPartIdx) is the body
 * and runs the fight through a phase byte (data_ov066_0211ae04: PHASE_* then
 * PATTERN_4..PATTERN_9); parts 1 and 2 are the hands it spawns. Each part
 * keeps a State (two pointer-to-member pairs, enter and run) in mState and
 * switches by func_ov066_02119454; the descriptors are the data_ov066_0211xxxx
 * tables (listed in the table comment above) and every handler's comment names
 * its state. The
 * parts talk through the shared byte globals declared below the includes.
 *
 * deslop leftovers:
 *  - Raw offsets remain in about thirty places: func_ov066_021175bc
 *    ((char *)self + 0x400 and a short at +0xd0 of the pointer it yields),
 *    func_ov066_02119398 (the char *p / player-position reads, +4 and +8 are
 *    the player's Y and Z), the dust-slot walks in Behavior (+0x4dc..0x4e4
 *    and the u16 at +0xd4 of the +0x400 block) and the dust clear loop in
 *    InitResources (typed access there costs size, measured). The dust slots
 *    are mDustPos in the header but these loops keep the walking-pointer shape.
 *  - Names inferred from behaviour only, not from a symbol or RTTI: mHitPoints,
 *    mTargetPos*, mHorzAccelStep, mTexTimer/mTexPhase, mDustCounter; patterns
 *    4..9 are numbered, not named; the SE_* ids are named by the situation
 *    that plays them; the HIT_* names lean on the best-effort bit table in
 *    dCc_c.h; Eyerok assigns mFlags outright (0x10000000, 0x2000000 or 0,
 *    wiping its other bits) and dActor_c.h lists those two bits as
 *    profile-authored with the consumer unrecovered, so what reads them is
 *    unknown; and the 0x2 bit of the collider flags (set and cleared by
 *    func_ov066_021162e8 / 0211632c / 021164ec) is
 *    unexplained.
 *  - The func_ov066 members and data_ov066 tables keep linker names.
 *  - EVec3 and M48 are plain word structs standing in for Vector3 and
 *    Matrix4x3. Vec4 is an unused stack object with a destructor.
 *  - Handler descriptions come from reading the code, not from play-testing.
 */

/* Turns off deferred codegen, which does two things at once here: it makes
 * the bracketed opt_common_subs / opt_strength_reduction pair around
 * _ZN6Eyerok8BehaviorEv bind to that member alone instead of leaking
 * file-wide, and it flips .text emission from reverse-source to source
 * order -- which is why this file is written ROM-ascending. */
#pragma defer_codegen off
/* Includes. decl_common.h is DELIBERATELY NOT included: every symbol it
 * would have supplied is declared below instead, with the spelling the
 * shards actually matched under. */
#include "Eyerok.h"
#include "types.h"
#include "dBgW.h"
#include "common.h"
#include "decl_Message.h"
#include "SharedFilePtr.h"
#include "TextureSequence.h"
#include "Player.h"
#include "Message.h"
#include "dCamera_c.h"

/* Actor ids, from symbols/actor_debug_names.tsv. */
enum {
    ACTOR_IWANTE = 176,   /* this class: the main instance spawns the two hand instances with it */
    ACTOR_PLAYER = 191
};

/* mPartIdx: which of the three instances of this class an object is. The low
 * byte of param1 selects it (InitResources). Part 0 is the boss body that runs
 * the fight; parts 1 and 2 are the hands it spawns. The values double as the
 * bit each hand uses in the shared hand masks below (data_ov066_0211abe0,
 * 0211ae00, 0211ae0c), and HANDS_BOTH is both bits. */
enum {
    PART_MAIN = 0,
    PART_HAND_1 = 1,
    PART_HAND_2 = 2,
    HANDS_BOTH = 3
};

/* data_ov066_0211ae04, the fight phase shared by all three instances. The
 * body's state handlers set it and the hands follow it (func_ov066_021168ec
 * moves a hand out of its waiting state into the state for the pattern
 * number). Patterns 4..9 are named by number only; what each one does is
 * described at its hand handler. */
enum {
    PHASE_DORMANT = 1,   /* initial; Render draws only the body, not the hands */
    PHASE_RISE = 2,      /* the hands wait for this, then come up (func_ov066_02118678) */
    PHASE_REST = 3,      /* hands sit at rest; the body is choosing the next pattern */
    PATTERN_4 = 4,
    PATTERN_5 = 5,
    PATTERN_6 = 6,
    PATTERN_7 = 7,
    PATTERN_8 = 8,
    PATTERN_9 = 9
};

/* dCc_c hit bits, as func_ov066_0211603c reads mdCcAcPos_c.hitFlags and as
 * the hands set vulnFlags. The names are include/dCc_c.h's best-effort table,
 * which is not proven by the ROM. HIT_HAND_VULNERABLE is every bit below
 * except HIT_MEGA; a hand's vulnFlags are armed with that plus HIT_MEGA
 * (0x427f0, func_ov066_021164ec) and later cleared back to just
 * HIT_SPIN_OR_GROUND_POUND among them (func_ov066_021165cc). */
enum {
    HIT_MEGA = 0x10,
    HIT_SPIN_OR_GROUND_POUND = 0x20,
    HIT_PUNCH = 0x40,
    HIT_KICK = 0x80,
    HIT_BREAKDANCE = 0x100,
    HIT_SLIDE_KICK = 0x200,
    HIT_DIVE = 0x400,
    HIT_EGG = 0x2000,
    HIT_FIRE = 0x40000,
    HIT_HAND_VULNERABLE = 0x427e0
};

/* Positional sound effects passed to func_02012694 (id, camera-space
 * position), named only by where this file plays them. */
enum {
    SE_ANIM_FRAME_0 = 0x140,    /* a hand's animation is on its first whole frame */
    SE_HAND_HIT = 0x141,        /* a hand took a hit and survived */
    SE_HAND_DEFEATED = 0x142,   /* a hand took its last hit */
    SE_LANDING = 0x143,         /* dust-and-shake after a landing (func_ov066_02116ac4) */
    SE_START_MOVING = 0x144,    /* a hand starts moving to a new target (also at the arming frame in pattern 9 and when the rise starts) */
    SE_TALK_STARTED = 0x145,    /* the talk message was shown */
    SE_DEFEAT_POOF = 0x146      /* a defeated hand is removed in a puff of dust */
};

/* Message ids for Player::ShowMessage in func_ov066_0211903c: 0xb8 is used
 * while both hands are alive (the talk before the fight), 0xb9 once neither is. */
enum {
    MSG_BEFORE_FIGHT = 0xb8,
    MSG_AFTER_FIGHT = 0xb9
};

/* EVec3 is three plain words: unlike a Vector3, a local of it has no destructor. */
struct EVec3 { int x, y, z; };
typedef int (Eyerok::*EyerokPMF)();
/* A state descriptor: enter PMF at +0 (run by func_ov066_02119454 on install),
 * run PMF at +8 (run by Behavior every frame). */
struct EyerokState { EyerokPMF enter; EyerokPMF run; };
struct CLPS_Block;

extern "C" {
/* ---- ov066 .bss: 8-byte SharedFilePtr slots (0x0211ae14..0x0211aebc) ----
 * Word [1] of each is the loaded file, which the states pass to SetAnim /
 * SetFile. Roles, from InitResources and the order __sinit_ov066_0211a418
 * constructs them (the number in brackets is the id each is built with):
 *   models:           ae6c [0x339] body, ae4c [0x33f] hand 1, aeb4 [0x33a] hand 2
 *   texture patterns: ae3c [0x333], aebc [0x334], ae2c [0x33d], ae9c [0x33e]
 *   animations:       ae5c [0x335], ae84 [0x336], aea4 [0x337], ae8c [0x338],
 *                     ae54 [0x340], ae94 [0x341], ae64 [0x342], ae44 [0x344],
 *                     ae74 [0x346], ae7c [0x347]
 *   collision files:  ae24 [0x332] body, aeac [0x33b], ae14 [0x33c],
 *                     ae1c [0x343], ae34 [0x345]
 * Which animation is which is described where it is started. */
extern int data_ov066_0211ae14[];
extern int data_ov066_0211ae1c[];
extern int data_ov066_0211ae24[];
extern int data_ov066_0211ae2c[];
extern int data_ov066_0211ae34[];
extern int data_ov066_0211ae3c[];
extern int data_ov066_0211ae44[];
extern int data_ov066_0211ae4c[];
extern int data_ov066_0211ae54[];
extern int data_ov066_0211ae5c[];
extern int data_ov066_0211ae64[];
extern int data_ov066_0211ae6c[];
extern int data_ov066_0211ae74[];
extern int data_ov066_0211ae7c[];
extern int data_ov066_0211ae84[];
extern int data_ov066_0211ae8c[];
extern int data_ov066_0211ae94[];
extern int data_ov066_0211ae9c[];
extern int data_ov066_0211aea4[];
extern int data_ov066_0211aeac[];
extern int data_ov066_0211aeb4[];
extern int data_ov066_0211aebc[];

/* ---- ov066 .bss / .data byte flags and counters, shared by all three parts ---- */
/* Hands that have reached their spot in pattern 8, one bit per hand. */
extern unsigned char data_ov066_0211ae00;
/* The fight phase (PHASE_* / PATTERN_*). */
extern unsigned char data_ov066_0211ae04;
/* Completion count. In the dormant state it counts frames with the player in
 * the trigger area; later it counts hands finished with the current pattern
 * (a hand defeated also adds one, and each finishing hand adds one more when
 * only one is left; the pattern 9 hand stores 2 outright); the decision state
 * clears it. */
extern unsigned char data_ov066_0211ae08;
/* A bit per hand used as a handshake: a hand sets its bit in when it has
 * finished rising or entered a pattern, the body waits for HANDS_BOTH
 * (func_ov066_021168b0), then writes the picked hand's part number here, and
 * that hand clears its bit when its turn is over. (In pattern 8 both hands
 * move; there it only selects which side, 1 -> X = 820.0.) */
extern unsigned char data_ov066_0211ae0c;
/* Alternates the picked hand between turns of patterns 5..7: 0 -> hand 2,
 * 1 -> hand 1. */
extern unsigned char data_ov066_0211ae10;
/* Hands still alive, one bit per hand (HANDS_BOTH at the start; a defeated
 * hand's bit is XORed out). 0 means the fight is won. */
extern unsigned char data_ov066_0211abe0;
/* Threshold for the special pattern: the decision state picks pattern 8 / 9
 * once mPickCount exceeds this + 3. Starts at 1. */
extern int data_ov066_0211abe4;
/* Offset of a hand's hit volume from its position, Fix12 x/y/z. One global
 * shared by both hands, rewritten by func_ov066_021162e8 and
 * func_ov066_0211632c, so it holds whatever the last of those calls left. */
extern int data_ov066_0211ad18[];

/* ---- ov066 .bss state descriptors, 0x10 bytes each ----
 * Each holds two pointer-to-member pairs, filled by __sinit_ov066_0211a418
 * (relocs.txt names the targets): the enter handler at +0, which
 * func_ov066_02119454 runs when it installs the state, and the run handler at
 * +8, which Behavior runs every frame. "Pattern n" means the state used while
 * data_ov066_0211ae04 is n; the handlers' own comments say what they do.
 *   address  state                  enter     run
 *   body
 *   b09c     dormant                0211944c  02119398
 *   b0ac     talk                   02119348  0211903c
 *   b0cc     decision               0211901c  02118e04
 *   b0dc     pattern 4              02118de0  02118cdc
 *   afcc     pattern 5              02118cb8  02118c00
 *   afdc     pattern 6              02118be0  02118b28
 *   affc     pattern 7              02118b08  02118a50
 *   b00c     pattern 8              02118a30  021189c0
 *   b02c     pattern 9              021189a0  02118954
 *   b03c     pattern running        02118934  021188b0
 *   hands
 *   b05c     rise                   021187c8  02118678
 *   b06c     waiting                02118658  02118604
 *   b07c     defeat                 02116d14  02116c6c
 *   b08c     pattern 4              021185e4  021184e0
 *   b0bc     pattern 5              021184c0  02118188
 *   b0ec     pattern 6              02118168  02117bf0
 *   afec     pattern 7              02117bd0  021175e8
 *   b01c     pattern 8              021175bc  021171b0
 *   b04c     pattern 9              02117190  02116db0
 * (the address column is the low 16 bits of data_ov066_0211xxxx) */
extern char data_ov066_0211afcc;
extern char data_ov066_0211afdc;
extern char data_ov066_0211afec;
extern char data_ov066_0211affc;
extern char data_ov066_0211b00c;
extern char data_ov066_0211b01c;
extern char data_ov066_0211b02c;
extern char data_ov066_0211b03c;
extern char data_ov066_0211b04c;
extern int data_ov066_0211b05c[];
extern char data_ov066_0211b06c;
extern char data_ov066_0211b07c;
extern char data_ov066_0211b08c;
extern int data_ov066_0211b09c[];
extern char data_ov066_0211b0ac;
extern char data_ov066_0211b0bc;
extern char data_ov066_0211b0cc;
extern char data_ov066_0211b0dc;
extern char data_ov066_0211b0ec;

/* ---- ov025 .data: CLPS blocks. ov025 is the overlay resident below ov066
 *      at these addresses (tools/overlay_residency.py rules out every other
 *      candidate) ---- */
extern CLPS_Block data_ov025_02112c08;
extern CLPS_Block data_ov025_02112c88;
extern CLPS_Block data_ov025_02112ca8;
extern CLPS_Block data_ov025_02112cc8;
extern CLPS_Block data_ov025_02112d48;

/* ---- arm9 data ---- */
extern int data_0209e650;
extern void *data_0209f318;
extern int data_020a0e68[];

/* ---- arm9 helpers (unmangled ROM names) ---- */
extern int AngleDiff(int a, int b);
extern int ApproachAngle(s16 *angle, int target, int a, int b, int max);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
extern void Matrix4x3_FromRotationY(void *m, short ang);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void MulVec3Mat4x3(void *a, void *m, void *b);
extern int RandomIntInternal(int *seed);
extern int Vec3_ApproachHorz(void *out, void *a, int maxStep);
extern int Vec3_Dist(const void *a, const void *b);
extern void Vec3_Asr(void *d, void *s, int sh);
extern int Vec3_HorzDist(const void *a, const void *b);
extern s16 Vec3_HorzAngle(const void *a, const void *b);
extern void func_0200d8c8(void *cam, void *v, int strength);
extern void func_020092c4(void *cam, void *out, void *target);
extern void func_02011cfc(void);
extern void func_02011d2c(void);
extern void func_02012694(int a, void *p);
extern void func_020393c4(void *p, void *v);
extern void func_020393d4(void *p, void *v);
extern void func_020398fc(void *p);

/* ---- arm9 / ov002 methods, mangled ROM spelling ---- */
extern void _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(void *out, void *tgt, int step);
extern void _Z14ApproachLinearRiii(int *r, int target, int step);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *self, void *kcl, void *mtx, int fix, short s, void *clps);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, void *actor, Vector3 *v, s32 f1, s32 f2, u32 a, u32 b);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *self, void *bca, int a, int b, int fix, unsigned short t);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, void *btp, int a, int fix, unsigned int b);
extern void _ZN15TextureSequence8LoadFileER13SharedFilePtr(void *sfp);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *sfp);
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int a, int b);
extern void _ZN5Sound22LoadAndSetMusic_Layer3Ej(unsigned int a);
extern void _ZN5Sound22StopLoadedMusic_Layer3Ev(void);
extern void _ZN6Player16IncMegaKillCountEv(void *p);
extern void _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *sfp);
extern void _ZN7fBase_c18MarkForDestructionEv(void *self);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int a, int x, int y, int z);
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 a, u32 b, int x, int y, int z, const void *v, void *cb);
extern void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
extern void *_ZN8dActor_c13ClosestPlayerEv(void *self);
extern void _ZN8dActor_c15HugeLandingDustEb(void *self, int b);
extern void _ZN8dActor_c16TriplePoofDustAtERK7Vector3(void *self, const void *v);
extern int _ZN8dActor_c18HorzAngleToCPlayerEv(void *self);
extern void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 id, u32 b, Vector3 *pos, void *p, int e, int f);
extern u8 _ZN8dActor_c9TrackStarEjj(void *actor, u32 a, u32 b);
extern void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(void *self, void *sm, void *m, int rad, int h, unsigned int u);
/* local extern: the call passes an untyped this, so it cannot use the header method. */
extern int _ZN15dExtFrameCtrl_c8FinishedEv(void *self);
/* local extern: the call passes an untyped this, so it cannot use the header method. */
extern void _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(void *sfp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *bmd, int a, int b);
/* local extern: the call passes an untyped this, so it cannot use the header method. */
extern int _ZNK15dExtFrameCtrl_c13GetFrameCountEv(void *self);

/* ---- the dBgW callback veneer just past this unit (0x0211a35c, its own
 *      src/ file) ---- */
extern void func_ov066_0211a35c(void *a, void *b, void *c);

/* ---- this TU's own members, forward-declared: the file is written
 *      ROM-ascending, so a member that calls one defined further down
 *      needs a declaration first ---- */
}

typedef struct { int w[12]; } M48;

// @symbol _ZN6Eyerok19func_ov066_0211603cEv
/* Hit check for a hand. The hitter is mdCcAcPos_c.otherOwner, a uniqueID
 * (0 = none) that dActor_c::FindWithID turns back into an actor. Returns 0 when
 * nothing counted: no hitter, hitter not found, the angle between the hand's
 * facing and the direction to the closest player is 0x4000 (a quarter turn) or
 * more, or no qualifying hit bit. Returns 1 when the hand was hit and has hit
 * points left, 2 when this hit took the last one.
 *
 * Damage: if the hitter is the player (ACTOR_PLAYER) and mIsMetal == 1 the hand
 * loses a point, and HIT_MEGA in hitFlags costs another (a mega kill is credited
 * to the player when that leaves 0 or fewer). Only if neither applied does a hit
 * in HIT_HAND_VULNERABLE count: one point, plus one more for a HIT_PUNCH whose
 * hitter has param1 == 2. HIT_FIRE also sets mDustCounter to 1.
 *
 * Survived: plays the animation pair in data_ov066_0211ae5c / ae84 (hand 2 /
 * hand 1, with the texture pattern from ae3c / aebc) and SE_HAND_HIT. Defeated:
 * this hand's bit is XORed into data_ov066_0211abe0 (the mask of hands still
 * alive), the shared completion count data_ov066_0211ae08 goes up by one,
 * data_ov066_0211abe4 is set to -3, SE_HAND_DEFEATED plays and the hand enters
 * the defeat state (data_ov066_0211b07c). Callers read the return value to tell
 * these apart. */
int Eyerok::func_ov066_0211603c()
{
    enum Bool { FALSE, TRUE };
    dActor_c *actor;
    int flags;
    int hit;
    unsigned int id;
    u16 type;
    enum Bool is_player;

    id = mdCcAcPos_c.otherOwner;
    if (id == 0)
        goto fail;

    actor = dActor_c::FindWithID(id);
    if (actor == 0)
        return 0;

    if (AngleDiff(mAngleY, HorzAngleToCPlayer()) >= 0x4000)
        return 0;

    type = actor->actorID;
    hit = 0;
    flags = mdCcAcPos_c.hitFlags;
    is_player = (enum Bool)(type == ACTOR_PLAYER);
    if (is_player == FALSE)
        goto other;

    if (((Player *)actor)->mIsMetal == 1) {
        (mHitPoints)--;
        hit = 1;
    }
    if (flags & HIT_MEGA) {
        (mHitPoints)--;
        if (mHitPoints <= 0)
            ((Player *)actor)->IncMegaKillCount();
        hit = 1;
    }

other:
    if (hit == 0) {
        if (flags & HIT_HAND_VULNERABLE) {
            if (flags & HIT_PUNCH) {
                if (actor->param1 == 2)
                    (mHitPoints)--;
            }
            if (flags & HIT_FIRE)
                mDustCounter = 1;
            (mHitPoints)--;
            hit = 1;
        }
    }

    if (hit == 0)
        goto fail;

    if (mHitPoints > 0) {
        if (mPartIdx == PART_HAND_2) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
                &mBlendModelAnim, (void *)data_ov066_0211ae5c[1], 4, 0x40000000, 0x1000, 0);
            _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
                &mTextureSequence, (void *)data_ov066_0211ae3c[1], 0x40000000, 0x1000, 0);
        } else {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
                &mBlendModelAnim, (void *)data_ov066_0211ae84[1], 4, 0x40000000, 0x1000, 0);
            _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
                &mTextureSequence, (void *)data_ov066_0211aebc[1], 0x40000000, 0x1000, 0);
        }
        func_02012694(SE_HAND_HIT, &mCamSpacePosX);
        return 1;
    }

    {
        u8 x = data_ov066_0211ae08;
        u8 y = data_ov066_0211abe0;
        int side = mPartIdx;
        data_ov066_0211abe0 = (u8)(y ^ side);
        data_ov066_0211ae08 = (u8)(x + 1);
        data_ov066_0211abe4 = -3;
    }
    func_02012694(SE_HAND_DEFEATED, &mCamSpacePosX);
    func_ov066_02119454(&data_ov066_0211b07c);
    return 2;

fail:
    return 0;
}

// @symbol _ZN6Eyerok19func_ov066_021162e8Ev
/* Hit volume for a hand at rest: sets the 0x2 bit of mdCcAcPos_c.flags (what it
 * means is not known here; func_ov066_0211632c clears it), radius and height
 * 0x64000 (100.0 each), and the volume's offset from the hand,
 * data_ov066_0211ad18, to (0, 0x20000, -0x10000) = (0, 32.0, -16.0). */
void Eyerok::func_ov066_021162e8()
{
    mdCcAcPos_c.flags |= 2;
    mdCcAcPos_c.radius = 0x64000;
    mdCcAcPos_c.height = 0x64000;
    data_ov066_0211ad18[0] = 0;
    data_ov066_0211ad18[1] = 0x20000;
    data_ov066_0211ad18[2] = -0x10000;
}

// @symbol _ZN6Eyerok19func_ov066_0211632cEv
/* Counterpart of func_ov066_021162e8, run every frame by the pattern 4 hand
 * handler: clears the 0x2 flags bit, radius 0x9c000 (156.0), height 0x164000 (356.0),
 * and the offset becomes (+0x55000 = +85.0 for hand 2, -85.0 otherwise;
 * -0xc0000 = -192.0; 0x80000 = 128.0). */
void Eyerok::func_ov066_0211632c()
{
    mdCcAcPos_c.flags &= ~2;
    mdCcAcPos_c.radius = 0x9c000;
    mdCcAcPos_c.height = 0x164000;
    if (mPartIdx == PART_HAND_2)
        data_ov066_0211ad18[0] = 0x55000;
    else
        data_ov066_0211ad18[0] = -0x55000;
    data_ov066_0211ad18[1] = -0xc0000;
    data_ov066_0211ad18[2] = 0x80000;
}

// @symbol _ZN6Eyerok19func_ov066_02116390Ev
/* Texture-pattern swap timer for a hand. mTexTimer counts down; at 0, if
 * mTexPhase is 0 the hand switches to the pattern in data_ov066_0211ae2c (hand
 * 2) / ae9c and mTexTimer is re-armed to 0x32 + 2 * (0..15) frames (50..80);
 * otherwise it switches back to ae3c / aebc for 8 frames. mTexPhase flips each
 * time. */
void Eyerok::func_ov066_02116390()
{
    DecIfAbove0_Short(&mTexTimer);
    if (mTexTimer != 0)
        return;
    if (mTexPhase == 0) {
        if (mPartIdx == PART_HAND_2)
            _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence, (void *)data_ov066_0211ae2c[1], 0x40000000, 0x1000, 0);
        else
            _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence, (void *)data_ov066_0211ae9c[1], 0x40000000, 0x1000, 0);
        mTexTimer = (((unsigned int)RandomIntInternal(&data_0209e650) >> 8) & 0xf) * 2 + 0x32;
    } else {
        if (mPartIdx == PART_HAND_2)
            _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence, (void *)data_ov066_0211ae3c[1], 0x40000000, 0x1000, 0);
        else
            _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence, (void *)data_ov066_0211aebc[1], 0x40000000, 0x1000, 0);
        mTexTimer = 8;
    }
    mTexPhase ^= 1;
}

// @symbol _ZN6Eyerok19func_ov066_021164ecEv
/* Arm a hand's hit volume. Does nothing unless mStateWork1 is 0 and the animation
 * is on its first whole frame (currFrame >> 12 == 0). Then: vulnFlags gets
 * HIT_HAND_VULNERABLE | HIT_MEGA, mFlags is assigned 0x10000000 (dActor_c.h
 * lists that bit as profile-authored; what reads it is unknown), the 0x2 flags
 * bit is set, the
 * animation in data_ov066_0211ae64 (hand 2) / ae44 is started, and mStateWork1
 * becomes 1. The pattern states only call the hit check once mStateWork1 is 1. */
void Eyerok::func_ov066_021164ec()
{
    if (mStateWork1 != 0) return;
    if ((unsigned short)(mBlendModelAnim.currFrame >> 0xc) != 0) return;
    mdCcAcPos_c.vulnFlags |= HIT_HAND_VULNERABLE | HIT_MEGA;
    mFlags = 0x10000000;
    mdCcAcPos_c.flags |= 2;
    if (mPartIdx == PART_HAND_2) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&mBlendModelAnim, (void *)data_ov066_0211ae64[1], 4, 0, 0x1000, 0);
    } else {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&mBlendModelAnim, (void *)data_ov066_0211ae44[1], 4, 0, 0x1000, 0);
    }
    mStateWork1 = 1;
}

// @symbol _ZN6Eyerok19func_ov066_021165ccEv
/* Stand a hand down (the name only says it ends the hittable window; the
 * animation in ae54 / ae94 plays forward here, and func_ov066_021166c8 plays
 * the same one reversed): starts the animation in data_ov066_0211ae54 (hand 2) / ae94
 * with the texture pattern from ae3c / aebc, sets the animation speed to 0x1000
 * (1.0), clears the vulnFlags bits func_ov066_021164ec armed except
 * HIT_SPIN_OR_GROUND_POUND, and sets mFlags to 0. */
void Eyerok::func_ov066_021165cc()
{
    if (mPartIdx == PART_HAND_2) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
            &mBlendModelAnim, (void *)data_ov066_0211ae54[1], 4, 0x40000000, 0x1000, 0);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
            &mTextureSequence, (void *)data_ov066_0211ae3c[1], 0x40000000, 0x1000, 0);
    } else {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
            &mBlendModelAnim, (void *)data_ov066_0211ae94[1], 4, 0x40000000, 0x1000, 0);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
            &mTextureSequence, (void *)data_ov066_0211aebc[1], 0x40000000, 0x1000, 0);
    }
    mBlendModelAnim.speed = 0x1000;
    {
        u32 *p = &mdCcAcPos_c.vulnFlags;
        *p = *p & (~(HIT_HAND_VULNERABLE | HIT_MEGA) | HIT_SPIN_OR_GROUND_POUND);
        mFlags = 0;
    }
}

// @symbol _ZN6Eyerok19func_ov066_021166c8Ev
/* Put a hand into its lowered pose: starts the animation in data_ov066_0211ae54
 * (hand 2) / ae94 with the texture pattern from ae2c / ae9c, disables the hand's
 * dBgW_Kc collision if it is enabled, loads the one in ae34 (hand 1) / ae1c with
 * the CLPS block from ov025 (0x02112cc8 / 0x02112c88), registers the two dBgW
 * callbacks and enables it. The animation then starts at its last frame
 * (GetFrameCount() - 1, as a 20.12 value) and runs backwards (speed -0x1000 =
 * -1.0). */
void Eyerok::func_ov066_021166c8()
{

    if (mPartIdx == PART_HAND_2) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&mBlendModelAnim, (void *)data_ov066_0211ae54[1], 4, 0x40000000, 0x1000, 0);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence, (void *)data_ov066_0211ae2c[1], 0x40000000, 0x1000, 0);
    } else {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&mBlendModelAnim, (void *)data_ov066_0211ae94[1], 4, 0x40000000, 0x1000, 0);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence, (void *)data_ov066_0211ae9c[1], 0x40000000, 0x1000, 0);
    }

    if (mMeshCollider2.IsEnabled() != 0)
        mMeshCollider2.Disable();

    if (mPartIdx == PART_HAND_1) {
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMeshCollider2, (void *)data_ov066_0211ae34[1], &mClsnMat2, 0x199,
                                   mAngleY, &data_ov025_02112cc8);
    } else {
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMeshCollider2, (void *)data_ov066_0211ae1c[1], &mClsnMat2, 0x199,
                                   mAngleY, &data_ov025_02112c88);
    }

    func_020393d4(&mMeshCollider2, (void *)&dBgW::UpdatePosWithTransform);
    func_020393c4(&mMeshCollider2, (void *)func_ov066_0211a35c);
    func_020398fc(&mMeshCollider2);
    mMeshCollider2.Enable(this);

    {
        int n = mBlendModelAnim.GetFrameCount();
        mBlendModelAnim.currFrame = (int)(((unsigned int)((n - 1) << 0x10)) >> 4);
        mBlendModelAnim.speed = -0x1000;
    }
}

// @symbol _ZN6Eyerok19func_ov066_021168b0Ev
/* Body-side wait used by the pick states: sub-state 0 waits for both hands to have
 * reported in (data_ov066_0211ae0c == HANDS_BOTH), then clears it and moves to
 * sub-state 1. Returns 0 while waiting, 1 afterwards. */
int Eyerok::func_ov066_021168b0()
{
    if (mSubState == 0) {
        if (data_ov066_0211ae0c != HANDS_BOTH) return 0;
        mSubState = 1;
        data_ov066_0211ae0c = 0;
    }
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_021168ecEv
/* Phase check, called by hand states. Returns 0 while the hand is airborne
 * (mVertAccel != 0). If the phase is PHASE_REST and the hand is not already in
 * its waiting state (data_ov066_0211b06c) it is moved into it and 3 is
 * returned. From the waiting state, once the phase is a pattern number, the hand
 * enters that pattern's state (4: b08c, 5: b0bc, 6: b0ec, 7: afec, 8: b01c,
 * 9: b04c) and the phase is returned. Otherwise 0. */
int Eyerok::func_ov066_021168ec()
{
    unsigned char m;
    if (mVertAccel != 0) return 0;
    m = data_ov066_0211ae04;
    if (m == PHASE_REST) {
        if (mState != (void *)&data_ov066_0211b06c) {
            func_ov066_02119454((void *)&data_ov066_0211b06c);
            return 3;
        }
    }
    if ((unsigned char)(m + 0xfc) <= 5
        && mState == (void *)&data_ov066_0211b06c) {
        if (m == PATTERN_4) { func_ov066_02119454((void *)&data_ov066_0211b08c); return data_ov066_0211ae04; }
        if (m == PATTERN_5) { func_ov066_02119454((void *)&data_ov066_0211b0bc); return data_ov066_0211ae04; }
        if (m == PATTERN_6) { func_ov066_02119454((void *)&data_ov066_0211b0ec); return data_ov066_0211ae04; }
        if (m == PATTERN_7) { func_ov066_02119454((void *)&data_ov066_0211afec); return data_ov066_0211ae04; }
        if (m == PATTERN_8) { func_ov066_02119454((void *)&data_ov066_0211b01c); return data_ov066_0211ae04; }
        if (m == PATTERN_9) { func_ov066_02119454((void *)&data_ov066_0211b04c); return data_ov066_0211ae04; }
    }
    return 0;
}

// @symbol _ZN6Eyerok19func_ov066_02116a68Ev
/* Which depth band the closest player is in, by its mPosZ (no null check on the
 * player). Returns -0xc52000 (-3154.0) if Z is below that, else -0xb50000
 * (-2896.0) if Z is below that, else 0 if Z is at or above -0xa68000 (-2664.0),
 * else -0xa68000. The decision state tests for the first two values. */
int Eyerok::func_ov066_02116a68()
{
    volatile int dummy[3];
    (void)dummy;
    Player *p = ClosestPlayer();
    int dist = p->mPosZ;
    int lo = (int)0xff3ae000;
    if (dist < lo) return lo;
    int lo2 = -(int)0xb50000;
    if (dist < lo2) return lo2;
    int hi = (int)0xff598000;
    if (dist >= hi) return 0;
    return hi;
}

// @symbol _ZN6Eyerok19func_ov066_02116ac4Ei
/* Landing effect. Calls func_0200d8c8 with the camera (data_0209f318), the hand's
 * position and `strength` (every caller in this file passes 0x7d0000 = 2000.0). Then, with
 * mPosX/Z temporarily moved by 0x80000 (128.0) (X by -128.0 for hand 1, +128.0
 * otherwise; Z by +128.0), it spawns the huge landing dust at that shifted spot
 * and plays SE_LANDING at the object's own cached camera-space position
 * (mCamSpacePosX, which the shift does not change), and restores the position. */
void Eyerok::func_ov066_02116ac4(int strength)
{
    volatile int s0, s1, s2;
    func_0200d8c8(data_0209f318, &mPosX, strength);
    s0 = mPosX;
    s1 = mPosY;
    s2 = mPosZ;
    if (mPartIdx == PART_HAND_1)
        mPosX -= 0x80000;
    else
        mPosX += 0x80000;
    mPosZ += 0x80000;
    _ZN8dActor_c15HugeLandingDustEb(this, 1);
    func_02012694(SE_LANDING, &mCamSpacePosX);
    mPosX = s0;
    mPosY = s1;
    mPosZ = s2;
}

// @symbol _ZN6Eyerok19func_ov066_02116b78Ev
/* Keep a hand inside the arena. Bounds on mPosX (the values are in units and
 * shifted << 12 here): hand 1 -800..900, hand 2 -900..800, each end widened by
 * 80 unless data_ov066_0211abe0 is HANDS_BOTH (a hand is gone). mPosZ is held
 * between -0xf01000 (-3841.0) and -0x73a000 (-1850.0), the upper bound raised by
 * 0x8c000 (140.0) when a hand is gone. When a bound is hit the position is
 * clamped, mHorzSpeed is zeroed and 1 is returned; otherwise 0. */
int Eyerok::func_ov066_02116b78()
{
    int b1 = 0x320;
    int lo = -0x320;
    int ip = mPartIdx;
    if (ip == 2) { b1 = 0x384; lo = -0x384; }
    unsigned char flag = data_ov066_0211abe0;
    int v = mPosX;
    if (flag != 3) lo -= 0x50;
    lo = lo << 0xc;
    if (v < lo) {
        mPosX = lo;
        mHorzSpeed = 0;
        return 1;
    }
    b1 = 0x320;
    if (ip == 1) b1 = 0x384;
    if (flag != 3) b1 += 0x50;
    b1 = b1 << 0xc;
    if (v > b1) {
        mPosX = b1;
        mHorzSpeed = 0;
        return 1;
    }
    int z = mPosZ;
    int n = 0xff0ff000;
    if (z < n) {
        mPosZ = n;
        mHorzSpeed = 0;
        return 1;
    }
    n = 0xff8c6000;
    if (flag != 3) n += 0x8c000;
    if (z > n) {
        mPosZ = n;
        mHorzSpeed = 0;
        return 1;
    }
    return 0;
}

// @symbol _ZN6Eyerok19func_ov066_02116c6cEv
/* Defeat state, run handler. After the animation passes whole frame 12 the
 * hand's mHorzSpeed is zeroed. When the animation finishes it spawns particles
 * 0x7c and 0x7d at the hand, a triple poof of dust, plays SE_DEFEAT_POOF and
 * marks the actor for destruction. */
int Eyerok::func_ov066_02116c6c()
{
    if ((unsigned int)((unsigned int)(mBlendModelAnim.currFrame << 4) >> 0x10) > 0xc) {
        mHorzSpeed = 0;
    }
    if (mBlendModelAnim.Finished() != 0) {
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x7c, mPosX, mPosY, mPosZ);
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x7d, mPosX, mPosY, mPosZ);
        {
            EVec3 v;
            v.x = mPosX;
            v.y = mPosY;
            v.z = mPosZ;
            _ZN8dActor_c16TriplePoofDustAtERK7Vector3(this, &v);
        }
        func_02012694(SE_DEFEAT_POOF, &mCamSpacePosX);
        _ZN7fBase_c18MarkForDestructionEv(this);
    }
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02116d14Ev
/* Defeat state, enter handler: clears the work words, timer and sub-state, sets
 * mHorzSpeed to -0xa000 (-10.0) and starts the animation in data_ov066_0211aea4
 * (hand 2) / ae8c. */
int Eyerok::func_ov066_02116d14()
{
    mStateWork0 = 0;
    mStateWork1 = 0;
    mTimer1 = 0;
    mSubState = 0;
    mHorzSpeed = -0xa000;
    if (mPartIdx == PART_HAND_2) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&mBlendModelAnim, (void *)data_ov066_0211aea4[1], 4, 0x40000000, 0x1000, 0);
    } else {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&mBlendModelAnim, (void *)data_ov066_0211ae8c[1], 4, 0x40000000, 0x1000, 0);
    }
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02116db0Ev
/* Pattern 9, hand run handler (the body picks pattern 9 when a hand is gone and
 * enough picks have passed). Sub-states:
 *   0  lowered pose (func_ov066_021166c8).
 *   1  aim: until mStateWork1 is set, the target is the closest player's
 *      position plus (0, 0, 0x3e8000 = 1000.0) rotated about Y by the angle from
 *      the hand to that position (a point 1000.0 further along that line, if
 *      angle 0 is +Z); the hand turns toward it (ApproachAngle with 2, 0x400,
 *      0x200), plays SE_ANIM_FRAME_0 on frame 0 of the animation and arms its hit
 *      volume there (SE_START_MOVING). Once armed it runs
 *      the hit check (1 -> sub-state 2) and walks at 0x37000 (55.0) per frame.
 *      Reaching the target (or an arena bound) stands it down -> 3.
 *   2  after a hit: when the animation finishes, stand down -> 3.
 *   3  when the animation finishes, swap the collision file (ov025 0x02112c08 /
 *      0x02112d48) and re-enable it -> 4.
 *   4  return to the rest position at 55.0 per frame, angle 0; on arrival the
 *      position is snapped and data_ov066_0211ae08 is set to 2 -> 5.
 *   5  waits for the phase to leave PATTERN_9, then back to the waiting state. */
int Eyerok::func_ov066_02116db0()
{
    EVec3 in, out;
    s16 ang;
    int r;

    switch (mSubState) {
    case 0:
        func_ov066_021166c8();
        mSubState = 1;
        break;

    case 1:
        if (mStateWork1 == 0) {
            Player *p = ClosestPlayer();
            if (p != 0) {
                EVec3 *pp = (EVec3 *)&p->mPosX;
                mTargetPosX = pp->x;
                mTargetPosY = pp->y;
                mTargetPosZ = pp->z;

                in.x = 0;
                in.y = 0;
                in.z = 0;
                out.x = 0;
                out.y = 0;
                out.z = 0;
                in.z = 0x3e8000;

                ang = Vec3_HorzAngle(&mPosX, &mTargetPosX);
                Matrix4x3_FromRotationY(data_020a0e68, ang);
                MulVec3Mat4x3(&in, data_020a0e68, &out);

                mTargetPosX += out.x;
                mTargetPosZ += out.z;
            }

            if ((unsigned short)(mBlendModelAnim.currFrame >> 0xc) == 0)
                func_02012694(SE_ANIM_FRAME_0, &mCamSpacePosX);

            if ((unsigned short)(mBlendModelAnim.currFrame >> 0xc) == 0) {
                func_ov066_021164ec();
                func_02012694(SE_START_MOVING, &mCamSpacePosX);
            }

            ang = Vec3_HorzAngle(&mPosX, &mTargetPosX);
            ApproachAngle(&mAngleY, ang, 2, 0x400, 0x200);
        }

        if (mStateWork1 == 1) {
            r = func_ov066_0211603c();
            if (r != 0) {
                if (r == 1)
                    mSubState = 2;
                break;
            }
            Vec3_ApproachHorz(&mPosX, &mTargetPosX, 0x37000);
            if (func_ov066_02116b78() == 1 || Vec3_HorzDist(&mPosX, &mTargetPosX) <= 0x37000) {
                func_ov066_021165cc();
                mSubState = 3;
            }
        }
        break;

    /* case 3 body before case 2 to match ROM placement */
    case 3:
        if (mBlendModelAnim.Finished() != 0) {
            if (mMeshCollider2.IsEnabled() != 0)
                mMeshCollider2.Disable();
            if (mPartIdx == PART_HAND_1)
                _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                    &mMeshCollider2, (void *)data_ov066_0211ae14[1], &mClsnMat2, 0x199,
                    mAngleY, &data_ov025_02112c08);
            else
                _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                    &mMeshCollider2, (void *)data_ov066_0211aeac[1], &mClsnMat2, 0x199,
                    mAngleY, &data_ov025_02112d48);
            func_020393d4(&mMeshCollider2, (void *)&dBgW::UpdatePosWithTransform);
            func_020393c4(&mMeshCollider2, (void *)func_ov066_0211a35c);
            func_020398fc(&mMeshCollider2);
            mMeshCollider2.Enable(this);
            mSubState = 4;
        }
        break;

    case 2:
        if (mBlendModelAnim.Finished() != 0) {
            func_ov066_021165cc();
            mSubState = 3;
        }
        break;

    case 4:
        func_ov066_021162e8();
        ApproachAngle(&mAngleY, 0, 2, 0x400, 0x200);
        Vec3_ApproachHorz(&mPosX, &mRestPosX, 0x37000);
        if (Vec3_HorzDist(&mPosX, &mRestPosX) <= 0x37000) {
            mPosX = mRestPosX;
            mPosY = mRestPosY;
            mPosZ = mRestPosZ;
            data_ov066_0211ae08 = 2;
            mAngleY = 0;
            mSubState = 5;
        }
        break;

    case 5:
        if (data_ov066_0211ae04 != PATTERN_9)
            func_ov066_02119454(&data_ov066_0211b06c);
        break;
    }
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02117190Ev
/* Pattern 9, hand enter handler: clears the work words, timer and sub-state. */
int Eyerok::func_ov066_02117190()
{
    mStateWork0 = 0;
    mStateWork1 = 0;
    mTimer1 = 0;
    mSubState = 0;
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_021171b0Ev
/* Pattern 8, hand run handler: both hands move to the player's depth, then each
 * lands, then hops three more times (counted by mStateWork0). Sub-states:
 *   0  pick the target: the closest player's position with Z lowered by 0xc8000
 *      (200.0) and held within -3154.0..-1850.0; X is a fixed spot (820.0 when
 *      data_ov066_0211ae0c is 1, otherwise -370.0, and 242.0 further toward -X
 *      for hand 1); mPrevAngleY is set to -0x4000 or 0x4000 (a quarter turn); Y is
 *      the rest height + 0x1c2000 (450.0). SE_START_MOVING. -> 1 (needs a player).
 *   1  fly there at 0x28000 (40.0) per frame; on arrival set this hand's bit in
 *      data_ov066_0211ae00 and wait for both bits; then mTimer1 = 10 frames (18
 *      for hand 2 when data_ov066_0211ae0c is 1, for hand 1 otherwise) and the
 *      target Y drops to rest height + 0x1a000 (26.0) -> 2.
 *   2  after the timer, close in at 0x32000 (50.0) per frame; on arrival the
 *      landing effect plays (2000.0) and mTimer1 = 15 -> 3.
 *   3  on mTimer1 == 1 the hand jumps (mVertSpeed 0x7c000 = 124.0, mVertAccel
 *      -0x14000 = -20.0, mHorzSpeed 0x1e000 = 30.0, mFlags 0x2000000, a
 *      profile-authored bit whose consumer is not recovered). On each landing the
 *      landing effect plays, mTimer1 = 15 and mStateWork0 counts it; after three
 *      -> 4, otherwise it stays here.
 *   4  at mTimer1 == 1 clear this hand's bit in data_ov066_0211ae00 and wait for
 *      both to be clear; then return toward the rest X/Z at 40.0 per frame; on
 *      arrival clear data_ov066_0211ae0c, snap to rest, data_ov066_0211ae08 += 1
 *      -> 5.
 *   5  waits for the phase to leave PATTERN_8, then back to the waiting state. */
int Eyerok::func_ov066_021171b0()
{

    switch (mSubState) {
    case 0: {
        Player *p = ClosestPlayer();
        if (p == 0)
            break;
        {
            EVec3 *pp = (EVec3 *)&p->mPosX;
            mTargetPosX = pp->x;
            mTargetPosY = pp->y;
            mTargetPosZ = pp->z;
        }
        mTargetPosZ -= 0xc8000;
        if (mTargetPosZ < (int)0xff3ae000) {
            mTargetPosZ = (int)0xff3ae000;
        } else if (mTargetPosZ > (int)0xff8c6000) {
            mTargetPosZ = (int)0xff8c6000;
        }
        if (data_ov066_0211ae0c == PART_HAND_1) {
            mPrevAngleY = -0x4000;
            mTargetPosX = 0x334000;
            if (mPartIdx == PART_HAND_1) {
                mTargetPosX -= 0xf2000;
            }
        } else {
            mPrevAngleY = 0x4000;
            mTargetPosX = (int)0xffe8e000;
            if (mPartIdx == PART_HAND_1) {
                mTargetPosX -= 0xf2000;
            }
        }
        func_02012694(SE_START_MOVING, &mCamSpacePosX);
        mTargetPosY = mRestPosY + 0x1c2000;
        mSubState = 1;
        break;
    }
    case 1:
        _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(&mPosX, &mTargetPosX, 0x28000);
        if (Vec3_Dist(&mPosX, &mTargetPosX) > 0x28000)
            break;
        mPosX = mTargetPosX;
        mPosY = mTargetPosY;
        mPosZ = mTargetPosZ;
        data_ov066_0211ae00 |= mPartIdx;
        if (data_ov066_0211ae00 != HANDS_BOTH)
            break;
        mTimer1 = 0xa;
        if (data_ov066_0211ae0c == PART_HAND_1) {
            if (mPartIdx == PART_HAND_2)
                mTimer1 = 0x12;
        } else {
            if (mPartIdx == PART_HAND_1)
                mTimer1 = 0x12;
        }
        mTargetPosY = mRestPosY + 0x1a000;
        mSubState = 2;
        break;
    case 2:
        if (mTimer1 != 0)
            break;
        _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(&mPosX, &mTargetPosX, 0x32000);
        if (Vec3_Dist(&mPosX, &mTargetPosX) > 0x32000)
            break;
        mPosX = mTargetPosX;
        mPosY = mTargetPosY;
        mPosZ = mTargetPosZ;
        func_ov066_02116ac4(0x7d0000);
        mTimer1 = 0xf;
        mSubState = 3;
        break;
    case 3: {
        unsigned short st = mTimer1;
        if (st != 0) {
            if (st != 1)
                break;
            mVertSpeed = 0x7c000;
            mVertAccel = -0x14000;
            mHorzSpeed = 0x1e000;
            mFlags = 0x2000000;
            break;
        }
        if (mVertAccel == 0)
            break;
        if (mRestPosY < mPosY)
            break;
        mPosY = mRestPosY;
        mVertSpeed = 0;
        mVertAccel = 0;
        mHorzSpeed = 0;
        func_ov066_02116ac4(0x7d0000);
        mTimer1 = 0xf;
        mStateWork0 += 1;
        if (mStateWork0 < 3)
            mSubState = 3;
        else
            mSubState = 4;
        break;
    }
    case 4:
        if (mTimer1 == 1) {
            data_ov066_0211ae00 ^= mPartIdx;
        }
        if (data_ov066_0211ae00 != 0)
            break;
        Vec3_ApproachHorz(&mPosX, &mRestPosX, 0x28000);
        if (Vec3_HorzDist(&mPosX, &mRestPosX) > 0x28000)
            break;
        data_ov066_0211ae0c = 0;
        mPosX = mRestPosX;
        mPosY = mRestPosY;
        mPosZ = mRestPosZ;
        data_ov066_0211ae08 += 1;
        mSubState = 5;
        break;
    case 5:
        if (data_ov066_0211ae04 == PATTERN_8)
            break;
        mFlags = 0;
        func_ov066_02119454(&data_ov066_0211b06c);
        break;
    }
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_021175bcEv
/* Pattern 8, hand enter handler: clears the work words and sub-state, mTimer1
 * (written as the halfword at r1 + 0xd0 from a +0x400 base, the shape that
 * matched) and data_ov066_0211ae00. */
int Eyerok::func_ov066_021175bc()
{
    int r3 = 0;
    mStateWork0 = r3;
    mStateWork1 = r3;
    char *r1 = (char *)this + 0x400;
    char *r2 = (char *)&data_ov066_0211ae00;
    *(short *)(r1 + 0xd0) = r3;
    *r2 = r3;
    mSubState = r3;
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_021175e8Ev
/* Pattern 7, hand run handler. The hand whose number is in data_ov066_0211ae0c is
 * the one that moves; the other lowers and waits, hittable. Sub-states:
 *   0  selected hand: target = the closest player's position if there is one
 *      (otherwise the previous target is kept) with Z
 *      lowered by 200.0, X shifted by 0x58000 (88.0) (+ for hand 1, - for hand
 *      2), Z held within -3154.0..-1850.0; Y target = rest + 450.0;
 *      SE_START_MOVING -> 1. The other hand: once mTimer1 is 0, lowered pose
 *      -> 4.
 *   1  fly to the target at 40.0 per frame; on arrival set mFlags to 0x2000000 (a
 *      profile-authored bit, consumer not recovered), mTimer1 = 10 and the target Y
 *      becomes the rest height -> 2.
 *   2  after the timer, come down at 50.0 per frame; on arrival the landing
 *      effect plays, mTimer1 = 10 and the state moves on to 7. If the player is
 *      in no depth band (func_ov066_02116a68 == 0): mTimer1 = 0x24 (36) and, if
 *      the player is less than 0x400000 (1024.0) away horizontally, mPrevAngleY is
 *      set to +-0x4000 toward the player's side and the hand starts a sideways
 *      run -> 3.
 *   3  sideways run: mHorzAccelStep grows (by 0x1a while mTimer1 is nonzero, by
 *      0x130 after, while it is below 0x2710) and mHorzSpeed is brought toward
 *      0x258000 (600.0) with that step. At an arena bound speed and angle are
 *      zeroed -> 7.
 *   4  the waiting hand: arm the hit volume, hit checks, texture swap; a hit that
 *      leaves it alive -> 5; once data_ov066_0211ae08 is nonzero stand down -> 6.
 *   5  when the animation finishes, stand down -> 6.
 *   6  when the animation finishes, restore the hit volume and collision file,
 *      data_ov066_0211ae08 += 1 -> 8.
 *   7  after mTimer1, return toward the rest X/Z at 40.0 per frame; on arrival
 *      clear this hand's bit in data_ov066_0211ae0c, snap to rest,
 *      data_ov066_0211ae08 += 1 (and once more if a hand is gone) -> 8.
 *   8  waits for the phase to leave PATTERN_7, then back to the waiting state. */
int Eyerok::func_ov066_021175e8()
{
    EVec3 v;

    switch (mSubState) {
    case 0:
        if (data_ov066_0211ae0c == mPartIdx) {
            Player *p = ClosestPlayer();
            if (p != 0) {
                EVec3 *pp = (EVec3 *)&p->mPosX;
                mTargetPosX = pp->x;
                mTargetPosY = pp->y;
                mTargetPosZ = pp->z;
                mTargetPosZ -= 0xc8000;
                if (mPartIdx == PART_HAND_1)
                    mTargetPosX += 0x58000;
                else
                    mTargetPosX -= 0x58000;
                if (mTargetPosZ < -0xc52000)
                    mTargetPosZ = -0xc52000;
                else if (mTargetPosZ > -0x73a000)
                    mTargetPosZ = -0x73a000;
            }
            func_02012694(SE_START_MOVING, &mCamSpacePosX);
            mTargetPosY = mRestPosY + 0x1c2000;
            mSubState = 1;
        } else if (mTimer1 == 0) {
            func_ov066_021166c8();
            mSubState = 4;
        }
        break;
    case 1:
        _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(&mPosX, &mTargetPosX, 0x28000);
        if (Vec3_Dist(&mPosX, &mTargetPosX) <= 0x28000) {
            mTimer1 = 0xa;
            mFlags = 0x2000000;
            mPosX = mTargetPosX;
            mPosY = mTargetPosY;
            mPosZ = mTargetPosZ;
            mTargetPosY = mRestPosY;
            mSubState = 2;
        }
        break;
    case 2:
        if (mTimer1 == 0)
            _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(&mPosX, &mTargetPosX, 0x32000);
        if (Vec3_Dist(&mPosX, &mTargetPosX) <= 0x32000) {
            func_ov066_02116ac4(0x7d0000);
            mPosX = mTargetPosX;
            mPosY = mTargetPosY;
            mPosZ = mTargetPosZ;
            mTimer1 = 0xa;
            mSubState = 7;
            if (func_ov066_02116a68() == 0) {
                Player *p = ClosestPlayer();
                if (p != 0) {
                    EVec3 *pp = (EVec3 *)&p->mPosX;
                    v.x = pp->x;
                    v.y = pp->y;
                    v.z = pp->z;
                    mTimer1 = 0x24;
                    if (Vec3_HorzDist(&mPosX, &v) < 0x400000) {
                        if (v.x < mPosX)
                            mPrevAngleY = -0x4000;
                        else
                            mPrevAngleY = 0x4000;
                        mHorzAccelStep = 0;
                        mHorzSpeed = 0;
                        mSubState = 3;
                    }
                }
            }
        }
        break;
    case 3:
        if (mHorzAccelStep < 0x2710) {
            if (mTimer1 != 0)
                mHorzAccelStep += 0x1a;
            else
                mHorzAccelStep += 0x130;
        }
        _Z14ApproachLinearRiii(&mHorzSpeed, 0x258000, mHorzAccelStep);
        if (func_ov066_02116b78() == 1) {
            mHorzSpeed = 0;
            mPrevAngleY = 0;
            mSubState = 7;
        }
        break;
    case 4:
        func_ov066_021164ec();
        if ((unsigned short)(mBlendModelAnim.currFrame >> 0xc) == 0)
            func_02012694(SE_ANIM_FRAME_0, &mCamSpacePosX);
        if (mStateWork1 == 1) {
            int r = func_ov066_0211603c();
            func_ov066_02116390();
            if (r != 0) {
                if (r == 1)
                    mSubState = 5;
                break;
            }
        }
        if (data_ov066_0211ae08 != 0) {
            func_ov066_021165cc();
            mSubState = 6;
        }
        break;
    case 5:
        if (mBlendModelAnim.Finished() != 0) {
            func_ov066_021165cc();
            mSubState = 6;
        }
        break;
    case 6:
        if (mBlendModelAnim.Finished() != 0) {
            func_ov066_021162e8();
            if (mMeshCollider2.IsEnabled() != 0)
                mMeshCollider2.Disable();
            if (mPartIdx == PART_HAND_1)
                _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                    &mMeshCollider2, (void *)data_ov066_0211ae14[1], &mClsnMat2, 0x199,
                    mAngleY, &data_ov025_02112c08);
            else
                _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                    &mMeshCollider2, (void *)data_ov066_0211aeac[1], &mClsnMat2, 0x199,
                    mAngleY, &data_ov025_02112d48);
            func_020393d4(&mMeshCollider2, (void *)&dBgW::UpdatePosWithTransform);
            func_020393c4(&mMeshCollider2, (void *)func_ov066_0211a35c);
            func_020398fc(&mMeshCollider2);
            mMeshCollider2.Enable(this);
            data_ov066_0211ae08 += 1;
            mSubState = 8;
        }
        break;
    case 7:
        if (mTimer1 == 0) {
            Vec3_ApproachHorz(&mPosX, &mRestPosX, 0x28000);
            if (Vec3_HorzDist(&mPosX, &mRestPosX) <= 0x28000) {
                data_ov066_0211ae0c ^= mPartIdx;
                mPosX = mRestPosX;
                mPosY = mRestPosY;
                mPosZ = mRestPosZ;
                data_ov066_0211ae08 += 1;
                if (data_ov066_0211abe0 != HANDS_BOTH)
                    data_ov066_0211ae08 += 1;
                mSubState = 8;
            }
        }
        break;
    case 8:
        if (data_ov066_0211ae04 != PATTERN_7) {
            mFlags = 0;
            func_ov066_02119454(&data_ov066_0211b06c);
        }
        break;
    }
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02117bd0Ev
/* Pattern 7, hand enter handler: clears the work words, timer and sub-state. */
int Eyerok::func_ov066_02117bd0()
{
    mStateWork0 = 0;
    mStateWork1 = 0;
    mTimer1 = 0;
    mSubState = 0;
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02117bf0Ev
/* Pattern 6, hand run handler. Like pattern 7 without the descent: the selected
 * hand (data_ov066_0211ae0c == mPartIdx) walks level to a target and may run
 * sideways; the other hand lowers and waits, hittable. Sub-states:
 *   0  selected hand: target = the closest player's position if there is one
 *      (otherwise the previous target is kept), Z lowered
 *      by 200.0 and held within -3154.0..-1850.0; if that Z is above -2664.0, X
 *      is pulled toward 0 (from X < 0: +450.0 for hand 1, +300.0 for hand 2; from
 *      X >= 0: -300.0 for hand 1, -450.0 for hand 2). SE_START_MOVING -> 1. The
 *      other hand: once mTimer1 is 0, lowered pose -> 3.
 *   1  walk at 0x28000 (40.0) per frame; on arrival -> 6, unless the player is in
 *      no depth band, in which case the same 36-frame wait and sideways-run
 *      decision as pattern 7 sub-state 2 applies (-> 2).
 *   2  sideways run (pattern 7, sub-state 3) -> 6 at an arena bound.
 *   3  the waiting hand: arm the hit volume, hit checks, texture swap; a hit that
 *      leaves it alive -> 4; once data_ov066_0211ae08 is nonzero stand down -> 5.
 *   4  when the animation finishes, stand down -> 5.
 *   5  when the animation finishes, restore the hit volume and collision file,
 *      data_ov066_0211ae08 += 1 -> 7.
 *   6  return toward the rest X/Z at 40.0 per frame; on arrival clear this hand's
 *      bit in data_ov066_0211ae0c, snap to rest, data_ov066_0211ae08 += 1 (and
 *      once more if a hand is gone) -> 7.
 *   7  waits for the phase to leave PATTERN_6, then back to the waiting state. */
int Eyerok::func_ov066_02117bf0()
{
    EVec3 v;

    switch (mSubState) {
    case 0:
        if (data_ov066_0211ae0c == mPartIdx) {
            Player *p = ClosestPlayer();
            if (p != 0) {
                EVec3 *pp = (EVec3 *)&p->mPosX;
                mTargetPosX = pp->x;
                mTargetPosY = pp->y;
                mTargetPosZ = pp->z;
                mTargetPosZ -= 0xc8000;
                if (mTargetPosZ < -0xc52000)
                    mTargetPosZ = -0xc52000;
                else if (mTargetPosZ > -0x73a000)
                    mTargetPosZ = -0x73a000;
            }
            if (mTargetPosZ > -0xa68000) {
                if (mTargetPosX < 0) {
                    if (mPartIdx == PART_HAND_1)
                        mTargetPosX += 0x1c2000;
                    else
                        mTargetPosX += 0x12c000;
                } else {
                    if (mPartIdx == PART_HAND_1)
                        mTargetPosX -= 0x12c000;
                    else
                        mTargetPosX -= 0x1c2000;
                }
            }
            func_02012694(SE_START_MOVING, &mCamSpacePosX);
            mSubState = 1;
        } else if (mTimer1 == 0) {
            func_ov066_021166c8();
            mSubState = 3;
        }
        break;
    case 1:
        Vec3_ApproachHorz(&mPosX, &mTargetPosX, 0x28000);
        if (Vec3_HorzDist(&mPosX, &mTargetPosX) <= 0x28000) {
            mSubState = 6;
            if (func_ov066_02116a68() == 0) {
                Player *p = ClosestPlayer();
                if (p != 0) {
                    EVec3 *pp = (EVec3 *)&p->mPosX;
                    v.x = pp->x;
                    v.y = pp->y;
                    v.z = pp->z;
                    mTimer1 = 0x24;
                    if (Vec3_HorzDist(&mPosX, &v) < 0x400000) {
                        if (v.x < mPosX)
                            mPrevAngleY = -0x4000;
                        else
                            mPrevAngleY = 0x4000;
                        mHorzAccelStep = 0;
                        mHorzSpeed = 0;
                        mSubState = 2;
                    }
                }
            }
        }
        break;
    case 2:
        if (mHorzAccelStep < 0x2710) {
            if (mTimer1 != 0)
                mHorzAccelStep += 0x1a;
            else
                mHorzAccelStep += 0x130;
        }
        _Z14ApproachLinearRiii(&mHorzSpeed, 0x258000, mHorzAccelStep);
        if (func_ov066_02116b78() == 1) {
            mHorzSpeed = 0;
            mPrevAngleY = 0;
            mSubState = 6;
        }
        break;
    case 3:
        func_ov066_021164ec();
        if ((unsigned short)(mBlendModelAnim.currFrame >> 0xc) == 0)
            func_02012694(SE_ANIM_FRAME_0, &mCamSpacePosX);
        if (mStateWork1 == 1) {
            int r = func_ov066_0211603c();
            func_ov066_02116390();
            if (r != 0) {
                if (r == 1)
                    mSubState = 4;
                break;
            }
        }
        if (data_ov066_0211ae08 != 0) {
            func_ov066_021165cc();
            mSubState = 5;
        }
        break;
    case 4:
        if (mBlendModelAnim.Finished() != 0) {
            func_ov066_021165cc();
            mSubState = 5;
        }
        break;
    case 5:
        if (mBlendModelAnim.Finished() != 0) {
            func_ov066_021162e8();
            if (mMeshCollider2.IsEnabled() != 0)
                mMeshCollider2.Disable();
            if (mPartIdx == PART_HAND_1)
                _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                    &mMeshCollider2, (void *)data_ov066_0211ae14[1], &mClsnMat2, 0x199,
                    mAngleY, &data_ov025_02112c08);
            else
                _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                    &mMeshCollider2, (void *)data_ov066_0211aeac[1], &mClsnMat2, 0x199,
                    mAngleY, &data_ov025_02112d48);
            func_020393d4(&mMeshCollider2, (void *)&dBgW::UpdatePosWithTransform);
            func_020393c4(&mMeshCollider2, (void *)func_ov066_0211a35c);
            func_020398fc(&mMeshCollider2);
            mMeshCollider2.Enable(this);
            data_ov066_0211ae08 += 1;
            mSubState = 7;
        }
        break;
    case 6:
        Vec3_ApproachHorz(&mPosX, &mRestPosX, 0x28000);
        if (Vec3_HorzDist(&mPosX, &mRestPosX) <= 0x28000) {
            data_ov066_0211ae0c ^= mPartIdx;
            mPosX = mRestPosX;
            mPosY = mRestPosY;
            mPosZ = mRestPosZ;
            data_ov066_0211ae08 += 1;
            if (data_ov066_0211abe0 != HANDS_BOTH)
                data_ov066_0211ae08 += 1;
            mSubState = 7;
        }
        break;
    case 7:
        if (data_ov066_0211ae04 != PATTERN_6)
            func_ov066_02119454(&data_ov066_0211b06c);
        break;
    }
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02118168Ev
/* Pattern 6, hand enter handler: clears the work words, timer and sub-state. */
int Eyerok::func_ov066_02118168()
{
    mStateWork0 = 0;
    mStateWork1 = 0;
    mTimer1 = 0;
    mSubState = 0;
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02118188Ev
/* Pattern 5, hand run handler: the selected hand hops once while the other waits,
 * hittable. Sub-states:
 *   0  after mTimer1: the selected hand jumps (mVertSpeed 0x64000 = 100.0,
 *      mVertAccel -0xa000 = -10.0, mFlags 0x2000000) -> 1; the other hand takes
 *      the lowered pose -> 2.
 *   1  on landing: snap to rest, landing effect, data_ov066_0211ae08 += 1 (and
 *      once more if a hand is gone) -> 5.
 *   2  the waiting hand: once armed and data_ov066_0211ae08 is nonzero, after
 *      more than 0x14 (20) frames it stands down -> 4; while that counter runs
 *      (and otherwise) the hit volume is armed, hit checks and texture swap run
 *      on the frames they apply, and a hit that leaves it alive -> 3.
 *   3  when the animation finishes, stand down -> 4.
 *   4  when the animation finishes, restore the hit volume and collision file,
 *      data_ov066_0211ae08 += 1 -> 5.
 *   5  waits for the phase to leave PATTERN_5, then back to the waiting state. */
int Eyerok::func_ov066_02118188()
{

    switch (mSubState) {
    case 0:
        if (mTimer1 != 0)
            break;
        if (data_ov066_0211ae0c == mPartIdx) {
            mVertAccel = -0xa000;
            mVertSpeed = 0x64000;
            mFlags = 0x2000000;
            mSubState = 1;
        } else {
            func_ov066_021166c8();
            mSubState = 2;
        }
        break;

    case 1:
        if (mVertAccel == 0)
            break;
        if (mRestPosY < mPosY)
            break;
        mPosY = mRestPosY;
        mVertSpeed = 0;
        mVertAccel = 0;
        func_ov066_02116ac4(0x7d0000);
        data_ov066_0211ae08 += 1;
        if (data_ov066_0211abe0 != HANDS_BOTH)
            data_ov066_0211ae08 += 1;
        mSubState = 5;
        break;

    case 2:
        if (mStateWork1 == 1) {
            if (data_ov066_0211ae08 != 0) {
                if (mStateWork0 > 0x14) {
                    mStateWork0 = 0;
                    func_ov066_021165cc();
                    mSubState = 4;
                    break;
                }
                mStateWork0 += 1;
            }
        }

        if ((unsigned short)(mBlendModelAnim.currFrame >> 0xc) == 0) {
            func_02012694(SE_ANIM_FRAME_0, &mCamSpacePosX);
        }

        func_ov066_021164ec();

        if (mStateWork1 != 1)
            break;

        func_ov066_02116390();
        {
            int r = func_ov066_0211603c();
            if (r == 0)
                break;
            if (r == 1) {
                mSubState = 3;
            }
        }
        break;

    case 3:
        if (mBlendModelAnim.Finished() == 0)
            break;
        func_ov066_021165cc();
        mSubState = 4;
        break;

    case 4:
        if (mBlendModelAnim.Finished() == 0)
            break;
        func_ov066_021162e8();
        if (mMeshCollider2.IsEnabled() != 0)
            mMeshCollider2.Disable();

        if (mPartIdx == PART_HAND_1) {
            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                &mMeshCollider2, (void *)data_ov066_0211ae14[1], &mClsnMat2, 0x199,
                mAngleY, &data_ov025_02112c08);
        } else {
            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                &mMeshCollider2, (void *)data_ov066_0211aeac[1], &mClsnMat2, 0x199,
                mAngleY, &data_ov025_02112d48);
        }

        func_020393d4(&mMeshCollider2, (void *)&dBgW::UpdatePosWithTransform);
        func_020393c4(&mMeshCollider2, (void *)func_ov066_0211a35c);
        func_020398fc(&mMeshCollider2);
        mMeshCollider2.Enable(this);

        data_ov066_0211ae08 += 1;
        mSubState = 5;
        break;

    case 5:
        if (data_ov066_0211ae04 == PATTERN_5)
            break;
        mFlags = 0;
        func_ov066_02119454(&data_ov066_0211b06c);
        break;
    }

    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_021184c0Ev
/* Pattern 5, hand enter handler: clears the work words, timer and sub-state. */
int Eyerok::func_ov066_021184c0()
{
    mStateWork0 = 0;
    mStateWork1 = 0;
    mTimer1 = 0;
    mSubState = 0;
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_021184e0Ev
/* Pattern 4, hand run handler: the hands take turns hopping in place. It first
 * calls func_ov066_021168ec (twice), whose phase check moves the hand back to
 * its waiting state when the phase returns to PHASE_REST. If the first call
 * returns nonzero and the second does not return 4 the hand stands down
 * (mFlags = 0, func_ov066_021162e8) and returns; as the code reads, the first
 * call returns 3 right after it moves the hand to the waiting state and the
 * second then returns 0, so that is the path taken when the phase is
 * PHASE_REST. Otherwise func_ov066_0211632c is applied
 * every frame. Sub-state 0: the hand whose number is in data_ov066_0211ae0c
 * jumps (mVertSpeed 0x64000 = 100.0, mVertAccel -0x14000 = -20.0) -> 1.
 * Sub-state 1: on landing, snap to rest, landing effect, clear this hand's bit
 * in data_ov066_0211ae0c -> 0. */
int Eyerok::func_ov066_021184e0()
{
    if (func_ov066_021168ec() != 0 && func_ov066_021168ec() != 4) {
        mFlags = 0;
        func_ov066_021162e8();
        return 1;
    }
    func_ov066_0211632c();
    switch (mSubState) {
    case 0:
        if (data_ov066_0211ae0c == mPartIdx) {
            int *p = &mSubState;
            mVertAccel = -0x14000;
            mVertSpeed = 0x64000;
            *p = *p + 1;
        }
        break;
    case 1:
        if (mVertAccel != 0) {
            if (mRestPosY >= mPosY) {
                mPosY = mRestPosY;
                mVertSpeed = 0;
                mVertAccel = 0;
                func_ov066_02116ac4(0x7d0000);
                if ((data_ov066_0211ae0c & mPartIdx) != 0)
                    data_ov066_0211ae0c ^= mPartIdx;
                mSubState = 0;
            }
        }
        break;
    }
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_021185e4Ev
/* Pattern 4, hand enter handler: clears the work words, timer and sub-state. */
int Eyerok::func_ov066_021185e4()
{
    mStateWork0 = 0;
    mStateWork1 = 0;
    mTimer1 = 0;
    mSubState = 0;
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02118604Ev
/* Waiting state, run handler for a hand. When func_ov066_021168ec has just sent
 * the hand into a pattern, this hand's bit is XORed into data_ov066_0211ae0c (so
 * once both hands have done it the mask reads HANDS_BOTH, which the body-side
 * wait in func_ov066_021168b0 looks for); with a hand gone, both bits are forced
 * on. */
int Eyerok::func_ov066_02118604() {
    int r = func_ov066_021168ec();
    if (r != 0) {
        data_ov066_0211ae0c ^= mPartIdx;
        if (data_ov066_0211abe0 != HANDS_BOTH) {
            data_ov066_0211ae0c |= HANDS_BOTH;
        }
    }
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02118658Ev
/* Waiting state, enter handler for a hand: clears the work words, timer and
 * sub-state. */
int Eyerok::func_ov066_02118658()
{
    mStateWork0 = 0;
    mStateWork1 = 0;
    mTimer1 = 0;
    mSubState = 0;
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02118678Ev
/* Rise state, hand run handler (the first state a hand runs). Until the phase is
 * PHASE_RISE it only waits, with the animation held at speed 0. Then: animation
 * speed 0x1000 (1.0), a hop (hand 1: mVertSpeed 0x2d000 = 45.0, mVertAccel
 * -0x2000 = -2.0; hand 2: 0xa000 = 10.0 and -0x800 = -0.5), SE_START_MOVING. The
 * hand moves toward its rest X/Z at 0x14000 (20.0) per frame; on landing it is
 * snapped to rest height and the landing effect plays. Once it has landed, is
 * within 20.0 of the rest X/Z and the animation has finished, its collision is
 * enabled, its bit is ORed into data_ov066_0211ae0c and it enters the waiting
 * state (data_ov066_0211b06c). */
int Eyerok::func_ov066_02118678()
{
    if (mStateWork0 == 0) {
        if (data_ov066_0211ae04 == PHASE_RISE) {
            mBlendModelAnim.speed = 0x1000;
            if (mPartIdx == PART_HAND_1) {
                mVertSpeed = 0x2d000;
                mVertAccel = -0x2000;
            } else {
                mVertSpeed = 0xa000;
                mVertAccel = -0x800;
            }
            mStateWork0 = 1;
            func_02012694(SE_START_MOVING, &mCamSpacePosX);
        }
        return 1;
    }

    Vec3_ApproachHorz(&mPosX, &mRestPosX, 0x14000);
    if (mVertAccel != 0) {
        int v = mRestPosY;
        if (v >= mPosY) {
            mPosY = v;
            mVertSpeed = 0;
            mVertAccel = 0;
            func_ov066_02116ac4(0x7d0000);
        }
    }

    if (mVertAccel == 0
        && Vec3_HorzDist(&mPosX, &mRestPosX) <= 0x14000
        && mBlendModelAnim.Finished()) {
        mMeshCollider2.Enable(this);
        data_ov066_0211ae0c |= mPartIdx;
        func_ov066_02119454(&data_ov066_0211b06c);
    }
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_021187c8Ev
/* Rise state, hand enter handler: starts the animation in data_ov066_0211ae74
 * (hand 2) / ae7c with the texture pattern from ae3c / aebc, freezes it
 * (speed 0) and clears the work words. */
int Eyerok::func_ov066_021187c8(){
  if(mPartIdx == PART_HAND_2){
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&mBlendModelAnim, (void*)data_ov066_0211ae74[1], 4, 0x40000000, 0x1000, 0);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence, (void*)data_ov066_0211ae3c[1], 0x40000000, 0x1000, 0);
  } else {
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(&mBlendModelAnim, (void*)data_ov066_0211ae7c[1], 4, 0x40000000, 0x1000, 0);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence, (void*)data_ov066_0211aebc[1], 0x40000000, 0x1000, 0);
  }
  mBlendModelAnim.speed = 0;
  mStateWork0 = 0;
  mStateWork1 = 0;
  return 1;
}

// @symbol _ZN6Eyerok19func_ov066_021188b0Ev
/* Body state while a pattern runs (enter handler func_ov066_02118934). If both
 * hands are gone (data_ov066_0211abe0 == 0) it sets mTimer2 = 100 and enters the
 * talk state (data_ov066_0211b0ac). Otherwise, once the completion count
 * data_ov066_0211ae08 is 2 or more it sets PHASE_REST and returns to the decision
 * state (data_ov066_0211b0cc). */
int Eyerok::func_ov066_021188b0(){
  if(data_ov066_0211abe0==0){
    mTimer2=0x64;
    func_ov066_02119454(&data_ov066_0211b0ac);
    return 1;
  }
  if(data_ov066_0211ae08>=2){
    data_ov066_0211ae04 = PHASE_REST;
    func_ov066_02119454(&data_ov066_0211b0cc);
  }
  return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02118934Ev
/* Body state while a pattern runs, enter handler: clears the work words, timer
 * and sub-state. */
int Eyerok::func_ov066_02118934()
{
    mStateWork0 = 0;
    mStateWork1 = 0;
    mTimer1 = 0;
    mSubState = 0;
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02118954Ev
/* Pattern 9, body run handler. Once func_ov066_021168b0 reports that the hands
 * are in, resets mPickCount and data_ov066_0211ae10 and enters the
 * pattern-running state (data_ov066_0211b03c). No hand is picked. */
s32 Eyerok::func_ov066_02118954() {
    s32 r = func_ov066_021168b0();
    if (r == 0) {
        return 1;
    }
    mPickCount = 0;
    data_ov066_0211ae10 = 0;
    func_ov066_02119454(&data_ov066_0211b03c);
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_021189a0Ev
/* Pattern 9, body enter handler: clears the work words, timer and sub-state. */
int Eyerok::func_ov066_021189a0()
{
    mStateWork0 = 0;
    mStateWork1 = 0;
    mTimer1 = 0;
    mSubState = 0;
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_021189c0Ev
/* Pattern 8, body run handler. Once func_ov066_021168b0 reports that the hands are
 * in, picks data_ov066_0211ae0c from the top bit of a random number (0: hand 2,
 * 1: hand 1), resets mPickCount and enters the pattern-running state
 * (data_ov066_0211b03c). */
int RandomIntInternal(int* seed);
int Eyerok::func_ov066_021189c0(){
  if(func_ov066_021168b0() == 0) return 1;
  if((((unsigned int)RandomIntInternal(&data_0209e650) >> 0x1f) & 1) == 0)
    data_ov066_0211ae0c = PART_HAND_2;
  else
    data_ov066_0211ae0c = PART_HAND_1;
  mPickCount = 0;
  func_ov066_02119454(&data_ov066_0211b03c);
  return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02118a30Ev
/* Pattern 8, body enter handler: clears the work words, timer and sub-state. */
int Eyerok::func_ov066_02118a30()
{
    mStateWork0 = 0;
    mStateWork1 = 0;
    mTimer1 = 0;
    mSubState = 0;
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02118a50Ev
/* Pattern 7, body run handler. Once func_ov066_021168b0 reports that the hands are
 * in, picks the hand: with both alive it alternates using the low bit of
 * data_ov066_0211ae10 (0: hand 2, 1: hand 1), otherwise it is the surviving hand;
 * then mPickCount += 1, the toggle flips and the body enters the
 * pattern-running state (data_ov066_0211b03c). The pattern 5 and 6 body handlers
 * (func_ov066_02118c00, func_ov066_02118b28) have identical code. */
s32 Eyerok::func_ov066_02118a50() {
    s32 r = func_ov066_021168b0();
    if (r == 0) return 1;
    if (data_ov066_0211abe0 == HANDS_BOTH) {
        if (!(data_ov066_0211ae10 & 1)) data_ov066_0211ae0c = PART_HAND_2;
        else data_ov066_0211ae0c = PART_HAND_1;
    } else {
        data_ov066_0211ae0c = data_ov066_0211abe0;
    }
    {
        unsigned char* p = (unsigned char*)((int)&mPickCount);
        *p += 1;
    }
    data_ov066_0211ae10 += 1;
    data_ov066_0211ae10 &= 1;
    func_ov066_02119454(&data_ov066_0211b03c);
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02118b08Ev
/* Pattern 7, body enter handler: clears the work words, timer and sub-state. */
int Eyerok::func_ov066_02118b08()
{
    mStateWork0 = 0;
    mStateWork1 = 0;
    mTimer1 = 0;
    mSubState = 0;
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02118b28Ev
/* Pattern 6, body run handler; the same pick as func_ov066_02118a50. */
s32 Eyerok::func_ov066_02118b28() {
    s32 r = func_ov066_021168b0();
    if (r == 0) return 1;
    if (data_ov066_0211abe0 == HANDS_BOTH) {
        if (!(data_ov066_0211ae10 & 1)) data_ov066_0211ae0c = PART_HAND_2;
        else data_ov066_0211ae0c = PART_HAND_1;
    } else {
        data_ov066_0211ae0c = data_ov066_0211abe0;
    }
    {
        unsigned char* p = (unsigned char*)((int)&mPickCount);
        *p += 1;
    }
    data_ov066_0211ae10 += 1;
    data_ov066_0211ae10 &= 1;
    func_ov066_02119454(&data_ov066_0211b03c);
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02118be0Ev
/* Pattern 6, body enter handler: clears the work words, timer and sub-state. */
int Eyerok::func_ov066_02118be0()
{
    mStateWork0 = 0;
    mStateWork1 = 0;
    mTimer1 = 0;
    mSubState = 0;
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02118c00Ev
/* Pattern 5, body run handler; the same pick as func_ov066_02118a50. */
s32 Eyerok::func_ov066_02118c00() {
    s32 r = func_ov066_021168b0();
    if (r == 0) return 1;
    if (data_ov066_0211abe0 == HANDS_BOTH) {
        if (!(data_ov066_0211ae10 & 1)) data_ov066_0211ae0c = PART_HAND_2;
        else data_ov066_0211ae0c = PART_HAND_1;
    } else {
        data_ov066_0211ae0c = data_ov066_0211abe0;
    }
    {
        unsigned char* p = (unsigned char*)((int)&mPickCount);
        *p += 1;
    }
    data_ov066_0211ae10 += 1;
    data_ov066_0211ae10 &= 1;
    func_ov066_02119454(&data_ov066_0211b03c);
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02118cb8Ev
/* Pattern 5, body enter handler: like the others, but mTimer1 starts at 30. */
int Eyerok::func_ov066_02118cb8()
{
    mStateWork0 = 0;
    mStateWork1 = 0;
    mTimer1 = 30;
    mSubState = 0;
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02118cdcEv
/* Pattern 4, body run handler: keeps the hands hopping while the player stays in
 * the far band. After func_ov066_021168b0 reports the hands are in: once
 * mTimer1 (30 frames) is 0 and the player is no longer in the -3154.0 band
 * (func_ov066_02116a68 returns something else), then, when no hand is mid-hop
 * (data_ov066_0211ae0c == 0), it sets PHASE_REST, mTimer2 = 30 and returns to
 * the decision state. Whenever no hand is mid-hop it also picks the next one:
 * with both alive hand 1 first, then alternating (mStateWork0 flips between 0
 * and 1); with one gone, that one. */
int Eyerok::func_ov066_02118cdc() {
    if (func_ov066_021168b0() == 0)
        return 1;
    if (mTimer1 == 0) {
        if (func_ov066_02116a68() != (int)0xff3ae000) {
            if (data_ov066_0211ae0c == 0) {
                data_ov066_0211ae04 = PHASE_REST;
                mTimer2 = 0x1e;
                func_ov066_02119454(&data_ov066_0211b0cc);
            }
            return 1;
        }
    }
    if (data_ov066_0211ae0c == 0) {
        if (data_ov066_0211abe0 == HANDS_BOTH) {
            if (mStateWork0 == 0)
                data_ov066_0211ae0c = PART_HAND_1;
            else
                data_ov066_0211ae0c = PART_HAND_2;
        } else {
            data_ov066_0211ae0c = data_ov066_0211abe0;
        }
        volatile int* tmp = (volatile int*)((int)&mStateWork0);
        *tmp = *tmp + 1;
        *tmp = *tmp & 1;
    }
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02118de0Ev
/* Pattern 4, body enter handler: like the others, but mTimer1 starts at 30. */
int Eyerok::func_ov066_02118de0()
{
    mStateWork0 = 0;
    mStateWork1 = 0;
    mTimer1 = 30;
    mSubState = 0;
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02118e04Ev
/* Decision state, run handler (the body's idle state between patterns). If both
 * hands are gone it sets mTimer2 = 100 and enters the talk state. It waits for a
 * player and for mTimer2 to be 0, then resets the completion count
 * (data_ov066_0211ae08) and the hand mask (data_ov066_0211ae0c), takes a coin flip
 * from the top bit of a random number, and chooses the next pattern:
 *   player's Z below -3154.0             -> PATTERN_4 (body state b0dc)
 *   mPickCount > data_ov066_0211abe4 + 3 -> PATTERN_8 (b00c) with both hands
 *                                           alive, otherwise PATTERN_9 (b02c);
 *                                           with both alive abe4 flips between
 *                                           1 and 0, otherwise it is set to -3
 *   player in the -2896.0 band           -> PATTERN_5 (afcc)
 *   coin flip 0 / 1                      -> PATTERN_7 (affc) / PATTERN_6 (afdc)
 * It enters the matching body state and sets data_ov066_0211ae04 to the pattern
 * number, which is what the hands follow (func_ov066_021168ec). */
int Eyerok::func_ov066_02118e04()
{
    Player* p = ClosestPlayer();
    int coinFlip;
    int v;

    if (data_ov066_0211abe0 == 0) {
        mTimer2 = 0x64;
        func_ov066_02119454(&data_ov066_0211b0ac);
        return 1;
    }

    if (p == 0 || mTimer2 != 0)
        return 1;

    coinFlip = ((unsigned int)RandomIntInternal(&data_0209e650) >> 31) & 1;

    data_ov066_0211ae08 = 0;
    data_ov066_0211ae0c = 0;
    v = func_ov066_02116a68();
    if (v == (int)0xff3ae000) {
        data_ov066_0211ae04 = PATTERN_4;
        func_ov066_02119454(&data_ov066_0211b0dc);
        return 1;
    }

    if ((int)mPickCount > data_ov066_0211abe4 + 3) {
        if (data_ov066_0211abe0 == HANDS_BOTH) {
            data_ov066_0211abe4++;
            data_ov066_0211abe4 &= 1;
            data_ov066_0211ae04 = PATTERN_8;
            func_ov066_02119454(&data_ov066_0211b00c);
        } else {
            data_ov066_0211abe4 = -3;
            data_ov066_0211ae04 = PATTERN_9;
            func_ov066_02119454(&data_ov066_0211b02c);
        }
        return 1;
    }

    v = func_ov066_02116a68();
    if (v == -0xb50000) {
        data_ov066_0211ae04 = PATTERN_5;
        func_ov066_02119454(&data_ov066_0211afcc);
        return 1;
    }

    if (coinFlip == 0) {
        data_ov066_0211ae04 = PATTERN_7;
        func_ov066_02119454(&data_ov066_0211affc);
    } else {
        data_ov066_0211ae04 = PATTERN_6;
        func_ov066_02119454(&data_ov066_0211afdc);
    }
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_0211901cEv
/* Decision state, enter handler: clears the work words, timer and sub-state. */
int Eyerok::func_ov066_0211901c()
{
    mStateWork0 = 0;
    mStateWork1 = 0;
    mTimer1 = 0;
    mSubState = 0;
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_0211903cEv
/* Talk state, run handler (enter handler func_ov066_02119348). Used twice: before
 * the fight (both hands alive, once they have risen) and after it (both gone).
 * Waits for mTimer2. On the first frame it sets the camera flag
 * (dCamera_c::SetFlag_3), remembers the closest player in mTalkPlayer and locks it
 * with SetNoControlState(5, -1, 0). From then on the camera's look-at (+0x80) and
 * position (+0x8c) are moved (func_020092c4) to the body's position + (0,
 * 0x100000, 0) = 256.0 up, and + (0x10000, 0x100000, 0x564000) = (16.0, 256.0,
 * 1380.0). With both hands alive it also waits for data_ov066_0211ae0c ==
 * HANDS_BOTH. mStateWork1 0: shows the message (MSG_BEFORE_FIGHT, or
 * MSG_AFTER_FIGHT with the music volume turned down) from a point
 * (0, +50.0, -50.0) from the body, and on success sets mStateWork1 = 1 and plays
 * SE_TALK_STARTED. mStateWork1 1: when the player's talk state goes negative
 * (the message is over) camera flag 8 is cleared and Message::EndTalk runs.
 * Then, before the fight, music 0x2d is loaded and the body enters the decision
 * state; after it, the music volume is restored, the loaded music is stopped and
 * the body spawns its star (UntrackAndSpawnStar at (0, -1500.0, -3660.0), with
 * the id and tracking state from InitResources) and marks itself for
 * destruction. */
int Eyerok::func_ov066_0211903c() {
    struct Vector3 v1, v2, in, out, star;
    dCamera_c* cam;
    int msgid;

    if (mTimer2) return 1;

    cam = (dCamera_c *)data_0209f318;
    if (mSubState == 0) {
        cam->SetFlag_3();
        mTalkPlayer = ClosestPlayer();
        if (mTalkPlayer != 0)
            ((Player *)(mTalkPlayer))->SetNoControlState(5, -1, 0);
        mSubState = 1;
    } else {
        v1.x = mPosX;
        v1.y = mPosY;
        v1.z = mPosZ;
        v2.x = mPosX;
        v2.y = mPosY;
        v2.z = mPosZ;
        v1.y += 0x100000;
        v2.x += 0x10000;
        v2.y += 0x100000;
        v2.z += 0x564000;
        func_020092c4(cam, &cam->lookAt, &v1);
        func_020092c4(cam, &cam->pos, &v2);
    }

    if (data_ov066_0211abe0 == HANDS_BOTH) {
        if (data_ov066_0211ae0c != HANDS_BOTH) return 1;
    }

    if (mStateWork1 == 0) {
        if (mTalkPlayer != 0) {
            in.x = 0; in.y = 0; in.z = 0;
            out.x = 0; out.y = 0; out.z = 0;
            in.y = 0x32000;
            in.z = -0x32000;

            Matrix4x3_FromRotationY(data_020a0e68, 0);
            MulVec3Mat4x3(&in, data_020a0e68, &out);

            out.x += mPosX;
            out.y += mPosY;
            out.z += mPosZ;

            msgid = MSG_BEFORE_FIGHT;
            if (data_ov066_0211abe0 == 0) {
                msgid = MSG_AFTER_FIGHT;
                _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x14, 0x15666);
            }

            mTalkPlayer->mStateFlags |= 0x400;
            Message::PrepareTalk();
            if (((Player *)(mTalkPlayer))->ShowMessage(*(fBase_c *)this, msgid, &out, 0, 0) == 1) {
                mStateWork1 = 1;
                func_02012694(SE_TALK_STARTED, &mCamSpacePosX);
            }
        }
    } else {
        if (mTalkPlayer != 0) {
            if (((Player *)(mTalkPlayer))->GetTalkState() < 0) {
                cam->mFlags &= ~8;
                Message::EndTalk();
                if (data_ov066_0211abe0 == HANDS_BOTH) {
                    _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2d);
                    func_02011d2c();
                    func_ov066_02119454(&data_ov066_0211b0cc);
                } else {
                    _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x15666);
                    _ZN5Sound22StopLoadedMusic_Layer3Ev();
                    func_02011cfc();
                    star.x = 0;
                    star.y = (int)0xffa24000;
                    star.z = (int)0xff1b4000;
                    UntrackAndSpawnStar(*(signed char*)(&mStarTracked), mStarId, star, 4);
                    ((fBase_c *)this)->MarkForDestruction();
                }
            }
        }
    }
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02119348Ev
/* Talk state, enter handler: disables the body's collision (mMeshCollider2) if it
 * is enabled, then clears the work words, timer and sub-state. */
int Eyerok::func_ov066_02119348()
{
    if (mMeshCollider2.IsEnabled() != 0) {
        mMeshCollider2.Disable();
    }
    mStateWork0 = 0;
    mStateWork1 = 0;
    mTimer1 = 0;
    mSubState = 0;
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02119398Ev
/* Dormant state, run handler (the body's first state). Each frame that the body
 * is on screen (mFlags bit 0x8, "off screen", is clear), the closest player's
 * mPosY (read as playerPos + 4, with playerPos = player + 0x5c = mPosX) is below
 * -0x300000 (-768.0) and its mPosZ (playerPos + 8) is below -0xd70000
 * (-3440.0), data_ov066_0211ae08 is incremented. When it exceeds 2 it is reset to
 * 0, the phase becomes PHASE_RISE and the body enters the talk state
 * (data_ov066_0211b0ac). */
struct Vec4 { int a, b, self, d; ~Vec4(){} };
int Eyerok::func_ov066_02119398()
{
    Vec4 sp;
    /* Member loads of the player's position come out a different size.
       The base pointer is what matches. */
    char *p = (char *)ClosestPlayer();
    if (p != 0) {
        char* playerPos = p + 0x5c;
        int v1 = *(int*)(playerPos + 4);
        int v2 = *(int*)(playerPos + 8);
        if (v1 < -0x300000) {
            int f = (int)((mFlags & 8) != 0);
            if (f == 0) {
                if (v2 < -0xd70000) {
                    data_ov066_0211ae08 += 1;
                }
            }
        }
    }
    if (data_ov066_0211ae08 > 2) {
        data_ov066_0211ae08 = 0;
        data_ov066_0211ae04 = PHASE_RISE;
        func_ov066_02119454(&data_ov066_0211b0ac);
    }
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_0211944cEv
/* Dormant state, enter handler: does nothing. */
int Eyerok::func_ov066_0211944c()
{
    return 1;
}

// @symbol _ZN6Eyerok19func_ov066_02119454EPv
/* Install a state. `pv` points at a descriptor made of two pointer-to-member
 * pairs (8 bytes each: enter handler at +0, run handler at +8, filled in by
 * __sinit_ov066_0211a418). It is stored in mState (+0x48c) and the enter handler
 * is called on the object unless that first word is null. */
int Eyerok::func_ov066_02119454(void *pv) { EyerokState *p = (EyerokState *)pv; mState = p; EyerokState *q = (EyerokState *)mState; if (*(int *)q == 0) return 1; return (this->*q->enter)(); }

// @symbol _ZN6Eyerok19func_ov066_021194a4Ev
/* Refresh the collision matrix mClsnMat2 from the object (rotation about Y by
 * mAngleY, translation = position) and hand it to dBgW_KcMbg::Transform. */
void Eyerok::func_ov066_021194a4() {
  Matrix4x3_FromRotationY(&mClsnMat2, mAngleY);
  mClsnMat2.t.x = mPosX;
  mClsnMat2.t.y = mPosY;
  mClsnMat2.t.z = mPosZ;
  ((dBgW_KcMbg *)(&mMeshCollider2))->Transform(mClsnMat2, mAngleY);
}

// @symbol _ZN6Eyerok19func_ov066_021194fcEv
/* Refresh the model matrices. The matrix is a translation of mPos >> 3 rotated
 * by mAngleX/Y/Z and is stored in mModel2 (the body) or mBlendModelAnim (a hand).
 * A hand above its rest height also gets a drop shadow: a matrix at mPosX
 * +0x64000 (+100.0; -100.0 for hand 1), mPosY - 0x8000 (8.0 lower), mPosZ +
 * 0xa0000 (160.0), each >> 3, passed to dActor_c::DropShadowRadHeight with
 * radius 0x140000 (320.0) and height 0x258000 (600.0). */
void Eyerok::func_ov066_021194fc()
{
    int v[3];
    Vec3_Asr(v, &mPosX, 3);
    Matrix4x3_FromTranslation(data_020a0e68, v[0], v[1], v[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(data_020a0e68, mAngleX, mAngleY, mAngleZ);
    if (mPartIdx == PART_MAIN)
        *(M48 *)&mModel2.mat4x3 = *(M48*)data_020a0e68;
    else
        *(M48 *)&mBlendModelAnim.mat4x3 = *(M48*)data_020a0e68;
    if (mPartIdx == PART_MAIN)
        return;
    if (mRestPosY >= mPosY)
        return;
    {
        int d;
        if (mPartIdx == PART_HAND_2)
            d = 0x64000;
        else
            d = -0x64000;
        Matrix4x3_FromTranslation(data_020a0e68,
            (mPosX + d) >> 3,
            (mPosY - 0x8000) >> 3,
            (mPosZ + 0xa0000) >> 3);
    }
    *(M48 *)mShadowMtx = *(M48*)data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel, mShadowMtx, 0x140000, 0x258000, 0xf);
}

// @symbol _ZN6Eyerok16CleanupResourcesEv
/* Disables the collision if it is still enabled; the body (mPartIdx 0) alone
 * then releases the 22 shared files InitResources loaded: 3 models, 4 texture
 * patterns, 10 animations and 5 collision files. */
int Eyerok::CleanupResources()
{
  if(((dBgW *)&mMeshCollider2)->IsEnabled())
    ((dBgW *)&mMeshCollider2)->Disable();
  if(mPartIdx==0){
    ((SharedFilePtr *)(data_ov066_0211ae6c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae4c))->Release();
    ((SharedFilePtr *)(data_ov066_0211aeb4))->Release();
    ((SharedFilePtr *)(data_ov066_0211aebc))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae9c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae3c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae2c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae5c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae84))->Release();
    ((SharedFilePtr *)(data_ov066_0211aea4))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae8c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae54))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae94))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae64))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae44))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae74))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae7c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae24))->Release();
    ((SharedFilePtr *)(data_ov066_0211aeac))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae14))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae1c))->Release();
    ((SharedFilePtr *)(data_ov066_0211ae34))->Release();
  }
  return 1;
}

// @symbol _ZN6Eyerok16OnPendingDestroyEv
/* Eyerok::OnPendingDestroy -- vtable slot 12. The ROM body is empty: the
 * override exists only to occupy the slot. */
void Eyerok::OnPendingDestroy()
{
}

// @symbol _ZN6Eyerok6RenderEv
/* Body: draws mModel2, only during PHASE_DORMANT. Hands: not drawn during
 * PHASE_DORMANT; afterwards the texture pattern is updated from the model data
 * and mBlendModelAnim is drawn. */
int Eyerok::Render()
{
  if (mPartIdx == PART_MAIN) {
    if (data_ov066_0211ae04 == PHASE_DORMANT) {
      mModel2.Render(0);
    }
    return 1;
  }
  if (data_ov066_0211ae04 == PHASE_DORMANT) return 1;
  ((TextureSequence *)&mTextureSequence)->TextureSequence::Update(mBlendModelAnim.data);
  mBlendModelAnim.Render(0);
  return 1;
}


/* Bracketed, and it binds only because of the file-top
 * `#pragma defer_codegen off`: with codegen deferred (mwccarm 2004/b56s
 * default) a bracketed opt_* pragma does not bind and these two go
 * file-global, which costs func_ov066_021184e0 (4 words) and
 * func_ov066_021194fc (a size change).  Deleting them outright instead
 * costs _ZN6Eyerok8BehaviorEv: 33/34, a 999-word content divergence plus one
 * wrong relocation destination, with the size UNCHANGED at 0x4b0.  (An earlier
 * revision of this comment said 0x4b0 -> 0x4ac; that size change belongs to a
 * ROM-descending arrangement, not this one.  Re-measured by negative control on
 * the shipped source.) */
#pragma opt_common_subs off
#pragma opt_strength_reduction off
// @symbol _ZN6Eyerok8BehaviorEv
/* Per frame, for all three parts: count mTimer1 and mTimer2 down, then run the
 * current state's run handler (the second pointer-to-member of the descriptor
 * mState points at). Then the dust burst: while mDustCounter is nonzero, on each
 * even value a position near the object is written into
 * mDustPos[mDustCounter >> 1] (sideways spread when mAngleY is nonzero, otherwise
 * a random X and +25.0 in Z, rising by mDustCounter * 10 + 35 units; in the
 * defeat state a different random spread, Z randomised too, and +150.0 up), and every frame all
 * nonzero slots have particles 0x13a and 0x13b (re)issued. The counter then goes
 * up by one, and past 0x26 the slots are cleared and it returns to 0. After that
 * the body refreshes its matrices and collision and returns. A hand also sets
 * mRestPosY to mSpawnPosY + 0x8000 (8.0), runs UpdatePos(0), moves its hit cylinder
 * (mdCcAcPos_c) to its position plus data_ov066_0211ad18, refreshes matrices and
 * collision, clears its hit results, re-registers it (Update links it into the
 * collision list) and advances both animations. */
/* Eyerok::Behavior -- vtable slot 6. Real C++ method over the shared header.
 * EVec3 is a local plain-int triple (stack temps); callees whose ROM symbols
 * carry by-value/ref class parameters keep their literal mangled extern "C"
 * spellings. */
int Eyerok::Behavior()
{
    char *c = (char *)this;

    DecIfAbove0_Short(&mTimer1);
    DecIfAbove0_Short(&mTimer2);

    {
        EyerokState *st = (EyerokState *)mState;
        if (*(int *)&st->run != 0)
            (this->*st->run)();
    }

    if (mDustCounter != 0) {
        if ((mDustCounter & 1) == 0) {
            int rnd = RandomIntInternal(&data_0209e650);
            int off = (mDustCounter >> 1) * 0xc;
            int base_dc = 0x4dc;
            int base_e4 = 0x4e4;
            char *bx = c + base_dc;
            char *bz = c + base_e4;
            char *by = c + 0x4e0;
            int *px;
            int *pz;
            int *py;
            int zero;
            EVec3 vin;
            EVec3 vout;
            *(int *)(bx + off) = mPosX;
            *(int *)(by + off) = mPosY;
            *(int *)(bz + off) = mPosZ;
            px = (int *)(bx + off);
            py = (int *)(by + off);
            pz = (int *)(bz + off);
            zero = 0;
            vin.x = zero;
            vin.y = zero;
            vin.z = zero;
            vout.x = zero;
            vout.y = zero;
            vout.z = zero;
            if (mState != (void *)&data_ov066_0211b07c) {
                if (mAngleY != 0) {
                    vin.z = (0x7e - (((rnd >> 8) & 0x3f) << 2)) << 12;
                    Matrix4x3_FromRotationY(data_020a0e68, (s16)(mAngleY - 0x4000));
                    MulVec3Mat4x3(&vin, data_020a0e68, &vout);
                    *px += vout.x;
                    *pz += vout.z;
                } else {
                    if (((rnd >> 16) & 1) == 0)
                        *px += (((rnd >> 8) & 3) * 0x28) << 12;
                    else
                        *px -= (((rnd >> 8) & 3) * 0x28) << 12;
                    *pz += 0x19000;
                }
                *py += ((mDustCounter * 0xa) + 0x23) << 12;
            } else {
                if (mAngleY != 0) {
                    vin.z = (0x7e - (((rnd >> 8) & 0x3f) << 2)) << 12;
                    Matrix4x3_FromRotationY(data_020a0e68, (s16)(mAngleY - 0x4000));
                    MulVec3Mat4x3(&vin, data_020a0e68, &vout);
                    *px += vout.x;
                    *pz += vout.z;
                } else {
                    int a = ((rnd >> 24) & 7) * 0x1e;
                    int b = ((rnd >> 16) & 7) * 0x1e;
                    *pz -= 0x64000;
                    *px += (0x69 - a) << 12;
                    *pz += (0x69 - b) << 12;
                }
                *py += 0x96000;
            }
        }

        {
            int i = 0;
            char *cur = c;
            u32 id0 = 0x13a;
            u32 id1 = 0x13b;
            int z0 = 0;
            for (; i < 0x14; i++) {
                if (*(int *)(cur + 0x4dc) != 0 || *(int *)(cur + 0x4e0) != 0 || *(int *)(cur + 0x4e4) != 0) {
                    mDustParticle1[i] =
                        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                            mDustParticle1[i], id0,
                            *(int *)(cur + 0x4dc), *(int *)(cur + 0x4e0), *(int *)(cur + 0x4e4),
                            (void *)z0, (void *)z0);
                    mDustParticle2[i] =
                        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                            mDustParticle2[i], id1,
                            *(int *)(cur + 0x4dc), *(int *)(cur + 0x4e0), *(int *)(cur + 0x4e4),
                            (void *)z0, (void *)z0);
                }
                cur += 0xc;
            }
        }

        {
            /* c400 + 0xd4 is mDustCounter reached the long way round -- the
               ROM materialises c + 0x400 first and offsets from it, and
               spelling that step away is not free. */
            int o4d4 = 0x4d4;
            u16 *p = &mDustCounter;
            u16 v = *p;
            char *c400 = c + 0x400;
            *p = (u16)(v + 1);
            if (*(u16 *)(c400 + 0xd4) > 0x26) {
                int j = 0;
                char *q = c;
                *(u16 *)(c400 + 0xd4) = (u16)j;
                for (; j < 0x14; j++) {
                    *(int *)(q + 0x4dc) = 0;
                    *(int *)(q + 0x4e0) = 0;
                    *(int *)(q + 0x4e4) = 0;
                    q += 0xc;
                }
            }
        }
    }

    if (mPartIdx == PART_MAIN) {
        func_ov066_021194fc();
        if (((dBgW *)&mMeshCollider2)->IsEnabled() != 0)
            func_ov066_021194a4();
        return 1;
    }

    {
        EVec3 vrel;
        mRestPosY = mSpawnPosY + 0x8000;
        ((dActor_c *)c)->UpdatePos(0);
        mdCcAcPos_c.pos.x = mPosX;
        mdCcAcPos_c.pos.y = mPosY;
        mdCcAcPos_c.pos.z = mPosZ;
        vrel.x = data_ov066_0211ad18[0];
        vrel.y = data_ov066_0211ad18[1];
        vrel.z = data_ov066_0211ad18[2];
        ((dCcAcPos_c *)&mdCcAcPos_c)->SetPosRelativeToActor(*(Vector3 *)&vrel);
        func_ov066_021194fc();
        if (((dBgW *)&mMeshCollider2)->IsEnabled() != 0)
            func_ov066_021194a4();
        ((dCc_c *)&mdCcAcPos_c)->Clear();
        ((dCc_c *)&mdCcAcPos_c)->dCc_c::Update();
        ((BlendModelAnim *)&mBlendModelAnim)->Advance();
        ((dExtFrameCtrl_c *)&mTextureSequence)->Advance();
    }
    return 1;
}


#pragma opt_strength_reduction on
#pragma opt_common_subs on

// @symbol _ZN6Eyerok13InitResourcesEv
/* mPartIdx comes from the low byte of param1 (0xff, or anything above 2, means
 * the body); mStarId from param1 bits 12..15; TrackStar(mStarId, 2) is stored in
 * mStarTracked. The body loads every shared file and the hands bind their model
 * and texture patterns; a hand also initialises its shadow cylinder and a hit
 * cylinder (radius and height 0x64000 = 100.0, flags 0x200002, vulnFlags 0, offset
 * data_ov066_0211ad18). The dust slots are cleared and mTerminalVelocity is
 * -0x64000 (-100.0).
 * Body: mPosZ -= 0x7c000 (124.0) and the result is the rest position; it spawns
 * hand 1 (param1 1) at X + 0x193000 (403.0) and hand 2 (param1 2) at X -
 * 0x18c000 (396.0), keeping their uniqueIDs; resets the shared state
 * (data_ov066_0211ae10 = 0, ae08 = 0, ae0c = 0, abe4 = 1, ae04 = PHASE_DORMANT,
 * abe0 = HANDS_BOTH); loads and enables its own collision; mTimer2 = 100; enters
 * the dormant state (data_ov066_0211b09c).
 * Hand: the rest and spawn positions are set from the position, then the rest X
 * moves 0x31f000 (799.0) toward -X (hand 1) or +X (hand 2) and the rest Z
 * 0x32000 (50.0) toward -Z; the matching collision file is loaded;
 * mHitPoints = 3; data_ov066_0211ae00 = 0; it enters the rise state
 * (data_ov066_0211b05c). */
int Eyerok::InitResources()
{
    char *c = (char *)((void *)this);
    Vector3 v;
    Vector3 w;

    mPartIdx = (s32)param1 & 0xFF;
    if (mPartIdx == 0xFF)
        mPartIdx = 0;
    mStarId = (param1 >> 0xC) & 0xF;
    mStarTracked = _ZN8dActor_c9TrackStarEjj(c, mStarId, 2);
    if (mPartIdx > 2)
        mPartIdx = 0;

    switch (mPartIdx) {
    case 0:
        _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel2, _ZN5Model8LoadFileER13SharedFilePtr(data_ov066_0211ae6c), 1, -1);
        _ZN5Model8LoadFileER13SharedFilePtr(data_ov066_0211ae4c);
        _ZN5Model8LoadFileER13SharedFilePtr(data_ov066_0211aeb4);
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov066_0211aebc);
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov066_0211ae9c);
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov066_0211ae3c);
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov066_0211ae2c);
        _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(data_ov066_0211ae5c);
        _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(data_ov066_0211ae84);
        _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(data_ov066_0211aea4);
        _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(data_ov066_0211ae8c);
        _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(data_ov066_0211ae54);
        _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(data_ov066_0211ae94);
        _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(data_ov066_0211ae64);
        _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(data_ov066_0211ae44);
        _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(data_ov066_0211ae74);
        _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(data_ov066_0211ae7c);
        _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov066_0211ae24);
        _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov066_0211aeac);
        _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov066_0211ae14);
        _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov066_0211ae1c);
        _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov066_0211ae34);
        break;
    case 1:
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mBlendModelAnim, (void *)data_ov066_0211ae4c[1], 1, -1) == 0)
            return 0;
        TextureSequence::Prepare(*(BMD_File *)data_ov066_0211ae4c[1], *(BTP_File *)data_ov066_0211aebc[1]);
        TextureSequence::Prepare(*(BMD_File *)data_ov066_0211ae4c[1], *(BTP_File *)data_ov066_0211ae9c[1]);
        break;
    case 2:
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mBlendModelAnim, (void *)data_ov066_0211aeb4[1], 1, -1) == 0)
            return 0;
        TextureSequence::Prepare(*(BMD_File *)data_ov066_0211aeb4[1], *(BTP_File *)data_ov066_0211ae3c[1]);
        TextureSequence::Prepare(*(BMD_File *)data_ov066_0211aeb4[1], *(BTP_File *)data_ov066_0211ae2c[1]);
        break;
    }

    if (mPartIdx != PART_MAIN) {
        mShadowModel.InitCylinder();
        w.x = data_ov066_0211ad18[0];
        w.y = data_ov066_0211ad18[1];
        w.z = data_ov066_0211ad18[2];
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, c, &w, 0x64000, 0x64000, 0x200002, 0);
    }

    {
        /* NOT mDustPos[i]: the ROM walks a running char* and re-derives the
           three stores from it. Spelling this as `Vector3 *p = mDustPos; p->x
           = 0; ... p += 1;` costs the function its size -- measured. */
        int i = 0;
        char *p = c;
        do {
            *(s32 *)(p + 0x4DC) = 0;
            *(s32 *)(p + 0x4E0) = 0;
            i += 1;
            *(s32 *)(p + 0x4E4) = 0;
            p += 0xC;
        } while (i < 0x14);
    }

    mTerminalVelocity = -0x64000;
    mHandUniqueID1 = 0;
    mHandUniqueID2 = 0;

    if (mPartIdx == PART_MAIN) {
        dActor_c *r;
        mPosZ -= 0x7C000;
        mRestPosX = mPosX;
        mRestPosY = mPosY;
        mRestPosZ = mPosZ;
        v.x = mPosX;
        v.y = mPosY;
        v.z = mPosZ;
        v.x += 0x193000;
        r = (dActor_c *)_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(ACTOR_IWANTE, 1, &v, 0, mAreaId, -1);
        if (r != 0)
            mHandUniqueID1 = r->uniqueID;
        v.x = mPosX;
        v.x -= 0x18C000;
        r = (dActor_c *)_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(ACTOR_IWANTE, 2, &v, 0, mAreaId, -1);
        if (r != 0)
            mHandUniqueID2 = r->uniqueID;
        data_ov066_0211ae10 = 0;
        data_ov066_0211ae08 = 0;
        data_ov066_0211ae0c = 0;
        data_ov066_0211abe4 = 1;
        data_ov066_0211ae04 = PHASE_DORMANT;
        data_ov066_0211abe0 = HANDS_BOTH;
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMeshCollider2, (void *)data_ov066_0211ae24[1], &mClsnMat2, 0x199, mAngleY, &data_ov025_02112ca8);
        func_020393d4(&mMeshCollider2, (void *)&dBgW::UpdatePosWithTransform);
        func_020393c4(&mMeshCollider2, (void *)func_ov066_0211a35c);
        ((dBgW *)&mMeshCollider2)->Enable(this);
        mTimer2 = 0x64;
        func_ov066_02119454(data_ov066_0211b09c);
    } else {
        mRestPosX = mPosX;
        mRestPosY = mPosY;
        mRestPosZ = mPosZ;
        mSpawnPosX = mPosX;
        mSpawnPosY = mPosY;
        mSpawnPosZ = mPosZ;
        if (mPartIdx == PART_HAND_1) {
            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMeshCollider2, (void *)data_ov066_0211ae14[1], &mClsnMat2, 0x199, mAngleY, &data_ov025_02112c08);
            mRestPosX -= 0x31F000;
        } else {
            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMeshCollider2, (void *)data_ov066_0211aeac[1], &mClsnMat2, 0x199, mAngleY, &data_ov025_02112d48);
            mRestPosX += 0x31F000;
        }
        func_020393d4(&mMeshCollider2, (void *)&dBgW::UpdatePosWithTransform);
        func_020393c4(&mMeshCollider2, (void *)func_ov066_0211a35c);
        func_020398fc(&mMeshCollider2);
        mRestPosZ -= 0x32000;
        mHitPoints = 3;
        data_ov066_0211ae00 = 0;
        func_ov066_02119454(data_ov066_0211b05c);
    }
    return 1;
}

// @symbol _ZN6Eyerok16OnAimedAtWithEggEv
/* Slot 29, attributed by the vtable: _ZTV6Eyerok + 4*29 = 0x0211ad64 + 0x74
   = 0x0211ade8, and config/arm9/overlays/ov066/relocs.txt relocates
   0x0211ade8 -> 0x0211a2dc. The body is a constant: 163840 (0x28000). */
int Eyerok::OnAimedAtWithEgg()
{
    return 163840;
}

/* -------------------------------------------------------------------------- */
/* FOLDED ZERO-GAP NEIGHBOURS. These three sit between this unit and the
   module's sinit at 0x0211a418, and they belong to this class: the factory
   installs _ZTV6Eyerok, which is this unit's vtable. */
/* -------------------------------------------------------------------------- */
extern unsigned char data_ov066_0211ae04;
extern unsigned char data_ov066_0211ae08;

// @symbol func_ov066_0211a2e4
extern "C" void func_ov066_0211a2e4(void *a, int b)
{
    volatile int dummy[3];
    (void)dummy;
    void *p;
    if (data_ov066_0211ae04 != 1) return;
    if (b == 0) return;
    p = _ZN8dActor_c13ClosestPlayerEv(a);
    if (p == 0) return;
    if (*(int *)((char *)p + 0x64) < (int)0xff387000) {
        data_ov066_0211ae08++;
    }
}

/* Arg-shifting tail-call veneer. Drops the first argument and forwards the next
   two. long_calls is scoped to this one function: the pooled
   `ldr ip,[pc,#8]; bx ip` absolute tail-call it needs is a file-global
   pragma, and leaving it on for the 59 functions above would change their
   call emission.

   The forwarder's second argument is an int, not a pointer, and the
   forwarder itself is void. Its old one-function sources declared it
   `extern int func_ov066_0211a2e4(void*, void*)` and gave the veneer an
   `int` return it never produced; in C both mismatches were invisible and
   every spelling passed the same register. With the real prototypes in
   scope the body is spelled against them. The emitted code is unchanged:
   the veneer is a `ldr ip,[pc,#8]; bx ip` tail-call, which never writes r0
   itself. */
// @symbol func_ov066_0211a35c
#pragma long_calls on
extern "C" void func_ov066_0211a35c(void *a, void *b, void *c)
{
    func_ov066_0211a2e4(b, (int)c);
}
#pragma long_calls off

extern "C" void *_ZN10dBgActor_cC2Ev(void *thiz);
extern int _ZTV6Eyerok[];
extern "C" void *_ZN14BlendModelAnimC1Ev(void *thiz);
extern "C" void *_ZN5ModelC1Ev(void *thiz);
extern "C" void *_ZN10dCcAcPos_cC1Ev(void *thiz);
extern "C" void *_ZN10dBgW_KcMbgC1Ev(void *thiz);
extern "C" void *_ZN17dExtShadowModel_cC1Ev(void *thiz);
extern "C" void *_ZN15TextureSequenceC1Ev(void *thiz);
extern "C" void __cxa_vec_ctor(void *p, int a, int b, void *f1, void *f2);
extern "C" Vector3 *_ZN7Vector3D1Ev(Vector3 *object);
extern "C" void func_0203d384(void);

// @symbol daIwante_c_classInit
/* Reconstructed source-style name: SM64DS proves daIwante_c through RTTI,
 * allocation size, vtable identity, and the IWANTE registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Eyerok_Spawn. */
extern "C" void *daIwante_c_classInit(void)
{
    unsigned char *c = (unsigned char *)_ZN7fBase_cnwEj(0x874);
    if (c) {
        _ZN10dBgActor_cC2Ev(c);
        *(void **)c = _ZTV6Eyerok;
        _ZN10dCcAcPos_cC1Ev(c + 0x320);
        _ZN14BlendModelAnimC1Ev(c + 0x360);
        _ZN5ModelC1Ev(c + 0x3d0);
        _ZN17dExtShadowModel_cC1Ev(c + 0x420);
        _ZN15TextureSequenceC1Ev(c + 0x448);
        __cxa_vec_ctor(c + 0x4dc, 0x14, 0xc, (void *)func_0203d384,
                       (void *)_ZN7Vector3D1Ev);
        _ZN10dBgW_KcMbgC1Ev(c + 0x674);
    }
    return c;
}
