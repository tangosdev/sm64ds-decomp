//cpp
// @symbol _ZN8IceBlock15OnHitByMegaCharER6Player
#include "IceBlock.h"
// recovered name: IceBlock_OnHitByMegaChar
/* recovered: renamed to Class_Method */
/* daObjIceBlock_c::OnHitByMegaChar - recovered from vtable slot identity.
   The draft called into a 32-entry vtable-shim struct's slot 31 (index
   past v0..v30); that slot is Kill(), already a named virtual on this
   class, so this calls it directly instead. */
extern "C" void _ZN6Player16IncMegaKillCountEv(void*);

void IceBlock::OnHitByMegaChar(Player &player) {
    _ZN6Player16IncMegaKillCountEv(&player);
    Kill();
}
