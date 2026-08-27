//cpp
struct HeapAllocator;
extern "C" void func_020512f0(int *p);
extern "C" void _ZN18NestedHeapIterator6RemoveEP13HeapAllocator(HeapAllocator *a, void *self);

struct Elem { char pad[0x1c]; };
extern Elem data_020a4d6c[];

extern "C" void func_0204f278(char *thiz);

void func_0204f278(char *thiz)
{
    int *p = *(int **)(thiz + 8);
    if (p == 0) return;
    func_020512f0(p);
    int *q = *(int **)(thiz + 0xc);
    if (q != 0) {
        q[2] = 0;
        return;
    }
    int idx = *(int *)(thiz + 0x10);
    _ZN18NestedHeapIterator6RemoveEP13HeapAllocator(
        (HeapAllocator *)((char *)&data_020a4d6c[idx] + 0xc), thiz);
}
