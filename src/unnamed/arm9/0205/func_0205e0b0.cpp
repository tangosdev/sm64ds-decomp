//cpp
#include "types.h"
#include "CP15.h"

extern "C" {

struct CacheInfo {
    u32 unk0;
    u32 base;
    u32 start;
    u32 size;
};

void func_0205a588(u32 dst, u32 val, u32 size);

void func_0205e0b0(struct CacheInfo *info) {
    u32 r6 = info->base;
    u32 r5 = info->start;
    u32 r1 = info->size;
    u32 r4 = r5 + r1;
    CP15::InvalidateInstructionCache(r6, r4);
    CP15::InvalidateDataCache(r6, r4);
    func_0205a588(r6 + r5, 0, r4 - r5);
}
}
