#include "types.h"
#include "ds_width.h"
extern void nds_vprintf(void *dst, const char *fmt, char *args);

/* The variable arguments start on the stack word after fmt (va_start). */
#ifndef SM64DS_HOST_64
void nds_printf(void *dst, const char *fmt, ...) {
    char *args = (char *)(((u32)&fmt & ~3u) + 4);
    nds_vprintf(dst, fmt, args);
}
#else
#include <stdarg.h>
void nds_printf(void *dst, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    nds_vprintf(dst, fmt, (char *)ap);
    va_end(ap);
}
#endif
