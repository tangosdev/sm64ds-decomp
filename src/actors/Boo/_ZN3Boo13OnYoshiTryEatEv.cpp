//cpp
// @symbol _ZN3Boo13OnYoshiTryEatEv
/* Boo::OnYoshiTryEat (slot 18, include/dActor_c.h) - name recovered from the
   vtable slot it fills. Tests the actor-ID "kind" field (fBase_c +0xc) for
   0xd1 -- the same constant Boo::CleanupResources checks -- and returns 7 for
   that specific Boo variant, 0 otherwise. */
extern "C" int _ZN3Boo13OnYoshiTryEatEv(void *c) {
    unsigned short v = *(unsigned short*)((char*)c + 0xc);
    int r;
    if (v == 0xd1) r = 1; else r = 0;
    if (r != 0) r = 7; else r = 0;
    return r;
}
