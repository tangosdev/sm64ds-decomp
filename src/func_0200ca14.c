// NONMATCHING: different op / idiom (div=9). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern unsigned char CURR_PLAYER_ID;

void func_0200ca14(int r0, unsigned int r1, int r2)
{
    if (r1 != (unsigned int)CURR_PLAYER_ID)
        return;

    int *ptr = (int *)(r0 + 0x154);
    unsigned int val = *ptr;

    if (r2 == 0)
        val |= 0x20000u;
    else
        val |= 0x10000u;

    *ptr = (int)val;
}
