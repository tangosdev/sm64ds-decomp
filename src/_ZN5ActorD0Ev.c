/* Actor::~Actor (deleting / D0) at 0x02011314
 *
 * Inheritance: Actor : ActorDerived : ActorBase
 * Virtual deleting destructor (CodeWarrior 1.2):
 *   - write Actor vtable to [this]
 *   - run two Actor-subobject teardown calls operating on (this + 0x50)
 *     (the Actor-specific members begin at 0x50)
 *   - write ActorDerived vtable to [this]
 *   - call the immediate base destructor ActorBase::~ActorBase
 *   - Memory::Deallocate(this, *gameHeapPtr)
 *   - return this
 *
 * Pool literals (relocations, wildcarded by matcher):
 *   0x0208e3a4 = data_0208e3a4
 *   0x0209b468 = global passed as first arg to the 0x0203b27c teardown
 *   0x0208e4b8 = data_0208e4b8
 *   0x020a0eac = Memory::gameHeapPtr
 * Calls:
 *   0x0203b27c = teardown(global, &actorSub)   (unnamed)
 *   0x02044104 = teardown2(&actorSub)          (unnamed)
 *   0x02043d48 = _ZN9ActorBaseD2Ev
 *   0x0203c1e8 = Memory::Deallocate(void*, Heap*)
 */

struct Heap;

struct Actor {
    void *vtable; /* 0x0 */
    /* Actor-specific subobject begins at 0x50 */
};

extern void *data_0208e3a4[];
extern void *data_0208e4b8[];

extern void func_0203b27c(void *global, void *actorSub);   /* 0x0203b27c */
extern void func_02044104(void *actorSub);                 /* 0x02044104 */
extern void _ZN9ActorBaseD2Ev(struct Actor *thiz);           /* 0x02043d48 */
extern void _ZN6Memory10DeallocateEPvP4Heap(void *ptr, struct Heap *heap); /* 0x0203c1e8 */

extern void *data_0209b468;                           /* 0x0209b468 */
extern struct Heap *data_020a0eac;                      /* 0x020a0eac */

struct Actor *_ZN5ActorD0Ev(struct Actor *thiz)
{
    thiz->vtable = (void *)data_0208e3a4;
    func_0203b27c(&data_0209b468, (char *)thiz + 0x50);
    func_02044104((char *)thiz + 0x50);
    thiz->vtable = (void *)data_0208e4b8;
    _ZN9ActorBaseD2Ev(thiz);
    _ZN6Memory10DeallocateEPvP4Heap(thiz, data_020a0eac);
    return thiz;
}
