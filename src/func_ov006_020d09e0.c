// NONMATCHING: register allocation (div=18). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct V3 { int x, y, z; };
extern void func_020553a4(void* p);
extern void func_0203cd80(struct V3* out, int a, int b);
extern void func_ov006_020cf2fc(char* p);
extern char VIEW_MATRIX_ASR_3[];
extern char data_ov006_02140990[];
void func_ov006_020d09e0(void) {
    struct V3 s;
    *(volatile int*)0x4000440 = 2;
    func_020553a4(VIEW_MATRIX_ASR_3);
    s.x = 0;
    s.y = 0;
    s.z = 0xfffff008;
    func_0203cd80(&s, -0x2000, 0xfffff008);
    *(volatile int*)0x40004c8 = (0x3ff & ((s.x << 16) >> 19)) | ((0x3ff & ((s.y << 16) >> 19)) << 10) | ((0x3ff & ((s.z << 16) >> 19)) << 20) | 0x40000000;
    *(volatile int*)0x40004cc = 0x40007fff;
    char* p = data_ov006_02140990;
    int i;
    for (i = 0; i < 4; i++) {
        if (*(unsigned char*)(p + 0x328) != 0) {
            func_ov006_020cf2fc(p);
        }
        p += 0x32c;
    }
}
