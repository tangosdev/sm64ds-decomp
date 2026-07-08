//cpp
// NONMATCHING: missing logic (ROM does more) (div=28). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct V3 { int x, y, z; };
extern "C" {
extern void _ZN5Actor11SpawnNumberERK7Vector3jbtPS_(void* self, struct V3* pos, unsigned a, int b, unsigned short c, void* d);
extern void func_ov002_020e7554(void* self);
extern signed char NUM_VS_STARS_OBTAINED_PLAYER[];
void func_ov002_020e7218(char* c, char* a, int gate) {
    if (gate == 0) {
        struct V3 pos;
        int* v = (int*)(a + 0x5c);
        pos.x = v[0];
        pos.y = v[1] + 0xc8000;
        pos.z = v[2];
        _ZN5Actor11SpawnNumberERK7Vector3jbtPS_(c, &pos, NUM_VS_STARS_OBTAINED_PLAYER[*(unsigned char*)(a+0x6d8)], *(unsigned short*)(c+0xc) != 0xb2, 0x15, a);
    }
    *(unsigned short*)(c+0x4a2) |= 0x40;
    func_ov002_020e7554(c);
}
}
