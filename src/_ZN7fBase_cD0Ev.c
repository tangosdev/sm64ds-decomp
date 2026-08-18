/* fBase_c::~fBase_c (deleting / D0) at 0x02043d78
 *
 * fBase_c is the root class; no base dtor to chain to. It destroys its two
 * ProcessingListNode members (renderNode @0x38, behavNode @0x28, reverse order)
 * then frees itself.
 *   [this+0] = data_02099edc (0x02099edc)
 *   ProcessingListNode::~ProcessingListNode(this+0x38)   (0x020440e8)
 *   ProcessingListNode::~ProcessingListNode(this+0x28)
 *   Memory::Deallocate(this, Memory::gameHeapPtr)        (0x0203c1e8)
 * returns this.
 */

struct Heap;

struct fBase_c {
    void **vtable;          /* 0x00 */
    char pad0[0x28 - 4];
    char behavNode[0x10];   /* 0x28 */
    char renderNode[0x10];  /* 0x38 */
};

extern void *data_02099edc[];

extern void func_020440e8(void *node);   /* 0x020440e8 */
extern void _ZN6Memory10DeallocateEPvP4Heap(void *ptr, struct Heap *heap); /* 0x0203c1e8 */
extern struct Heap *data_020a0eac;          /* 0x020a0eac */

struct fBase_c *_ZN7fBase_cD0Ev(struct fBase_c *thiz)
{
    thiz->vtable = (void **)data_02099edc;
    func_020440e8(thiz->renderNode);
    func_020440e8(thiz->behavNode);
    _ZN6Memory10DeallocateEPvP4Heap(thiz, data_020a0eac);
    return thiz;
}
