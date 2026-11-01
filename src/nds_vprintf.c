/* The va_list half of nds_printf: nds_printf, its only caller, builds the
 * argument pointer and passes it here. The text is formatted into the shared
 * 0x100-byte buffer at data_0209cde8 by func_0206e3dc (vsnprintf over the
 * cstd4.cpp formatter: buffer, length, format, args; it NUL-terminates within
 * the length), then drawn with nds_print. */
#include "types.h"
extern unsigned int func_0206e3dc(char *buf, s32 size, u32 fmtAddr, u32 val);
extern void nds_print(void *arg0, char *buf);
extern char data_0209cde8[]; /* 0x0209cde8 */
void nds_vprintf(void *dst, u32 fmt, u32 args) {
    func_0206e3dc(data_0209cde8, 0x100, fmt, args);
    nds_print(dst, data_0209cde8);
}
