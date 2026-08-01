//cpp
// @symbol func_ov077_02126194
// recovered name: Spiny_OnTurnIntoEgg
/* recovered: renamed to Class_Method */
/* daTgz_c::OnTurnIntoEgg - recovered from vtable slot identity */
extern "C" {
extern int _ZN6Player15IsCollectingCapEv(void *p);
extern void _ZN6Player20RegisterEggCoinCountEjbb(void *p, unsigned int a, char b, char c);
extern void _ZN5Actor15GivePlayerCoinsER6Playerhj(void *self, void *p, unsigned char n, unsigned int u);
extern void _ZN9ActorBase18MarkForDestructionEv(void *self);
void func_ov077_02126194(void *self, void *p)
{
    if (_ZN6Player15IsCollectingCapEv(p))
        _ZN5Actor15GivePlayerCoinsER6Playerhj(self, p, 1, 0);
    else
        _ZN6Player20RegisterEggCoinCountEjbb(p, 1, 0, 0);
    _ZN9ActorBase18MarkForDestructionEv(self);
}
}
