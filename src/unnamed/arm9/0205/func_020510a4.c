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
extern void _ZN18SolidHeapAllocator9LoadStateEj(int alloc, unsigned int n);
extern void _ZN18SolidHeapAllocator9SaveStateEj(int alloc, unsigned int n);
extern void func_02050fb0(void);
extern void func_02050fd4(void *this);
extern void func_02051244(int *this);

void func_020510a4(int *this, int n)
{
    struct Node *a;
    struct Node *first;

    a = 0;
    if (n == 0) {
        func_02051244(this);
        return;
    }
    if (n < *(unsigned short *)((char *)this + 0xc)) {
        do {
            first = _ZN18NestedHeapIterator8PreviousEP13HeapAllocator((char *)this + 4, 0);
            a = _ZN18NestedHeapIterator8PreviousEP13HeapAllocator(first, a);
            while (a != 0) {
                if (a->fc != 0) {
                    a->fc(&a->body, a->f8, a->f10, a->f14);
                }
                a = _ZN18NestedHeapIterator8PreviousEP13HeapAllocator(first, a);
            }
            _ZN18NestedHeapIterator6RemoveEP13HeapAllocator((char *)this + 4, first);
        } while (n < *(unsigned short *)((char *)this + 0xc));
    }
    _ZN18SolidHeapAllocator9LoadStateEj(this[0], n);
    func_02050fb0();
    _ZN18SolidHeapAllocator9SaveStateEj(this[0], *(unsigned short *)((char *)this + 0xc));
    func_02050fd4(this);
}
