typedef unsigned int u32; typedef signed int s32;
struct State { u32 id; void* head; void* tail; struct State* prev; };
s32 _ZN18SolidHeapAllocator9LoadStateEj(void* self, u32 id)
{
    struct State* st;
    void* base = (char*)self + 0x24;
    st = *(struct State**)((char*)base + 8);
    if (id != 0) {
        while (st != 0) {
            if (st->id == id) break;
            st = st->prev;
        }
    }
    if (st == 0) return 0;
    *(void**)base = st->head;
    *(void**)((char*)base + 4) = st->tail;
    *(void**)((char*)base + 8) = st->prev;
    return 1;
}
