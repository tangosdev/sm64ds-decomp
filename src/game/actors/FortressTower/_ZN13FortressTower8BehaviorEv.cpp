//cpp
// @symbol _ZN13FortressTower8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "FortressTower.h"
#include "dBgW.h"
extern "C" {
int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void* c, int a, int b);
extern unsigned char data_0209f2d8[];
}

int FortressTower::Behavior()
{
    unsigned short id = actorID;
    int r1 = 0;
    switch (id) {
    case 0x31: r1 = 0x900000; break;
    case 0x3b: r1 = 0x600000; break;
    case 0x3d: r1 = 0x600000; break;
    case 0x12a: r1 = 0x600000; break;
    case 0x154: r1 = 0x300000; break;
    case 0x155: r1 = 0x300000; break;
    }
    int on = (data_0209f2d8[0] == 1);
    if (on) {
        if (!((dBgW *)((char*)&(*(u8 *)&mMeshCollider)))->IsEnabled())
            ((dBgW *)((char*)&mMeshCollider))->Enable((dActor_c *)(((char*)this)));
    } else {
        _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(((char*)this), r1, 0);
    }
    return 1;
}
