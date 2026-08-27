//cpp
// @symbol _ZN10BowserFire8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BowserFire.h"
struct dActor_c;
typedef void (dActor_c::*PMF)();
struct Entry { PMF pmf; };
extern "C" Entry data_ov060_0211afb4[];
extern "C" void dBgCh_Actr_UpdateDiscreteNoLava_veneer(void *p);
extern "C" int _ZNK10dBgCh_Actr10IsOnGroundEv(void *c);

int BowserFire::Behavior()
{
    dActor_c *self = (dActor_c*)((char *)this);
    *(int*)(((int)((char *)this) + 0x370)) += 1;
    (self->*data_ov060_0211afb4[mVariant].pmf)();
    mFrameCount += 1;
    if (mVertAccel != 0) {
        dBgCh_Actr_UpdateDiscreteNoLava_veneer((char *)&mWithMeshClsn);
        if (mVariant != 4) {
            if (_ZNK10dBgCh_Actr10IsOnGroundEv((char *)&mWithMeshClsn) != 0) {
                mVertSpeed = 0;
                mVertAccel = 0;
            }
        }
    }
    func_ov060_02116740(((char *)this));
    func_ov060_02117624(((char *)this));
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}
