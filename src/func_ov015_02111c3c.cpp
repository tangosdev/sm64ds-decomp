//cpp
// func_ov015_02111c3c at 0x02111c3c
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov015).
struct Vector3 { int x, y, z; };

extern "C" {
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void _ZN5Actor10PoofDustAtERK7Vector3(void* self, const Vector3& vec);
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, const Vector3& pos);
void _ZN9ActorBase18MarkForDestructionEv(void* self);
}

extern "C" void func_ov015_02111c3c(char* self);
void func_ov015_02111c3c(char* self) {
    Vector3 vec;
    Vector3 vec2;
    vec.x = *(int*)(self + 0x5c);
    vec.y = *(int*)(self + 0x60);
    vec.z = *(int*)(self + 0x64);
    vec.y += 0xc8000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x10a, vec.x, vec.y, vec.z);
    ((int*)&vec2)[0] = ((int*)&vec)[0];
    ((int*)&vec2)[1] = ((int*)&vec)[1];
    ((int*)&vec2)[2] = ((int*)&vec)[2];
    _ZN5Actor10PoofDustAtERK7Vector3(self, vec2);
    _ZN5Sound9PlayBank3EjRK7Vector3(0x41, *(Vector3*)(self + 0x74));
    _ZN9ActorBase18MarkForDestructionEv(self);
}
