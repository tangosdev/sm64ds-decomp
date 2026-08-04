// @symbol func_ov006_020db720
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgCard_c.h"
// recovered name: dScMgCard_c_OnTurnIntoEgg
/* recovered: renamed to Class_Method */
/* dScMgCard_c::OnTurnIntoEgg - recovered from vtable slot identity */
typedef short s16;

extern void FreeGfxSlotsById(int arg);
extern int func_ov006_020c1718(void* p);
extern void func_ov004_020b56c8(int a);
extern int _Z15ApproachLinear2Rsss(s16* r, s16 t, s16 s);

extern s16 data_ov004_020bf9e4;
extern void* func_020beb68;

int func_ov006_020db720(char* c)
{
    struct dScMgCard_c *self = (struct dScMgCard_c *)(void *)c;
    switch (self->unk_5388) {
    case 0xe: {
        int r = func_ov006_020da5e8(c + 0x51a8, c + 0x5298);
        if (r == -1)
            FreeGfxSlotsById(0xa);
        else if (r == 1)
            FreeGfxSlotsById(9);
        self->unk_538a = 0;
        *(s16*)(((int)c + 0x5388)) += 1;
        break;
    }
    case 0xf:
        if (func_ov006_020c1718(c + 0x4f38) != 0) {
            int r = func_ov006_020da5e8(c + 0x51a8, c + 0x5298);
            if (r == -1) {
                func_ov004_020b5ed0();
                self->unk_538a = 0;
            } else if (r == 1) {
                int v = data_ov006_0213bd48[func_ov006_020da4ac(c + 0x51a8, 0)];
                func_ov004_020b56c8(v * data_ov004_020bfa18);
                self->unk_538a = 0;
            } else {
                func_ov004_020b5d74();
                self->unk_538a = 0x1e;
            }
            *(s16*)(((int)c + 0x5388)) += 1;
        }
        break;
    case 0x10:
        if (data_ov004_020bf9e4 == 1) {
            int r = func_ov006_020da5e8(c + 0x51a8, c + 0x5298);
            func_ov006_020da4ac(c + 0x51a8, 0);
            if (r == -1) {
                *(s16*)(((int)c + 0x5388)) += 1;
            } else {
                return 1;
            }
        } else {
            if (_Z15ApproachLinear2Rsss((s16*)(c + 0x538a), 0, 1) != 0)
                FreeGfxSlotsById(0x1d);
        }
        break;
    case 0x11: {
        int i;
        char* a;
        char* b;
        void* p = func_020beb68;
        if ((p != 0 ? *(int*)((char*)p + 0xa8) : 0) != 0)
            return 1;
        a = c + 0x51a8;
        b = c + 0x5298;
        for (i = 0; i < 5; i++) {
            func_ov006_020d99a4(a);
            func_ov006_020d99a4(b);
            a += 0x30;
            b += 0x30;
        }
        self->unk_538a = 0x3c;
        *(s16*)(((int)c + 0x5388)) += 1;
        break;
    }
    case 0x12:
        if (_Z15ApproachLinear2Rsss((s16*)(c + 0x538a), 0, 1) != 0)
            return 1;
        break;
    }
    return 0;
}
