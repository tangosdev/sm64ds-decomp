//cpp
// @symbol _ZN18dScMgTrampoline2_c6RenderEv
#include "types.h"
#include "decl_common.h"
#include "dScMgTrampoline2_c.h"
/* dScMgTrampoline2_c::Render -- vtable slot 9, ov006 0x021231ac.
 *
 * Attributed by the vtable: dScMgTrampoline2_c's own table is ov006 0x0213fc7c
 * and its slot 9 relocates here. The signature is include/dScMgBase_c.h's own
 * slot 9, `virtual s32 Render()`.
 *
 * The two volatile stores are the 3D engine's fog/clear registers at
 * 0x040004c8 and 0x040004cc, the same pair dScMgSingle3DBase_c's own
 * AfterCleanupResources writes -- here the colour is built from a rotated
 * vector rather than being a constant. They stay `volatile` because this is
 * memory-mapped I/O, not codegen steering.
 *
 * unk_7ba8 is this class's own, and reads as a member; the three-word `m` is a
 * local the rotation helper fills in place. */

extern "C" {
void func_0203cd80(int *m, short angle);
int GetGameLanguage(void);
void DrawOamSprite(void *a0, void *a1, int a2, void *a3);
void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
}

s32 dScMgTrampoline2_c::Render()
{
    int m[3];
    int count;
    int a1v;
    int i;

    m[0] = 0;
    m[1] = 0;
    m[2] = 0xfffff008;
    func_0203cd80(m, -0x4000);

    *(volatile int *)0x40004c8 =
        (((short)m[0] >> 3) & 0x3ff) |
        ((((short)m[1] >> 3) & 0x3ff) << 10) |
        ((((short)m[2] >> 3) & 0x3ff) << 20);
    *(volatile int *)0x40004cc = 0x7fff;
    *(volatile int *)0x40004cc = 0x40007fff;

    func_ov006_02120c08();
    func_ov006_020eef58();

    if (*(u16 *)((char *)this + 0x4664) == 1) {
        count = data_ov006_0213b0f0;
        a1v = 0x6e;
        if (GetOwnerLanguage() == 5 || GetOwnerLanguage() == 4)
            a1v -= 4;
        for (i = 0; i < 3; i++) {
            if (i >= count) {
                int idx = GetGameLanguage();
                DrawOamSprite((void *)data_ov006_0213fc48[idx][1], (void *)a1v, 0xc, (void *)0);
            } else {
                RenderOamMainScreen(data_ov006_02134ecc, a1v, 0xc, -1, -1);
            }
            a1v += 0x12;
        }
    }

    func_ov004_020b1a5c(data_ov006_02140830, 6);

    if (unk_7ba8 == 0) {
        func_ov006_020caadc();
        func_ov006_020d09e0();
        func_ov006_020ced84();
    }
    func_ov006_02122a4c();
    return 1;
}
