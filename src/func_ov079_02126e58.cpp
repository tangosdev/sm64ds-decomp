//cpp
// func_ov079_02126e58 at 0x020bb3b8
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
struct Vector3 { int x, y, z; };

extern "C" {
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void _ZN5Actor19DisappearPoofDustAtERK7Vector3(void* self, const Vector3& vec);
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, const Vector3& pos);
int _ZN9ActorBase18MarkForDestructionEv(void* self);
}

extern "C" int func_ov079_02126e58(char* self);
int func_ov079_02126e58(char* self) {
    Vector3 vec;
    Vector3 vec2;
    int x = *(int*)(self + 0x5c);
    int y = *(int*)(self + 0x60) + 0x96000;
    int z = *(int*)(self + 0x64);
    vec.x = x;
    vec.y = y;
    vec.z = z;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x8d, vec.x, vec.y, vec.z);
    ((int*)&vec2)[0] = ((int*)&vec)[0];
    ((int*)&vec2)[1] = ((int*)&vec)[1];
    ((int*)&vec2)[2] = ((int*)&vec)[2];
    _ZN5Actor19DisappearPoofDustAtERK7Vector3(self, vec2);
    _ZN5Sound9PlayBank3EjRK7Vector3(0xf, *(Vector3*)(self + 0x74));
    return _ZN9ActorBase18MarkForDestructionEv(self);
}
