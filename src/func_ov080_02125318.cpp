//cpp
// func_ov080_02125318 at 0x02125318
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov080).
struct Vector3 { int x, y, z; };

extern "C" {
int _ZN6Player15IsCollectingCapEv(void* player);
void _ZN5Actor15GivePlayerCoinsER6Playerhj(void* self, void* player, unsigned char a, unsigned int b);
void _ZN5Actor10PoofDustAtERK7Vector3(void* self, const Vector3& vec);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, const Vector3& pos);
void _ZN9ActorBase18MarkForDestructionEv(void* self);
}

extern "C" void func_ov080_02125318(char* self, void* player);
void func_ov080_02125318(char* self, void* player) {
    if (_ZN6Player15IsCollectingCapEv(player)) {
        _ZN5Actor15GivePlayerCoinsER6Playerhj(self, player, 5, 0);
    }
    Vector3 vec;
    Vector3 vec2;
    int x = *(int*)(self + 0x5c);
    int z = *(int*)(self + 0x64);
    int y = *(int*)(self + 0x60) + 0xb4000;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    ((int*)&vec2)[0] = ((int*)&vec)[0];
    ((int*)&vec2)[1] = ((int*)&vec)[1];
    ((int*)&vec2)[2] = ((int*)&vec)[2];
    _ZN5Actor10PoofDustAtERK7Vector3(self, vec2);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(6, vec.x, vec.y, vec.z);
    _ZN5Sound9PlayBank3EjRK7Vector3(0x41, *(Vector3*)(self + 0x74));
    _ZN9ActorBase18MarkForDestructionEv(self);
}