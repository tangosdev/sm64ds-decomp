//cpp
#include "types.h"
#include "CP15.h"

extern "C" {

typedef struct {
    u32   unk0;
    void *ptr;      /* +4 */
    u32   start;    /* +8 */
    u32   size;     /* +c */
} CodeRegion;

extern void MultiStore_Int(u32 val, void *dst, u32 size);

void func_02017fd0(CodeRegion *rgn)
{
    u32   start;
    u32   siz;
    void *ptr;
    u32   len;
    volatile u32 fillVal;

    start = rgn->start;
    siz   = rgn->size;
    ptr   = rgn->ptr;
    len   = start + siz;

    CP15::InvalidateInstructionCache((u32)ptr, len);
    CP15::InvalidateDataCache((u32)ptr, len);

    fillVal = 0xe6000010;
    MultiStore_Int(fillVal, ptr, len);
}
}
