int *Vec3_AsrInPlace(int *v, int sh)
{
    v[0] >>= sh;
    *(v + 1) >>= sh;
    *(v + 2) >>= sh;
    return v;
}
