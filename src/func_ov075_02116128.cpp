//cpp
// NONMATCHING: register allocation (div=31 on the canonical mwccarm 2004/b56, and the
// same 31 on the 1.2 line this banner originally cited). Logic verified correct vs ROM.
// Counts as decompiled, not matched.
//
// The schedule is not the problem: the ROM's instruction order is reproducible from C
// and the whole residue is one register rotation. A closer draft (div 19) is banked in
// nearmiss/db.jsonl; see notes/mwccarm-codegen.md 6bo for the three attractors, the
// pragma x source-shape lever that reached it, and what has already been ruled out.
extern "C" {
unsigned short *_ZN2G212GetBG1ScrPtrEv();
int func_02030958(int val);
}
extern unsigned char data_0209fc50;

extern "C" void func_ov075_02116128(int unused, int r6, int r5)
{
    int b = data_0209fc50;
    unsigned short *bg = _ZN2G212GetBG1ScrPtrEv();
    int a = func_02030958(r6);
    int t = 4 - b;
    int m = r6 * (t * 0x20 + 0xa0) + (t * 0x40 + 0x84);
    unsigned short *p = bg + m;
    unsigned int angle;
    int i;
    unsigned int hi;
    angle = (unsigned short)(a + 0xa);
    if (r5 != 0)
        angle = (unsigned short)(angle - 4);
    hi = angle << 0x1c;
    p[0x20] = (unsigned short)((p[0x20] & 0xfff) | (hi >> 16));
    p[0x40] = (unsigned short)((p[0x40] & 0xfff) | (hi >> 16));
    p[0x37] = (unsigned short)((p[0x37] & 0xfff) | (hi >> 16));
    p[0x57] = (unsigned short)((p[0x57] & 0xfff) | (hi >> 16));
    for (i = 0; i < 0x18; i++) {
        p[0] = (unsigned short)((p[0] & 0xfff) | (hi >> 16));
        p[0x60] = (unsigned short)((p[0x60] & 0xfff) | (hi >> 16));
        p++;
    }
}
