//cpp
// @symbol func_ov077_02124aa4
// recovered name: Lakitu_OnTurnIntoEgg
/* recovered: renamed to Class_Method */
/* daJgm_c::OnTurnIntoEgg - recovered from vtable slot identity */
extern "C" {
extern int _ZN6Player15IsCollectingCapEv(void *p);
extern void _ZN6Player20RegisterEggCoinCountEjbb(void *p, unsigned int a, char b, char c);
extern void _ZN5Actor15GivePlayerCoinsER6Playerhj(void *self, void *p, unsigned char n, unsigned int u);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(void *self);
void func_ov077_02124aa4(void *self, void *p)
{
    if (_ZN6Player15IsCollectingCapEv(p))
        _ZN5Actor15GivePlayerCoinsER6Playerhj(self, p, 5, 0);
    else
        _ZN6Player20RegisterEggCoinCountEjbb(p, 5, 0, 0);
    _ZN5Actor24KillAndTrackInDeathTableEv(self);
}
}
