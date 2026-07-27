/* Scene::~Scene (deleting / D0) at 0x0202e170
 *
 * vtable chain Stage -> ActorDerived:
 *   0x02092680 = _ZTV5Stage
 *   0x0208e4b8 = data_0208e4b8
 *   bl 0x02043d48 = ActorBase::~ActorBase
 *   bl 0x0203c1e8 = Memory::Deallocate(this, *gameHeapPtr)
 *   return this;
 */

struct Scene { void **vtable; /* 0x0 */ };
struct Heap;

extern void *_ZTV5Stage[];
extern void *data_0208e4b8[];

extern void _ZN9ActorBaseD2Ev(struct Scene *self);              /* 0x02043d48 */
extern void _ZN6Memory10DeallocateEPvP4Heap(void *ptr, struct Heap *heap); /* 0x0203c1e8 */
extern struct Heap *data_020a0eac;                   /* 0x020a0eac */

struct Scene *_ZN5SceneD0Ev(struct Scene *self)
{
    self->vtable = (void **)_ZTV5Stage;
    self->vtable = (void **)data_0208e4b8;
    _ZN9ActorBaseD2Ev(self);
    _ZN6Memory10DeallocateEPvP4Heap(self, data_020a0eac);
    return self;
}