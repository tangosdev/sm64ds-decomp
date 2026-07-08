int *Vec3_LslInPlace(int *v, int sh)
{
    v[0] <<= sh;
    *(int *)(((long long)(int)(v + 1)) & 0xFFFFFFFFFFFFFFFFLL) <<= sh;
    *(int *)(((long long)(int)(v + 2)) & 0xFFFFFFFFFFFFFFFFLL) <<= sh;
    return v;
}
