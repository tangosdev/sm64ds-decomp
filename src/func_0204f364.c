typedef unsigned char u8;

extern char data_020a4d54[];
extern char data_020a4d60[];

char* _ZN18NestedHeapIterator4NextEP13HeapAllocator(char* self, char* a);
void _ZN18NestedHeapIterator6RemoveEP13HeapAllocator(char* self, char* node);
void func_0204f3e0(unsigned char* c);
void func_0204f400(char* self);

char* func_0204f364(int key)
{
    char* node = _ZN18NestedHeapIterator4NextEP13HeapAllocator(data_020a4d54, 0);
    if (node == 0) {
        node = _ZN18NestedHeapIterator4NextEP13HeapAllocator(data_020a4d60, 0);
        if (key < *(u8*)(node + 0x3d)) {
            return 0;
        }
        func_0204f3e0((unsigned char*)node);
    }
    _ZN18NestedHeapIterator6RemoveEP13HeapAllocator(data_020a4d54, node);
    *(u8*)(node + 0x3d) = key;
    func_0204f400(node);
    return node;
}
