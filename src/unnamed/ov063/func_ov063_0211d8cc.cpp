//cpp
#include "types.h"
// @symbol func_ov063_0211d8cc
/* recovered: shared common types */
#include "common.h"
extern "C" {
u8 DecIfAbove0_Byte(u8* p);
void* _ZN8dActor_c10FindWithIDEj(unsigned int id);
void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* self, const Vector3* v, unsigned int a, int fix, unsigned int b, unsigned int d, unsigned int e);
void func_0201267c(int a, void* p);
void Vec3_Sub(Vector3* out, Vector3* a, Vector3* b);
int LenVec3(Vector3* v);
s16 Vec3_HorzAngle(const Vector3* v0, const Vector3* v1);
int _Z14ApproachLinearRsss(s16* v, s16 target, s16 step);
void func_ov063_0211ddac(void* c, int i);
void _ZN9Animation7AdvanceEv(void* a);
void _ZN5dCc_c5ClearEv(void* c);
void* _ZN8dActor_c13ClosestPlayerEv(void* self);
void _ZN5dCc_c6UpdateEv(void* c);
}

extern s16 data_02082214[];

#pragma opt_strength_reduction off
extern "C" void func_ov063_0211d8cc(char* c)
{
    void* a;
    int i;
    volatile Vector3 proj;
    Vector3 vhurt;
    Vector3 tmp;
    int zero;
    int one;
    int three;
    int knock;
    int len;
    char* t;
    int k;
    char* p;
    int x;
    int z;

    if (DecIfAbove0_Byte((u8*)(c + 0x6e0)) != 0)
        return;

    *(int*)(c + 0x98) = 0x5000;
    a = 0;
    i = 0;
    zero = 0;
    one = 1;
    three = 3;
    knock = 0xc000;

    for (; i < 2; i = i + 1) {
        int id = *(int*)(c + i * 0x40 + 0x4b0);
        if (id != 0) {
            a = _ZN8dActor_c10FindWithIDEj((unsigned int)id);
            if (a != 0) {
                int isBf = (*(u16*)((char*)a + 0xc) == 0xbf);
                if (isBf != false) {
                    vhurt.x = *(int*)(c + 0x5c);
                    vhurt.y = *(int*)(c + 0x60);
                    vhurt.z = *(int*)(c + 0x64);
                    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &vhurt, three, knock, one, zero, one);
                }
            }
        }
        if (a != 0)
            break;
    }

    if ((u16)(*(int*)(c + 0x378) >> 0xc) == 0)
        func_0201267c(0x106, c + 0x74);

    t = *(char**)(c + 0x6d0);
    if (t != 0) {
        Vec3_Sub(&tmp, (Vector3*)(c + 0x5c), (Vector3*)(t + 0x5c));
        len = LenVec3(&tmp);
        _Z14ApproachLinearRsss(
            (s16*)(c + 0x94),
            Vec3_HorzAngle((Vector3*)(c + 0x5c), (Vector3*)(*(char**)(c + 0x6d0) + 0x5c)),
            0x200);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        if (len > 0x3ab000) {
            if ((u16)(*(int*)(c + 0x378) >> 0xc) == 0) {
                func_ov063_0211ddac(c, 0);
                return;
            }
        }
    } else {
        if ((u16)(*(int*)(c + 0x378) >> 0xc) == 0) {
            func_ov063_0211ddac(c, 0);
            return;
        }
    }

    _ZN9Animation7AdvanceEv(c + 0x370);

    x = *(int*)(c + 0x5c);
    proj.x = x;
    proj.y = *(int*)(c + 0x60);
    z = *(int*)(c + 0x64);
    proj.z = z;
    {
        int s0 = data_02082214[(*(volatile u16*)(c + 0x8e) >> 4) * 2];
        proj.x = x + (int)(((s64)s0 * 0x24000 + 0x800) >> 12);
    }
    proj.z = z + (int)(((s64)data_02082214[(*(volatile u16*)(c + 0x8e) >> 4) * 2 + 1] * 0x24000 + 0x800) >> 12);

    *(int*)(c + 0x4c0) = *(volatile int*)(c + 0x5c);
    *(int*)(c + 0x4c4) = *(volatile int*)(c + 0x60);
    *(int*)(c + 0x4c8) = *(volatile int*)(c + 0x64);
    *(int*)(c + 0x500) = proj.x;
    *(int*)(c + 0x504) = proj.y;
    *(int*)(c + 0x508) = proj.z;

    p = c + 0x48c;
    for (k = 0; k < 2; k = k + 1) {
        *(int*)(c + k * 0x40 + 0x490) = 0x9b000;
        _ZN5dCc_c5ClearEv(p);
        if (*(u8*)((char*)_ZN8dActor_c13ClosestPlayerEv(c) + 0x6fb) == 0)
            _ZN5dCc_c6UpdateEv(p);
        p += 0x40;
    }
}
