#include "file_struc.h"

/* MSL __close_console for the debug console: nothing to do, returns 0. It
   takes the standard close_proc handle and ignores it. It is the close_proc
   after __read_console and __write_console in each of the three console
   FILEs of the MSL __files table at 0x0209a330 (0x4c-byte FILEs;
   read/write/close_proc at +0x3c/+0x40/+0x44). */
int __close_console(__file_handle handle) {
    return 0;
}
