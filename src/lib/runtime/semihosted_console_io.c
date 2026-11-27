/* The ARM runtime's semihosted_console_io: the three SYS semihosting stubs
 * and the MSL debug-console procs built on them. pret pokediamond carries
 * the same six functions as the one RUNTIME_ARM object
 * arm9/asm/RUNTIME_ARM_semihosted_console_io.s (sys_writec, sys_readc,
 * sys_exit, __read_console, __write_console, __close_console), which is the
 * evidence they are one translation unit here.
 *
 * The semihosting stubs are hand-written assembly and emit first in source
 * order; the console procs are compiled C and follow in reverse source
 * order, so the C bodies below are written __read_console, __write_console,
 * __close_console against the cartridge's ascending close/write/read.
 * This cart's close proc precedes the write proc where pokediamond's
 * object carries it last; the C half of this file is a newer revision
 * either way. */
#include "types.h"
#include "file_struc.h"

/* The ARM semihosting SYS_WRITEC call (r0 = 3). */
asm void sys_writec(const unsigned char *c) {
    str lr, [sp, #-4]!
    mov r1, r0
    mov r0, #3
    swi 0x123456
    ldr pc, [sp], #4
}

/* The ARM semihosting SYS_READC call (r0 = 7). */
asm unsigned char sys_readc(void) {
    str lr, [sp, #-4]!
    mov r1, #0
    mov r0, #7
    swi 0x123456
    ldr pc, [sp], #4
}

/* The ARM semihosting SYS_EXIT call (r0 = 0x18). */
asm void sys_exit(void) {
    mov r1, #0
    mov r0, #0x18
    swi 0x123456
    mov pc, lr
}

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

/* MSL __close_console for the debug console: nothing to do, returns 0. It
   takes the standard close_proc handle and ignores it. It is the close_proc
   after __read_console and __write_console in each of the three console
   FILEs of the MSL __files table at 0x0209a330 (0x4c-byte FILEs;
   read/write/close_proc at +0x3c/+0x40/+0x44). */
int __close_console(__file_handle handle) {
    return 0;
}
