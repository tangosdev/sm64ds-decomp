//cpp
// NONMATCHING: different op / idiom (div=29). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
int func_ov006_020e6e3c(int a, int b);
void func_ov006_020c4d3c(char* c);
extern short SINE_TABLE[];
}

extern "C" void func_ov006_020c4e8c(char* c) {
    if (*(short*)(c + 0xea) < 0x4000) {
        *(short*)(c + 0xea) = *(short*)(c + 0xea) + 0x400;
        short* tbl = SINE_TABLE;
        int ang = *(unsigned short*)(c + 0xea) >> 4;
        int r0 = (int)(((long long)((tbl[ang * 2] >> 1) + 0x1000) * (long long)0x1400 + 0x800) >> 0xc);
        *(int*)(c + 0xcc) = r0;
        *(int*)(c + 0xd0) = (int)(((long long)((tbl[ang * 2 + 1] >> 1) + 0x800) * (long long)0x1400 + 0x800) >> 0xc);
        *(int*)(c + 0xd4) = r0;
        return;
    }
    func_ov006_020e6e3c(0x1c7, *(int*)(c + 0x9c));
    func_ov006_020c4d3c(c);
}
