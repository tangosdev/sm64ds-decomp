typedef struct { int x, y, z; } Vec3_Fix12;

void Vec3_MulScalar(Vec3_Fix12 *out, const Vec3_Fix12 *in, int scale)
{
    int rz = (int)(((long long)in->z * scale + 0x800) >> 12);
    int ry = (int)(((long long)in->y * scale + 0x800) >> 12);
    int rx = (int)(((long long)in->x * scale + 0x800) >> 12);
    out->x = rx;
    out->y = ry;
    out->z = rz;
}
