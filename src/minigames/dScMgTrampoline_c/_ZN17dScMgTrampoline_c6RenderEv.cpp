//cpp
// @symbol _ZN17dScMgTrampoline_c6RenderEv
#include "decl_common.h"
#include "dScMgTrampoline_c.h"
/* dScMgTrampoline_c::Render -- vtable slot 9, ov006 0x021212fc.
 *
 * Named from the table: 0x021212fc is the word slot 9 of
 * _ZTV17dScMgTrampoline_c holds where dScMgD3DBase_c's table holds something
 * else, so it is this class's own override of the virtual fBase_c declares.
 *
 * THE RECEIVER HAD TO STOP BEING AN `int`. The pre-migration file took it as
 * a plain integer and spelled every access as `*(T*)(self + off)`; that form
 * matches under -lang c99 but NOT under -lang c++, which is what this file
 * now compiles as. Every offset here is past 0xfff, so it takes two
 * instructions either way, and the two front ends pick different pairs: the
 * ROM (and the C front end) emit `add rN,sl,#0x5000` + `ldr rN,[rN,#0xd94]`,
 * while the C++ front end on the same integer arithmetic pools the whole
 * constant and emits `ldr rN,[pc]` + `ldr rN,[sl,rN]` -- 0x230 bytes against
 * the ROM's 0x1fc. Doing the arithmetic on a `char *` instead restores the
 * ROM's pairing exactly, under BOTH front ends (checked: the pre-migration
 * file with only `int self` -> `char *self` changed still matches as C).
 *
 * The offsets themselves stay raw. Half of what this reads (0x4664, 0x5d84,
 * 0x5da0) lands inside spans include/dScMgTrampoline_c.h leaves as pad or
 * inside the base's own body, so spelling them as members would be a layout
 * claim this rename has no evidence for. */

extern "C" {
int  GetGameLanguage(void);
void DrawOamSprite(void *a0, void *a1, int a2, void *a3);
int  RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);
}

s32 dScMgTrampoline_c::Render()
{
    char *self = (char *)this;
    int count;
    int a1v;
    int i;
    int aa;
    int r6, r5;
    int t;

    func_ov006_0212093c((short*)(self + 0x5d84), *(int*)(self + 0x5d94));
    func_ov006_02120c08();

    if (*(unsigned short*)(self + 0x4664) == 1) {
        count = data_ov006_0213b0ec;
        a1v = 0x6e;
        for (i = 0; i < 3; i++) {
            if (i >= count) {
                int idx = GetGameLanguage();
                DrawOamSprite((void*)data_ov006_0213fb04[idx][1], (void*)a1v, 0xc, (void*)0);
            } else {
                RenderOamMainScreen(data_ov006_02134ecc, a1v, 0xc, -1, -1);
            }
            a1v += 0x12;
        }
    }

    func_ov004_020b1a5c(data_ov006_02140588, 4);

    aa = *(short*)(self + 0x5dba);
    r6 = 1;
    r5 = 1;
    if (aa == 0) {
        if (*(short*)(self + 0x5dc0) != 0) r6 = 0;
    }
    if (aa != 0) {
        if (*(short*)(self + 0x5dc0) != 0) r5 = 0;
    }

    t = data_ov006_0212f0c8[0] - (*(int*)(self + 0x5d94) + *(int*)(self + 0x5da0));
    func_ov004_020afdd0(data_ov006_02134f08, *(int*)(self + 0x5da4) + 0xf0, t, -1, 2);
    func_ov004_020afdd0(data_ov006_02134f00[r6], 0xf0, t - 0x20, -1, 2);

    t = data_ov006_0212f0c8[1] - (*(int*)(self + 0x5d94) + *(int*)(self + 0x5da0));
    func_ov004_020afdd0(data_ov006_02134f08, *(int*)(self + 0x5da8) + 0xf0, t, -1, 2);
    func_ov004_020afdd0(data_ov006_02134f00[r5], 0xf0, t - 0x20, -1, 2);

    if (*(short*)(self + 0x5dc2) == 0) {
        func_ov006_020cd270();
        func_ov006_020d09e0();
    }
    return 1;
}
