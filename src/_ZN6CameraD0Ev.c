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
extern void _ZN9ActorBaseD2Ev(struct Camera *self);                 /* _ZN9ActorBaseD2Ev */
extern void _ZN6Memory10DeallocateEPvP4Heap(void *ptr, struct Heap *heap);    /* _ZN6Memory10DeallocateEPvP4Heap */

extern void *_ZTV6Camera;        /* _ZTV6Camera        */
extern void *_ZTV4View;          /* _ZTV4View          */
extern void *data_0208e4b8;  /* _ZTV12ActorDerived */
extern struct Heap *data_020a0eac; /* _ZN6Memory11gameHeapPtrE */

struct Camera *_ZN6CameraD0Ev(struct Camera *self) {
    self->vtable = &_ZTV6Camera;
    self->vtable = &_ZTV4View;
    self->vtable = &data_0208e4b8;
    _ZN9ActorBaseD2Ev(self);
    _ZN6Memory10DeallocateEPvP4Heap(self, data_020a0eac);
    return self;
}