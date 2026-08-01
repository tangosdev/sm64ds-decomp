//cpp
// @symbol func_ov070_0211f0a4
/* recovered: shared common types */
#include "common.h"


struct Actor {
    void SmallPoofDust();
    void SpawnCoins(Vector3 const &, unsigned int, int, short);
    void KillAndTrackInDeathTable();
};

extern "C" int func_ov070_0211f0a4(void *c) {
    Actor *a = (Actor *)c;
    a->SmallPoofDust();
    Vector3 pos;
    pos.x = *(int *)((char *)c + 0x5c);
    pos.y = *(int *)((char *)c + 0x60);
    pos.z = *(int *)((char *)c + 0x64);
    unsigned int coins = *(unsigned char *)((char *)c + 0x10a) + 1;
    a->SpawnCoins(pos, coins, 0xa000, 0);
    a->KillAndTrackInDeathTable();
}
