//cpp
// @symbol _ZN9PushBlock4KillEv
/* PushBlock::Kill() -- vtable slot 31, ov002 0x020b8c9c. Attributed by the
 * vtable: _ZTV9PushBlock carries this address at slot 31 -- vtable + 0x7c --
 * where _ZTV10dBgActor_c carries _ZN10dBgActor_c4KillEv, and slot 30 carries
 * dActor_c's 0x020100dc in both tables. So this is PushBlock's own override of
 * the one virtual dBgActor_c adds.
 *
 * Kept as an extern "C" free function under the literal mangled name rather
 * than a real method -- the body is unconverted, still reading `this` as a
 * raw `char*` and calling PoofDustAt/PlayBank3/MarkForDestruction by their
 * mangled names directly. */
struct Vector3 { int x, y, z; };
extern "C" {
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _ZN8dActor_c10PoofDustAtERK7Vector3(void* self, const Vector3& vec);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, const Vector3& pos);
extern void _ZN7fBase_c18MarkForDestructionEv(void* self);
}
extern "C" void _ZN9PushBlock4KillEv(char* c);
void _ZN9PushBlock4KillEv(char* c) {
    Vector3 vec;
    Vector3 vec2;
    vec.x = *(int*)(c + 0x5c);
    vec.y = *(int*)(c + 0x60);
    vec.z = *(int*)(c + 0x64);
    vec.y += 0x96000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x48, vec.x, vec.y, vec.z);
    ((int*)&vec2)[0] = ((int*)&vec)[0];
    ((int*)&vec2)[1] = ((int*)&vec)[1];
    ((int*)&vec2)[2] = ((int*)&vec)[2];
    _ZN8dActor_c10PoofDustAtERK7Vector3(c, vec2);
    _ZN5Sound9PlayBank3EjRK7Vector3(0x41, *(Vector3*)(c + 0x74));
    _ZN7fBase_c18MarkForDestructionEv(c);
}
