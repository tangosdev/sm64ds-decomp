//cpp
// @symbol _ZN10BowserFire8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BowserFire.h"
struct Actor;
typedef void (Actor::*PMF)();
struct Entry { PMF pmf; };
extern "C" Entry data_ov060_0211afb4[];
extern "C" void WithMeshClsn_UpdateDiscreteNoLava_veneer(void *p);
extern "C" int _ZNK12WithMeshClsn10IsOnGroundEv(void *c);
struct CylinderClsn { void Clear(); void Update(); };

int BowserFire::Behavior()
{
    Actor *self = (Actor*)((char *)this);
    *(int*)(((int)((char *)this) + 0x370)) += 1;
    (self->*data_ov060_0211afb4[unk_35c].pmf)();
    *(unsigned short*)(((int)((char *)this) + 0x374)) += 1;
    if (unk_09c != 0) {
        WithMeshClsn_UpdateDiscreteNoLava_veneer((char *)&mWithMeshClsn);
        if (unk_35c != 4) {
            if (_ZNK12WithMeshClsn10IsOnGroundEv((char *)&mWithMeshClsn) != 0) {
                unk_0a8 = 0;
                unk_09c = 0;
            }
        }
    }
    func_ov060_02116740(((char *)this));
    func_ov060_02117624(((char *)this));
    ((CylinderClsn*)((char *)&mMovingCylinderClsn))->Clear();
    ((CylinderClsn*)((char *)&mMovingCylinderClsn))->Update();
    return 1;
}
