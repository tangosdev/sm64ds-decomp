struct ActorDerived { void **vtable; };
struct Heap;
extern void *_ZTV12ActorDerived[];   /* 0x0208e4b8 */
extern void _ZN9ActorBaseD2Ev(struct ActorDerived *thiz);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *ptr, struct Heap *heap);
extern struct Heap *data_020a0eac;

struct ActorDerived *_ZN12ActorDerivedD0Ev(struct ActorDerived *thiz)
{
    thiz->vtable = (void **)_ZTV12ActorDerived;
    _ZN9ActorBaseD2Ev(thiz);
    _ZN6Memory10DeallocateEPvP4Heap(thiz, data_020a0eac);
    return thiz;
}
