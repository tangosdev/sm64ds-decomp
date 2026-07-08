// NONMATCHING: different op / idiom (div=12). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef int s32;
extern s32 data_ov006_0212ed24[];
extern s32 FRAME_TIMER;
extern struct P { s32 a, b; } data_ov006_02142ab4[];
extern void *data_ov006_021376ec[];
extern void func_ov004_020af68c(void *a0, s32 a1, s32 a2, s32 a3, s32 a4);

void func_ov006_02107b94(void **sl)
{
    int i;
    for (i = 0; i < 5; i++) {
        char *obj = (char *)sl[i];
        int r0, m, idx, a1, a2;
        if (obj == 0) return;
        *(int *)(obj + 0x28) = 0;
        r0 = *(short *)((char *)sl + 0x14);
        m = *(short *)(obj + 0x2c);
        idx = data_ov006_0212ed24[r0];
        if (FRAME_TIMER & 8) idx++;
        if (m >= 0xc && m <= 0x13) idx += 2;
        a1 = data_ov006_02142ab4[m].a >> 12;
        a2 = data_ov006_02142ab4[m].b >> 12;
        if (r0 == 3) a1 += 0x58;
        func_ov004_020af68c(data_ov006_021376ec[idx], a1, a2, -1, -1);
    }
}
