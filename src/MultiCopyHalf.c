#include "types.h"

/* Copies size bytes from src to dst one halfword at a time. */
void MultiCopyHalf(const void *src, void *dst, s32 size)
{
    s32 offset = 0;

    for (;;) {
        if (offset >= size)
            break;
        *(u16 *)((u32)dst + offset) = *(const u16 *)((u32)src + offset);
        offset += 2;
    }
}
