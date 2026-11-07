#include "types.h"
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern s32 PARTICLE_RNG_STATE;
/* func_0204da4c at 0x0204da4c, size=0x68
 * LCG RNG: generates 3 random values for x,y,z, then normalizes the vector.
 */
extern void NormalizeVec3(const struct Vector3* src, struct Vector3* dst);

void func_0204da4c(struct Vector3* out)
{
    s32 s = PARTICLE_RNG_STATE;
    s32 x = (s32)((unsigned int)s * 0x5eedf715u + 0x1b0cb173u);
    PARTICLE_RNG_STATE = x;
    out->x = x >> 8;
    s = PARTICLE_RNG_STATE;
    s32 y = (s32)((unsigned int)s * 0x5eedf715u + 0x1b0cb173u);
    PARTICLE_RNG_STATE = y;
    out->y = y >> 8;
    s = PARTICLE_RNG_STATE;
    s32 z = (s32)((unsigned int)s * 0x5eedf715u + 0x1b0cb173u);
    PARTICLE_RNG_STATE = z;
    out->z = z >> 8;
    NormalizeVec3(out, out);
}
