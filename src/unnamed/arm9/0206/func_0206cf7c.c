#include "nitro/hw/mmap_shared.h"
#include "private/debugger_block.h"

/* Test mask against the debugger's flags word (+0x14 of the block at
   HW_ISD_RESERVED). The console and overlay code read bits 1, 4, 0x2000
   and 0x20000. */
int func_0206cf7c(int mask)
{
    struct DebuggerBlock *sh = (struct DebuggerBlock *)HW_ISD_RESERVED;
    return (mask & sh->flags) ? 1 : 0;
}
