// NONMATCHING: register allocation (div=20). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern int RandomIntInternal(int *seed);
extern int RNG_STATE;
extern void func_ov063_02119e38(void *c, int a, int b, int d);
extern void func_ov063_02119b1c(char *c);

void func_ov063_02119960(char *c)
{
    *(unsigned short *)(c + 0x5d4) &= ~0x40;
    if (*(unsigned short *)(c + 0x100) == 0) {
        int r0 = RandomIntInternal(&RNG_STATE);
        *(int *)(c + 0x588) = (((unsigned)r0 >> 16) & 0xfff) * 5;
        int x = ((unsigned)RandomIntInternal(&RNG_STATE) >> 16) & 0xfff;
        *(int *)(c + 0x58c) = (int)((((long long)x << 7) + 0x800) >> 12);
        if (*(unsigned short *)(c + 0x5c0) == 0) {
            func_ov063_02119e38(c, -100, (short)(*(int *)(c + 0x58c) + 0x180), 0xfe0);
            func_ov063_02119b1c(c);
        }
    }
}
