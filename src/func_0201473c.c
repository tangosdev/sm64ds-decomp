#include "types.h"
extern s32 func_0206e3dc(char *buf, s32 size, u32 fmtAddr, u32 val);
extern void nds_print(void *arg0, char *buf);
extern char gPrintBuf[]; /* 0x0209cde8 */
void func_0201473c(void *arg0, u32 fmtAddr, u32 val)
{
    func_0206e3dc(gPrintBuf, 0x100, fmtAddr, val);
    nds_print(arg0, gPrintBuf);
}
