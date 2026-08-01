struct ActorDerived { void **vtable; };
struct Heap;
extern void *data_0208e4b8[];
extern void _ZN9ActorBaseD2Ev(struct ActorDerived *thiz);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *ptr, struct Heap *heap);
extern struct Heap *data_020a0eac;

struct ActorDerived *_ZN12ActorDerivedD0Ev(struct ActorDerived *thiz)
{
    thiz->vtable = (void **)data_0208e4b8;
    _ZN9ActorBaseD2Ev(thiz);
    _ZN6Memory10DeallocateEPvP4Heap(thiz, data_020a0eac);
    return thiz;
}
