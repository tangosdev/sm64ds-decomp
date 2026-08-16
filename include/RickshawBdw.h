#ifndef RICKSHAWBDW_H
#define RICKSHAWBDW_H

#include "types.h"
#include "daObjKurumajiku_c.h"

/* The Bob-omb Battlefield rickshaw axle. ROM name daObjKm1_Kurumajiku_c.
 *
 * IT DOES NOT DERIVE FROM Platform. It derives from daObjKurumajiku_c, which
 * derives from Platform, and the difference is visible in the bytes rather than
 * only in the RTTI: _ZN11RickshawBdwD1Ev stores THREE vptrs -- its own,
 * daObjKurumajiku_c's, then Platform's. Modelled as a direct Platform subclass
 * the destructor compiles to 0x44 against the ROM's 0x50, twelve bytes short,
 * exactly one `ldr` + `str` + pool word.
 *
 *   _ZTI21daObjKm1_Kurumajiku_c  ov043 0x02112338
 *   _ZTS21daObjKm1_Kurumajiku_c  ov043 0x02112350
 *   _ZTV11RickshawBdw            ov043 0x0211238c  (its record sits at V-4)
 *   kind                         __si_class_type_info, ONE base, offset 0
 *   base                         daObjKurumajiku_c, ov002 0x021092f8
 *
 * NO FIELDS OF ITS OWN. RickshawBdw_Spawn passes 816 = 0x330 to
 * ActorBase::operator new, and daObjKurumajiku_c is 0x330 already -- the four
 * mounted-actor IDs at 0x320 are read by the base's own Behavior, so they are
 * the base's. This class overrides slots 0 and 3, which the base leaves null;
 * their bodies are still ov043 func_ names (0x021114c4, 0x021114b0).
 */

#ifdef __cplusplus

struct RickshawBdw : daObjKurumajiku_c {
    /* --- vtable --- */
    virtual ~RickshawBdw();            /* slots 16 (D1), 17 (D0) */
};

typedef char RickshawBdw_size_must_be_0x330[sizeof(RickshawBdw) == 0x330 ? 1 : -1];

#endif /* __cplusplus */

#endif /* RICKSHAWBDW_H */
