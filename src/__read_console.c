#include "types.h"
#include "file_struc.h"

extern unsigned char sys_readc(void);

/* MSL __read_console for the debug console: read up to *count bytes into
   buffer through sys_readc, the ARM semihosting SYS_READC call (swi
   0x123456, r0 = 7). A CR or LF ends the read and *count becomes the count
   including it. Returns 0. handle and idle_proc are the standard proc
   parameters; the console ignores both. */
int __read_console(__file_handle handle, unsigned char *buffer, size_t *count,
                   __idle_proc idle_proc) {
    unsigned int i;
    unsigned int n = *count;
    i = 0;
    while (i < n) {
        unsigned char c;
        buffer[i] = sys_readc();
        c = buffer[i];
        if (c == 0xd || c == 0xa) {
            *count = i + 1;
            break;
        }
        i++;
    }
    return 0;
}
