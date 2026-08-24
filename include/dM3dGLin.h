/* The cartridge's own line segment primitive, named by the ROM's RTTI --
 * _ZTS8dM3dGLin at 0x02099284 -- and listed as a BASE of dBgCh_Lin at offset
 * 56 = 0x38 in that class's __vmi_class_type_info record.
 *
 * 0x18 BYTES, two Vector3, pinned by matched code:
 *
 *   _ZN8dM3dGLin3SetERK7Vector3S2_   start = a; end = b   (both, word stores)
 *   func_ov002_020fea68              a[0..2] = b[0..2]    GetStart
 *   func_ov002_020fea4c              a[0..2] = b[3..5]    GetEnd
 *   func_ov002_020feab8              `bx lr'              the trivial ~dM3dGLin
 *
 * NON-POLYMORPHIC: no vptr anywhere in those bodies, so despite appearing in
 * a polymorphic class's base list it contributes no vtable store to any
 * constructor. Its RTTI lives in arm9 even though those three method bodies
 * are overlay-resident -- the TYPE is not overlay-resident. See
 * notes/collision-system.md.
 *
 * An earlier auto-generated version modelled `start' as pad_000[0xc] and
 * named only end.x/end.y/end.z (unk_00c/unk_010/unk_014); the Set body above
 * writes both vectors, which settles it.
 */
#ifndef DM3DGLIN_H
#define DM3DGLIN_H
#include "types.h"

struct dM3dGLin {
    Vector3 start;          /* 0x00 */
    Vector3 end;            /* 0x0c */

#ifdef __cplusplus
    void Set(const Vector3 &start, const Vector3 &end);
#endif
};

/* In C the tag alone is not a type name; this makes both spellings work. */
typedef struct dM3dGLin dM3dGLin;

typedef char dM3dGLin_size_must_be_0x18[sizeof(dM3dGLin) == 0x18 ? 1 : -1];

#endif
