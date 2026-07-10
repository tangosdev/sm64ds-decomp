//cpp
struct Vector3 { int x, y, z; };
extern "C" {
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* thiz, void* cc);
extern void _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(void* thiz, const Vector3& v, int f);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int n, int x, int y, int z);
extern void func_0201267c(int a, void* v);

void func_ov025_02111a84(char* c)
{
    Vector3 v[2];
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
    if (*(int*)(c + 0xa8) >= 0)
        *(int*)(c + 0x9c) = -0x4000;
    else
        *(int*)(c + 0x9c) = -0x8000;
    if (*(int*)(c + 0x60) > *(int*)(c + 0x394))
        return;
    *(int*)(c + 0x60) = *(int*)(c + 0x394);
    v[1].x = *(int*)(c + 0x5c);
    v[1].y = *(int*)(c + 0x60);
    v[1].z = *(int*)(c + 0x64);
    _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(c, v[1], 0x7d0000);
    *(unsigned char*)(c + 0x39e) = 0x3c;
    *(unsigned char*)(((int)c + 0x39f) & 0xFFFFFFFFFFFFFFFFLL) =
        *(unsigned char*)(((int)c + 0x39f) & 0xFFFFFFFFFFFFFFFFLL) + 1;
    *(int*)(c + 0x398) = 6;
    v[0].x = *(int*)(c + 0x5c);
    v[0].y = *(int*)(c + 0x60);
    v[0].z = *(int*)(c + 0x64);
    v[0].y = v[0].y + 0x3c000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x2e, v[0].x, v[0].y, v[0].z);
    func_0201267c(0xc7, c + 0x74);
}
}
