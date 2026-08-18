//cpp
// @symbol func_ov070_0211f0a4
/* recovered: shared common types */
#include "common.h"


struct dActor_c {
    void SmallPoofDust();
    void SpawnCoins(Vector3 const &, unsigned int, int, short);
    void KillAndTrackInDeathTable();
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *, Vector3 const &, unsigned int, int, short);


extern "C" int func_ov070_0211f0a4(void *c) {
    dActor_c *a = (dActor_c *)c;
    a->SmallPoofDust();
    Vector3 pos;
    pos.x = *(int *)((char *)c + 0x5c);
    pos.y = *(int *)((char *)c + 0x60);
    pos.z = *(int *)((char *)c + 0x64);
    unsigned int coins = *(unsigned char *)((char *)c + 0x10a) + 1;
    _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(a, pos, coins, 0xa000, 0);
    a->KillAndTrackInDeathTable();
}
