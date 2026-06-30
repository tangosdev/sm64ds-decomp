//cpp
// func_ov073_021223f4 at 0x021223f4
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov073).
struct Vector3 { int x, y, z; };
extern "C" {
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _ZN5Actor10PoofDustAtERK7Vector3(void* self, const Vector3& vec);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, const Vector3& pos);
}
extern "C" void func_ov073_021223f4(char* c);
void func_ov073_021223f4(char* c) {
    Vector3 vec;
    Vector3 vec2;
    vec.x = *(int*)(c + 0x5c);
    vec.y = *(int*)(c + 0x60);
    vec.z = *(int*)(c + 0x64);
    vec.y += 0x32000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xa, vec.x, vec.y, vec.z);
    ((int*)&vec2)[0] = ((int*)&vec)[0];
    ((int*)&vec2)[1] = ((int*)&vec)[1];
    ((int*)&vec2)[2] = ((int*)&vec)[2];
    _ZN5Actor10PoofDustAtERK7Vector3(c, vec2);
    _ZN5Sound9PlayBank3EjRK7Vector3(0x41, *(Vector3*)(c + 0x74));
}