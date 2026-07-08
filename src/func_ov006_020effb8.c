// NONMATCHING: register allocation (div=5). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void func_ov006_020efdf0(char* c);
extern unsigned char TOP_SCREEN_RELATED;

void func_ov006_020effb8(char* c, int idx) {
    *(int*)(c + 0x47ec + idx * 0x14) += 0x2000;
    func_ov006_020efdf0(c);
    if ((*(int*)(c + 0x47ec + idx * 0x14) >> 12) < 0xa0) return;
    *(unsigned char*)(c + 0x47f5 + idx * 0x14) += 1;
    TOP_SCREEN_RELATED &= ~4;
}
