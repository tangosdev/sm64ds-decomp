#include "types.h"
extern void func_02057ce8(void *arg0, u32 fmtAddr, u32 val);

void func_02057d00(void *arg0, u32 fmt, ...)
{
    u32 ap = (((u32)&fmt) & ~3u) + 4;
    func_02057ce8(arg0, fmt, ap);
}
