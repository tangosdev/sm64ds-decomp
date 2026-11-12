// MTX_RotX33_ @ 0x02052550 (arm9, Thumb): build 3x3 fx32 X-rotation matrix
// from (sin, cos): {1, 0, 0, 0, c, s, 0, -s, c} with 4096 = 1.0. Identified as
// the Nitro SDK's FX_mtx33.c MTX_RotX33_ from pret's FX_mtx33.c, whose body
// is the same instructions.
// The named `one` local + opt_propagation off keeps 1<<12 materialized as
// movs #1; lsls #12 instead of a literal-pool load. Both are needed: dropping
// either (or both, written 1 << 12 or 0x1000) gives 0x20 against the ROM's 0x1c.
#include "math/Matrix.h"

#pragma thumb on
#pragma opt_propagation off

void MTX_RotX33_(Matrix3x3 *m, s32 s, s32 c) {
    s32 one = 1;
    m->m[0] = one << 12;
    m->m[1] = 0;
    m->m[2] = 0;
    m->m[3] = 0;
    m->m[4] = c;
    m->m[5] = s;
    m->m[6] = 0;
    m->m[7] = -s;
    m->m[8] = c;
}
