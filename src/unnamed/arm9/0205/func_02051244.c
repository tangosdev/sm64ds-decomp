typedef void (*Fn)(void *a, int b, int c, int d);
struct Node {
    char pad8[8];
    int f8;     /* +8 */
    Fn fc;      /* +0xc */
    int f10;    /* +0x10 */
    int f14;    /* +0x14 */
    char pad[8];
    char body;  /* +0x20 */
};
extern struct Node *_ZN18NestedHeapIterator8PreviousEP13HeapAllocator(void *it, void *ha);
extern void _ZN18NestedHeapIterator6RemoveEP13HeapAllocator(void *it, void *ha);
extern void _ZN18SolidHeapAllocator5ResetEj(int alloc, unsigned int n);
extern void func_02050fb0(void);
extern void func_02050fd4(void *this);

void func_02051244(int *this)
{
    struct Node *r7;
    struct Node *r6;
    r7 = _ZN18NestedHeapIterator8PreviousEP13HeapAllocator((char*)this + 4, 0);
    if (r7 == 0) goto end;
    do {
        r6 = _ZN18NestedHeapIterator8PreviousEP13HeapAllocator(r7, 0);
        while (r6 != 0) {
            if (r6->fc != 0) {
                r6->fc(&r6->body, r6->f8, r6->f10, r6->f14);
            }
            r6 = _ZN18NestedHeapIterator8PreviousEP13HeapAllocator(r7, r6);
        }
        _ZN18NestedHeapIterator6RemoveEP13HeapAllocator((char*)this + 4, r7);
        r7 = _ZN18NestedHeapIterator8PreviousEP13HeapAllocator((char*)this + 4, 0);
    } while (r7 != 0);
end:
    _ZN18SolidHeapAllocator5ResetEj(this[0], 3);
    func_02050fb0();
    func_02050fd4(this);
}
