struct dBase_c { void **vtable; };
struct Heap;
extern void *_ZTV7dBase_c[];   /* 0x0208e4b8 */
extern void _ZN7fBase_cD2Ev(struct dBase_c *thiz);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *ptr, struct Heap *heap);
extern struct Heap *data_020a0eac;

struct dBase_c *_ZN7dBase_cD0Ev(struct dBase_c *thiz)
{
    thiz->vtable = (void **)_ZTV7dBase_c;
    _ZN7fBase_cD2Ev(thiz);
    _ZN6Memory10DeallocateEPvP4Heap(thiz, data_020a0eac);
    return thiz;
}
