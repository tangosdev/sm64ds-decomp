//cpp
/* The backup write task func_02060484 runs (the send-and-verify mode of
 * pret's CARDi_RequestStreamCommandCore): copy up to a page of the source
 * into backup_cache_page_buf, flush it, program it (CARD_REQ_PROGRAM_BACKUP,
 * 10 retries) and verify it, and advance until len is 0 or a request fails;
 * then end the task. */
#include "types.h"
#include "nitro/os/cache.h"
#include "nitro/card/card_common.h"
#include "nitro/mi/memory.h"


void func_0206062c(CARDiCommon *p) {
    do {
        u32 len = p->len;
        if (len > sizeof(p->backup_cache_page_buf))
            len = sizeof(p->backup_cache_page_buf);
        MI_CpuCopy8((const void *)p->src, p->backup_cache_page_buf, len);
        DC_FlushRange(p->backup_cache_page_buf, len);
        DC_WaitWriteBufferEmpty();
        p->cmd->src = (u32)p->backup_cache_page_buf;
        p->cmd->dest = p->dst;
        p->cmd->len = len;
        if (!CARDi_Request(p, CARD_REQ_PROGRAM_BACKUP, 10))
            break;
        if (!CARDi_Request(p, CARD_REQ_VERIFY_BACKUP, 1))
            break;
        p->src += len;
        p->dst += len;
        p->len -= len;
    } while (p->len != 0);
    CARDi_EndTask(p);
}
