//cpp
struct HeapAllocator;
struct Node { unsigned short f0[4]; unsigned int f10[2]; unsigned int f18; };

extern "C" {
    void func_0204f934(void **p);
    void func_0204f3e0(unsigned char *c);
    void *func_0204f364(int x);
    void func_0204f460(void *node, void *r5);
}
struct NestedHeapIterator {
    unsigned char *Next(HeapAllocator *a);
};
extern unsigned char _ZN5Sound7PLAYERSE[];

extern "C" void *func_0204f63c(void **p, int idx, int r7)
{
    void *node = &_ZN5Sound7PLAYERSE[idx * 0x1c];
    void *prev = p[0];
    void **r5;
    if (prev != 0) func_0204f934(p);
    if (*(unsigned short*)((char*)node + 8) >= *(unsigned int*)((char*)node + 0x18)) {
        unsigned char *it = ((NestedHeapIterator*)node)->Next(0);
        if (it == 0) return 0;
        if (r7 < (int)it[0x3d]) return 0;
        func_0204f3e0(it);
    }
    r5 = (void**)func_0204f364(r7);
    if (r5 == 0) return 0;
    func_0204f460(node, r5);
    r5[0] = p;
    p[0] = r5;
    return r5;
}
