//cpp
/* func_ov062_02119628 at 0x02119628 (ov062), size 0x80
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
extern "C" {
struct Actor {
    virtual void v0();
    virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4();
    virtual void v5(); virtual void v6(); virtual void v7(); virtual void v8();
    virtual void v9(); virtual void v10(); virtual void v11(); virtual void v12();
    virtual void v13(); virtual void v14(); virtual void v15(); virtual void v16();
    virtual void v17(); virtual int Slot18();
    char pad[0x104]; unsigned char f108;
};
extern int _ZN6Player15IsCollectingCapEv(void *p);
extern void _ZN6Player20RegisterEggCoinCountEjbb(void *p, unsigned int a, bool b, bool c);
extern void _ZN5Actor15GivePlayerCoinsER6Playerhj(void *self, void *p, unsigned char b, unsigned int n);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(void *self);
void func_ov062_02119628(Actor *c, void *player) {
#pragma optimize_for_size on
    if (c->f108 == 3) {
        if (c->Slot18() == 6 && _ZN6Player15IsCollectingCapEv(player) == 0) {
            _ZN6Player20RegisterEggCoinCountEjbb(player, 0, 0, 1);
        } else {
            _ZN5Actor15GivePlayerCoinsER6Playerhj(c, player, 1, 2);
        }
    }
    _ZN5Actor24KillAndTrackInDeathTableEv(c);
}
}
