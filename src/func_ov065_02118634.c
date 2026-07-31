// @symbol func_ov065_02118634
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef long long s64;

struct Actor;



extern struct Actor* _ZN5Actor10FindWithIDEj(u32 id);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* self, void* bca, int n, int fix, u32 flags);
extern void _ZN12CylinderClsn5ClearEv(char* c);
extern void func_0201267c(u32 id, char* p);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern void _Z14ApproachLinearRiii(int* p, int target, int step);
extern void _Z14ApproachLinearRsss(s16* p, s16 target, s16 step);
extern void _ZN12CylinderClsn6UpdateEv(char* c);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(char* c, const struct Vector3* v);

void func_ov065_02118634(char* self)
{
    int found = 0;

    {
        u32 id = *(u32*)(self + 0x1164);
        if (id != 0) {
            struct Actor* p = _ZN5Actor10FindWithIDEj(id);
            if (p != 0) {
                if (*(u8*)((char*)p + 0x6de) == 0) found = 1;
            }
        }
    }

    if (*(int*)(self + 0x1130) != 0 || found != 0) {
        (*(u8*)(((int)self + 0x11b4) & 0xFFFFFFFFFFFFFFFFLL))++;
        *(int*)(self + 0x98) = 0;
        *(s16*)(self + 0x11a6) = 0;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0xec, data_ov065_0211d768[1], 0x40000000, 0x1000, 0);
        _ZN12CylinderClsn5ClearEv(self + 0x110c);
        _ZN12CylinderClsn5ClearEv(self + 0x1140);
        *(s16*)(self + 0x11b2) = 0;
        *(u8*)(self + 0x11b0) = 0;
        *(int*)(self + 0x11a8) = *(u8*)(self + 0x11b0);
        func_0201267c(0xe6, self + 0x74);
    } else {
        s16 target;
        int approach;
        char* obj = *(char**)(self + 0x118c);
        if (obj != 0) {
            target = *(s16*)(obj + 0x8e);
            approach = 0x7000;
        } else {
            int a = _ZN4cstd5atan2E5Fix12IiES1_(0x7d0000, *(int*)(self + 0x11a0) - 0x7d0000);
            s16 base = *(s16*)(self + 0x11a4);
            s16 diff = (s16)(*(s16*)(self + 0x8e) - base);
            if (diff < 0) a = (s16)(-a);
            target = (s16)(base + a);
            approach = 0x3800;
        }
        _Z14ApproachLinearRiii((int*)(self + 0x98), approach, 0x400);

        {
            int d = (s16)(target - *(s16*)(self + 0x8e));
            _Z14ApproachLinearRsss((s16*)(self + 0x11a6), (s16)(d / 100), 5);
        }

        {
            s16* pang = (s16*)(((int)self + 0x8e) & 0xFFFFFFFFFFFFFFFFLL);
            *pang = *pang + *(s16*)(self + 0x11a6);
        }
        *(s16*)(self + 0x94) = *(s16*)(self + 0x8e);

        _ZN12CylinderClsn5ClearEv(self + 0x110c);
        _ZN12CylinderClsn5ClearEv(self + 0x1140);
        _ZN12CylinderClsn6UpdateEv(self + 0x110c);
        {
            struct Vector3 v;
            v.x = 0;
            v.y = 0x50000;
            v.z = 0x150000;
            _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(self + 0x1140, &v);
        }
        _ZN12CylinderClsn6UpdateEv(self + 0x1140);
    }
}
