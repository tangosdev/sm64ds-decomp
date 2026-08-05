//cpp
// @symbol _ZN11CastleWater8BehaviorEv
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjMcWater_c.h"
// recovered name: daObjMcWater_c_Behavior
/* recovered: renamed to Class_Method */
/* daObjMcWater_c::Behavior - recovered from vtable slot identity */
class Animation {
public:
void Advance();
};

extern "C" int _ZN11CastleWater8BehaviorEv(char *r0) {
    struct daObjMcWater_c *self = (struct daObjMcWater_c *)(void *)r0;
self->unk_32c = 0x1000;
((Animation *)(r0 + 0x320))->Advance();
return 1;
}
