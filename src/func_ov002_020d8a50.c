// @symbol func_ov002_020d8a50
/* recovered: shared common types, declarations from a shared header */
#include "decl_Animation.h"
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;


struct RaycastLine { int head[5]; int surf; int rest[25]; };

extern int _ZNK6Player14GetBodyModelIDEjb(char* p, unsigned int j, int b);
extern void _ZN12CylinderClsn5ClearEv(void* c);
extern void func_ov002_020dbf4c(char* c);
extern void _ZN12CylinderClsn6UpdateEv(void* c);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, struct Vector3* v);
extern void _ZN11RaycastLineC1Ev(struct RaycastLine* r);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(struct RaycastLine* r, struct Vector3* a, struct Vector3* b, void* actor);
extern int _ZN11RaycastLine10DetectClsnEv(struct RaycastLine* r);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* s, struct Vector3* n);
extern void _ZN11RaycastLine10GetClsnPosEv(struct Vector3* ret, struct RaycastLine* r);
extern void _ZN11RaycastLineD1Ev(struct RaycastLine* r);

extern s16 data_02082214[];

void func_ov002_020d8a50(char* self, int which)
{
    int flag = 1;
    u32 i2 = (u8)(which << 1);
    char* model;
    char* anim;
    int frame;

    model = *(char**)(self + _ZNK6Player14GetBodyModelIDEjb(self, *(u32*)(self + 8) & 0xff, 0) * 4 + 0xdc);
    anim = (char*)((long long)(int)(model + 0x50));
    frame = (u16)(*(int*)(anim + 8) >> 12);
    if (frame < data_ov002_0210a5cc[i2] || frame > data_ov002_0210a5cc[i2 + 1]) {
        flag = 0;
        _ZN12CylinderClsn5ClearEv(self + 0x314);
    }

    if (*(u8*)(self + 0x726) != 0)
        return;

    switch (which) {
    case 0: func_ov002_020dbf4c(self); break;
    case 1: func_ov002_020dbe70(self); break;
    case 2: func_ov002_020dbd94(self); break;
    case 3: func_ov002_020dbbc0(self); break;
    }

    if (*(int*)(self + 0x338) != 0) {
        *(int*)(self + 0x98) = 0xa000;
        *(s16*)(self + 0x94) = *(s16*)(self + 0x8e) + 0x8000;
        func_ov002_020d8854(self);
    }

    _ZN12CylinderClsn5ClearEv(self + 0x314);
    if (flag == 1)
        _ZN12CylinderClsn6UpdateEv(self + 0x314);

    model = *(char**)(self + _ZNK6Player14GetBodyModelIDEjb(self, *(u32*)(self + 8) & 0xff, 0) * 4 + 0xdc);
    if (!_ZNK9Animation12WillHitFrameEi(model + 0x50, data_ov002_0210a5cc[i2]))
        return;

    switch (which) {
    case 0:
    case 1:
        func_ov002_020ef070(self + 0x380, self);
        break;
    case 2:
    case 3:
        func_ov002_020eeeb8(self + 0x380, self);
        break;
    }

    {
        u16 t = *(u16*)(self + 0x6ce) & 0x20;
        if (t == 0)
            return;
    }

    *(u16*)((long long)(int)(self + 0x6ce)) &= ~0x20;
    _ZN5Sound9PlayBank0EjRK7Vector3(0xb5, (struct Vector3*)(self + 0x74));

    {
        u16 ang = *(u16*)(self + 0x8e);
        struct RaycastLine ray;
        struct Vector3 v1;
        struct Vector3 v2;
        struct Vector3 normal;
        struct Vector3 clsnPos;
        int idx;

        _ZN11RaycastLineC1Ev(&ray);
        idx = ang >> 4;
        v1.x = *(int*)(self + 0x5c);
        v1.y = *(int*)(self + 0x60);
        v1.z = *(int*)(self + 0x64);
        v1.y += 0x32000;
        v2.x = v1.x;
        v2.y = v1.y;
        v2.z = v1.z;
        v2.x += data_02082214[idx * 2] * 0x64;
        v2.z += data_02082214[idx * 2 + 1] * 0x64;
        _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(&ray, &v1, &v2, self);
        if (_ZN11RaycastLine10DetectClsnEv(&ray)) {
            _ZNK11SurfaceInfo12CopyNormalToER7Vector3(&ray.surf, &normal);
            if (normal.y == 0) {
                *(int*)(self + 0x98) = 0xa000;
                *(s16*)(self + 0x94) = *(s16*)(self + 0x8e) + 0x8000;
                _ZN11RaycastLine10GetClsnPosEv(&clsnPos, &ray);
                func_ov002_020d8d10(self, &clsnPos);
            }
        }
        _ZN11RaycastLineD1Ev(&ray);
    }
}
