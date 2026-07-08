// NONMATCHING: different op / idiom (div=45). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned int u32;
typedef int s32;
typedef int Fix12i;

extern u32 func_02022c80(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const void* dir);
extern u32 func_02022cbc(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const void* dir);
extern u32 func_02022d00(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, void* dir);
extern void _ZN8Particle6System12NewBigSplashE5Fix12IiES2_S2_(Fix12i x, Fix12i y, Fix12i z);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, const void* v);
extern char* WATER_HEIGHT;

void func_ov002_020ce9c8(char* self)
{
    func_02022c80(0, *(unsigned char*)(self + 0x703) ? 0x54 : 0xce, *(int*)(self + 0x5c), (int)(WATER_HEIGHT + 0x4b000), *(int*)(self + 0x64), 0);
    func_02022cbc(0, *(unsigned char*)(self + 0x703) ? 0x55 : 0xcf, *(int*)(self + 0x5c), (int)(WATER_HEIGHT + 0x3000), *(int*)(self + 0x64), 0);

    if (*(unsigned char*)(self + 0x703))
        *(int*)(self + 0x628) = func_02022d00(*(int*)(self + 0x628), 0x56, *(int*)(self + 0x5c), (int)WATER_HEIGHT, *(int*)(self + 0x64), 0);
    else
        _ZN8Particle6System12NewBigSplashE5Fix12IiES2_S2_(*(int*)(self + 0x5c), (int)WATER_HEIGHT, *(int*)(self + 0x64));

    if (*(unsigned char*)(self + 0x703))
        _ZN5Sound9PlayBank0EjRK7Vector3(0xd4, self + 0x74);
    else
        _ZN5Sound9PlayBank0EjRK7Vector3(0x16, self + 0x74);
}
