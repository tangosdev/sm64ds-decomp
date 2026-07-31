//cpp
#include "types.h"
// @symbol func_ov064_02119afc
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern "C" {
void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void* t, const Vector3& v);
void* _ZN5Actor10FindWithIDEj(unsigned int id);
s16 Vec3_VertAngle(const Vector3* v1, const Vector3* v0);
int AngleDiff(int a, int b);
short _ZN5Actor18HorzAngleToCPlayerEv(void* t);
void _ZN6Player4HealEi(void* p, int amt);
void func_ov064_02119ecc(void* c, void* p);
extern Vector3 data_ov064_0211c3d0;
}

static inline void CopyVec(Vector3* d, const Vector3* s)
{
    d->x = s->x;
    d->y = s->y;
    d->z = s->z;
}

extern "C" void func_ov064_02119afc(char* c)
{
    Vector3 hv;
    Vector3 v;
    char* a;
    int b;
    u32 id;

    *(int*)(c + 0x368) = 0x1000;
    v = data_ov064_0211c3d0;
    _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0x110, v);
    id = *(u32*)(c + 0x134);
    if (id == 0) return;
    a = (char*)_ZN5Actor10FindWithIDEj(id);
    if (a == 0) return;
    b = (*(u16*)(a + 0xc) == 0xbf);
    if (b == 0) return;
    {
        u32 base = ((u32)a + 0x5c) & 0xFFFFFFFFFFFFFFFFull;
        hv.x = *(int*)base;
        hv.y = *(int*)(base + 4);
        hv.z = *(int*)(base + 8);
    }
    if (AngleDiff(*(s16*)(c + 0x8c), Vec3_VertAngle((Vector3*)(c + 0x5c), &hv)) >= 0x3000)
        return;
    if (*(int*)(c + 0x37c) != 1) goto Lcheck;
    if (((*(s16*)(c + 0x388) >> 16) & 1) != ((_ZN5Actor18HorzAngleToCPlayerEv(c) >> 16) & 1))
        goto Lpassed;
Lcheck:
    if (*(int*)(c + 0x37c) == 1) goto Lkeep;
Lpassed:
    if (*(int*)(c + 0x37c) == 1)
        _ZN6Player4HealEi(a, 0x100);
    *(int*)(c + 0x368) = 0x4000;
    func_ov064_02119ecc(c, &data_ov064_0211c944);
    return;
Lkeep:
    *(s16*)(c + 0x388) = _ZN5Actor18HorzAngleToCPlayerEv(c);
    return;
}
