//cpp
// @symbol _ZN10dScMgCup_c8BehaviorEv
#include "decl_common.h"
#include "dScMgCup_c.h"
/* dScMgCup_c::Behavior -- vtable slot 6.
 *
 * Attributed by the ROM's vtable at ov006 0x0213c154, which holds 0x020e0204 in
 * the one slot where the table differs from dScMgSingle3DBase_c's. The old file's
 * `recovered name: dScMgCup_c_Behavior` agrees for once -- unlike the four the
 * reference commit had to correct, this address really is Cup's own.
 *
 * The dispatch through data_ov006_02141870 is a pointer-to-member table indexed by
 * mState. `C` stays a bare non-polymorphic stand-in rather than dScMgCup_c: a PMF
 * on a polymorphic class is a different representation, so the shape here is
 * codegen and not decoration -- the same reason
 * src/_ZN15daObjMarioCap_c8BehaviorEv.cpp keeps its own stand-in.
 *
 * The three parallel per-cup arrays keep their `(int)` launders; those steer the
 * address arithmetic and are not spellings that can be tidied. */
#pragma opt_strength_reduction off

class C { public: int dummy; };
typedef void (C::*PMF)();

extern "C" PMF data_ov006_02141870[];

typedef struct Frame {
    int a, b;
} Frame;

extern "C" Frame *data_ov006_0213c0d8[];

s32 dScMgCup_c::Behavior()
{
    char *o = (char *)this;
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
