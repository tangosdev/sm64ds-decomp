//cpp
// @symbol func_ov079_02123e60
// recovered name: Whomp_OnHitByMegaChar
/* recovered: shared common types, renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types, renamed to Class_Method */
/* daBtn_c::OnHitByMegaChar - recovered from vtable slot identity */
extern "C" {
extern void _ZN6Player16IncMegaKillCountEv(void* thiz);
extern void func_02012694(int a, void* v);
extern void func_ov079_02123d4c(void* out, void* c);
extern void _ZN5Actor10PoofDustAtERK7Vector3(void* thiz, const Vector3& v);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int n, int a, int b, int c);
extern void _ZN9ActorBase18MarkForDestructionEv(void* thiz);

void func_ov079_02123e60(char* c, void* player)
{
    if (*(unsigned char*)(c + 0x414) != 0) return;
    if (*(int*)(c + 0x10c) == 8) return;
    _ZN6Player16IncMegaKillCountEv(player);
    func_02012694(0x1e, c + 0x74);
    Vector3 pos;
    func_ov079_02123d4c(&pos, c);
    Vector3 dust;
    pos.y = pos.y + 0x28000;
    dust.x = pos.x;
    dust.y = pos.y;
    dust.z = pos.z;
    _ZN5Actor10PoofDustAtERK7Vector3(c, dust);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(5, pos.x, pos.y, pos.z);
    _ZN9ActorBase18MarkForDestructionEv(c);
    if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x418) != 0)
        _ZN16MeshColliderBase7DisableEv(c + 0x418);
}
}
