//cpp
struct HeapAllocator {
    char pad[0x3d];
    unsigned char field_3d;
};

struct NestedHeapIterator {
    HeapAllocator *Next(HeapAllocator *);
    void Remove(HeapAllocator *);
};

extern NestedHeapIterator data_020a4d54;
extern NestedHeapIterator data_020a4d60;

extern "C" void func_0204f3e0(HeapAllocator *);
extern "C" void func_0204f400(HeapAllocator *);

extern "C" HeapAllocator *func_0204f364(int arg)
{
    HeapAllocator *h = data_020a4d54.Next(0);
    if (h == 0) {
        h = data_020a4d60.Next(0);
        int f = h->field_3d;
        if (arg < f)
            return 0;
        func_0204f3e0(h);
    }
    data_020a4d54.Remove(h);
    h->field_3d = (unsigned char)arg;
    func_0204f400(h);
    return h;
}
