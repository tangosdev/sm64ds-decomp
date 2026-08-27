extern void *_ZN4Heap24CreateSolidHeapAllocatorEPvjj(void *p, unsigned int sz, unsigned int z);
extern int func_02051024(void *self, void *alloc);
extern void func_0204ebb8(void *alloc);

void *func_0205130c(unsigned int addr, unsigned int size)
{
    unsigned int end = addr + size;
    unsigned int aligned = (addr + 3) & ~3u;
    void *alloc;
    if (aligned > end)
        return 0;
    if (end - aligned < 0x10)
        return 0;
    alloc = _ZN4Heap24CreateSolidHeapAllocatorEPvjj((void *)(aligned + 0x10),
                                                    (end - aligned) - 0x10, 0);
    if (alloc == 0)
        return 0;
    if (func_02051024((void *)aligned, alloc) != 0)
        return (void *)aligned;
    func_0204ebb8(alloc);
    return 0;
}
