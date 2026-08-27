#include "types.h"
// @symbol func_ov006_0212101c
// recovered name: dScMgTrampoline_c_OnAttacked2
/* recovered: renamed to Class_Method */
/* dScMgTrampoline_c::OnAttacked2 - recovered from vtable slot identity */
typedef struct UnkObj UnkObj;
struct UnkObj {
    int (**vt)(UnkObj*);
};

typedef struct Vec2s { s16 x, y; } Vec2s;

extern UnkObj* data_0209f5bc;
extern int data_ov006_0213fa9c;

int func_ov004_020ae1a0(void);
unsigned int func_02012790(unsigned int a);
int func_ov006_020d0c38(Vec2s* a, Vec2s* b);
void func_02012718(void* a, int b);
int func_02054d88(void);
void MultiStore16(u16 val, void* dst, int nbytes);

int func_ov006_0212101c(char* self)
{
    volatile u16 z;
    Vec2s v1;
    Vec2s v2;
    s16 a;
    int dst;

    if (!func_ov004_020ae1a0())
        return 0;

    if (data_0209f5bc->vt[5](data_0209f5bc)) {
        data_ov006_0213fa9c = 0;
    } else if (data_0209f5bc->vt[7](data_0209f5bc)) {
        data_ov006_0213fa9c = 1;
    }

    if (data_ov006_0213fa9c != 0)
        return 0;

    if (*(int*)(self + 0x4628) != 0)
        return 0;

    if (*(u8*)(self + 0x5dc5) != 0) {
        a = *(s16*)(self + 0x5db4);
        if ((a < 0x24 && *(s16*)(self + 0x5db0) < 0x24) ||
            (a > 0xd4 && *(s16*)(self + 0x5db0) > 0xd4)) {
            func_02012790(0xe);
        } else {
            v1.x = *(volatile s16*)(self + 0x5db4);
            v1.y = *(s16*)(self + 0x5db6);
            v2.x = *(s16*)(self + 0x5db0);
            v2.y = *(s16*)(self + 0x5db2);
            if (func_ov006_020d0c38(&v1, &v2)) {
                func_02012718((void*)0x1af, (*(s16*)(self + 0x5db4) + *(s16*)(self + 0x5db0)) << 11);
            } else {
                func_02012790(0xe);
            }
        }
        dst = func_02054d88();
        z = 0;
        MultiStore16(z, (void*)dst, 0x6000);
        *(u8*)(self + 0x5dc4) = 0;
        *(u8*)(self + 0x5dc5) = 0;
    }
    return 1;
}
