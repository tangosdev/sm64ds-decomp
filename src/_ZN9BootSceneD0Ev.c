// NONMATCHING: vtable-pool loads emit in source order; ROM interleaves the two vtable stores differently (2-slot ordering divergence, likely the pool-ordering floor). Byte-correct except those 2 slots. Was a false match (referenced _ZTV* names absent from config, so linkcheck was blind).
/* _ZN9BootSceneD0Ev (deleting / D0) at 0x020235d4
 *
 * This is the BootScene flavor of the Scene deleting dtor; vtable chain
 * (from the literal pool) is BootScene -> Scene -> ActorDerived:
 *   0x02091528 = data_02091528
 *   0x02092680 = data_02092680
 *   0x0208e4b8 = data_0208e4b8
 *   bl 0x02043d48 = ActorBase::~ActorBase
 *   bl 0x0203c1e8 = Memory::Deallocate(this, *gameHeapPtr)
 *   return this;
 */

struct Scene { void **vtable; /* 0x0 */ };
struct Heap;

extern void *data_02091528[];
extern void *data_02092680[];
extern void *data_0208e4b8[];

extern void _ZN9ActorBaseD1Ev(struct Scene *self);              /* 0x02043d48 */
extern void _ZN6Memory10DeallocateEPvP4Heap(void *ptr, struct Heap *heap); /* 0x0203c1e8 */
extern struct Heap *data_020a0eac;                   /* 0x020a0eac */

struct Scene *_ZN9BootSceneD0Ev(struct Scene *self)
{
    self->vtable = (void **)data_02091528;
    self->vtable = (void **)data_02092680;
    self->vtable = (void **)data_0208e4b8;
    _ZN9ActorBaseD1Ev(self);
    _ZN6Memory10DeallocateEPvP4Heap(self, data_020a0eac);
    return self;
}