extern void Crash(void);

struct Heap {
    void* heapStart;
    unsigned int heapSize;
    Heap* parentHeap;
    unsigned int flags;

    void Rescue();
    unsigned int ResizeToFit();
    /* THE DESTRUCTOR PAIR, SPELLED AS TWO PLAIN VIRTUALS ON PURPOSE. mwccarm
       gives `virtual ~Heap()` TWO vtable entries -- D1 complete and D0
       deleting, the Itanium pair the ROM's _ZTV4Heap carries at 0x02099d90
       (0x0203ca44 and 0x0203ca20) -- and MSVC folds them into ONE. Spelt as a
       destructor this declaration indexed correctly on the ARM and ONE SLOT
       EARLY on the host, so ResizeToFit's own `VResizeToFit()` below compiled
       to `call [eax+38h]` = index 14, which in the sixteen-slot ROM-ordered
       table hal/heap_vtable.cpp seats is VGetNodeID. Every V-method after the
       destructor was off by one the same way. Two ordinary virtuals occupy the
       SAME two entries under mwccarm, so the ROM bytes are untouched
       (_ZN4Heap11ResizeToFitEv at 0x0203c390 size 0x48 still MATCHES 2004/b56
       before and after), and they occupy two under MSVC as well, so the host
       lands where the ARM does. Neither is ever called from here; they hold the
       two slots the ROM's table holds. Same shape as the FaderBrightness fix in
       src/ProcessKuppaScript.cpp -- port/stage_lifecycle_map.txt sections 16
       and 17 are the measurement. */
    virtual void Destructor1();
    virtual void Destructor0();
    virtual void VDestroy() = 0;
    virtual void* VAllocate(unsigned int size, int align) = 0;
    virtual bool VDeallocate(void* ptr) = 0;
    virtual void VDeallocateAll() = 0;
    virtual bool VIntact() = 0;
    virtual void VRescue() = 0;
    virtual unsigned int VReallocate(void* ptr, unsigned int newSize) = 0;
    virtual unsigned int VSizeof(void* ptr) = 0;
    virtual unsigned int VMaxAllocationUnitSize() = 0;
    virtual unsigned int VMaxAllocatableSize() = 0;
    virtual unsigned int VMemoryLeft() = 0;
    virtual unsigned int VSetNodeID(unsigned int id) = 0;
    virtual unsigned int VGetNodeID() = 0;
    virtual unsigned int VResizeToFit() = 0;
};

unsigned int Heap::ResizeToFit()
{
    unsigned int result = VResizeToFit();
    if (!result)
    {
        if (flags & 0x4000)
        {
            Rescue();
            Crash();
        }
    }
    return result;
}
