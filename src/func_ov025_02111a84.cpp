//cpp
// NONMATCHING: different op / idiom (div=13). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
struct Vector3 { int x, y, z; };
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* thiz, void* cc);
extern void _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(void* thiz, const Vector3& v, int f);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int n, Vector3 v);
extern void func_0201267c(int a, void* v);

void func_ov025_02111a84(char* c)
{
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
    if (*(int*)(c + 0xa8) >= 0)
        *(int*)(c + 0x9c) = -0x4000;
    else
        *(int*)(c + 0x9c) = -0x8000;
    if (*(int*)(c + 0x60) > *(int*)(c + 0x394)) return;
    *(int*)(c + 0x60) = *(int*)(c + 0x394);
    Vector3 ps;
    Vector3 eq;
    eq.x = *(int*)(c + 0x5c);
    eq.y = *(int*)(c + 0x60);
    eq.z = *(int*)(c + 0x64);
    _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(c, eq, 0x7d0000);
    *(unsigned char*)(c + 0x39e) = 0x3c;
    *(unsigned char*)(c + 0x39f) += 1;
    *(int*)(c + 0x398) = 6;
    ps.x = *(int*)(c + 0x5c);
    ps.y = *(int*)(c + 0x60);
    ps.z = *(int*)(c + 0x64);
    ps.y = *(int*)(c + 0x60) + 0x3c000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x2e, ps);
    func_0201267c(0xc7, c + 0x74);
}
}
