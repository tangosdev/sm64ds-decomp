//cpp
// NONMATCHING: different op / idiom (div=13). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
void MulVec3Mat4x3(void* v, void* m, void* out);
int _ZN4cstd4fdivEii(int a, int b);
extern short data_02082214[];

void func_ov007_020bfd70(char* r4, void* r1, int* r5, int* r6) {
    int out[3];
    MulVec3Mat4x3(r1, r4 + 0x44, out);
    int ang = *(unsigned short*)(r4 + 0xd4) >> 4;
    int i = ang * 2;
    int cosv = data_02082214[i];
    int sinv = data_02082214[i + 1];
    int z = out[2];
    if (z < 0) z = -z;
    int r = (int)(((long long)cosv * (long long)z + 0x800) >> 0xc);
    int f = _ZN4cstd4fdivEii(sinv, r);
    int a = (int)(((long long)f * (long long)out[1] + 0x800) >> 0xc);
    int b = (int)(((long long)f * (long long)out[0] + 0x800) >> 0xc);
    r5[0] = ((b * 0x60) >> 0xc) + 0x80;
    r6[0] = ((-a * 0x60) >> 0xc) + 0x60;
}
}
