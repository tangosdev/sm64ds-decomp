#ifndef DAOTS_C_H
#define DAOTS_C_H

#include "types.h"
#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

/* The shared base of the three Bully variants. The ROM's RTTI names it daOts_c and
 * gives it exactly three children: daDonketu_c (Bully), daBDonketu_c (BigBully) and
 * daIDonketu_c. All three ROM names are the tree's names too now; the coined
 * `ChillBully` this file used to gloss daIDonketu_c with is gone. The production name
 * daOts_c is kept, as #1400 did for
 * dScMgBase_c -- it also makes the compiler's _ZTV7daOts_c agree with the RTTI
 * string for free.
 *
 * FOUR WITNESSES, which is one more than a leaf class ever gets:
 *
 *   _ZN7daOts_cD1Ev (ov064 0x02115ee0, 0x48) -- this class's own destructor. It stores
 *   data_ov064_0211b768 and destroys ShadowModel 0x370, dCcAc_c 0x33c,
 *   dBgCh_Actr 0x174, ModelAnim 0x110, then chains to _ZN12dEnemyBase_cD2Ev. D0 at
 *   0x02115f28 is the same plus Memory::Deallocate.
 *
 *   An out-of-line D1/D0 pair and the inline destructor below are not in conflict:
 *   mwcc inlines a virtual destructor at every direct-call site AND emits one
 *   out-of-line copy for the vtable to point at. The children inline it (no `bl`
 *   anywhere), and slots 16/17 of _ZTV7daOts_c hold these two addresses.
 *
 *   Bully_Spawn, BigBully_Spawn and daIDonketu_c_Spawn are the SAME function apart
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
 * 0x174, dBgCh_Actr 0x1bc ends at 0x330, mFileTable and its padding end at 0x33c,
 * dCcAc_c 0x34 ends at 0x370, ShadowModel 0x28 ends at 0x398. Nothing
 * allocates a plain daOts_c, so the ceiling comes from the children instead: the
 * earliest own field any of the three declares is at 0x3fa, and 0x398..0x3f9 is
 * referenced by no child source at all -- so it stays the children's padding rather
 * than being annexed into the base on no evidence.
 */
/* THE VTABLE, all 37 slots diffed against dEnemyBase_c's and against all three children's:
 *
 *   slot 0  InitResources    LITERAL ZERO here -- pure virtual. All three children
 *   slot 6  Behavior         override both, which is why they must.
 *   slot 3  CleanupResources 0x02116ca0 in ALL FOUR tables, so it is this class's.
 *   slot 9  Render           0x02116cf0 here, in Bully and in daIDonketu_c; BigBully
 *                            is the only one that overrides it.
 *   slot 29 OnAimedAtWithEgg 0x02115f84 in all four, so also this class's.
 *   slot 16/17               the destructor pair, one per class.
 *
 * THE NAMING DEFECT THIS NOTE USED TO RECORD IS NOW FIXED. The tree called 0x02116ca0
 * and 0x02116cf0 `_ZN5Bully16CleanupResourcesEv` and `_ZN5Bully6RenderEv`; the table
 * above says they are daOts_c's, because Bully does not override either -- it inherits
 * them. Both are renamed, and so are the three placeholders: 0x02115f84 (which carried
 * a `daBDonketu_c::OnAimedAtWithEgg -- recovered from vtable slot identity` comment,
 * naming a BASE's method after a CHILD) and the destructor pair.
 *
 * The blast radius turned out to be five files and five symbol lines: no source
 * anywhere referenced any of the five by name, which is what makes a misattributed
 * ROM symbol so easy to leave in place and so cheap to correct.
 *
 * The two pure-virtual slots ARE declared `= 0` below, and the note that used to say
 * they deliberately were not rested on a premise that has since stopped being true.
 * It argued this class's vtable is never emitted, so nothing could disagree with the
 * cartridge. That held while the inline destructor left the class without a key
 * function -- but `virtual int CleanupResources();` is declared non-inline here and
 * DEFINED in src/_ZN7daOts_c16CleanupResourcesEv.cpp, which makes it the key function
 * and makes that translation unit emit _ZTV7daOts_c. Measured: the emitted table
 * carried dEnemyBase_c's concrete InitResources and Behavior in slots 0 and 6 where
 * the cartridge has zeros, eight bytes wrong, and no byte gate could see it -- the
 * ROM build compares .text only. The zero words were always the evidence; the
 * declaration is what makes the compiler act on it.
 */
struct daOts_c : dEnemyBase_c {
    ModelAnim           mModelAnim;             /* 0x110 */
    dBgCh_Actr        mWithMeshClsn;          /* 0x174 */
    /* All three children declare a field here, which is what makes it the base's
       rather than any one of theirs. Bully is the only one that says what it is,
       and says it twice: InitResources points it at data_ov064_0211b834, and
       CleanupResources releases five SharedFilePtrs through it. BigBully's own
       header called it a u8; two of the three call it a word, and the use above
       is a pointer, so a word it is. */
    s32                 mFileTable;             /* 0x330 */
    u8                  pad_334[0x8];
    dCcAc_c  mdCcAc_c;    /* 0x33c */
    ShadowModel         mShadowModel;           /* 0x370 */

    /* INLINE, AND IT MUST BE. Both children's destructors store their own vtable,
       then this one's, then destroy all four members in place -- there is no `bl`
       to a base destructor anywhere. That is what an inline destructor compiles to,
       and an out-of-line declaration here would make each child emit a call the ROM
       does not have. Being inline also leaves this class without a key function, so
       merely including this header does not emit _ZTV7daOts_c. Same reasoning, same
       wording, as include/dBgActor_c.h. */
    virtual ~daOts_c() {}

    /* The three slots this class owns outright, each named by the diff above rather
       than by any one child's source. InitResources and Behavior are the pure-virtual
       pair, declared at the end of the class. */
    virtual int CleanupResources();     /* slot  3 */
    virtual int Render();               /* slot  9 */
    virtual int OnAimedAtWithEgg();     /* slot 29 -- still a C file, see its source */
    /* New slots 31..36. Their positions and override ownership are fixed by the
       four ROM vtables. The spellings describe their call sites; the stripped
       image carries no original method names. */
    virtual int  UpdateRunState();      /* slot 31 -- called only from state 1 */
    virtual void UpdateDeathState();    /* slot 32 -- called only from state 4 */
    virtual void PlayStepSound();       /* slot 33 -- animation frames 4 and 7 */
    virtual void PlayHitSound();        /* slot 34 -- entering the hit state */
    virtual void PlayShellHitSound();   /* slot 35 -- shell/knockback response */
    virtual void PlayDeathSound();      /* slot 36 -- entering state 4 */

    /* THE NULL SLOTS THE NOTE ABOVE ALREADY NAMES, SPELT SO THE COMPILER AGREES.
       mwccarm lays down a bare 0x00000000 with no relocation for a pure virtual --
       there is no __cxa_pure_virtual in this image for it to point at -- so a zero
       word in a ROM vtable IS the `= 0`, and it is the only thing that produces one.
       Left undeclared, this class silently inherits dEnemyBase_c's concrete bodies and
       the vtable it emits disagrees with the cartridge at exactly these slots.
       Measured by tools/romdata_check.py, which is the only gate that reads them:
       the ROM build's 106/106 compares .text alone and is blind here.

       DECLARED LAST, AND WITH `virtual` -- unlike the plain overrides above. The
       pure-specifier is only valid on a declaration carrying the keyword, and a
       pure virtual has no body to emit, so it can never become the key function:
       whichever virtual was first and non-inline before is still first now. */
    virtual int InitResources() = 0;        /* slot  0 */
    virtual int Behavior() = 0;             /* slot  6 */
};

typedef char daOts_c_size_must_be_0x398[sizeof(daOts_c) == 0x398 ? 1 : -1];

#endif /* DAOTS_C_H */
