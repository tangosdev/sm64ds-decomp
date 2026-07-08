typedef unsigned int u32;

extern void *func_020511d4(void *a, u32 elemSize, u32 p2, u32 p3, u32 p4);
extern void *func_0205130c(unsigned int addr, unsigned int size);
extern void _ZN18NestedHeapIterator7AddLastEP13HeapAllocator(void *iter, void *alloc);
extern void func_0204f278(void);
extern char _ZN5Sound7PLAYERSE[];

int func_0204fa3c(int owner, void *b, u32 c)
{
    char *r4;
    void *res;
    r4 = (char *)func_020511d4(b, c + 0x14, (u32)func_0204f278, 0, 0);
    if (!r4) return 0;
    *(int *)(r4 + 0xc) = 0;
    *(int *)(r4 + 0x10) = owner;
    *(int *)(r4 + 8) = 0;
    res = func_0205130c((unsigned int)(r4 + 0x14), c);
    if (!res) return 0;
    *(void **)(r4 + 8) = res;
    _ZN18NestedHeapIterator7AddLastEP13HeapAllocator(
        _ZN5Sound7PLAYERSE + owner * 0x1c + 0xc, r4);
    return 1;
}
