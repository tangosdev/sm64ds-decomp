int *Vec3_AsrInPlace(int *v, int sh)
{
    v[0] >>= sh;
    *(int *)(v + 1) >>= sh;
    *(int *)(v + 2) >>= sh;
    return v;
}
