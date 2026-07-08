//cpp
struct Heap;
extern "C" void *_ZN6Memory8AllocateEjiP4Heap(unsigned int size, int align, Heap *heap);
extern "C" void func_0206e2f8(void *p, int a, unsigned int size);

extern Heap *_ZN6Memory11gameHeapPtrE;

extern "C" void *_ZN9ActorBasenwEj(unsigned int size)
{
    void *p = _ZN6Memory8AllocateEjiP4Heap(size, -4, _ZN6Memory11gameHeapPtrE);
    if (!p)
        return 0;
    func_0206e2f8(p, 0, size);
    return p;
}
