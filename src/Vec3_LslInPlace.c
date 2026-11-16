int *Vec3_LslInPlace(int *v, int sh)
{
    v[0] <<= sh;
    v[1] <<= sh;
    v[2] <<= sh;
    return v;
}
