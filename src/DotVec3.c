/* DotVec3 at 0x0205380c */
#include "common.h"

Fix12i DotVec3(const Vector3 *a, const Vector3 *b)
{
    long long sum = (long long)a->x * b->x
                  + (long long)a->y * b->y
                  + (long long)a->z * b->z;
    return (Fix12i)((sum + 0x800) >> 12);
}
