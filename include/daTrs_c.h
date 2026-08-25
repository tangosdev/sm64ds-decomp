/* Derives from dCapEnemy_c (on main; the ROM's own RTTI names it daTrs_c and gives
 * dCapEnemy_c two children, daKrb_c/Goomba-as-was and daTrs_c/Boo-as-was -- the unmerged
 * cpp/goomba-family branch renames dCapEnemy_c -> dCapEnemy_c and will sweep this
 * file's spelling along with it when it merges, or in a trivial follow-up).
 *
 * rtti_reconcile came back 'no_belief' on this edge (base not auto-inferred),
 * so it is established by hand, the same way as its sibling daKrb_c:
 *
 *   - Backward: daTrs_c's own destructor (_ZN7daTrs_cD1Ev, src/actors/daTrs_c/_ZN7daTrs_cD1Ev.cpp)
 *     tears down its own six members, then calls func_ov002_020aedbc -- the
 *     exact function _ZN7daKrb_cD1Ev (src/_ZN7daKrb_cD1Ev.c) calls after tearing
 *     down its own members. func_ov002_020aedbc sits at 0x020aedbc and is
 *     immediately followed, at 0x020aedf4, by the already-matched
 *     _ZN11dCapEnemy_cD0Ev -- so 0x020aedbc is dCapEnemy_c's own out-of-line D1, and
 *     both sibling destructors chain to it.
 *   - Forward: Boo_Spawn (src/actors/daTrs_c/Boo_Spawn.cpp) calls
 *     _ZN11dCapEnemy_cC2Ev(t) before storing _ZTV7daTrs_c -- exactly Goomba_Spawn's
 *     shape, which calls the same _ZN11dCapEnemy_cC2Ev before storing _ZTV7daKrb_c.
 *
 * Not renaming func_ov002_020aedbc here -- that is dCapEnemy_c's own file and
 * belongs to the goomba-family rename.
 *
 * SIZE 0x5e0, the literal Boo_Spawn passes to fBase_c::operator new
 * (src/actors/daTrs_c/Boo_Spawn.cpp: `_ZN7fBase_cnwEj(0x5e0)`). dCapEnemy_c ends at
 * 0x180 (include/dCapEnemy_c.h); everything from there down is daTrs_c's own.
 *
 * The six members close exactly on one another -- evidenced twice, in the
 * same offsets and order in both Boo_Spawn's construction and
 * _ZN7daTrs_cD1Ev / _ZN7daTrs_cD0Ev's teardown (reverse order):
 *
 *     0x184 dCcAcPos_c   0x40  -> 0x1c4
 *     0x1c4 dBgCh_Actr                0x1bc -> 0x380
 *     0x380 ModelAnim                   0x64  -> 0x3e4
 *     0x3e4 Model                       0x50  -> 0x434
 *     0x434 ShadowModel (1st)           0x28  -> 0x45c
 *     0x45c ShadowModel (2nd)           0x28  -> 0x484
 *
 * Field NAMES beyond the typed members are placeholders; nothing in this
 * class's own matched functions names them yet. Slots 18 (OnYoshiTryEat) and
 * 29 (OnAimedAtWithEgg) are this class's own overrides -- see
 * include/dActor_c.h for the slot table.
 *
 * No daTrs_c() constructor is declared: Boo_Spawn builds the object field-by-field
 * (calling dCapEnemy_c::dCapEnemy_c, storing the vtable, then each member's own
 * constructor) rather than through a daTrs_c::daTrs_c(), so declaring one risks an
 * implicit body the compiler would inline somewhere the ROM does not.
 *
 * ~daTrs_c() is declared but not defined in-class -- the destructor's actual
 * bodies are the existing hand-written _ZN7daTrs_cD1Ev / _ZN7daTrs_cD0Ev, kept as
 * manual `extern "C"` definitions rather than compiler-synthesized ones so
 * the base-chain call keeps spelling func_ov002_020aedbc. `_ZN11dCapEnemy_cD2Ev`
 * now exists (0x0200651c, named by the goomba-family rename) but it is a
 * DIFFERENT function: the base-chain target here is 0x020aedbc, dCapEnemy_c's
 * own out-of-line D1, which is still unnamed. A compiler-synthesized destructor
 * would emit a call to the former, not the latter.
 */
#ifndef DATRS_C_H
#define DATRS_C_H
#include "types.h"

#include "dCapEnemy_c.h"
#include "Model.h"
#include "ModelAnim.h"
#include "dCcAcPos_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

/* named (not anonymous inline) so check_header_offsets can parse the member
   declaration; layout identical -- four low bits of one u16. */
struct BooFlags16 { u16 b0:1, b1:1, b2:1, b3:1; };
typedef char BooFlags16_size_must_be_0x2[sizeof(BooFlags16) == 0x2 ? 1 : -1];

struct daTrs_c : dCapEnemy_c {
    u8  pad_180[0x4];
    dCcAcPos_c mdCcAcPos_c;  /* 0x184 */
    dBgCh_Actr               mWithMeshClsn;               /* 0x1c4 */
    ModelAnim                  mModelAnim;                  /* 0x380 */
    /* Not "mModel": dCapEnemy_c's own Model at 0x114 is inherited, and calling
       this one mModel shadows it -- which is exactly what the three bodies below
       were relying on. Renaming it to mModel_3e4 without touching them moved
       Render and the destructor pair onto 0x114 and cost three functions
       (ov063 daTrs_c D1/D0/Render), so the shadow was load-bearing, not cosmetic.
       This is the Boo's own body model: Render draws it at unk_510 and both
       destructors run Model::D1 over it. */
    Model                      mBodyModel;                  /* 0x3e4 */
    ShadowModel                mShadowModel1;               /* 0x434 */
    ShadowModel                mShadowModel2;               /* 0x45c */
    u8  pad_484[0x8c];
    u8  unk_510;            /* 0x510 */
    u8  pad_511[0x87];
    u8  unk_598;            /* 0x598 */
    u8  pad_599[0x2f];
    u8  unk_5c8;            /* 0x5c8 */
    u8  pad_5c9[0x3];
    u8  unk_5cc;            /* 0x5cc */
    u8  pad_5cd[0x2];
    u8  unk_5cf;            /* 0x5cf */
    u8  pad_5d0[0x4];
    /* 0x5d4 -- the flags halfword InitResources spells FLAGS16; Render reads
       bits 1 and 3. Typed (not a u8 placeholder) so member access compiles to
       the ROM's add+ldrh instead of a literal-pool address load. */
    BooFlags16 mFlags_5d4;  /* 0x5d4 */
    u8  pad_5d6[0xa];

    virtual ~daTrs_c();

    /* --- vtable, own overrides --- */
    virtual int  OnYoshiTryEat();      /* slot 18 */
    virtual int  OnAimedAtWithEgg();   /* slot 29 */

    int  Behavior();           /* slot 6, fBase_c */
    int  CleanupResources();
    int  InitResources();
    void OnPendingDestroy();
    int  Render();
};

typedef char daTrs_c_size_must_be_0x5e0[sizeof(daTrs_c) == 0x5e0 ? 1 : -1];

#endif
