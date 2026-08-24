#ifndef DM3DGSPH_H
#define DM3DGSPH_H

#include "types.h"

/* The cartridge's own sphere primitive: a centre and a radius, with a vtable.
 * Named by the ROM's RTTI -- _ZTS8dM3dGSph at 0x020992ec, _ZTI8dM3dGSph at
 * 0x020992e4, vtable at 0x020994cc, named _ZTV8dM3dGSph in symbols.txt so
 * objisolate can rebind vptr stores against it (the Model-family arrangement;
 * the addend loses 8 per the standing vtable-preamble rule).
 *
 * 0x14 BYTES, every offset pinned by a one-line matched function:
 *
 *   _ZN8dM3dGSphC1Ev   p[0] = _ZTV8dM3dGSph   the vptr at 0x00
 *   func_0203ac00   d[1..3] = s[0..2]         the centre  at 0x04
 *   func_0203abcc   p[4] = v                  the radius  at 0x10
 *   func_0203abd4   calls both of the above   -- SetCentreAndRadius
 *   func_0203ac50 / func_0203ac2c              D1 / D0
 *   _ZN8dM3dGSphD2Ev @0x0203ac1c               D2, already named
 *
 * WHERE IT APPEARS. Twice, in two different roles:
 *
 *   - as a BASE of dBgCh_SphCrr at offset 0x38, per that class's
 *     __vmi_class_type_info record. That is the query sphere itself, and it is
 *     why dBgCh_SphCrr carries a third vptr.
 *   - as a MEMBER of dBgCh_Lin at 0x64, which is the segment's bounding sphere:
 *     SetObjAndLine fills it with the midpoint and (length >> 1) + 0x1000, and
 *     dBgCh_Lin::DetectClsn culls colliders against it before walking.
 *
 * PROMOTED to a real polymorphic class (Phase 2c of notes/collision-system.md):
 * under C++ the vptr is placed implicitly by the virtual declaration, and the
 * constructor is DECLARED here and DEFINED in src/_ZN8dM3dGSphC1Ev.cpp as real
 * C++ -- its ROM body is exactly the one synthesized vptr store. The
 * destructor stays DECLARED AND NEVER DEFINED AS A METHOD -- the key-function
 * arrangement from include/ModelBase.h / include/dBgCh.h, keeping this header
 * from making any TU emit a second copy of the vtable.
 *
 * The C branch keeps an explicit vptr word because check_header_offsets.py
 * reads these structs without modelling the implicit one; the size assert
 * covers the C++ branch instead.
 */

#ifdef __cplusplus

struct dM3dGSph {
    /* --- members --- */
    Vector3 centre;         /* 0x04 */
    Fix12i radius;          /* 0x10 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    /* DECLARED FIRST AND NEVER DEFINED AS A METHOD -- slots 0 (D1) and 1
     * (D0). First virtual declaration places the vptr implicitly at 0x00. */
    virtual ~dM3dGSph();

    /* DECLARED, never defined in this header; the definition lives in
     * src/_ZN8dM3dGSphC1Ev.cpp. */
    dM3dGSph();
};

typedef char dM3dGSph_size_must_be_0x14[sizeof(dM3dGSph) == 0x14 ? 1 : -1];

#else

struct dM3dGSph {
    void **vtable;          /* 0x00 */
    Vector3 centre;         /* 0x04 */
    Fix12i radius;          /* 0x10 */
};

#endif /* __cplusplus */

#endif /* DM3DGSPH_H */
