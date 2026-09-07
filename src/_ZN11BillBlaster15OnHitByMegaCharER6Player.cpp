//cpp
// @symbol _ZN11BillBlaster15OnHitByMegaCharER6Player
#include "BillBlaster.h"
// recovered name: daObjBkKillerdai_c_OnHitByMegaChar
/* recovered: renamed to Class_Method */
/* daObjBkKillerdai_c::OnHitByMegaChar - recovered from vtable slot identity.
   unk_08e/unk_094 in the old shadow struct are dBgActor_c's own mAngleY /
   mPrevAngleY (declared in dBgActor_c.h, inherited by BillBlaster), so this
   now reads/writes them by name through `this` instead of a raw shadow
   cast. */
extern "C" void _ZN6Player16IncMegaKillCountEv(void*);
extern "C" void _ZN10dBgActor_c14KillByMegaCharER6Player(void*, void*);

void BillBlaster::OnHitByMegaChar(Player& p) {
    _ZN6Player16IncMegaKillCountEv(&p);
    _ZN10dBgActor_c14KillByMegaCharER6Player(this, &p);
    mAngleY = mPrevAngleY;
}
