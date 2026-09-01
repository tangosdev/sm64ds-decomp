//cpp
#include "types.h"
#include "dScMgRoulette_c.h"
// @symbol _ZN15dScMgRoulette_c13OnTurnIntoEggEi
// recovered name: dScMgRoulette_c_OnTurnIntoEgg
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgRoulette_c::OnTurnIntoEgg - recovered from vtable slot identity */
extern "C" {
extern void FreeGfxSlotsById(int a);
extern int func_ov006_020c1718(char *c);
extern void func_ov004_020b56c8(void);
extern u16 data_ov004_020bf9e4;
}

int dScMgRoulette_c::OnTurnIntoEgg(int /* mode */)
{
    char *self = (char *)this;

    switch (*(s16 *)(self + 0x53e6)) {
    case 5:
        FreeGfxSlotsById(0x1d);
        if (func_ov006_02107a6c() != 0)
            (*(s16 *)(self + 0x53e6))++;
        break;
    case 6:
        if (func_ov006_020c1718(self + 0x4f38) != 0) {
            *(u16 *)(self + 0x53e8) = 0x3c;
            *(int *)(self + 0x53f8) = 0;
            (*(s16 *)(self + 0x53e6))++;
        }
        break;
    case 7:
        (*(s16 *)(self + 0x53e8))--;
        if (*(s16 *)(self + 0x53e8) == 0) {
            if (*(s16 *)(self + 0x53f2) != 0)
                func_ov004_020b56c8();
            (*(s16 *)(self + 0x53e6))++;
        }
        break;
    case 8:
        (*(s16 *)(self + 0x53e6))++;
        /* fall through */
    case 9:
    default:
        if (data_ov004_020bf9e4 <= 1)
            return 1;
        break;
    }
    return 0;
}
