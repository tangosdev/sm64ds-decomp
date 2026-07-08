//cpp
// NONMATCHING: different op / idiom (div=34). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef short s16;
struct V3_16f;
extern "C" {
int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int a, unsigned int b, int c, int d, int e, void* v, void* cb);
extern short SINE_TABLE[];

void func_ov024_02111350(char* c){
    char* g = c + 0x300;
    int a1 = ((int)((unsigned int)*(unsigned short*)(g + 0xb2) << 0x1e)) >> 0x10;
    *(int*)(c + 0x5c) = (short)SINE_TABLE[((unsigned short)a1 >> 4) * 2] * (short)0x28 + *(int*)(c + 0x3a0);
    if ((unsigned short)*(unsigned short*)(g + 0xb2) < 0x3c) {
        int a2 = ((int)((unsigned int)*(unsigned short*)(g + 0xb2) << 0x1d)) >> 0x10;
        int m = (short)SINE_TABLE[((unsigned short)a2 >> 4) * 2] * (short)0xa;
        if (m < 0) m = -m;
        *(int*)(c + 0x60) = *(int*)(c + 0x3a4) + m;
    } else {
        short* w = (short*)(c + 0x3b0);
        *w = *w + 0x100;
        if (*(short*)(g + 0xb0) > 0x1800) {
            *(short*)(g + 0xb0) = 0x1800;
            *(int*)(c + 0xa8) = 0x5000;
        }
        *(short*)(c + 0x8e) = *(short*)(c + 0x8e) + *(short*)(g + 0xb0);
        *(int*)(c + 0x60) = *(int*)(c + 0x60) + *(int*)(c + 0xa8);
    }
    *(int*)(c + 0x3ac) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(unsigned int*)(c + 0x3ac), 0x23, *(int*)(c + 0x5c), *(int*)(c + 0x60),
        *(int*)(c + 0x64), (void*)0, (void*)0);
    if (*(unsigned short*)(c + 0x3b2) == 0x96) {
        *(unsigned char*)(c + 0x3b7) = 2;
    }
}
}
