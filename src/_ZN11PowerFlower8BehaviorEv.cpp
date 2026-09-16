//cpp
#include "dActor_c.h"
#include "SaveData.h"
#include "fBase_c.h"
#include "dCc_c.h"
// @symbol _ZN11PowerFlower8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_SaveData.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PowerFlower.h"
extern "C" {
}

int PowerFlower::Behavior()
{
    int b = (int)((mFlags & 0x40000) != 0);
    if (b != 0) return 1;
    mScaleX = 0xfa0;
    mScaleY = 0xfa0;
    mScaleZ = 0xfa0;
    func_ov002_020b9750(((char*)this));
    func_ov002_020b979c(((char*)this));
    func_ov002_020b9a1c(((char*)this));
    func_ov002_020b993c(((char*)this));
    ((dCc_c *)&mdCcAc_c)->Clear();
    ((dCc_c *)&mdCcAc_c)->Update();
    if (SaveData::HasPlayerLostCap()) {
        ((dActor_c *)(((char*)this)))->SmallPoofDust();
        ((fBase_c *)(((char*)this)))->MarkForDestruction();
    }
    return 1;
}
