//cpp
// NONMATCHING: register allocation (div=15). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef int Fix12;
struct Matrix4x3;
struct G3i {
    static void PerspectiveW_(Fix12, int, int, int, int, int, bool, Matrix4x3*);
};
extern short SINE_TABLE[];

extern "C" void func_ov007_020c0078(char* c)
{
    *(int*)0x4000580 = *(unsigned char*)(c + 0xb8)
                     | (*(unsigned char*)(c + 0xba) << 8)
                     | (*(unsigned char*)(c + 0xb9) << 16)
                     | (*(unsigned char*)(c + 0xbb) << 24);

    int a = (int)*(unsigned short*)(c + 0xd4) >> 4;
    G3i::PerspectiveW_(
        SINE_TABLE[a * 2],
        SINE_TABLE[a * 2 + 1],
        ((unsigned int)*(int*)(c + 0xbc) >> 0x14) | (*(int*)(c + 0xc0) << 12),
        *(int*)(c + 0xc4),
        *(int*)(c + 0xc8),
        0x1000,
        true,
        (Matrix4x3*)(c + 0x74));

    *(int*)0x400044c = 0;
}
