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
 *   0x0208e3a4 = _ZTV5Actor
 *   0x0209b468 = global passed as first arg to the 0x0203b27c teardown
 *   0x0208e4b8 = _ZTV12ActorDerived
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

extern void *_ZTV5Actor[];
extern void *_ZTV12ActorDerived[];

extern void Actor_TeardownA(void *global, void *actorSub);   /* 0x0203b27c */
extern void Actor_TeardownB(void *actorSub);                 /* 0x02044104 */
extern void _ZN9ActorBaseD2Ev(struct Actor *thiz);           /* 0x02043d48 */
extern void Memory_Deallocate(void *ptr, struct Heap *heap); /* 0x0203c1e8 */

extern void *Actor_TeardownGlobal;                           /* 0x0209b468 */
extern struct Heap *Memory_gameHeapPtr;                      /* 0x020a0eac */

struct Actor *_ZN5ActorD0Ev(struct Actor *thiz)
{
    thiz->vtable = (void *)_ZTV5Actor;
    Actor_TeardownA(&Actor_TeardownGlobal, (char *)thiz + 0x50);
    Actor_TeardownB((char *)thiz + 0x50);
    thiz->vtable = (void *)_ZTV12ActorDerived;
    _ZN9ActorBaseD2Ev(thiz);
    Memory_Deallocate(thiz, Memory_gameHeapPtr);
    return thiz;
}
