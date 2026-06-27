// NONMATCHING: base materialization / addressing (div=3). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct Struct_ov010_0211125c {
    char pad[144];
    short field_90;
    char pad2[782];
    int field_3A0;
};

void func_ov010_0211125c(struct Struct_ov010_0211125c *arg0) {
    short *r2;
    do {
        r2 = (short *)((char *)arg0 + 0x90);
    } while (0);
    
    int r1 = *r2;
    r1 += 0x400;
    *r2 = r1;
    
    r1 = arg0->field_90;
    if (r1 > 0) {
        arg0->field_90 = 0;
        arg0->field_3A0 = 0;
    }
}
