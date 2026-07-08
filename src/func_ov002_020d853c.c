//cpp
// NONMATCHING: different op / idiom (div=17). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct State;
extern "C" {
extern int _ZN6Player7IsStateERNS_5StateE(void* self, struct State* s);
extern unsigned _ZNK6Player14GetBodyModelIDEjb(void* self, unsigned a, int b);
extern short Vec3_HorzAngle(void* v0, void* v1);
extern int AngleDiff(int a, int b);
extern struct State _ZN6Player13ST_PUNCH_KICKE;
extern unsigned short data_ov002_0210a5dc[];
int func_ov002_020d853c(char* c, char* o) {
    if (_ZN6Player7IsStateERNS_5StateE(c, &_ZN6Player13ST_PUNCH_KICKE)) {
        int j = (*(unsigned char*)(c+0x6e2) << 1) & 0xff;
        unsigned idx = _ZNK6Player14GetBodyModelIDEjb(c, *(int*)(c+8) & 0xff, 0);
        char* mp = *(char**)(c + 0xdc + idx*4);
        int v = *(int*)(mp + 0x58);
        int k = (int)((unsigned)(v << 4) >> 16);
        if (k < data_ov002_0210a5dc[j]) return 1;
        if (k > data_ov002_0210a5dc[j+1]) return 1;
        {
            int d = AngleDiff(Vec3_HorzAngle(c+0x5c, o+0x5c), *(short*)(c+0x8e));
            if (d < 0x4000) return 0;
        }
    }
    return 1;
}
}
