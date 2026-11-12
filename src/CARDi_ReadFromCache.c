/* NitroSDK CARDi_ReadFromCache (card_rom.c): if the page cache holds the
 * page src is in, copy what the transfer needs from it and advance. Returns
 * whether bytes are left to read. */
#include "nitro/card/card_rom.h"
#include "nitro/mi/memory.h"

/* MI_CpuCopy8. local extern: no include/ header declares MI_CpuCopy8; it is
 * defined as an asm body in src/engine/mi/MI_Memory.c. */

int CARDi_ReadFromCache(CARDRomStat *p) {
    CARDiCommon *c = &cardi_common;
    u32 cur_page = c->src & ~(CARD_ROM_PAGE_SIZE - 1);
    if (cur_page == (u32)p->cache_page) {
        u32 mod = c->src - cur_page;
        u32 len = CARD_ROM_PAGE_SIZE - mod;
        if (len > c->len)
            len = c->len;
        MI_CpuCopy8(p->cache_buf + mod, (void *)c->dst, len);
        c->src += len;
        c->dst += len;
        c->len -= len;
    }
    return c->len != 0;
}
