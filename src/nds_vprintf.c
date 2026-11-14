/* The va_list half of nds_printf: nds_printf, its only caller, builds the
 * argument pointer and passes it here. The text is formatted into the shared
 * 0x100-byte buffer at data_0209cde8 by MSL's vsnprintf
 * (src/cstd4.cpp: buffer, length, format, args; it NUL-terminates within the
 * length), then drawn with nds_print. */
#include "types.h"
extern int _Z9vsnprintfPcmPKcS_(char *s, size_t n, const char *format, char *arg);
extern void nds_print(unsigned short *dst, const char *str);
extern char data_0209cde8[]; /* 0x0209cde8 */
void nds_vprintf(void *dst, const char *fmt, char *args) {
    _Z9vsnprintfPcmPKcS_(data_0209cde8, 0x100, fmt, args);
    nds_print(dst, data_0209cde8);
}
