//cpp
// @symbol func_ov009_02111c4c
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjMcWater_c.h"
// @emits daObjMcWater_c_Behavior
/* recovered: renamed to Class_Method */
/* daObjMcWater_c::Behavior - recovered from vtable slot identity */
class Animation {
public:
void Advance();
};

extern "C" int daObjMcWater_c_Behavior(char *r0) {
    struct daObjMcWater_c *self = (struct daObjMcWater_c *)(void *)r0;
self->unk_32c = 0x1000;
((Animation *)(r0 + 0x320))->Advance();
return 1;
}
