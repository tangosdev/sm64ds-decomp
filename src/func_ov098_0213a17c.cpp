//cpp
struct Vector3 { int x, y, z; };
extern "C" {
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _ZN5Actor10PoofDustAtERK7Vector3(void* self, const Vector3& vec);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, const Vector3& pos);
}
extern "C" void func_ov098_0213a17c(char* c);
void func_ov098_0213a17c(char* c) {
    Vector3 vec;
    Vector3 vec2;
    vec.x = *(int*)(c + 0x5c);
    vec.y = *(int*)(c + 0x60);
    vec.z = *(int*)(c + 0x64);
    vec.y += 0x32000;
    unsigned int id = 0x48;
    int flag = (*(unsigned short*)(c + 0xc) == 0x8b);
    if (flag) {
        id = 0x49;
    }
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(id, vec.x, vec.y, vec.z);
    ((int*)&vec2)[0] = ((int*)&vec)[0];
    ((int*)&vec2)[1] = ((int*)&vec)[1];
    ((int*)&vec2)[2] = ((int*)&vec)[2];
    _ZN5Actor10PoofDustAtERK7Vector3(c, vec2);
    _ZN5Sound9PlayBank3EjRK7Vector3(0x41, *(Vector3*)(c + 0x74));
    *(short*)(c + 0x33a) = 0x1e;
    *(int*)(c + 0x5c) = *(int*)(c + 0x320);
    *(int*)(c + 0x60) = *(int*)(c + 0x324);
    *(int*)(c + 0x64) = *(int*)(c + 0x328);
    *(char*)(c + 0x340) = 0;
}
