#include "types.h"
#include "Ov007PaletteBuf.h"
#include "nitro/mi/memory.h"

/* Reloads the buffer's colours from its source, or clears them when it has
 * none. */
void func_ov007_020c940c(Ov007PaletteBuf *buf) {
    if (buf->source != 0) {
        MI_CpuCopy16(buf->source, buf->colors, 0x40);
    } else {
        volatile unsigned short tmp = 0;
        MI_CpuFill16(buf->colors, tmp, 0x40);
    }
}
