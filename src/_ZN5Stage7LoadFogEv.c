// NONMATCHING: register allocation (div=15). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern unsigned char NUM_FOG_OBJS;
extern char* FOG_OBJS_PTR;
extern void _ZN3Fog4InitEt5Fix12IiES1_(char* self, unsigned short color, int nearv, int farv);
void _ZN5Stage7LoadFogEv(char* self) {
    unsigned char a = 0, b = 0x28;
    int i;
    for (i = 0; i < 0x20; i++) {
        *(unsigned char*)(self + 0x96c + i) = a;
        *(unsigned char*)(self + 0x994 + i) = b;
        a += 4;
        b += 2;
    }
    *(unsigned char*)(self + 0x98c) = 0;
    *(unsigned char*)(self + 0x9b4) = 1;
    *(unsigned char*)(self + 0x9b5) = 6;
    *(unsigned short*)(self + 0x9b6) = 0;
    *(unsigned short*)(self + 0x9b8) = 0x6000;
    if (NUM_FOG_OBJS == 0) return;
    char* dst = self + 0x96c;
    unsigned char* src = (unsigned char*)FOG_OBJS_PTR;
    int j;
    for (j = 0; j < NUM_FOG_OBJS; j++) {
        if (src[0] == 1) {
            unsigned short color = (src[1] >> 3) | ((src[2] >> 3) << 5) | ((src[3] >> 3) << 10);
            _ZN3Fog4InitEt5Fix12IiES1_(dst, color, *(unsigned short*)(src + 4), *(unsigned short*)(src + 6));
        } else {
            *(unsigned char*)(dst + 0x20) = 0;
        }
        src += 8;
        dst += 0x28;
    }
}
