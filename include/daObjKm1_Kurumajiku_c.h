#ifndef DAOBJKM1_KURUMAJIKU_C_H
#define DAOBJKM1_KURUMAJIKU_C_H

#include "types.h"
#include "daObjKurumajiku_c.h"

/* The Bob-omb Battlefield rickshaw axle -- the spindle the cart rides on.
 * ROM name daObjKm1_Kurumajiku_c; the tree called it RickshawBdw, a coined
 * spelling with no cartridge attestation.
 *
 * MEASURED in extracted/overlays/overlay_0043.bin (ov043 base 0x021111a0):
 * the bytes at 0x02112350 are the NUL-terminated "21daObjKm1_Kurumajiku_c",
 * and the typeinfo record's middle word at 0x0211233c points at exactly that
 * address. The name is read off the cartridge, not coined.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjKurumajiku_c, and
 * the ROM states this twice over. _ZTI21daObjKm1_Kurumajiku_c +8 (0x02112340)
 * holds 0x021092f8 = _ZTI17daObjKurumajiku_c, an __si_class_type_info with one
 * base at subobject offset 0; and the destructors store THREE vptrs rather
 * than two -- 0x0211238c (its own), 0x02109320 (daObjKurumajiku_c's), then
 * 0x0210ae38 (dBgActor_c's), read out of the literal pools at 0x02111440 and
 * 0x021114a0. Modelled flat as a direct dBgActor_c subclass the destructor
 * compiles to 0x44 against the ROM's 0x50, exactly one `ldr` + `str` + pool
 * word short.
 *
 *   _ZTI21daObjKm1_Kurumajiku_c  ov043 0x02112338
 *   _ZTS21daObjKm1_Kurumajiku_c  ov043 0x02112350
 *   _ZTV21daObjKm1_Kurumajiku_c  ov043 0x0211238c  (storage starts at 0x02112384)
 *   kind                         __si_class_type_info, ONE base, offset 0
 *   base                         daObjKurumajiku_c, ov002 0x021092f8
 *
 * 0x0211238c IS THE ADDRESS POINT, measured: 0x02112384 holds the
 * offset-to-top word 0 and 0x02112388 holds 0x02112338, the address of this
 * class's typeinfo record. The table runs 32 slots, 0x0211238c..0x0211240c,
 * ending exactly where _ZTI17daObjKm1_Kuruma_c begins -- the same slot count
 * daObjKurumajiku_c and dBgActor_c carry, so this class adds none.
 *
 * TWO OWN OVERRIDES, and they are the two the abstract base leaves pure:
 * slot 0 (InitResources, 0x021114c4) and slot 3 (CleanupResources,
 * 0x021114b0), plus the destructor pair at slots 16 and 17 (0x021113fc,
 * 0x0211144c). Slot 6 holds 0x020b6b38 and slot 9 holds 0x020b6b10, both in
 * ov002 and both daObjKurumajiku_c's own Behavior and Render, so this class
 * inherits them and must not redeclare them.
 *
 * NO FIELDS OF ITS OWN. daObjKm1_Kurumajiku_c_classInit (historical alias
 * RickshawBdw_Spawn) loads r0 with 0x330 before calling fBase_c::operator new,
 * and daObjKurumajiku_c is 0x330 already -- the four mounted-actor IDs at
 * 0x320 are read by the base's own Behavior, which lives in ov002 with the
 * base, so they are the base's. The ov047 twin daObjKm3_Kurumajiku_c passes
 * the same 0x330 and likewise adds nothing.
 */

#ifdef __cplusplus

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

struct daObjKm1_Kurumajiku_c : daObjKurumajiku_c {
    /* --- vtable --- */
    /* INLINE ON PURPOSE, the same spelling include/daObjKm3_Kurumajiku_c.h
       uses for this class's ov047 twin: defining the empty body in the class
       body makes mwccarm emit the used D1/D0 pair in the cartridge's order
       and never materialize the otherwise homeless D2. */
    virtual ~daObjKm1_Kurumajiku_c() {}    /* slots 16 (D1), 17 (D0) */

    int CleanupResources();                /* slot  3 */
    int InitResources();                   /* slot  0 */

    /* Overriding both of the base's pure virtuals is what makes this leaf
       concrete, so the factory can spell its allocation as a plain `new`
       rather than a hand-rolled operator-new call and a pair of raw vptr
       stores. include/daObjKm1_Kuruma_c.h carries the same pairing for the
       cart this axle drives. */
    static void *operator new(unsigned long size);
};

inline void *daObjKm1_Kurumajiku_c::operator new(unsigned long size)
{
    return _ZN7fBase_cnwEj((unsigned)size);
}

typedef char daObjKm1_Kurumajiku_c_size_must_be_0x330[
    sizeof(daObjKm1_Kurumajiku_c) == 0x330 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJKM1_KURUMAJIKU_C_H */
