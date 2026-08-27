#include "types.h"
extern void func_0201473c(void *arg0, u32 fmtAddr, u32 val);

void nds_printf(void *arg0, u32 fmt, ...)
{
    u32 ap = (((u32)&fmt) & ~3u) + 4;
    func_0201473c(arg0, fmt, ap);
}
