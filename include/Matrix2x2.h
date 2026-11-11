#ifndef SM64DS_MATRIX2X2_H
#define SM64DS_MATRIX2X2_H

#include "types.h"

/* The 2x2 fixed-point matrix the affine BG and OAM paths take, 0x10 bytes.
 *
 * The NAME is the ROM's in the only sense available: it is already spelled into
 * the mangled symbols this tree commits to, e.g.
 * _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2. include/nitro/gx/g2.h and include/OAM.h
 * forward-declare it for their pointer parameters; this is the definition they
 * were missing, which five translation units each wrote out instead -- three as
 * `int m[4]`, one as `s32 m00, m01, m10, m11` and one as `int _00, _01, _10,
 * _11`, all 0x10 and all the same four words.
 *
 * The row-major field order is not a guess. G2x_SetBGyAffine_'s own body reads
 * the array spelling as `m[0] * dx + m[1] * dy` for x and `m[2] * dx + m[3] * dy`
 * for y, so indices 0..3 are exactly m00, m01, m10, m11. The named spelling is
 * kept over the array because it says that, and nothing indexes the matrix with
 * a variable. */
struct Matrix2x2 {
    s32 m00; /* 0x0 */
    s32 m01; /* 0x4 */
    s32 m10; /* 0x8 */
    s32 m11; /* 0xc */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char Matrix2x2_size_must_be_0x10[sizeof(struct Matrix2x2) == 0x10 ? 1 : -1];
#endif

#endif /* SM64DS_MATRIX2X2_H */
