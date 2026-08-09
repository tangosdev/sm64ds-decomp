//cpp
// @symbol func_ov081_02127be0
/* recovered: shared common types */
#include "common.h"

struct Vector3_16_local { unsigned short x, y, z; };
struct ActorBase { void MarkForDestruction(); };
struct Actor {
    static Actor* Spawn(unsigned int a, unsigned int b, const Vector3& v, const Vector3_16_local* v16, signed char e, short f);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" Actor* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, const Vector3& v, const Vector3_16_local* v16, int e, int f);


extern "C" void func_ov081_02127be0(char* c)
{
    char* o = *(char**)(c + 0x364);
    if (o == 0) return;
    if (*(unsigned short*)(o + 0xc) != 0xb2) return;

    Vector3 pos;
    Vector3* src = (Vector3*)(((int)o + 0x5c));
    unsigned int flags = *(unsigned int*)(o + 8);
    pos.x = src->x;
    pos.y = src->y;
    pos.z = src->z;

    char* o2 = *(char**)(c + 0x364);
    Vector3_16_local rot;
    {
        int b = *(unsigned short*)(o2 + 0x8e);
        int a = *(unsigned short*)(o2 + 0x8c);
        rot.y = b;
        rot.x = a;
        rot.z = *(unsigned short*)(o2 + 0x90);
    }

    ((ActorBase*)*(char**)(c + 0x364))->MarkForDestruction();

    unsigned int newflags = (unsigned char)(flags & 0xf) | 0x20;
    _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(0xb4, newflags, pos, &rot, *(signed char*)(c + 0xcc), -1);
    *(char**)(c + 0x364) = (char*)_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(0xb2, newflags, pos, &rot, *(signed char*)(c + 0xcc), -1);
    *(int*)(((int)c + 0xb0)) |= 0x4000000;
}
