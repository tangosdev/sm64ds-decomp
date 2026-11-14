//cpp
/* The backup read task func_02060558 runs (the receive mode of pret's
 * CARDi_RequestStreamCommandCore): request up to a page from backup address
 * src into backup_cache_page_buf (CARD_REQ_READ_BACKUP), copy it out to dst,
 * and advance until len is 0 or a request fails; then end the task. */
#include "types.h"
#include "nitro/os/cache.h"
#include "nitro/card/card_common.h"
#include "nitro/mi/memory.h"

/* opt_loop_invariants stays off for schedule colour only: with it on the function
   is still 0xcc, but the prologue hoists &p->src ahead of the 0x100, 6 and 1
   constants -- five words, unreachable from the source either with or without
   the `buf` alias. */
#pragma opt_loop_invariants off
/* MI_CpuCopy8 */

void func_0206071c(CARDiCommon *p) {
    u32 len;
    u8 *buf;

    buf = p->backup_cache_page_buf;

    do {
        len = p->len;
        if (len > sizeof(p->backup_cache_page_buf))
            len = sizeof(p->backup_cache_page_buf);
        DC_InvalidateRange(p->backup_cache_page_buf, sizeof(p->backup_cache_page_buf));
        p->cmd->src = p->src;
        p->cmd->dest = (u32)p->backup_cache_page_buf;
        p->cmd->len = len;
        if (!CARDi_Request(p, CARD_REQ_READ_BACKUP, 1))
            break;
        MI_CpuCopy8(buf, (void *)p->dst, len);
        p->src += len;
        p->dst += len;
        p->len -= len;
    } while (p->len != 0);

    CARDi_EndTask(p);
}
