// NONMATCHING: different op / idiom (div=29). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern int RandomIntInternal(int* seed);
extern char* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, void* v, void* w, int e, int f);
extern int RNG_STATE;

void func_ov002_020ec80c(char* a, void* b, int count, int sl, short arg5)
{
    unsigned int r;
    int zero;
    char* n;
    int rv;
    int prev;
    int i;
    int v98;
    prev = 0xff;
    if (count > 1) {
        if (sl < 0x4000) sl = 0x4000;
    }
    i = 0;
    if (count <= 0) return;
    zero = i;
    do {
        n = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x122, 2, b, 0, *(signed char*)(a + 0xcc), -1);
        if (n != 0) {
            do {
                rv = (int)(((unsigned int)RandomIntInternal(&RNG_STATE) >> 16) << 27) >> 16;
            } while (rv == prev);
            r = (unsigned int)RandomIntInternal(&RNG_STATE) >> 16;
            *(short*)(n + 0x92) = zero;
            v98 = sl * ((r % 50) + 100) / 100;
            prev = rv;
            *(short*)(n + 0x94) = arg5 + rv;
            *(short*)(n + 0x96) = zero;
            *(int*)(n + 0x98) = v98;
        }
        i++;
    } while (i < count);
}
