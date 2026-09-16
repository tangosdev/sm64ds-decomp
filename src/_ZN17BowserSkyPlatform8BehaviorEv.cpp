//cpp
#include "dBgActor_c.h"
// @symbol _ZN17BowserSkyPlatform8BehaviorEv
#include "BowserSkyPlatform.h"
// recovered name: daKpa3Bg_c_Behavior
/* recovered: renamed to Class_Method */
/* daKpa3Bg_c::Behavior - recovered from vtable slot identity */
struct C;
typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern Entry data_ov060_0211b1ac[];
struct C {
    unsigned char pad[0x328];
    unsigned char idx;
    unsigned char pad2[2];
    unsigned char flag;
};
s32 BowserSkyPlatform::Behavior() {
    C* c = (C*)this;
    (c->*(data_ov060_0211b1ac[c->idx].pmf))();
    ((dBgActor_c *)c)->UpdateModelPosAndRotY();
    ((dBgActor_c *)c)->UpdateClsnPosAndRot();
    c->flag = 0;
    return 1;
}
