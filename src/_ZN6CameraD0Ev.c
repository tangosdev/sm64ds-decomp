/* Camera::~Camera (deleting / D0) - virtual deleting destructor.
 * Resets the vtable pointer down the destruction chain
 * (Camera -> View -> ActorDerived), runs the base destructor,
 * then frees the object through the game heap. Returns `this`. */

/* Minimal object layout: only the vtable pointer at offset 0 is touched here. */
struct Camera {
    void *vtable; /* 0x0 */
};

/* Heap handle type (opaque). */
struct Heap;

/* Reference symbols resolved from symbols/verified.tsv:
 *   0x02043d48 = ActorBase::~ActorBase  (base / complete-object destructor)
 *   0x0203c1e8 = Memory::Deallocate(void*, Heap*)
 *   0x020a0eac = Memory::gameHeapPtr    (Heap*)
 * The three vtable literals are _ZTV6Camera / _ZTV4View / _ZTV12ActorDerived. */
extern void ActorBaseDtor(struct Camera *self);                 /* _ZN9ActorBaseD2Ev */
extern void Memory_Deallocate(void *ptr, struct Heap *heap);    /* _ZN6Memory10DeallocateEPvP4Heap */

extern void *Camera_vtable;        /* _ZTV6Camera        */
extern void *View_vtable;          /* _ZTV4View          */
extern void *ActorDerived_vtable;  /* _ZTV12ActorDerived */
extern struct Heap *Memory_gameHeapPtr; /* _ZN6Memory11gameHeapPtrE */

struct Camera *_ZN6CameraD0Ev(struct Camera *self) {
    self->vtable = &Camera_vtable;
    self->vtable = &View_vtable;
    self->vtable = &ActorDerived_vtable;
    ActorBaseDtor(self);
    Memory_Deallocate(self, Memory_gameHeapPtr);
    return self;
}