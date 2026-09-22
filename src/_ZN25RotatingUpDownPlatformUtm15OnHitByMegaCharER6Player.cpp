//cpp
// @symbol _ZN25RotatingUpDownPlatformUtm15OnHitByMegaCharER6Player
/* recovered: real C++ override -- slot 27, attributed by the vtable.
   config/arm9/overlays/ov091/relocs.txt: 0x02134cc8 -> 0x021310fc, and
   _ZTV25RotatingUpDownPlatformUtm + 4*27 = 0x02134cc8. Not the key function
   (see include/RotatingUpDownPlatformUtm.h). */
#include "RotatingUpDownPlatformUtm.h"

struct Player;
extern "C" {
void _ZN6Player16IncMegaKillCountEv(Player *);
void func_02012694(int a, void *b);
void _ZN10dBgActor_c14KillByMegaCharER6Player(void *self, Player *player);
}

void RotatingUpDownPlatformUtm::OnHitByMegaChar(Player &player)
{
    char *self = (char *)this;
    Player *p = &player;
    unsigned short h = *(unsigned short *)(self + 0xc);
    int eq = (h == 0x1e);
    if (eq) return;
    _ZN6Player16IncMegaKillCountEv(p);
    func_02012694(0x1e, self + 0x74);
    _ZN10dBgActor_c14KillByMegaCharER6Player(self, p);
    *(short *)(self + 0x8e) = *(short *)(self + 0x94);
}
