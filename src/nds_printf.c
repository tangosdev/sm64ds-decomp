#include "types.h"
extern void nds_vprintf(void *dst, const char *fmt, char *args);

/* The variable arguments start on the stack word after fmt (va_start). */
void nds_printf(void *dst, const char *fmt, ...) {
    char *args = (char *)(((uintptr_t)&fmt & ~3u) + 4);
    nds_vprintf(dst, fmt, args);
}
