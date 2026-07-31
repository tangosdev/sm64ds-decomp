// @symbol func_02034ac0
// @emits dScMB_c_OnYoshiTryEat
/* recovered: renamed to Class_Method */
/* dScMB_c::OnYoshiTryEat - recovered from vtable slot identity */
struct MultiBootScene { void **vtable; };
struct Heap;
extern void *_ZTV14MultiBootScene[];
extern void *_ZTV5Scene[];
extern void *_ZTV12ActorDerived[];
extern void _ZN10FaderColorD1Ev(void *thiz);
extern void _ZN9ActorBaseD2Ev(struct MultiBootScene *thiz);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *ptr, struct Heap *heap);
extern struct Heap *_ZN6Memory11gameHeapPtrE;

struct MultiBootScene *dScMB_c_OnYoshiTryEat(struct MultiBootScene *thiz)
{
    thiz->vtable = (void **)_ZTV14MultiBootScene;
    _ZN10FaderColorD1Ev((char *)thiz + 0x50);
    thiz->vtable = (void **)_ZTV5Scene;
    thiz->vtable = (void **)_ZTV12ActorDerived;
    _ZN9ActorBaseD2Ev(thiz);
    _ZN6Memory10DeallocateEPvP4Heap(thiz, _ZN6Memory11gameHeapPtrE);
    return thiz;
}
