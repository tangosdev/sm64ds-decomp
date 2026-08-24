#ifndef DM3DGSPH_H
#define DM3DGSPH_H

#include "types.h"

/* The cartridge's own sphere primitive: a centre and a radius, with a vtable.
 * Named by the ROM's RTTI -- _ZTS8dM3dGSph at 0x020992ec, _ZTI8dM3dGSph at
 * 0x020992e4, vtable data_020994cc at 0x020994cc.
 *
 * 0x14 BYTES, every offset pinned by a one-line matched function:
 *
 *   _ZN8dM3dGSphC1Ev   p[0] = data_020994cc      the vptr at 0x00
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
 * Modelled here as a plain struct with an explicit vptr word rather than a
 * polymorphic C++ class, so tools/check_header_offsets.py can still read the
 * offsets -- it skips polymorphic structs because it does not model the
 * implicit vptr. Promoting it (and dBgCh_SphCrr's base list) is Phase 2c of
 * notes/collision-system.md.
 */

struct dM3dGSph {
    void **vtable;          /* 0x00 */
    Vector3 centre;         /* 0x04 */
    Fix12i radius;          /* 0x10 */
};

#endif /* DM3DGSPH_H */
