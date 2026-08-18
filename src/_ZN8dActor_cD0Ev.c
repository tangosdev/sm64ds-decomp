/* dActor_c::~dActor_c (deleting / D0) at 0x02011314
 *
 * Inheritance: dActor_c : dBase_c : fBase_c
 * Virtual deleting destructor (CodeWarrior 1.2):
 *   - write dActor_c vtable to [this]
 *   - run two dActor_c-subobject teardown calls operating on (this + 0x50)
 *     (the dActor_c-specific members begin at 0x50)
 *   - write dBase_c vtable to [this]
 *   - call the immediate base destructor fBase_c::~fBase_c
 *   - Memory::Deallocate(this, *gameHeapPtr)
 *   - return this
 *
 * Pool literals (relocations, wildcarded by matcher):
 *   0x0208e3a4 = _ZTV8dActor_c
 *   0x0209b468 = global passed as first arg to the 0x0203b27c teardown
 *   0x0208e4b8 = _ZTV7dBase_c
 *   0x020a0eac = Memory::gameHeapPtr
 * Calls:
 *   0x0203b27c = teardown(global, &actorSub)   (unnamed)
 *   0x02044104 = teardown2(&actorSub)          (unnamed)
 *   0x02043d48 = _ZN7fBase_cD2Ev
 *   0x0203c1e8 = Memory::Deallocate(void*, Heap*)
 */

struct Heap;

struct dActor_c {
    void *vtable; /* 0x0 */
    /* dActor_c-specific subobject begins at 0x50 */
};

extern void *_ZTV8dActor_c[];
extern void *_ZTV7dBase_c[];

extern void func_0203b27c(void *global, void *actorSub);   /* 0x0203b27c */
extern void func_02044104(void *actorSub);                 /* 0x02044104 */
extern void _ZN7fBase_cD2Ev(struct dActor_c *thiz);           /* 0x02043d48 */
extern void _ZN6Memory10DeallocateEPvP4Heap(void *ptr, struct Heap *heap); /* 0x0203c1e8 */

extern void *data_0209b468;                           /* 0x0209b468 */
extern struct Heap *data_020a0eac;                      /* 0x020a0eac */

struct dActor_c *_ZN8dActor_cD0Ev(struct dActor_c *thiz)
{
    thiz->vtable = (void *)_ZTV8dActor_c;
    func_0203b27c(&data_0209b468, (char *)thiz + 0x50);
    func_02044104((char *)thiz + 0x50);
    thiz->vtable = (void *)_ZTV7dBase_c;
    _ZN7fBase_cD2Ev(thiz);
    _ZN6Memory10DeallocateEPvP4Heap(thiz, data_020a0eac);
    return thiz;
}
