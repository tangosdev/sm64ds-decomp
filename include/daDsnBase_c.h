#ifndef DADSNBASE_C_H
#define DADSNBASE_C_H

#include "types.h"

/* The abstract base of the crushers: the Thwomp that drops on you and the
 * Grindel that slides. `dsn` is dossun, the Thwomp's Japanese name.
 *
 * A LAYER THE TREE DID NOT HAVE, and the last of the eleven
 * tools/rtti_reconcile.py graded `flattened`. #1521 modelled three and PR 1524
 * seven; this is the one whose shape did not fit either batch.
 *
 *   _ZTI11daDsnBase_c  ov025 0x021137f8
 *   _ZTS11daDsnBase_c  ov025 0x02113804   "11daDsnBase_c"
 *   vtable             ov091 0x021351fc, 32 slots, same count as the base
 *   kind               __si_class_type_info, ONE base, subobject offset 0
 *   base               dBgActor_c, ov002 0x021089ec -- the tree's dBgActor_c
 *
 * SO IT IS PLATFORM-FAMILY AFTER ALL, by base chain: the same dBgActor_c the
 * other ten sit on, not a family of its own. Two things make it read differently.
 *
 * ITS VTABLE IS IN ov091 AND ITS RECORD IN ov025, the same split
 * daObjFallBlock_c and daObjMaruta_c have. The relocations that reach it carry
 * the ambiguous tag `overlays(6,91)`; the config settles it from the other side
 * by marking ov006's copy of 0x021351fc `ambiguous` and leaving ov091's as the
 * definition. Both factories agree with that independently -- Thwomp_Spawn in
 * ov091 and Grindel_Spawn in ov025 both store 0x021351fc.
 *
 * AND IT IS THE ONLY ONE OF THE ELEVEN WITH SUB-OBJECTS OF ITS OWN. The other ten
 * add scalars or nothing; this one adds a TextureSequence and a ShadowModel, both
 * with destructors, which is why every leaf's destructor is 0x60 rather than 0x50
 * and why the empty body has to reproduce five calls rather than three.
 *
 * ABSTRACT IN TWO SLOTS. 0 (InitResources) and 6 (Behavior) are null and each
 * leaf supplies them. This class supplies 3 (CleanupResources), 9 (Render), 16
 * (D1) and 17 (D0).
 *
 * TWO DESCENDANTS: daDsn_c (Thwomp, ov091) and daDkk_c (ov025, whose factory the
 * tree calls Grindel_Spawn and whose class it has never named).
 *
 * SLOTS 3 AND 9 BELONG TO THIS CLASS, NOT TO Thwomp, and they are declared here
 * now. Dumping the cartridge's own _ZTV11daDsnBase_c settles it: slot 3 holds
 * ov091 0x021331b8 and slot 9 holds 0x02133210, and Thwomp's vtable inherits both
 * words rather than overriding them. The ROM spelled the two symbols
 * `_ZN6Thwomp16CleanupResourcesEv` and `_ZN6Thwomp6RenderEv`, which is why they
 * sat on Thwomp until now; this pass does the config change that header deferred
 * and renames them onto the class whose vtable they are in. Same shape as the
 * crossed ov047 "Bs" names #1521 recorded.
 *
 * THREE MEMBERS, and the two with destructors are placed twice over -- once by
 * each factory constructing them, once by this class's own destructor destroying
 * them, at the same offsets:
 *
 *   0x320  the file table. This class's own CleanupResources loads the pointer
 *          from here and releases entries [0], [1] and, when it is non-null, [3];
 *          its own Render reads entry [3] to decide whether to run the texture
 *          animation. Both are slot words of THIS class.
 *   0x324  TextureSequence. Thwomp_Spawn and Grindel_Spawn both call
 *          _ZN15TextureSequenceC1Ev at +0x324 while the vptr is this class's, and
 *          both destructors call its D1 there. sizeof is 0x14, so it closes on
 *          0x338.
 *   0x338  ShadowModel, same two-sided evidence, _ZN11ShadowModelC1Ev at +0x338.
 *          sizeof is 0x28, so it closes on 0x360.
 *
 * SIZE 0x360, WHICH IS THE MINIMAL READING AND SAID AS SUCH. The ShadowModel
 * closes there and nothing this class owns reaches higher. The leaves allocate
 * more -- Grindel_Spawn passes 928 = 0x3a0 and Thwomp_Spawn 932 = 0x3a4 -- and
 * because the two disagree, no literal fixes this class's end the way
 * daObjKm2_Ukishima_c fixes daObjUkiyuka_c's. 0x360..0x3a0 exists in both leaves and
 * is declared on them; if a later pass shows both reading the same offsets in it,
 * it moves up here.
 *
 * Field NAMES are coined from what the code does; nothing in the ROM names them.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "TextureSequence.h"
#include "ShadowModel.h"

struct daDsnBase_c : dBgActor_c {
    /* Field NAMES are placeholders. Offsets, widths and types are observed. */
    s32 mFileTable;                     /* 0x320 - a void** in practice */
    TextureSequence mTextureSequence;   /* 0x324 */
    ShadowModel mShadowModel;           /* 0x338 */

    /* --- vtable --- */
    /* INLINE ON PURPOSE, for the reason include/dBgActor_c.h gives for its own:
       every descendant's destructor inlines this body rather than calling
       _ZN11daDsnBase_cD1Ev (which does exist out of line, at ov091 0x02132d6c,
       still under its func_ov091_ name). An out-of-line declaration here would
       make each descendant emit a `bl` the ROM does not have.

       EMPTY, BUT NOT INERT. The two members above have destructors, so this body
       emits _ZN11ShadowModelD1Ev at +0x338 and _ZN15TextureSequenceD1Ev at +0x324
       in reverse declaration order -- which is the order the ROM uses. */
    virtual ~daDsnBase_c() {}

    /* ABSTRACT IN TWO SLOTS. The cartridge holds a bare 0x00000000 in slots 0 and
       6 of _ZTV11daDsnBase_c, with no relocation reaching either word -- which is
       how a `= 0` reads once the linker is done. Leaving them undeclared is not
       neutral: mwcc then inherits fBase_c's own bodies into both, which is what
       made this vtable and daDkk_c's disagree with the ROM. Each leaf supplies
       both for real (Thwomp.h, daDkk_c.h). */
    virtual s32 InitResources() = 0;   /* slot 0 -- null in the ROM */
    virtual s32 Behavior()      = 0;   /* slot 6 -- null in the ROM */

    /* AND IT OWNS SLOTS 3 AND 9, whatever the ROM spelled them. Declaring the two
       here is what puts ov091's own words in those slots instead of fBase_c's --
       in this vtable and, by inheritance, in daDkk_c's and Thwomp's.

       CleanupResources is now this class's ABI key function: it is the first
       declared virtual that is neither inline nor pure, so _ZTV11daDsnBase_c is
       emitted by the TU that defines it rather than weakly by all six TUs that
       merely mention the class. */
    virtual s32 CleanupResources();    /* slot 3 -- ov091:0x021331b8 */
    virtual s32 Render();              /* slot 9 -- ov091:0x02133210 */
};

typedef char daDsnBase_c_size_must_be_0x360[sizeof(daDsnBase_c) == 0x360 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DADSNBASE_C_H */
