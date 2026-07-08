//cpp
// NONMATCHING: missing logic (ROM does more) (div=2). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
struct Vector3 { int x, y, z; };
struct Vector3_16;
extern void func_ov102_02149684(int* dst, void* src);
extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, const Vector3& pos, const Vector3_16* r, int e, int f);
extern int RandomIntInternal(int* seed);
extern int CAMERA;
extern int RNG_STATE;

void func_ov102_0214953c(char* c, int p1, int p2)
{
    Vector3 pos;
    func_ov102_02149684((int*)&pos, c);
    char* o = (char*)_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
        0x10d, (unsigned int)(p2 | (p1 << 8)), pos, 0, *(signed char*)(c + 0xcc), -1);
    if (o == 0) return;
    char* g = *(char**)&CAMERA;
    volatile int vel[3];
    vel[0] = 0;
    vel[1] = 0x11000;
    vel[2] = 0;
    int rnd = RandomIntInternal(&RNG_STATE);
    int vsum = rnd + (*(short*)(g + 0x17c) + 0x8000);
    *(short*)(o + 0x92) = 0;
    *(short*)(o + 0x94) = (short)vsum;
    *(short*)(o + 0x96) = 0;
    *(int*)(o + 0x98) = 0x3320;
    *(int*)(o + 0xa4) = vel[0];
    *(int*)(o + 0xa8) = vel[1];
    *(int*)(o + 0xac) = vel[2];
}
}
