#include "types.h"
// @symbol func_ov002_020edca4
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
struct Actor;


extern int func_ov002_020ec610(unsigned char* p);
extern void _ZN5Actor11UntrackStarERa(struct Actor* self, signed char* r);
extern struct Actor* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 a, u32 b, const struct Vector3* pos, const void* v, int e, int f);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 a, int b, int c, int d);
extern void _ZN9ActorBase18MarkForDestructionEv(struct Actor* self);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, const struct Vector3* pos);

void func_ov002_020edca4(char* self)
{
    struct Vector3 pos;
    struct Actor* spawned;
    int xv, zv, yv, sy;

    if (*(u8*)(self + 0x426) != 0) return;

    func_ov002_020ec728(self);

    yv = *(int*)(self + 0x60);
    zv = *(int*)(self + 0x64);
    sy = yv + 0x28000;
    xv = *(int*)(self + 0x5c);
    pos.x = xv;
    pos.y = sy;
    pos.z = zv;
    if (func_ov002_020ec610((unsigned char*)self) != 0) {
        _ZN5Actor11UntrackStarERa((struct Actor*)self, (signed char*)(self + 0x427));

        spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xb3, 0x10,
            &pos, 0, *(signed char*)(*(char**)(self + 0x38c) + 0xcc), -1);
        if (spawned != 0) {
            func_ov002_020e7218((char*)spawned, *(char**)(self + 0x38c), 1);
        }
    }

    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x3f, *(int*)(self + 0x5c), *(int*)(self + 0x60), *(int*)(self + 0x64));
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x40, *(int*)(self + 0x5c), *(int*)(self + 0x60), *(int*)(self + 0x64));
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x41, *(int*)(self + 0x5c), *(int*)(self + 0x60), *(int*)(self + 0x64));
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x42, *(int*)(self + 0x5c), *(int*)(self + 0x60), *(int*)(self + 0x64));
    _ZN9ActorBase18MarkForDestructionEv((struct Actor*)self);
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(0, 0x103, (const struct Vector3*)(self + 0x74));

    *(u8*)(self + 0x426) = 1;
}
