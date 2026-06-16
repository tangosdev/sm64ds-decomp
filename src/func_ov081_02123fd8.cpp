//cpp
extern "C" {
extern int _ZN6Player15IsCollectingCapEv(void *p);
extern void _ZN6Player20RegisterEggCoinCountEjbb(void *p, unsigned int a, char b, char c);
extern void _ZN5Actor15GivePlayerCoinsER6Playerhj(void *self, void *p, unsigned char n, unsigned int u);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(void *self);
void func_ov081_02123fd8(void *self, void *p)
{
    if (!_ZN6Player15IsCollectingCapEv(p))
        _ZN6Player20RegisterEggCoinCountEjbb(p, 3, 0, 0);
    else
        _ZN5Actor15GivePlayerCoinsER6Playerhj(self, p, 3, 0);
    _ZN5Actor24KillAndTrackInDeathTableEv(self);
}
}
