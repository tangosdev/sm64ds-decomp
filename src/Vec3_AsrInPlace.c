// NONMATCHING: elements 1-2 need their base pointers materialized (add ip,v,#4 /
// add r3,v,#8), which the u64 launder achieves, but mwccarm 1.2/sp2p3 then colors
// the value/base into the r2/r3 pair opposite to the ROM. Pure register coloring;
// same size, source reordering does not move it (known coloring wall).
void Vec3_AsrInPlace(int *v, int s)
{
    int *a = (int*)(((long long)(int)(v + 1)) & 0xFFFFFFFFFFFFFFFFLL);
    int *b = (int*)(((long long)(int)(v + 2)) & 0xFFFFFFFFFFFFFFFFLL);
    v[0] >>= s;
    *a >>= s;
    *b >>= s;
}
