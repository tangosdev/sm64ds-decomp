#include "types.h"
#include "file_struc.h"

extern void sys_writec(const unsigned char *c);

/* MSL __write_console for the debug console: send *count bytes from buffer
   one at a time through sys_writec, the ARM semihosting SYS_WRITEC call
   (swi 0x123456, r0 = 3). Returns 0. handle and idle_proc are the standard
   proc parameters; the console ignores both. */
int __write_console(__file_handle handle, unsigned char *buffer, size_t *count,
                    __idle_proc idle_proc) {
    size_t i;
    size_t n = *count;
    for (i = 0; i < n; i++) {
        sys_writec(buffer + i);
    }
    return 0;
}
