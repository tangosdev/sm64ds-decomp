#ifndef DAOTS_C_H
#define DAOTS_C_H

#include "types.h"
#include "Enemy.h"
#include "ModelAnim.h"
#include "MovingCylinderClsn.h"
#include "ShadowModel.h"
#include "WithMeshClsn.h"

/* The shared base of the three Bully variants. The ROM's RTTI names it daOts_c and
 * gives it exactly three children: daDonketu_c (Bully), daBDonketu_c (BigBully) and
 * daIDonketu_c (ChillBully). The production name is kept, as #1400 did for
 * dScMgBase_c -- it also makes the compiler's _ZTV7daOts_c agree with the RTTI
 * string for free.
 *
 * FOUR WITNESSES, which is one more than a leaf class ever gets:
 *
 *   func_ov064_02115ee0 (ov064, 0x48) -- this class's own destructor, still unnamed.
 *   It stores data_ov064_0211b768 and destroys ShadowModel 0x370, MovingCylinderClsn
 *   0x33c, WithMeshClsn 0x174, ModelAnim 0x110, then chains to _ZN5EnemyD2Ev.
 *
 *   Bully_Spawn, BigBully_Spawn and ChillBully_Spawn are the SAME function apart
 *   from the allocation size and the last instruction. Each stores TWO vtables --
 *   this class's, from the inlined base constructor, and then its own -- with the
 *   four members constructed between them at those same four offsets.
 *
 * The vtable is data_ov064_0211b768: the word before it is 0x021138bc, which is
 * _ZTI7daOts_c. (Reading that address out of the merged symbol table answers
 * `data_ov009_021138bc`, and the destructor answers `_ZN7ChuckyaD1Ev` -- ov062
 * through ov066 all load their .text at 0x02115ee0. Resolve inside ov064 or the
 * table will lie to you.)
 *
 * SIZE 0x398, and every member closes exactly on the next: ModelAnim 0x64 ends at
 * 0x174, WithMeshClsn 0x1bc ends at 0x330, mFileTable and its padding end at 0x33c,
 * MovingCylinderClsn 0x34 ends at 0x370, ShadowModel 0x28 ends at 0x398. Nothing
 * allocates a plain daOts_c, so the ceiling comes from the children instead: the
 * earliest own field any of the three declares is at 0x3fa, and 0x398..0x3f9 is
 * referenced by no child source at all -- so it stays the children's padding rather
 * than being annexed into the base on no evidence.
 */
/* THE VTABLE, all 31 slots diffed against Enemy's and against all three children's:
 *
 *   slot 0  InitResources    LITERAL ZERO here -- pure virtual. All three children
 *   slot 6  Behavior         override both, which is why they must.
 *   slot 3  CleanupResources 0x02116ca0 in ALL FOUR tables, so it is this class's.
 *   slot 9  Render           0x02116cf0 here, in Bully and in ChillBully; BigBully
 *                            is the only one that overrides it.
 *   slot 29 OnAimedAtWithEgg 0x02115f84 in all four, so also this class's.
 *   slot 16/17               the destructor pair, one per class.
 *
 * NAMING DEFECT, recorded not fixed: the tree calls 0x02116ca0 and 0x02116cf0
 * `_ZN5Bully16CleanupResourcesEv` and `_ZN5Bully6RenderEv`. The table above says they
 * are daOts_c's -- Bully does not override either, it inherits them. Correcting that
 * is a ROM-symbol rename with its own blast radius and nothing here depends on it, so
 * the names stand for now and this note is the record. 0x02115f84 is still
 * func_ov064_02115f84 and is this class's OnAimedAtWithEgg.
 *
 * The two pure-virtual slots are deliberately NOT declared `= 0` below. Nothing in
 * the ROM needs them to be: this class's vtable is never emitted (see the inline
 * destructor note), and declaring them would change what the children emit for no
 * gain. The zero words are the evidence; the declaration would only restate it.
 */
struct daOts_c : Enemy {
    ModelAnim           mModelAnim;             /* 0x110 */
    WithMeshClsn        mWithMeshClsn;          /* 0x174 */
    /* All three children declare a field here, which is what makes it the base's
       rather than any one of theirs. Bully is the only one that says what it is,
       and says it twice: InitResources points it at data_ov064_0211b834, and
       CleanupResources releases five SharedFilePtrs through it. BigBully's own
       header called it a u8; two of the three call it a word, and the use above
       is a pointer, so a word it is. */
    s32                 mFileTable;             /* 0x330 */
    u8                  pad_334[0x8];
    MovingCylinderClsn  mMovingCylinderClsn;    /* 0x33c */
    ShadowModel         mShadowModel;           /* 0x370 */

    /* INLINE, AND IT MUST BE. Both children's destructors store their own vtable,
       then this one's, then destroy all four members in place -- there is no `bl`
       to a base destructor anywhere. That is what an inline destructor compiles to,
       and an out-of-line declaration here would make each child emit a call the ROM
       does not have. Being inline also leaves this class without a key function, so
       merely including this header does not emit _ZTV7daOts_c. Same reasoning, same
       wording, as include/Platform.h. */
    virtual ~daOts_c() {}
};

typedef char daOts_c_size_must_be_0x398[sizeof(daOts_c) == 0x398 ? 1 : -1];

#endif /* DAOTS_C_H */
