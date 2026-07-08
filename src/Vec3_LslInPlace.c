// NONMATCHING: identical register-coloring wall to Vec3_AsrInPlace - elements 1-2
// materialize their bases but color the value/base r2/r3 pair opposite to the ROM.
// Same size; pure coloring, unmoved by source reordering.
void Vec3_LslInPlace(int *v, int s)
{
    v[0] <<= s;
    *(int*)(((long long)(int)(v + 1)) & 0xFFFFFFFFFFFFFFFFLL) <<= s;
    *(int*)(((long long)(int)(v + 2)) & 0xFFFFFFFFFFFFFFFFLL) <<= s;
}
