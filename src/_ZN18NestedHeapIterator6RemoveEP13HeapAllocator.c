typedef unsigned int u32;
typedef unsigned short u16;

struct NHI { void* head; void* tail; u16 count; u16 off; };

void _ZN18NestedHeapIterator6RemoveEP13HeapAllocator(struct NHI* self, void* node)
{
    u16 off = self->off;
    void* prev = *(void**)((char*)node + off);
    if (prev == 0) {
        self->head = *(void**)((char*)node + off + 4);
    } else {
        *(void**)((char*)prev + off + 4) = *(void**)((char*)node + off + 4);
    }
    void* next = *(void**)((char*)node + off + 4);
    if (next == 0) {
        self->tail = *(void**)((char*)node + off);
    } else {
        u16 off2 = self->off;
        *(void**)((char*)next + off2) = *(void**)((char*)node + off);
    }
    *(void**)((char*)node + off) = 0;
    *(void**)((char*)node + off + 4) = 0;
    *(u16 *)(((u32)self + 8) & 0xFFFFFFFFFFFFFFFFULL) -= 1;
}
