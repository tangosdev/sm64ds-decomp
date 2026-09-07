#ifndef DAOBJKI_ITA_C_H
#define DAOBJKI_ITA_C_H

#include "types.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* Jolly Roger Bay's floating plank. `ita` is a board.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjFloatBoard_c, which derives from
 * dBgActor_c, and the difference is in the bytes rather than only in the RTTI: its
 * destructor stores THREE vptrs -- its own, daObjFloatBoard_c's, then dBgActor_c's.
 * A one-level chain emits two.
 *
 *   _ZTI13daObjKi_Ita_c  ov016 0x02114b80
 *   _ZTS13daObjKi_Ita_c  ov016 0x02114b98
 *   _ZTV13daObjKi_Ita_c  ov016 0x02114bcc  (public address point)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daObjFloatBoard_c, ov002 0x02108fb4
 *
 * NO FIELDS OF ITS OWN: daObjKi_Ita_c_classInit passes 840 = 0x348, which
 * daObjFloatBoard_c fills. It overrides slot 0 only -- the base supplies slot 3,
 * so this class has no CleanupResources.
 */

#ifdef __cplusplus

#include "daObjFloatBoard_c.h"

struct daObjKi_Ita_c : daObjFloatBoard_c {
    int InitResources();               /* slot  0 */

    static void *operator new(unsigned long size);

    /* Declared last and inline so class instantiation can emit the retail
       D1/D0 pair in cartridge order without a separate D2 body. */
    virtual ~daObjKi_Ita_c() {}
};

inline void *daObjKi_Ita_c::operator new(unsigned long size)
{
    return _ZN7fBase_cnwEj((unsigned)size);
}

typedef char daObjKi_Ita_c_size_must_be_0x348[sizeof(daObjKi_Ita_c) == 0x348 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJKI_ITA_C_H */
