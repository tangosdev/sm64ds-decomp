typedef unsigned int u32;
typedef signed int s32;
struct H { void* begin; void* end; };
s32 _ZN18SolidHeapAllocator14TryResizeToFitEv(void* self)
{
    struct H* base = (struct H*)((char*)self + 0x24);
    void* cur = *(void**)((char*)self + 0x1c);
    if ((s32)((char*)cur - (char*)base->end) != 0) return 0;
    *(void**)((char*)self + 0x1c) = base->begin;
    base->end = *(void**)((char*)self + 0x1c);
    return (s32)(*(char**)((char*)self + 0x1c) - (char*)self);
}
