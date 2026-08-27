#ifndef DAOBJMARUTA_C_H
#define DAOBJMARUTA_C_H

#include "types.h"
#include "dBgActor_c.h"

/* The abstract base of the rolling logs: the two that roll down a slope in Tall,
 * Tall Mountain and Lethal Lava Land. `maruta` is a log.
 *
 * A LAYER THE TREE DID NOT HAVE. The ROM's RTTI names this class and points it at
 * dBgActor_c; the tree named the base's base for both leaves.
 *
 *   _ZTI13daObjMaruta_c  ov022 0x02114350
 *   _ZTS13daObjMaruta_c  ov022 0x0211435c   "13daObjMaruta_c"
 *   vtable               ov080 0x02128338, 32 slots, same count as the base
 *   kind                 __si_class_type_info, ONE base, subobject offset 0
 *   base                 dBgActor_c, ov002 0x021089ec -- the tree's dBgActor_c
 *
 * THE VTABLE LIVES IN ov080, WITH THE CODE, and the typeinfo record does not --
 * the same split daObjFallBlock_c has. Every one of this class's own slots
 * resolves to an ov080 function. The relocations that reach the vtable carry the
 * ambiguous tag `overlays(79,80)`; the config settles it from the other side by
 * marking ov079's copy of 0x02128338 `kind:bss ambiguous`, which cannot hold a
 * vtable, and leaving ov080's as the definition.
 *
 * ABSTRACT IN THREE SLOTS. 0 (InitResources), 3 (CleanupResources) and 6
 * (Behavior) are all null, so each leaf brings its own rolling logic; this class
 * supplies only 9 (Render), 16 (D1), 17 (D0) and 27 (OnHitByMegaChar).
 *
 * TWO DESCENDANTS: daObjFlMaruta_c (RollingLogLll, ov022) and daObjHmMaruta_c
 * (RollingLogTtm, ov030). Each one's destructor stores this class's vtable between
 * its own and _ZTV10dBgActor_c.
 *
 * NO FIELDS HERE, AND THAT IS A DELIBERATE FLOOR RATHER THAN A FINDING. Its own
 * methods reach nothing above dBgActor_c: Render dispatches through the Model at
 * 0xd4, slot 27 forwards, and its destructor destroys only dBgActor_c's two members.
 * Both factories pass 836 = 0x344, which is 0x24 more than sizeof(dBgActor_c), but
 * nothing this class owns accounts for any of it and its Behavior slot is null, so
 * the span is declared on the leaves, where their own Behavior does read it. That
 * is the weaker of the two possible readings and the only one the evidence
 * supports: if a later pass shows the two leaves reading the same offsets for the
 * same purpose, the span moves up here.
 */

#ifdef __cplusplus

struct daObjMaruta_c : dBgActor_c {
    /* --- vtable --- */
    /* INLINE ON PURPOSE, for the reason include/dBgActor_c.h gives for its own:
       every descendant's destructor inlines this body rather than calling
       _ZN13daObjMaruta_cD1Ev (which does exist out of line, at ov080 0x02127014,
       still under its func_ov080_ name). An out-of-line declaration here would
       make each descendant emit a `bl` the ROM does not have. */
    virtual ~daObjMaruta_c() {}

    /* Slot 27, this class's own override, defined out of line in
       src/game/actors/daObjMaruta_c/_ZN13daObjMaruta_c15OnHitByMegaCharER6Player.cpp. LAYOUT-NEUTRAL: it
       re-uses the slot dActor_c already holds rather than appending one, and adds
       no field, so the 0x320 assert below is untouched.

       Because the destructor above is inline this class has no key function,
       so declaring the first out-of-line virtual makes THIS the key function
       and its translation unit emits _ZTV13daObjMaruta_c, the RTTI records and
       the implicit destructor bodies alongside the one function the file is
       for. That is the same shape every migrated D1 file in this family
       already has, and tools/objisolate.py reduces the object to the declared
       function before eligible.py and rombuild.py judge it -- checked on this
       file, not assumed. */
    void OnHitByMegaChar(Player &player);  /* void, see include/Stump.h */

    /* Slot 9, this class's own override. Not `virtual` -- fBase_c already
       declares it virtual, and re-adding the keyword here is a style
       choice this header hasn't made for its own other declarations. */
    s32 Render();
};

typedef char daObjMaruta_c_size_must_be_0x320[sizeof(daObjMaruta_c) == 0x320 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJMARUTA_C_H */
