//cpp
#include "types.h"
// @symbol func_ov006_020f8a3c
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgMCarlo_c.h"

extern "C" {
// recovered name: dScMgMCarlo_c_OnTurnIntoEgg
/* recovered: renamed to Class_Method */
/* dScMgMCarlo_c::OnTurnIntoEgg - recovered from vtable slot identity */
extern void FreeGfxSlotsById(int arg);
extern int func_ov006_020c1718(void* p);
extern void func_ov004_020b56c8(char* p);
extern int _Z15ApproachLinear2Rsss(s16* r, s16 t, s16 s);

extern char* data_ov006_0213d568;
extern char* data_ov006_02142504;
extern unsigned short data_ov004_020bf9e4;
extern void* data_ov004_020beb68;

int func_ov006_020f8a3c(char* c)
{
    struct dScMgMCarlo_c *self = (struct dScMgMCarlo_c *)(void *)c;
    switch (self->unk_60a8) {
    case 4:
        FreeGfxSlotsById(0x1d);
        *(s16*)(((int)c + 0x60a8)) += 1;
        break;
    case 5:
        if (func_ov006_020c1718(c + 0x4f38) != 0) {
            int v = data_ov006_0213d570;
            if (v != 0) {
                func_ov004_020b5e40(v);
                self->unk_60aa = -1;
            } else {
                func_ov004_020b56c8(data_ov006_0213d568);
            }
            self->unk_60ae = 0;
            *(s16*)(((int)c + 0x60a8)) += 1;
        }
        break;
    case 6: {
        void* q = data_ov006_02142500;
        if (q != 0 && *(u8*)((char*)q + 0x2e) != 0) {
            if (_Z15ApproachLinear2Rsss((s16*)(c + 0x60aa), 0, 1) != 0) {
                char* r3;
                int i;
                int n;
                self->unk_60aa = 0x10;
                n = self->unk_60ae;
                r3 = data_ov006_02142504;
                for (i = 0; i < n; i++) {
                    if (r3 == 0) break;
                    r3 = *(char**)(r3 + 4);
                }
                if (r3 != 0) *(u8*)(r3 + 0x2e) = 0;
                *(s16*)(((int)c + 0x60ae)) += 1;
            }
        } else {
            if (data_ov004_020bf9e4 <= 1)
                *(s16*)(((int)c + 0x60a8)) += 1;
        }
        break;
    }
    case 7: {
        void* p = data_ov004_020beb68;
        if ((p != 0 ? *(int*)((char*)p + 0xa8) : 0) != 0)
            return 1;
        func_ov006_020f7994();
        self->unk_60aa = 0x1e;
        *(s16*)(((int)c + 0x60a8)) += 1;
        break;
    }
    case 8:
        if (_Z15ApproachLinear2Rsss((s16*)(c + 0x60aa), 0, 1) != 0)
            return 1;
        break;
    }
    return 0;
}
}
