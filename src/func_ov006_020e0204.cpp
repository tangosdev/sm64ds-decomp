//cpp
// @symbol func_ov006_020e0204
// recovered name: dScMgCup_c_Behavior
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgCup_c::Behavior - recovered from vtable slot identity */
#pragma opt_strength_reduction off

class C { public: int dummy; };
typedef void (C::*PMF)();

extern "C" PMF data_ov006_02141870[];

typedef struct Frame {
    int a, b;
} Frame;

extern "C" Frame *data_ov006_0213c0d8[];


extern "C" int func_ov006_020e0204(char *o)
{
    int i;
    (((C *)o)->*data_ov006_02141870[*(int *)(o + 0x5418)])();
    for (i = 0; i < 3; i++) {
        Frame *f = &data_ov006_0213c0d8[*(int *)(((int)o + i * 4 + 0x5434))][*(int *)(o + i * 4 + 0x5440)];
        int n = f->b;
        if (n != 0) {
            *(int *)(((int)o + i * 4 + 0x544c)) += 1;
            if (*(int *)(((int)o + i * 4 + 0x544c)) >= n) {
                *(int *)(((int)o + i * 4 + 0x544c)) = 0;
                *(int *)(((int)o + i * 4 + 0x5440)) += 1;
                func_ov006_020dedfc(o, *(int *)(((int)o + i * 4 + 0x5434)),
                                    *(int *)(((int)o + i * 4 + 0x5440)), i);
            }
        }
    }
    func_ov006_020debfc(o + 0x50e8);
    func_ov006_020c2b8c(o + 0x4f38);
    return 1;
}
