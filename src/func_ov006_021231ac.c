#include "types.h"
// @symbol func_ov006_021231ac
// recovered name: dScMgTrampoline2_c_Render
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgTrampoline2_c::Render - recovered from vtable slot identity */
extern void func_0203cd80(int* m, short angle);
extern int GetGameLanguage(void);
extern void DrawOamSprite(void* a0, void* a1, int a2, void* a3);
extern void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);


int func_ov006_021231ac(char* self)
{
    int m[3];
    int count;
    int a1v;
    int i;

    m[0] = 0;
    m[1] = 0;
    m[2] = 0xfffff008;
    func_0203cd80(m, -0x4000);

    *(volatile int*)0x40004c8 =
        (((short)m[0] >> 3) & 0x3ff) |
        ((((short)m[1] >> 3) & 0x3ff) << 10) |
        ((((short)m[2] >> 3) & 0x3ff) << 20);
    *(volatile int*)0x40004cc = 0x7fff;
    *(volatile int*)0x40004cc = 0x40007fff;

    func_ov006_02120c08();
    func_ov006_020eef58();

    if (*(u16*)(self + 0x4664) == 1) {
        count = data_ov006_0213b0f0;
        a1v = 0x6e;
        if (GetOwnerLanguage() == 5 || GetOwnerLanguage() == 4)
            a1v -= 4;
        for (i = 0; i < 3; i++) {
            if (i >= count) {
                int idx = GetGameLanguage();
                DrawOamSprite((void*)data_ov006_0213fc48[idx][1], (void*)a1v, 0xc, (void*)0);
            } else {
                RenderOamMainScreen(data_ov006_02134ecc, a1v, 0xc, -1, -1);
            }
            a1v += 0x12;
        }
    }

    func_ov004_020b1a5c(data_ov006_02140830, 6);

    if (*(short*)(self + 0x7ba8) == 0) {
        func_ov006_020caadc();
        func_ov006_020d09e0();
        func_ov006_020ced84();
    }
    func_ov006_02122a4c();
    return 1;
}
