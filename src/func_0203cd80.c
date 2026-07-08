// NONMATCHING: register allocation (div=22). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern short SINE_TABLE[];

void func_0203cd80(int *m, short angle)
{
    unsigned int i = (unsigned short)angle >> 4;
    short s = SINE_TABLE[i * 2];
    short c = SINE_TABLE[i * 2 + 1];
    int v1 = m[2];
    int v2 = m[1];
    int o1 = (int)((((long long)c * v1) + 0x800) >> 12) + (int)((((long long)s * v2) + 0x800) >> 12);
    int o2 = (int)((((long long)c * v2) + 0x800) >> 12) - (int)((((long long)s * v1) + 0x800) >> 12);
    m[2] = o1;
    m[1] = o2;
}
