#ifndef DAOBJKM2_GURA_C_H
#define DAOBJKM2_GURA_C_H

#include "types.h"
#include "daObjGuragura_c.h"

/* Bowser in the Fire Sea's tilting slab.
 *
 * The class identity and inheritance are ROM evidence:
 *
 *   _ZTI15daObjKm2_Gura_c  ov045 0x02112fd0
 *   _ZTS15daObjKm2_Gura_c  ov045 0x02112fe8
 *   _ZTV15daObjKm2_Gura_c  ov045 0x02113020
 *   base                   daObjGuragura_c, ov002 0x0210905c
 *
 * The factory allocates 0x350 bytes, exactly sizeof(daObjGuragura_c), so this
 * leaf adds no fields. Historical project class alias: TiltingPlatformBfs.
 */

#ifdef __cplusplus

struct daObjKm2_Gura_c : daObjGuragura_c {
    /* The inline body makes mwccarm emit only the retail D1/D0 pair, in retail
     * order, with this TU's RTTI and vtable. */
    virtual ~daObjKm2_Gura_c() {}          /* slots 16 (D1), 17 (D0) */

    int CleanupResources();                /* slot  3 */
    int InitResources();                   /* slot  0 */
};

typedef char daObjKm2_Gura_c_size_must_be_0x350[
    sizeof(daObjKm2_Gura_c) == 0x350 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJKM2_GURA_C_H */
