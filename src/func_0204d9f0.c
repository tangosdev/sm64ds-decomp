// @symbol func_0204d9f0
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
/* func_0204d9f0 at 0x0204d9f0, size=0x5c
 * LCG RNG: generates 2 random values for x,y (z=0), normalizes the vector.
 */

typedef int s32;
typedef unsigned int u32;




extern void NormalizeVec3(const struct Vector3* src, struct Vector3* dst);

void func_0204d9f0(struct Vector3* out)
{
    s32 s = LCG_STATE_0204d9f0;
    s32 r1 = (s32)((unsigned int)s * 0x5eedf715u + 0x1b0cb173u);
    LCG_STATE_0204d9f0 = r1;
    out->x = r1 >> 8;
    s = LCG_STATE_0204d9f0;
    s32 r2 = (s32)((unsigned int)s * 0x5eedf715u + 0x1b0cb173u);
    LCG_STATE_0204d9f0 = r2;
    out->y = r2 >> 8;
    out->z = 0;
    NormalizeVec3(out, out);
}
