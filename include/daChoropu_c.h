#ifndef DACHOROPU_C_H
#define DACHOROPU_C_H
#include "types.h"
#include "dActor_c.h"
#include "dExtAnmModel_c.h"
#include "dCcAc_c.h"

/* TWO WITNESSES:
 *
 *   daChoropu_c_classInit  fBase_c::operator new(396 = 0x18c),
 *       dActor_c::dActor_c(), stores _ZTV11daChoropu_c, then the two members
 *       below in this order.
 *   _ZN11daChoropu_cD0Ev  the same two members destroyed in reverse, then
 *       ~dActor_c.
 *
 * SIZE 0x18c is the factory's own literal; mEmergeSystemID (4 bytes, 0x188) closes
 * exactly on it.
 *
 * Everything below 0x0d0 duplicated dActor_c's own fields under placeholder
 * names -- dActor_c ends at exactly 0x0d0, so unk_008 (dActor_c/fBase_c's
 * own param1 at 0x008) is the only pre-0xd0 field this class's consumers
 * touched; it was repointed to the inherited fBase_c name.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c. daChoropu_c
 * overrides slot 0 (InitResources), slot 3 (CleanupResources), slot 6
 * (Behavior) and slot 9 (Render) -- all still fBase_c's own slots in
 * dActor_c -- plus slot 29 (OnAimedAtWithEgg). Every other slot holds the
 * base's own word and is inherited, so it is deliberately not redeclared
 * here.
 *
 * SM64DS RTTI names the implementation daChoropu_c. The reconstructed
 * factory daChoropu_c_classInit (historical alias
 * MontyMole_Spawn) constructs it for the CHOROPU
 * registry profile.
 */
/* Actor IDs this class names (symbols/actor_debug_names.tsv). */
enum {
    daChoropu_ACTOR_ID           = 0x136,   /* 310 CHOROPU -- this class; used to find the others of its group */
    daChoropu_ACTOR_ONEUPKINOKO  = 0x114    /* 276 ONEUPKINOKO -- spawned on the knock-back that finds the tally (this mole's
                                           mTimesHit plus up to four partners') already at 7 */
};

/* mState indexes the six-row table data_ov080_02128438, which __sinit_ov080_021278c0
 * fills from six pointer-to-member records in .data (rows in this order:
 * data_ov080_02127fa0, 02127f88, 02127fa8, 02127f80, 02127f98, 02127f90). Each record
 * holds the update handler's address and a zero word. The names say what the
 * handler does; they are not labels recovered from the cartridge.
 *
 *   state  handler                 what it does
 *   0 SETUP       func_ov080_02123fcc  collision off, collect the group's partners, go to 1
 *   1 HIDDEN      func_ov080_02123ecc  underground; starts emerging when it has the turn and
 *                                      the Player is 250..1500 units away
 *   2 EMERGE      func_ov080_02123c24  animation 0x2d4, cylinder grows; then picks 3, 4 or 5
 *   3 THROW_ROCK  func_ov080_02123a34  animation 0x2d5; spawns a daChoro_Rock_c on frame 10
 *   4 WAIT        func_ov080_02123924  animation 0x2d3; to 5 when it ends or the Player is close
 *   5 LEAP        func_ov080_02123860  animation 0x2d2; cylinder height curve peaks at 288
 *                                      units, collision off from frame 15; back to 1
 */
enum {
    daChoropu_ST_SETUP      = 0,
    daChoropu_ST_HIDDEN     = 1,
    daChoropu_ST_EMERGE     = 2,
    daChoropu_ST_THROW_ROCK = 3,
    daChoropu_ST_WAIT       = 4,
    daChoropu_ST_LEAP       = 5
};

/* mGroupMode (param1 & 0xf): 0 = works alone, anything else gathers partners; 2 picks
 * its next state at random after emerging (see func_ov080_02123c24). Only 0 and 2 are
 * distinguished by the code in this TU. */
enum {
    daChoropu_GROUP_SOLO   = 0,
    daChoropu_GROUP_RANDOM = 2
};

/* dCcAc_c::flags bit 0 SET = collision disabled (dCc_c::Update bails on it). */
enum {
    daChoropu_CC_DISABLED = 0x1
};

/* hitFlags bits tested here (bit table in include/dCc_c.h). */
enum {
    daChoropu_HIT_MEGA   = 0x10,        /* mega character */
    daChoropu_HIT_PLAYER = 0x400000     /* player */
};

/* dActor_c::mFlags bit 0x10000000. Set when the mole starts to emerge (Hidden) and again on
 * frame 6 of Emerge, cleared whenever it hides (Setup, Leap from frame 15, knocked
 * back). dActor_c.h lists the bit as profile-authored with its consumer not recovered;
 * the name is a guess at its meaning and only the set/clear points are evidence. */
enum {
    daChoropu_FLAG_EMERGED = 0x10000000
};

struct daChoropu_c : dActor_c {
    u8  pad_0d0[0x4];                 /* untouched by any function in d_a_choropu.cpp */
    /* dExtAnmModel_c member, named by _ZN14dExtAnmModel_cD1Ev at +0xd4 -- a relocation the ROM build checks. */
    dExtAnmModel_c mAnmModel; /* 0x0d4 */
    /* dCcAc_c member, named by the class's own destructor calling
       dCcAc_c's D1 at +0x138. [_ZN11daChoropu_cD0Ev, now in src/game/actors/d_a_choropu.cpp] */
    dCcAc_c mdCcAc_c;            /* 0x138 -- its height (+0x8) is rewritten every frame by Emerge and Leap */
    u32 mPartnerIDs[4];     /* 0x16c -- unique IDs of the other daChoropu_c with the same mGroupId (filled by Setup) */
    s32 mState;             /* 0x17c -- row of the state table, daChoropu_ST_* */
    u8  mGroupMode;         /* 0x180 -- param1 & 0xf, see daChoropu_GROUP_* */
    u8  mHasTurn;           /* 0x181 -- 1 while this mole is the one allowed to emerge; passed on by func_ov080_02124360.
                                        A solo mole (mGroupMode 0) starts with 1; otherwise it starts as param1 bit 8 */
    u8  mGroupId;           /* 0x182 -- (param1 >> 4) & 0xf; moles with equal values are partners */
    u8  mNumPartners;       /* 0x183 -- entries used in mPartnerIDs */
    u8  mTimesHit;          /* 0x184 -- bumped each time this mole is knocked underground while the group's total is below 7 */
    u8  pad_185[0x3];
    /* 0x188 -- Particle::System unique ID of effect 0x29, re-passed to System::New on
       each Emerge frame 6..25 and zeroed when Emerge ends */
    u32 mEmergeSystemID;

    virtual ~daChoropu_c();            /* slots 16 (D1), 17 (D0) */

    virtual s32  InitResources();         /* slot  0 */
    virtual s32  CleanupResources();      /* slot  3 */
    virtual s32  Behavior();         /* slot  6 */
    virtual s32  Render();           /* slot  9 */
    virtual s32  OnAimedAtWithEgg();      /* slot 29 */

    /* Non-virtual. The first parameter was this mole, so each is a member.
       The cartridge keeps no English spelling; the ROM address is the name.
       func_ov080_02124418 is not one of these: its first argument is a rock. */
    void func_ov080_02123860();
    void func_ov080_02123924();
    void func_ov080_02123a34();
    void func_ov080_02123c24();
    void func_ov080_02123ecc();
    void func_ov080_02123fcc();
    void func_ov080_02124088();
    void func_ov080_02124208();
    void *func_ov080_02124360();
    void func_ov080_021243d8();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daChoropu_c_size_must_be_0x18c[sizeof(daChoropu_c) == 0x18c ? 1 : -1];
#endif

#endif
