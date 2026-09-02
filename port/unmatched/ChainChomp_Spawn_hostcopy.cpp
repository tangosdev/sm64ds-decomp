/* HOST COPY of src/ChainChomp_Spawn.cpp -- an ARM r0 RIDE-THROUGH in a spawn
 * factory, which is a new place for the port to meet one.
 *
 * The matched source spells Enemy's constructor
 *
 *     void func_020aed98(void);          // no parameter
 *     ...
 *     char *c = (char *)_ZN9ActorBasenwEj(0x620);
 *     if (c) { func_020aed98(); *(int **)c = _ZTV10ChainChomp; ... }
 *
 * and that is correct ARM: operator new returned the object in r0 and the
 * constructor wants `this` in r0, so mwcc emits the branch with nothing in
 * between and the C never has to name the argument. The reloc at 0x02112d34
 * settles which function it is -- ov002 0x020aed98, _ZN5EnemyC2Ev, the one
 * gate 16 brought in for the 1-up mushroom. (dsd names ov007 0x020aed98 as
 * well because the two overlays share a base; that one is 0x1e0 bytes and is
 * not a constructor. Every other Enemy factory in src/ -- BobOmb_Spawn,
 * KoopaTheQuick_Spawn, KingBobOmb_Spawn -- calls the same address and spells
 * the argument, which is the same reading from the other side.)
 *
 * Under MSVC cdecl a call that passes no argument passes no argument. Measured
 * before this copy existed: the constructor read `this` off the caller's frame
 * and ActorBase's own constructor faulted at c0000005 reading 0x000000db --
 * the actor id, 219, sitting in the spawn spine's stack slot.
 *
 * Everything else here is the matched source line for line, including the two
 * seven-element Model and ShadowModel arrays and the two seven-element
 * 0xc-byte ones the chain links are built from.
 */
extern "C" {

void *_ZN9ActorBasenwEj(unsigned size);
int _ZN5EnemyC2Ev(void *self);
int _ZN25MovingCylinderClsnWithPosC1Ev(void *self);
int _ZN9ModelAnimC1Ev(void *self);
int _ZN11ShadowModelC1Ev(void *self);
int func_020733a8(void *arr, int n, int stride, void *ctor, void *dtor);
void _ZN5ModelC1Ev(void);
void _ZN5ModelD1Ev(void);
void _ZN11ShadowModelD1Ev(void);
void func_020072c0(void);
void func_0203d384(void);
extern int _ZTV10ChainChomp[];

// PORT_HOST_ABI: matched src calls the Enemy constructor argless because operator new left this in r0 on ARM; MSVC cdecl passes nothing and faults the constructor, so this is passed explicitly
void *ChainChomp_Spawn(void)
{
    char *c = (char *)_ZN9ActorBasenwEj(0x620);
    if (c) {
        _ZN5EnemyC2Ev(c);
        *(int **)c = _ZTV10ChainChomp;
        _ZN25MovingCylinderClsnWithPosC1Ev(c + 0x110);
        _ZN9ModelAnimC1Ev(c + 0x150);
        _ZN11ShadowModelC1Ev(c + 0x1b4);
        func_020733a8(c + 0x1dc, 7, 0x50, (void *)_ZN5ModelC1Ev,
                      (void *)_ZN5ModelD1Ev);
        func_020733a8(c + 0x40c, 7, 0x28, (void *)_ZN11ShadowModelC1Ev,
                      (void *)_ZN11ShadowModelD1Ev);
        func_020733a8(c + 0x524, 7, 0xc, (void *)func_0203d384,
                      (void *)func_020072c0);
        func_020733a8(c + 0x578, 7, 0xc, (void *)func_0203d384,
                      (void *)func_020072c0);
    }
    return c;
}

}  /* extern "C" */
