// @symbol func_ov006_021212fc
// @emits dScMgTrampoline_c_Render
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgTrampoline_c::Render - recovered from vtable slot identity */
extern int func_ov004_020ad674(void);
extern void func_ov004_020afcf8(void* a0, void* a1, int a2, void* a3);
extern int func_ov004_020afa20(int a0, int a1, int a2, int a3, int a4);
extern void func_ov004_020afdd0(void* a0, int a1, int a2, int a3, int a4);


int dScMgTrampoline_c_Render(int self)
{
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
                int idx = func_ov004_020ad674();
                func_ov004_020afcf8((void*)data_ov006_0213fb04[idx][1], (void*)a1v, 0xc, (void*)0);
            } else {
                func_ov004_020afa20(data_ov006_02134ecc, a1v, 0xc, -1, -1);
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
