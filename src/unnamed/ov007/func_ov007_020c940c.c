#include "types.h"
#include "Ov007PaletteBuf.h"

extern void MIi_CpuCopy16(const void *src, void *dst, int nbytes);
extern void MIi_CpuClear16(unsigned short val, char *dst, int nbytes);

/* Reloads the buffer's colours from its source, or clears them when it has
 * none. */
void func_ov007_020c940c(Ov007PaletteBuf *buf) {
    if (buf->source != 0) {
        MIi_CpuCopy16(buf->source, buf->colors, 0x40);
    } else {
        volatile unsigned short tmp = 0;
        MIi_CpuClear16(tmp, (char *)buf->colors, 0x40);
    }
}
