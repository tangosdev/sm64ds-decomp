#ifndef DAKRB_C_H
#define DAKRB_C_H
#include "types.h"
#include "dCapEnemy_c.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "MaterialChanger.h"

/* The ROM's RTTI names this class daKrb_c and derives it from dCapEnemy_c, not from
 * dEnemyBase_c directly -- the tree used to believe `Goomba : Enemy`, which skipped the
 * whole cap-wearing layer (see notes/rtti-reconciliation.md and the old handoff note
 * this header replaces). rtti_reconcile.py's alias vote (build/rtti_reconcile.json)
 * corroborates dCapEnemy_c's identity with a margin of 2: this class and daTrs_c (once coined Boo)
 * both derive from it per their own destructor chains, so neither alone made the
 * pairing circular.
 *
 *   _ZTI7daKrb_c   ov084 0x021308e0
 *   _ZTS7daKrb_c   ov084 0x021308d4   "7daKrb_c"
 *   vtable         ov084 0x02130948, same address _ZTV7daKrb_c already names --
 *                  symbols.txt carries both today; the class itself is not yet
 *                  renamed daKrb_c, so the compiler still emits _ZTV7daKrb_c.
 *
 * DERIVES FROM dCapEnemy_c. _ZN7daKrb_cD1Ev tears down its own five members and then
 * chains to _ZN11dCapEnemy_cD2Ev (ov002 0x020aedbc), dCapEnemy_c's out-of-line
 * base-object destructor -- see include/daTrs_c.h for why that address carried the
 * placeholder name func_ov002_020aedbc until 2026-08-27. Goomba_Spawn
 * (and GoombaSmall_Spawn, GoombaLarge_Spawn -- all three build the identical class,
 * differing only in spawn param) call _ZN11dCapEnemy_cC2Ev(p) before storing this
 * class's own vtable, the same forward/backward pair CapEnemy's own header documents.
 *
 * SIZE 0x478 (1144 decimal), the literal all three factories pass to
 * fBase_c::operator new.
 *
 * OWN MEMBERS start at 0x180, where dCapEnemy_c ends -- a derived member cannot start
 * inside its base, so this is both floor and ceiling, same argument dCapEnemy_c.h makes
 * one level up. Five spans are typed sub-objects, evidenced by both witnesses:
 * Goomba_Spawn constructs them forward (dCcAc_c, dBgCh_Actr, ModelAnim,
 * ShadowModel, MaterialChanger) and _ZN7daKrb_cD1Ev tears them down in the opposite
 * order at the same offsets, before chaining to the base. They stay untyped, one
 * placeholder byte per span plus the sub-fields already evidenced inside them, rather
 * than named typed members: every existing consumer already reaches them through the
 * free C1/D1 functions on raw offsets (`_ZN7dCcAc_cC1Ev((char*)p + 0x180)`),
 * not through member access, so typing the whole span is a follow-on with no byte-shape
 * payoff yet and its own risk of silently mis-offsetting a nested field.
 *
 * VTABLE. Three slots resolve from the census worklist against dActor_c's own vtable
 * (include/dActor_c.h), all previously blocked because this class was a flat struct
 * with no base to diff against. Two are named here:
 *
 *   slot 18  OnYoshiTryEat()              ov084 0x0212bfc0  -- named, byte-verified
 *   slot 19  OnTurnIntoEgg(Player &)      ov084 0x0212b344  -- STILL func_ov084_0212b344
 *   slot 29  OnAimedAtWithEgg()           ov084 0x0212b30c  -- named, byte-verified
 *
 * SLOT 19 IS LEFT UNNAMED, on purpose. It has a PRE-EXISTING 4-byte gap that has
 * nothing to do with this migration: the untouched recovered free function does not
 * reproduce the ROM bytes either (candidate 0x1d0 vs target 0x1cc, confirmed against
 * the pre-rename original with tools/fdiff.py), and config/arm9/overlays/ov084/delinks.txt
 * never marked it `complete`. Isolated experiment (see the commit this header ships
 * with): the divergence appears verbatim on the ORIGINAL free function merely
 * recompiled with an `int` return instead of `void` -- nothing about turning it into
 * a method. dActor_c.h's `virtual int OnTurnIntoEgg(Player &player)` may simply have
 * the wrong return type -- a mangled name's parameter and return types are this
 * tree's own reconstruction, not something the ROM's symbol table records, and this
 * is a case where that reconstruction is directly falsifiable by the codegen. Naming
 * this slot needs that signature question settled first (on dActor_c, not here, since
 * every other override shares its declaration), so it stays its own follow-on.
 *
 * All other slots hold dCapEnemy_c's (or an ancestor's) word and are inherited.
 */
struct daKrb_c : dCapEnemy_c {
    dCcAc_c mdCcAc_c;         /* 0x180 */
    dBgCh_Actr mWithMeshClsn; /* 0x1b4 */
    ModelAnim mModelAnim;     /* 0x370 */
    ShadowModel mShadowModel; /* 0x3d4 */
    MaterialChanger mMaterialChanger; /* 0x3fc */
    Vector3 mSafePos;       /* 0x410 */
    Vector3 mHomePos;       /* 0x41c */
    Vector3 mStuckCheckPos; /* 0x428 */
    s32 mState;            /* 0x434 */
    s32 unk_438;            /* 0x438 */
    s32 mTargetUniqueID;    /* 0x43c */
    s32 mDistToPlayer;      /* 0x440 */
    s32 unk_444;            /* 0x444 */
    u8  pad_448[0x4];
    s32 mSavedParam;            /* 0x44c */
    s16 mHeadingHoldTimer;  /* 0x450 */
    u8  pad_452[0x2];
    s16 mWanderRerollTimer; /* 0x454 */
    u16 mStuckTimer;            /* 0x456 */
    u16 mTimer458;            /* 0x458 */
    s16 mInitAngleY;            /* 0x45a */
    u8  pad_45c[0x4];
    s32 mGoombaType;            /* 0x460 */
    u8  mRewardType;            /* 0x464 */
    s8  mStarTracked;            /* 0x465 */
    u8  mStarID;            /* 0x466 */
    u8  unk_467;            /* 0x467 */
    u8  mSoundLatchFlags;   /* 0x468 */
    u8  pad_469[0xf];

    virtual ~daKrb_c();

    /* methods */
    int Behavior();
    int CleanupResources();
    /* Declared here so src/_ZN7daKrb_c13InitResourcesEv.cpp can be a real method
       rather than an extern "C" free function under the mangled name. ~daKrb_c is
       still the first virtual DECLARED, so the key function -- and with it
       _ZTV7daKrb_c -- stays where it already was. */
    int InitResources();
    void OnPendingDestroy();                 /* slot 12 -- empty body in the ROM */
    int Render();

    /* --- vtable, resolved from the census worklist against dActor_c --- */
    int OnYoshiTryEat();                        /* slot 18 */
    /* slot 19 (OnTurnIntoEgg) is not declared here -- see the class comment.
       It stays src/func_ov084_0212b344.cpp, an un-migrated free function. */
    int OnAimedAtWithEgg();                     /* slot 29 */
};

typedef char daKrb_c_size_must_be_0x478[sizeof(daKrb_c) == 0x478 ? 1 : -1];

#endif
