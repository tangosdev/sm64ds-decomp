//cpp
typedef unsigned int u32;

struct Heap;
struct Obj { void *vtable; };

struct ActorBase {
    void *vtable;
    virtual void AfterInitResources(u32 result);
    void MarkForDestruction();
};

struct ActorDerived : public ActorBase {
    virtual void AfterInitResources(u32 result);
};

extern "C" {
extern void *vtbl_ActorDerived[];
extern void *_ZTV12ActorDerived[];
void base_dtor_ActorDerived(void *);
void _ZN9ActorBaseD2Ev(void *);
void _ZN6Memory10DeallocateEPvP4Heap(void *ptr, Heap *heap);
extern Heap *_ZN6Memory11gameHeapPtrE;
void func_02042ffc(void);
void _ZN9ActorBase18AfterInitResourcesEj(void *, u32);

struct Obj *_ZN12ActorDerivedD1Ev(struct Obj *thiz)
{
    thiz->vtable = (void *)vtbl_ActorDerived;
    base_dtor_ActorDerived(thiz);
    return thiz;
}

ActorDerived *_ZN12ActorDerivedD0Ev(ActorDerived *thiz)
{
    *(void ***)thiz = (void **)_ZTV12ActorDerived;
    _ZN9ActorBaseD2Ev(thiz);
    _ZN6Memory10DeallocateEPvP4Heap(thiz, _ZN6Memory11gameHeapPtrE);
    return thiz;
}

void _ZN12ActorDerived5SpawnEjP9ActorBaseii(void) {
    func_02042ffc();
}
}

void ActorDerived::AfterInitResources(u32 result)
{
    if (result == 1) {
        this->MarkForDestruction();
    }
    _ZN9ActorBase18AfterInitResourcesEj(this, result);
}
