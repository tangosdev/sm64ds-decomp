//cpp
extern "C" {

struct Target { int start; int end; };

extern void _ZN10MemoryNode6TargetC1EP10MemoryNode(struct Target* thiz, void* node);
extern void* _ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_(void* c, void* node);
extern void* _ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt(struct Target* t, unsigned short tt);
extern void* _ZN22ExpandingHeapAllocator8LinkNodeEP10MemoryNodeS1_S1_(void* c, void* node, void* link);
extern void MultiStore_Int(int val, int* dst, int len);

void* _ZN22ExpandingHeapAllocator12AllocateNodeEP10MemoryNodeS1_Pvjj(void* c, void* node, void* target, unsigned int size, unsigned short z)
{
    struct Target t0;
    struct Target t1;
    struct Target t2;
    void* link;
    int header;
    _ZN10MemoryNode6TargetC1EP10MemoryNode(&t0, node);

    int oldLimit = t0.end;
    header = (int)target - 0x10;
    int backStart = (int)size + (int)target;
    t0.end = header;
    t1.end = oldLimit;
    t1.start = backStart;

    link = _ZN22ExpandingHeapAllocator10UnlinkNodeEP10MemoryNodeS1_(c, node);

    unsigned int frontGap = t0.end - t0.start;
    if (frontGap < 0x10) {
        t0.end = t0.start;
    } else {
        void* newFront = _ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt(&t0, 0x4652);
        link = _ZN22ExpandingHeapAllocator8LinkNodeEP10MemoryNodeS1_S1_(c, newFront, link);
    }

    unsigned int backGap = t1.end - t1.start;
    if (backGap < 0x10) {
        t1.start = t1.end;
    } else {
        void* newBack = _ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt(&t1, 0x4652);
        link = _ZN22ExpandingHeapAllocator8LinkNodeEP10MemoryNodeS1_S1_(c, newBack, link);
    }

    int flagsWord = *(int*)((char*)c - 4);
    int* dst = (int*)t0.end;
    int len = t1.start - t0.end;
    if (((unsigned short)(flagsWord & 0xff)) & 1) {
        volatile int zero = 0;
        MultiStore_Int(zero, dst, len);
    }

    t2.start = header;
    t2.end = t1.start;
    void* allocNode = _ZN22ExpandingHeapAllocator10CreateNodeEPN10MemoryNode6TargetEt(&t2, 0x5544);

    unsigned short* flagsPtr = (unsigned short*)((long long)(int)((char*)allocNode + 2) & 0xFFFFFFFFFFFFFFFFLL);
    void* usedList = (char*)c + 8;
    *flagsPtr &= ~0x8000;
    *flagsPtr |= (z & 1) << 15;
    int t0e = t0.end;
    *flagsPtr &= ~0x7f00;
    *flagsPtr |= (((unsigned short)((int)allocNode - t0e)) & 0x7f) << 8;
    unsigned int cRaw = *(unsigned short*)((char*)c + 0x10);
    unsigned int cValue = cRaw & 0xff;
    *flagsPtr &= ~0xff;
    *flagsPtr |= cValue;

    void* usedTail = (void*)(*(int*)((char*)c + 0xc));
    _ZN22ExpandingHeapAllocator8LinkNodeEP10MemoryNodeS1_S1_(usedList, allocNode, usedTail);

    return target;
}

}
