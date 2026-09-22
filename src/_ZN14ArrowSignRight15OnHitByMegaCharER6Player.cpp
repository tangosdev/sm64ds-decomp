//cpp
// @symbol _ZN14ArrowSignRight15OnHitByMegaCharER6Player
#include "ArrowSignRight.h"
// recovered name: ArrowSignRight_OnHitByMegaChar
/* recovered: renamed to Class_Method */
/* daObjYajirusi_c::OnHitByMegaChar - recovered from vtable slot identity */
extern "C" {
int func_02012694(int, void*);
void _ZN6Player16IncMegaKillCountEv(void*);
void _ZN10dBgActor_c14KillByMegaCharER6Player(void*, void*);
}

void ArrowSignRight::OnHitByMegaChar(Player &player)
{
    _ZN6Player16IncMegaKillCountEv(&player);
    func_02012694(0x1e, (char*)this + 0x74);
    _ZN10dBgActor_c14KillByMegaCharER6Player(this, &player);
}
