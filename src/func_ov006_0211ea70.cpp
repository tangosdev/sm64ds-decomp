//cpp
// NONMATCHING: register allocation (div=35). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;

extern "C" int *_ZN3G2S13GetBG0CharPtrEv(void);

extern "C" void func_ov006_0211ea70(int self, int idx)
{
    int x, y;
    int cnt = 0;
    s32 *pA = (s32 *)(self + idx * 0x24 + 0x4660);
    s32 *pB = (s32 *)(self + idx * 0x24 + 0x4664);

    for (y = 0; y < 8; y++) {
        for (x = 0; x < 8; x++) {
            int gx = x + ((*pA >> 12) - 4);
            int gy = y + ((*pB >> 12) - 4);
            int *p = _ZN3G2S13GetBG0CharPtrEv();
            int word = p[((gx >> 3) + ((gy >> 3) << 5)) * 0x20 + (gy & 7)];
            int nib = (word >> ((gx & 7) << 2)) & 0xf;
            if (nib == 0)
                cnt++;
        }
    }

    if (cnt == 0)
        return;
    if (*(u8 *)(self + idx * 0x24 + 0x4000 + 0x67f) == 0)
        return;

    *(s16 *)((char *)(self + 0x466c) + idx * 0x24) += 0x8000;
}
