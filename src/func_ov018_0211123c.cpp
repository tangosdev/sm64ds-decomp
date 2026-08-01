//cpp
// @symbol func_ov018_0211123c
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjSm_Lift_c.h"
// recovered name: daObjSm_Lift_c_OnHitByMegaChar
/* recovered: renamed to Class_Method */
/* daObjSm_Lift_c::OnHitByMegaChar - recovered from vtable slot identity */
struct Player {
    void IncMegaKillCount();
};

struct Platform {
    void KillByMegaChar(Player &);
};

extern "C" void func_ov018_0211123c(char *c, void *p) {
    struct daObjSm_Lift_c *self = (struct daObjSm_Lift_c *)(void *)c;
    ((Player *)p)->IncMegaKillCount();
    ((Platform *)c)->KillByMegaChar(*(Player *)p);
    short val = self->unk_094;
    self->unk_08e = val + 0x4000;
}
