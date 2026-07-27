/* View::~View (deleting / D0) at 0x0202fcc8
 *
 * vtable chain View -> ActorDerived:
 *   0x02092720 = _ZTV4View
 *   0x0208e4b8 = _ZTV12ActorDerived
 *   bl 0x02043d48 = ActorBase::~ActorBase
 *   bl 0x0203c1e8 = Memory::Deallocate(this, *gameHeapPtr)
 *   return this;
 */

struct View { void **vtable; /* 0x0 */ };
struct Heap;

extern void *_ZTV4View[];
extern void *_ZTV12ActorDerived[];

extern void _ZN9ActorBaseD2Ev(struct View *self);              /* 0x02043d48 */
extern void _ZN6Memory10DeallocateEPvP4Heap(void *ptr, struct Heap *heap); /* 0x0203c1e8 */
extern struct Heap *_ZN6Memory11gameHeapPtrE;                  /* 0x020a0eac */

struct View *_ZN4ViewD0Ev(struct View *self)
{
    self->vtable = (void **)_ZTV4View;
    self->vtable = (void **)_ZTV12ActorDerived;
    _ZN9ActorBaseD2Ev(self);
    _ZN6Memory10DeallocateEPvP4Heap(self, _ZN6Memory11gameHeapPtrE);
    return self;
}
