#include "types.h"

void VEC_CrossProduct(const Vector3 *a, const Vector3 *b, Vector3 *out) {
    long long cx = (long long)a->y * b->z - (long long)a->z * b->y;
    long long cy = (long long)a->z * b->x - (long long)a->x * b->z;
    long long cz = (long long)a->x * b->y - (long long)a->y * b->x;
    out->x = (int)((cx + 0x800) >> 12);
    out->y = (int)((cy + 0x800) >> 12);
    out->z = (int)((cz + 0x800) >> 12);
}
