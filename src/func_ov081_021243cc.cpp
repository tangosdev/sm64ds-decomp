//cpp
#include "types.h"
// @symbol func_ov081_021243cc
/* recovered: shared common types */
#include "common.h"
extern "C" {
    void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void* c, void* v);
    void* _ZN5Actor10FindWithIDEj(u32 id);
    void func_020aea30(void* self, void* a, int b);
    s16 _ZN5Actor18HorzAngleToCPlayerEv(void* self);
    void func_02012694(int a, void* p);
    void func_ov081_02125488(void* self, void* p);
    void func_020ada40(void* self, void* v, void* a);
    void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, void* v, u32 a, int f, u32 c, u32 d, u32 e);
    void func_ov081_02124134(void* self);
}

extern Vector3 data_ov081_02128998;
extern void* data_ov081_02128e24;

extern "C" void func_ov081_021243cc(void* self)
{
    u8* c = (u8*)self;
    Vector3 v;
    v.x = data_ov081_02128998.x;
    v.y = data_ov081_02128998.y;
    v.z = data_ov081_02128998.z;
    _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3((void*)(c+0x110), &v);

    u32 id = *(u32*)(c+0x134);
    if (id == 0) return;
    void* found = _ZN5Actor10FindWithIDEj(id);
    if (found == 0) return;
    u8* f = (u8*)found;

    int hit;
    s32 flags = *(s32*)(c+0x130);
    hit = 0;

    if (flags & 0x2000) {
        *(s32*)(c+0x10c) = 2;
        func_020aea30(self, found, hit);
        *(u16*)(c+0x94) = (u16)(_ZN5Actor18HorzAngleToCPlayerEv(self) + 0x8000);
        hit = 1;
    }
    if (flags & 0x40000) {
        func_02012694(0xdb, (void*)(c+0x74));
        hit = 1;
        func_ov081_02125488(self, &data_ov081_02128e24);
    }
    int isbf = (int)(*(u16*)(f+0xc) == 0xbf);
    if (isbf) {
        if (*(u8*)(f+0x6fb) != 0) return;
        if (*(u8*)(f+0x6f9) == 1) {
            *(s32*)(c+0x10c) = 2;
            func_020aea30(self, found, 0);
            *(u16*)(c+0x94) = (u16)(_ZN5Actor18HorzAngleToCPlayerEv(self) + 0x8000);
            hit = 1;
        }
        if (flags & 0x10) {
            Vector3_16 vv;
            vv.x = (s16)-0x1200;
            vv.y = 0;
            vv.z = 0;
            func_020ada40(self, &vv, found);
            func_02012694(0x1d, (void*)(c+0x74));
            hit = 1;
        }
        if ((flags & 0x40) && *(s32*)(f+8) == 2) {
            *(s32*)(c+0x10c) = 2;
            func_020aea30(self, found, 0);
            *(u8*)(c+0x468) = 2;
            *(u16*)(c+0x94) = (u16)(_ZN5Actor18HorzAngleToCPlayerEv(self) + 0x8000);
            hit = 1;
        }
        if (hit == 0) {
            Vector3 hv;
            hv.x = *(s32*)(c+0x5c);
            hv.y = *(s32*)(c+0x60);
            hv.z = *(s32*)(c+0x64);
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(found, &hv, 2, 0xc000, 1, 0, 1);
        }
    }
    if (hit != 1) return;
    func_ov081_02124134(self);
}
