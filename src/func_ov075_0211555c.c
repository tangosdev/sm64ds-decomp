// NONMATCHING: different op / idiom (div=35). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;

extern void func_ov075_02114cd8(void* self);
extern void func_ov075_0211ab38(void* a, int* v);
extern int func_ov075_021148f0(void* p);
extern void func_ov075_02114894(void* c);
extern int func_ov075_02114ac4(char* c, char* p5, char* p4);
extern void func_ov075_021152d4(void* self);
extern int func_0203da9c(void);
extern void func_ov075_021151b4(void* self, int x);
extern int _ZN9Animation7AdvanceEv(void* a);
extern void func_ov075_0211b418(void* c);

extern u8 IS_PLAYER_ACTIVE;

int func_ov075_0211555c(char* c)
{
    if (*(u8*)(c + 0xf44) == 0) {
        int z = 0;
        char* ee = c + 0xe80;
        u8* g = &IS_PLAYER_ACTIVE;
        char* p = c;
        char* base = c + 0x920;
        int i = 0;
        int fp = 1;
        for (; i < 4; i++, base += 0x158, p += 0x158, g += 1) {
            func_ov075_02114cd8(base);
            if (*(u8*)(p + 0xa75)) {
                int* q = (int*)(base + 0x118);
                int v[3];
                v[0] = q[0];
                v[1] = q[1];
                v[2] = q[2];
                func_ov075_0211ab38(ee, v);
            }
            if (*g) {
                if (func_ov075_021148f0(base) == 0)
                    fp = z;
            }
        }

        if (*(u8*)(c + 0xf41) == 1 && fp != 0) {
            u8* g2 = &IS_PLAYER_ACTIVE;
            char* b2 = c + 0x920;
            int j;
            for (j = 0; j < 4; j++) {
                if (*g2)
                    func_ov075_02114894(b2);
                b2 += 0x158;
                g2 += 1;
            }
            *(u8*)(c + 0xf41) = 2;
        }

        if (*(u8*)(c + 0xf41) != 0) {
            if (func_ov075_02114ac4(c + 0x920 + *(u8*)(c + 0xf42) * 0x158, c + 0xf34, c + 0xf28) != 0) {
                func_ov075_021152d4(c);
            }
        }

        if (*(u8*)(c + 0xf40)) {
            int r = func_0203da9c();
            func_ov075_021151b4(c, r);
        }

        _ZN9Animation7AdvanceEv(c + 0x90c);
        func_ov075_0211b418(c + 0xe80);
    }
    return 1;
}
