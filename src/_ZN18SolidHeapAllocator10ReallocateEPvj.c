typedef unsigned int u32;
typedef signed int s32;
typedef unsigned short u16;
typedef unsigned char u8;

extern void MultiStore_Int(s32 val, void* dst, u32 count);

u32 _ZN18SolidHeapAllocator10ReallocateEPvj(void* c, void* ptr, u32 size)
{
    void** fl = (void**)((char*)c + 0x24);
    if (size == 0) size = 1;
    size = (size + 3) & ~3;
    void* begin = fl[0];
    u32 newEnd = size + (u32)ptr;
    u32 curSize = (u32)begin - (u32)ptr;
    if (size == curSize) {
        return size;
    }
    if (size > curSize) {
        if ((s32)(newEnd - (u32)fl[1]) > 0) {
            return 0;
        }
        u32 flags = *(u32*)((char*)c + 0x20);
        if ((u16)(flags & 0xff) & 1) {
            volatile s32 zero = 0;
            MultiStore_Int(zero, begin, size - curSize);
        }
    }
    fl[0] = (void*)newEnd;
    return size;
}
