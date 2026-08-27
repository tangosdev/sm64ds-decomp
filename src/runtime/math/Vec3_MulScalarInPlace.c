typedef long long s64;

void Vec3_MulScalarInPlace(int *v, int s)
{
    v[0] = (int)(((s64)v[0] * s + 0x800) >> 12);
    v[1] = (int)(((s64)v[1] * s + 0x800) >> 12);
    v[2] = (int)(((s64)v[2] * s + 0x800) >> 12);
}
