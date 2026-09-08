#ifndef DAOBJKM2_UKISHIMA_C_H
#define DAOBJKM2_UKISHIMA_C_H

#include "types.h"

/* Bowser in the Fire Sea's floating floor. `ukishima` is a floating island.
 *
 * ROM name daObjKm2_Ukishima_c; the tree called it FloatingFloorBfs, a coined
 * spelling with no cartridge attestation.
 *
 * MEASURED in extracted/overlays/overlay_0045.bin (ov045 base 0x021111a0): the
 * bytes at 0x02112f14 are the NUL-terminated "19daObjKm2_Ukishima_c", and the
 * typeinfo record's middle word at 0x02112f00 holds exactly that address. The
 * name is read off the cartridge, not coined.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daObjUkiyuka_c, which
 * derives from dBgActor_c, and the ROM states this twice over.
 * _ZTI19daObjKm2_Ukishima_c +8 (0x02112f04) holds 0x02109104 =
 * _ZTI14daObjUkiyuka_c, an __si_class_type_info with one base at subobject
 * offset 0; and both destructors store THREE vptrs rather than two --
 * 0x02112f50 (its own), 0x0210912c (daObjUkiyuka_c's), then 0x0210ae38
 * (dBgActor_c's), read out of the literal pools at 0x02111b58..0x02111b60 and
 * 0x02111bb8..0x02111bc0 rather than from relocs.txt, because tools/match.py
 * wildcards every relocated word. A one-level chain emits two.
 *
 *   _ZTI19daObjKm2_Ukishima_c  ov045 0x02112efc
 *   _ZTS19daObjKm2_Ukishima_c  ov045 0x02112f14
 *   _ZTV19daObjKm2_Ukishima_c  ov045 0x02112f50  (storage starts at 0x02112f48)
 *   kind                       __si_class_type_info, ONE base, offset 0
 *   base                       daObjUkiyuka_c, ov002 0x02109104
 *
 * 0x02112f50 IS THE ADDRESS POINT, measured: 0x02112f48 holds the
 * offset-to-top word 0 and 0x02112f4c holds 0x02112efc, the address of this
 * class's typeinfo record. The table runs 32 slots, 0x02112f50..0x02112fd0,
 * ending exactly where _ZTI15daObjKm2_Gura_c begins -- the same slot count
 * daObjUkiyuka_c and dBgActor_c carry, so this class adds none.
 *
 * TWO OWN OVERRIDES, and they are the two the abstract base leaves pure:
 * slot 0 (InitResources, 0x02111bdc) and slot 3 (CleanupResources,
 * 0x02111bc8), plus the destructor pair at slots 16 and 17 (0x02111b14,
 * 0x02111b64). Slot 6 holds 0x020b6494 and slot 9 holds 0x020b646c, both in
 * ov002 and both daObjUkiyuka_c's own Behavior and Render, so this class
 * inherits them and must not redeclare them.
 *
 * NO FIELDS OF ITS OWN, and this is the leaf that fixes the base's size:
 * daObjKm2_Ukishima_c_classInit loads r0 with 0x32c = 812 at 0x02111bf8
 * before calling fBase_c::operator new, and daObjUkiyuka_c is 0x32c already.
 * The other leaf is bigger -- daObjFl_Ukiyuka_c passes 816 = 0x330 -- which is
 * what fixes daObjUkiyuka_c's boundary at 0x32c rather than anywhere higher.
 */

#ifdef __cplusplus

#include "daObjUkiyuka_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

struct daObjKm2_Ukishima_c : daObjUkiyuka_c {
    /* --- vtable --- */
    /* INLINE ON PURPOSE, the same spelling the base above and
       include/dBgActor_c.h use for their own: defining the empty body in the
       class body makes mwccarm emit the used D1/D0 pair in the cartridge's
       order and never materialize the otherwise homeless leaf D2. Written out
       of line in the .cpp instead, the pair flips to D0-before-D1 and the
       isolation step rejects the object. */
    virtual ~daObjKm2_Ukishima_c() {}   /* slots 16 (D1), 17 (D0) */

    s32 CleanupResources();             /* slot  3 */
    s32 InitResources();                /* slot  0 */

    /* Overriding both of the base's pure virtuals is what makes this leaf
       concrete, so the factory can spell its allocation as a plain `new`
       rather than a hand-rolled operator-new call and a pair of raw vptr
       stores. The ov043 class daObjKm1_Kurumajiku_c, which has this class's
       exact member-size signature, carries the same pairing in its own
       header. */
    static void *operator new(unsigned long size);
};

inline void *daObjKm2_Ukishima_c::operator new(unsigned long size)
{
    return _ZN7fBase_cnwEj((unsigned)size);
}

typedef char daObjKm2_Ukishima_c_size_must_be_0x32c[
    sizeof(daObjKm2_Ukishima_c) == 0x32c ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJKM2_UKISHIMA_C_H */
