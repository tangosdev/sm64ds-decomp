int *Vec3_AsrInPlace(int *v, int sh)
{
    v[0] >>= sh;
    *(int *)(((long long)(int)(v + 1))) >>= sh;
    *(int *)(((long long)(int)(v + 2))) >>= sh;
    return v;
}
