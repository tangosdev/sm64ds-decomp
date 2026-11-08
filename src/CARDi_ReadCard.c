/* NitroSDK CARDi_ReadCard (card_rom.c): the CPU read routine CARD_Init
 * installs in rom_stat. Read a page at a time: straight to dst when the
 * transfer is page-aligned, word-aligned and at least a page long, else into
 * the page cache and out through CARDi_ReadFromCache. Stop when len runs
 * out. */
#include "nitro/card/card_rom.h"

#define REG_CARD_CNT (*(volatile u32 *)0x040001a4)
#define REG_CARD_DATA (*(volatile u32 *)0x04100010)
#define CARD_DATA_READY 0x00800000
#define CARD_START 0x80000000

extern void CARDi_SetRomOp(u32 cmd1, u32 cmd2);
extern int CARDi_ReadFromCache(CARDRomStat *p);

void CARDi_ReadCard(CARDRomStat *p) {
    CARDiCommon *c = &cardi_common;
    u32 *dst;
    u32 *cache = (u32 *)p->cache_buf;
    u32 mask = ~(CARD_ROM_PAGE_SIZE - 1);

    for (;;) {
        u32 src = c->src;
        u32 page = src & mask;
        u32 pos;
        u32 ctrl;
        u32 cnt;

        if (page == src && (c->dst & 3) == 0 && c->len >= CARD_ROM_PAGE_SIZE) {
            dst = (u32 *)c->dst;
        } else {
            dst = cache;
            p->cache_page = (u8 *)page;
        }

        CARDi_SetRomOp((page >> 8) | 0xb7000000, page << 24);

        cnt = p->ctrl;
        pos = 0;
        REG_CARD_CNT = cnt;
        do {
            ctrl = REG_CARD_CNT;
            if (ctrl & CARD_DATA_READY) {
                u32 data = REG_CARD_DATA;
                if (pos < CARD_ROM_PAGE_SIZE) {
                    dst[pos] = data;
                    pos++;
                }
            }
        } while (ctrl & CARD_START);

        if (dst == (u32 *)c->dst) {
            cardi_common.src += CARD_ROM_PAGE_SIZE;
            cardi_common.dst += CARD_ROM_PAGE_SIZE;
            cardi_common.len -= CARD_ROM_PAGE_SIZE;
            if (cardi_common.len == 0)
                return;
        } else {
            if (!CARDi_ReadFromCache(p))
                return;
        }
    }
}
