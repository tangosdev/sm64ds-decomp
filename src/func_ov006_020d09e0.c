// NONMATCHING: register allocation (div=11). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Residual: pure 6-reg recoloring of the fixed-point pack cluster (+0x40..+0x6c):
// ROM splits raw/shifted webs for y,z (out-of-place lsl) and colors mask=r3,
// candidate keeps all in-place with mask=r0. Term order, named temps, statement
// split, and C++ frontend all leave it invariant (wall family: short-web-first
// coloring inversion). Counts as decompiled, not matched.
struct V3 { int x, y, z; };
extern void func_020553a4(void* p);
extern void func_0203cd80(struct V3* out, int a, int b);
extern void func_ov006_020cf2fc(char* p);
extern char data_0209b3ec[];
extern char data_ov006_02140990[];
void func_ov006_020d09e0(void) {
    struct V3 s;
    int i;
    char* p;
    *(volatile int*)0x4000440 = 2;
    func_020553a4(data_0209b3ec);
    s.x = 0;
    s.y = 0;
    s.z = 0xfffff008;
    func_0203cd80(&s, -0x2000, 0xfffff008);
    *(volatile int*)0x40004c8 = (0x3ff & ((s.x << 16) >> 19)) | ((0x3ff & ((s.y << 16) >> 19)) << 10) | ((0x3ff & ((s.z << 16) >> 19)) << 20) | 0x40000000;
    *(volatile int*)0x40004cc = 0x40007fff;
    p = data_ov006_02140990;
    for (i = 0; i < 4; i++) {
        if (*(unsigned char*)(p + 0x328) != 0) {
            func_ov006_020cf2fc(p);
        }
        p += 0x32c;
    }
}
