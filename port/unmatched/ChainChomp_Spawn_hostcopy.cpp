/* HOST COPY of src/game/actors/d_a_wanwan.cpp -- an ARM r0 RIDE-THROUGH in a spawn
 * factory, which is a new place for the port to meet one.
 *
 * The matched source spells Enemy's constructor
 *
 *     void func_020aed98(void);          // no parameter
 *     ...
 *     char *c = (char *)_ZN7fBase_cnwEj(0x620);
 *     if (c) { func_020aed98(); *(int **)c = _ZTV10daWanwan_c; ... }
 *
 * and that is correct ARM: operator new returned the object in r0 and the
 * constructor wants `this` in r0, so mwcc emits the branch with nothing in
 * between and the C never has to name the argument. The reloc at 0x02112d34
 * settles which function it is -- ov002 0x020aed98, _ZN12dEnemyBase_cC2Ev, the one
 * gate 16 brought in for the 1-up mushroom. (dsd names ov007 0x020aed98 as
 * well because the two overlays share a base; that one is 0x1e0 bytes and is
 * not a constructor. Every other Enemy factory in src/ -- daBmb_c_classInit,
 * daRNk_c_classInit, daBombking_c_classInit -- calls the same address and spells
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

void *_ZN7fBase_cnwEj(unsigned size);
int _ZN12dEnemyBase_cC2Ev(void *self);
int _ZN10dCcAcPos_cC1Ev(void *self);
int _ZN9ModelAnimC1Ev(void *self);
int _ZN11ShadowModelC1Ev(void *self);
int __cxa_vec_ctor(void *arr, int n, int stride, void *ctor, void *dtor);
void _ZN5ModelC1Ev(void);
void _ZN5ModelD1Ev(void);
void _ZN11ShadowModelD1Ev(void);
void _ZN7Vector3D1Ev(void);
void func_0203d384(void);
extern int _ZTV10daWanwan_c[];

// PORT_HOST_ABI: matched src calls the Enemy constructor argless because operator new left this in r0 on ARM; MSVC cdecl passes nothing and faults the constructor, so this is passed explicitly
void *daWanwan_c_classInit(void)
{
    char *c = (char *)_ZN7fBase_cnwEj(0x620);
    if (c) {
        _ZN12dEnemyBase_cC2Ev(c);
        *(int **)c = _ZTV10daWanwan_c;
        _ZN10dCcAcPos_cC1Ev(c + 0x110);
        _ZN9ModelAnimC1Ev(c + 0x150);
        _ZN11ShadowModelC1Ev(c + 0x1b4);
        __cxa_vec_ctor(c + 0x1dc, 7, 0x50, (void *)_ZN5ModelC1Ev,
                      (void *)_ZN5ModelD1Ev);
        __cxa_vec_ctor(c + 0x40c, 7, 0x28, (void *)_ZN11ShadowModelC1Ev,
                      (void *)_ZN11ShadowModelD1Ev);
        __cxa_vec_ctor(c + 0x524, 7, 0xc, (void *)func_0203d384,
                      (void *)_ZN7Vector3D1Ev);
        __cxa_vec_ctor(c + 0x578, 7, 0xc, (void *)func_0203d384,
                      (void *)_ZN7Vector3D1Ev);
    }
    return c;
}

}  /* extern "C" */
