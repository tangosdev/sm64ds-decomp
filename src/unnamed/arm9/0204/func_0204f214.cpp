//cpp
extern "C" {
extern void _ZN18NestedHeapIterator6RemoveEP13HeapAllocator(void* self, void* node);
extern void func_0204f460(void* node, void* self);
extern void func_0204f400(void* self);
extern char data_020a4d60[];

void func_0204f214(char* thiz, int b)
{
    void* node = *(void**)(thiz + 4);
    if (node != 0) {
        _ZN18NestedHeapIterator6RemoveEP13HeapAllocator(node, thiz);
        *(void**)(thiz + 4) = 0;
    }
    _ZN18NestedHeapIterator6RemoveEP13HeapAllocator(data_020a4d60, thiz);
    *(unsigned char*)(thiz + 0x3d) = (unsigned char)b;
    if (node != 0) {
        func_0204f460(node, thiz);
    }
    func_0204f400(thiz);
}
}
