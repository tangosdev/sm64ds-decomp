#ifndef FLOATINGFLOORLLLSMALL_H
#define FLOATINGFLOORLLLSMALL_H

#include "types.h"

/* Lethal Lava Land's floating floor -- both sizes of it.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjUkiyuka_c, which derives from
 * dBgActor_c, and the difference is in the bytes rather than only in the RTTI: its
 * destructor stores THREE vptrs -- its own, daObjUkiyuka_c's, then dBgActor_c's.
 * A one-level chain emits two.
 *
 *   _ZTI17daObjFl_Ukiyuka_c  ov022 0x021140b4
 *   _ZTS17daObjFl_Ukiyuka_c  ov022 0x021140c0
 *   _ZTV21FloatingFloorLllSmall  ov022 0x0211412c  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjUkiyuka_c, ov002 0x02109104
 *
 * ONE FIELD OF ITS OWN, at 0x32c, and both of this class's own methods read it.
 * InitResources sets it from the actor id -- 0 for id 0x4b, 1 for id 0x4c -- and
 * then indexes a 0xc-stride table in ov022 with it; CleanupResources indexes the
 * same table with the same byte. So the two factories the tree gives separate
 * names, daObjFl_Ukiyuka_c_classInit_FL_UKIYUKA_L and daObjFl_Ukiyuka_c_classInit_FL_UKIYUKA, build ONE
 * class: both pass 816 = 0x330 and both store this vtable. 0x32c + 1 rounds to
 * 0x330, which closes the class on that literal, and daObjUkiyuka_c ends at 0x32c.
 *
 * SM64DS RTTI names the implementation daObjFl_Ukiyuka_c. The reconstructed factory
 * daObjFl_Ukiyuka_c_classInit_FL_UKIYUKA_L (historical alias FloatingFloorLllSmall_Spawn) installs this class's
 * cartridge vtable; the reconstructed profile global g_profile_FL_UKIYUKA_L
 * (historical alias FloatingFloorLllSmall_SpawnInfo) is its registry descriptor.
 */

#ifdef __cplusplus

#include "daObjUkiyuka_c.h"

struct FloatingFloorLllSmall : daObjUkiyuka_c {
    /* Field NAME is a placeholder; the offset and the width are observed. */
    u8  mVariant;           /* 0x32c */
    /* --- vtable --- */
    virtual ~FloatingFloorLllSmall(); /* slots 16 (D1), 17 (D0) */

    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */
};

typedef char FloatingFloorLllSmall_size_must_be_0x330[sizeof(FloatingFloorLllSmall) == 0x330 ? 1 : -1];

#endif /* __cplusplus */

#endif /* FLOATINGFLOORLLLSMALL_H */
