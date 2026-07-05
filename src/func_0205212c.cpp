//cpp
typedef unsigned int u32;

struct HeapAllocator;

struct IRQ {
    static u32 Disable();
    static void Restore(u32);
};

struct NestedHeapIterator {
    HeapAllocator *Next(HeapAllocator *);
    void Remove(HeapAllocator *);
};

extern "C" {
void func_020520dc(void *self);
void func_0205d4cc(void *p);
void func_020520a4(HeapAllocator *node);
}

struct HeapAllocator {
    char pad0[8];
    void *owner_8;   /* 0x8 */
};

struct Obj {
    char pad0[0x50];
    u32 u50;          /* 0x50 */
    char pad1[0xf0 - 0x54];
    u32 uf0;          /* 0xf0 */
    char pad2[0x114 - 0xf4];
    int *u114;        /* 0x114 */
};

extern NestedHeapIterator data_020a5bc8;

extern "C" void func_0205212c(Obj *self) {
    if (self->u114 != 0) {
        *self->u114 = 0;
        self->u114 = 0;
    }
    func_020520dc(self);
    int b = (int)((self->u50 & 0x10) != 0);
    if (b != 0) {
        func_0205d4cc((char*)self + 0x44);
    }
    u32 irq = IRQ::Disable();
    HeapAllocator *node = data_020a5bc8.Next(0);
    while (node != 0) {
        HeapAllocator *next = data_020a5bc8.Next(node);
        if (node->owner_8 == self) {
            data_020a5bc8.Remove(node);
            func_020520a4(node);
        }
        node = next;
    }
    IRQ::Restore(irq);
    u32 *p = (u32*)(((int)self + 0xf0) & 0xFFFFFFFFFFFFFFFF);
    *p &= ~1u;
    *p &= ~4u;
    *p &= ~2u;
}
