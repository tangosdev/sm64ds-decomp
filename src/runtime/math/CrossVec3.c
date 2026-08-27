typedef struct { int x, y, z; } Vec3_Fix12;

void CrossVec3(const Vec3_Fix12 *a, const Vec3_Fix12 *b, Vec3_Fix12 *out)
{
    long long cx = (long long)a->y * b->z - (long long)a->z * b->y;
    long long cy = (long long)a->z * b->x - (long long)a->x * b->z;
    long long cz = (long long)a->x * b->y - (long long)a->y * b->x;
    out->x = (int)((cx + 0x800) >> 12);
    out->y = (int)((cy + 0x800) >> 12);
    out->z = (int)((cz + 0x800) >> 12);
}
