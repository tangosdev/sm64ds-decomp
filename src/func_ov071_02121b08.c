#include "types.h"
typedef struct { short x, y, z; } Vector3_16f;
struct Foo {
    char pad[0x5c];
    Fix12 x, y, z;
    char pad2[0x2c0];
    void* particleSys;
};
struct Callback {};
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 id, u32 param, Fix12 x, Fix12 y, Fix12 z, const Vector3_16f* pos, struct Callback* cb);
extern void _ZN9ActorBase18MarkForDestructionEv(struct Foo* self);
void func_ov071_02121b08(struct Foo* c) {
    void* p = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        0, 0x45, c->x, c->y, c->z, (const Vector3_16f*)0, (struct Callback*)0
    );
    c->particleSys = p;
    _ZN9ActorBase18MarkForDestructionEv(c);
}
