#include "types.h"
// @symbol func_0204da4c
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
/* func_0204da4c at 0x0204da4c, size=0x68
 * LCG RNG: generates 3 random values for x,y,z, then normalizes the vector.
 */
extern void NormalizeVec3(const struct Vector3* src, struct Vector3* dst);

void func_0204da4c(struct Vector3* out)
{
    s32 s = LCG_STATE_0204da4c;
    s32 r1 = (s32)((unsigned int)s * 0x5eedf715u + 0x1b0cb173u);
    LCG_STATE_0204da4c = r1;
    out->x = r1 >> 8;
    s = LCG_STATE_0204da4c;
    s32 r2 = (s32)((unsigned int)s * 0x5eedf715u + 0x1b0cb173u);
    LCG_STATE_0204da4c = r2;
    out->y = r2 >> 8;
    s = LCG_STATE_0204da4c;
    s32 r3 = (s32)((unsigned int)s * 0x5eedf715u + 0x1b0cb173u);
    LCG_STATE_0204da4c = r3;
    out->z = r3 >> 8;
    NormalizeVec3(out, out);
}
