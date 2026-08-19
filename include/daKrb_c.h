#ifndef DAKRB_C_H
#define DAKRB_C_H
#include "types.h"
#include "dCapEnemy_c.h"

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
 * chains to func_ov002_020aedbc, which IS dCapEnemy_c::~dCapEnemy_c (dCapEnemy_c has no
 * named out-of-line D1 of its own to call by name -- see include/dCapEnemy_c.h). Goomba_Spawn
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
 * Goomba_Spawn constructs them forward (MovingCylinderClsn, WithMeshClsn, ModelAnim,
 * ShadowModel, MaterialChanger) and _ZN7daKrb_cD1Ev tears them down in the opposite
 * order at the same offsets, before chaining to the base. They stay untyped, one
 * placeholder byte per span plus the sub-fields already evidenced inside them, rather
 * than named typed members: every existing consumer already reaches them through the
 * free C1/D1 functions on raw offsets (`_ZN18MovingCylinderClsnC1Ev((char*)p + 0x180)`),
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
    u8  mMovingCylinderClsn;            /* 0x180 -- MovingCylinderClsn, 0x34 bytes */
    u8  pad_181[0x33];
    u8  mWithMeshClsn;            /* 0x1b4 -- WithMeshClsn, 0x1bc bytes */
    u8  pad_1b5[0x1bb];
    u8  mModelAnim;            /* 0x370 -- ModelAnim, 0x64 bytes */
    u8  pad_371[0x4f];
    u8  mAnimation;            /* 0x3c0 -- ModelAnim's Animation base, +0x50 */
    u8  pad_3c1[0xb];
    s32 unk_3cc;            /* 0x3cc -- ModelAnim's own field, +0x5c */
    s32 unk_3d0;            /* 0x3d0 -- ModelAnim's own field, +0x60 */
    u8  mShadowModel;            /* 0x3d4 -- ShadowModel, 0x28 bytes */
    u8  pad_3d5[0x27];
    u8  mMaterialChanger;            /* 0x3fc -- MaterialChanger, 0x14 bytes */
    u8  pad_3fd[0x13];
    s32 unk_410;            /* 0x410 */
    s32 unk_414;            /* 0x414 */
    s32 unk_418;            /* 0x418 */
    s32 unk_41c;            /* 0x41c */
    s32 unk_420;            /* 0x420 */
    s32 unk_424;            /* 0x424 */
    s32 unk_428;            /* 0x428 */
    s32 unk_42c;            /* 0x42c */
    s32 unk_430;            /* 0x430 */
    s32 mState;            /* 0x434 */
    s32 unk_438;            /* 0x438 */
    s32 unk_43c;            /* 0x43c */
    s32 unk_440;            /* 0x440 */
    s32 unk_444;            /* 0x444 */
    u8  pad_448[0x4];
    s32 unk_44c;            /* 0x44c */
    s16 unk_450;            /* 0x450 */
    u8  pad_452[0x2];
    s16 unk_454;            /* 0x454 */
    u16 mStuckTimer;            /* 0x456 */
    u16 unk_458;            /* 0x458 */
    s16 unk_45a;            /* 0x45a */
    u8  pad_45c[0x4];
    s32 mGoombaType;            /* 0x460 */
    u8  unk_464;            /* 0x464 */
    s8  unk_465;            /* 0x465 */
    u8  unk_466;            /* 0x466 */
    u8  unk_467;            /* 0x467 */
    u8  unk_468;            /* 0x468 */
    u8  pad_469[0xf];

    virtual ~daKrb_c();

    /* methods */
    int Behavior();
    int CleanupResources();
    int Render();

    /* --- vtable, resolved from the census worklist against dActor_c --- */
    int OnYoshiTryEat();                        /* slot 18 */
    /* slot 19 (OnTurnIntoEgg) is not declared here -- see the class comment.
       It stays src/func_ov084_0212b344.cpp, an un-migrated free function. */
    int OnAimedAtWithEgg();                     /* slot 29 */
};

typedef char daKrb_c_size_must_be_0x478[sizeof(daKrb_c) == 0x478 ? 1 : -1];

#endif
