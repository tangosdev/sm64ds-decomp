#include "types.h"
/* Card ROM reads (the Nitro SDK's card_rom.c), arm9 .text
 * 0x02060890..0x02060f60.
 *
 * ntrtwl/NitroSDK libraries/card/src/card_rom.c (and pret/pokediamond's
 * arm9/lib/NitroSDK/src/CARD_rom.c) define, in this order,
 * CARDi_ReadFromCache, CARDi_SetRomOp, CARDi_SetCardDma, CARDi_OnReadCard,
 * CARDi_TryReadCardDma, CARDi_ReadCard, CARDi_ReadRomIDCore,
 * CARDi_ReadRomSyncCore, CARDi_ReadRom, CARD_Init, CARD_WaitRomAsync,
 * CARD_TryWaitRomAsync and CARDi_GetRomAccessor. mwccarm emits the bodies in
 * reverse, so the ROM runs bottom-up: CARD_Init at 0x02060890,
 * CARDi_ReadFromCache at 0x02060ebc.
 *
 * This SDK is older than both. CARDi_ReadEnd is a function of the file, not
 * the private header's inline, and does no pulled-out check, so there is no
 * CARDi_ReadRomIDCore; there is no cardi_rom_base and no
 * CARDi_GetRomAccessor; CARDi_ReadRom waits for the card inline and sets the
 * callback after it; CARDi_TryReadCardDma flushes the whole destination; and
 * CARD_WaitRomAsync comes before CARD_Init. CARD_TryWaitRomAsync is not in
 * this ROM.
 */
#include "nitro/card/card_rom.h"
#include "nitro/os/cache.h"
#include "nitro/os/interrupt.h"
#include "nitro/os/system.h"
#include "nitro/os/tcm.h"
#include "nitro/mi/dma.h"
#include "nitro/mi/memory.h"

/* The card registers and ROMCTRL bits, as the SDK's private card_rom.h
 * names them. */
#define REG_CARD_MASTER_CNT (HW_REG_BASE + 0x1a1)
#define REG_CARDCNT (HW_REG_BASE + 0x1a4)
#define REG_CARD_CMD (HW_REG_BASE + 0x1a8)
#define REG_CARD_DATA (HW_REG_BASE + 0x100010)

#define CARDMST_SEL_ROM 0x00
#define CARDMST_IF_ENABLE 0x40
#define CARDMST_ENABLE 0x80

#define CARD_DATA_READY 0x00800000
#define CARD_COMMAND_PAGE 0x01000000
#define CARD_COMMAND_MASK 0x07000000
#define CARD_RESET_HI 0x20000000
#define CARD_READ_MODE 0x00000000
#define CARD_START 0x80000000

#define MROMOP_G_READ_PAGE 0xb7000000

/* the last DMA channel, as nitro/mi/dma.h names it in the SDK */
#define MI_DMA_MAX_NUM 3

#define CARD_ALIGN_HI_BIT(n) (((u32)(n)) & ~(CARD_ROM_PAGE_SIZE - 1))
#define CARD_ALIGN_LO_BIT(n) (((u32)(n)) & (CARD_ROM_PAGE_SIZE - 1))

/* The ITCM's start address, relocated: the ROM points it at 0x01ff8000,
 * where the symbol table names the first function autoloaded there. */
extern char func_01ff8000;
/* local extern: card_pullOut.c's CARD_InitPulledOutCallback in pret, still
 * unnamed in src/engine/card/CARD_PullOut.cpp, which no header declares. */
extern void func_02061138(void);

/* Whether [buf, buf + len) touches the ITCM (it ends at 0x02000000) or the
 * DTCM. */
static inline int CARDi_IsInTcm(u32 buf, u32 len) {
    const u32 d = OS_GetDTCMAddress();
    return (buf + len > (uintptr_t)(&func_01ff8000) && buf < 0x2000000) ||
           (d < buf + len && d + HW_DTCM_SIZE > buf);
}

/* The ROMCTRL value for a command: the game's own settings from the ROM
 * header, with the command bits replaced. */
static inline u32 CARDi_GetRomFlag(u32 flag) {
    const u32 rom_ctrl = *(vu32 *)(HW_ROM_HEADER_BUF + 0x60);
    return (u32)((rom_ctrl & ~CARD_COMMAND_MASK) | flag | CARD_READ_MODE | CARD_START |
                 CARD_RESET_HI);
}

static inline int CARDi_OnReadPageDirect(CARDRomStat *arg) {
    CARDiCommon *p = &cardi_common;
    (void)arg;

    p->src += CARD_ROM_PAGE_SIZE;
    p->dst += CARD_ROM_PAGE_SIZE;
    p->len -= CARD_ROM_PAGE_SIZE;

    return (p->len > 0);
}

// @symbol CARDi_ReadFromCache
static int CARDi_ReadFromCache(CARDRomStat *p) {
    CARDiCommon *c = &cardi_common;
    const u32 cur_page = CARD_ALIGN_HI_BIT(c->src);

    if (cur_page == (u32)p->cache_page) {
        const u32 mod = c->src - cur_page;
        u32 len = CARD_ROM_PAGE_SIZE - mod;

        if (len > c->len)
            len = c->len;

        MI_CpuCopy8(p->cache_buf + mod, (void *)c->dst, len);

        c->src += len;
        c->dst += len;
        c->len -= len;
    }

    return (c->len > 0);
}

/* A read is done: mark it successful and end the task. The body after the
 * result is the SDK's CARDi_EndTask(p, TRUE), written out: this SDK keeps
 * CARDi_EndTask out of line (src/CARDi_EndTask.cpp), and the ROM inlines it
 * here. */
// @symbol CARDi_ReadEnd
static void CARDi_ReadEnd(void) {
    CARDiCommon *const p = &cardi_common;
    const MIDmaCallback func = p->callback;
    void *const arg = p->callback_arg;
    OSIntrMode bak_psr;

    p->cmd->result = CARD_RESULT_SUCCESS;
    bak_psr = OS_DisableInterrupts();
    p->flag &= ~(CARD_STAT_BUSY | CARD_STAT_TASK);
    OS_WakeupThread(&p->busy_q);
    if ((p->flag & CARD_STAT_RECV) != 0) {
        OS_WakeupThreadDirect(&p->thread);
    }
    (void)OS_RestoreInterrupts(bak_psr);
    if (func) {
        (*func)(arg);
    }
}

// @symbol CARDi_SetRomOp
static void CARDi_SetRomOp(u32 cmd1, u32 cmd2) {
    while ((*(vu32 *)REG_CARDCNT & CARD_START) != 0)
        ;

    *(vu8 *)REG_CARD_MASTER_CNT = CARDMST_SEL_ROM | CARDMST_ENABLE | CARDMST_IF_ENABLE;
    {
        vu8 *const p_cmd = (vu8 *)REG_CARD_CMD;
        p_cmd[0] = (u8)(cmd1 >> (8 * 3));
        p_cmd[1] = (u8)(cmd1 >> (8 * 2));
        p_cmd[2] = (u8)(cmd1 >> (8 * 1));
        p_cmd[3] = (u8)(cmd1 >> (8 * 0));
        p_cmd[4] = (u8)(cmd2 >> (8 * 3));
        p_cmd[5] = (u8)(cmd2 >> (8 * 2));
        p_cmd[6] = (u8)(cmd2 >> (8 * 1));
        p_cmd[7] = (u8)(cmd2 >> (8 * 0));
    }
}

static inline void CARDi_SetRomOpReadPage1(u32 src) {
    CARDi_SetRomOp((u32)(MROMOP_G_READ_PAGE | (src >> 8)), (u32)(src << 24));
}

/* One DMA channel's registers at 0x040000b0 + n * 12: source, destination,
 * control. */
typedef struct {
    u32 src;
    u32 dst;
    u32 ctrl;
} DmaCh;

/* The later SDK's MIi_CardDmaCopy32 call, written straight to the channel:
 * one word per card-data request from the fixed REG_CARD_DATA port, 32-bit,
 * repeating (MI_CNT_CARDRECV32(4) | MI_DMA_CONTINUOUS_ON). */
// @symbol CARDi_SetCardDma
static void CARDi_SetCardDma(void) {
    CARDiCommon *const c = &cardi_common;
    CARDRomStat *const p = &rom_stat;
    DmaCh *ch = (DmaCh *)(0x040000b0 + c->dma * 12);
    ch->src = REG_CARD_DATA;
    ch->dst = c->dst;
    ch->ctrl = 0xaf000001;

    CARDi_SetRomOpReadPage1(c->src);
    *(vu32 *)REG_CARDCNT = p->ctrl;
}

// @symbol CARDi_OnReadCard
static void CARDi_OnReadCard(void) {
    CARDRomStat *const p = &rom_stat;

    if (!CARDi_OnReadPageDirect(p)) {
        MI_StopDma(cardi_common.dma);
        (void)OS_DisableIrqMask(OS_IE_CARD_DATA);
        (void)OS_ResetRequestIrqMask(OS_IE_CARD_DATA);
        CARDi_ReadEnd();
    } else {
        CARDi_SetCardDma();
    }
}

// @symbol CARDi_TryReadCardDma
int CARDi_TryReadCardDma(CARDRomStat *p) {
    CARDiCommon *const c = &cardi_common;
    u32 len;
    int is_async = 0;
    int whole_pages = 0;
    int not_in_tcm = 0;
    int dma_ok = 0;
    const u32 dst = c->dst;

    /* The SDK's one && chain, a test at a time: mwccarm keeps a flag per
     * step here, as the ROM does. */
    if (!(dst & 3) && c->dma <= MI_DMA_MAX_NUM)
        dma_ok = 1;
    if (dma_ok) {
        len = c->len;
        if (!CARDi_IsInTcm(dst, len))
            not_in_tcm = 1;
    }
    if (not_in_tcm && !CARD_ALIGN_LO_BIT(c->src | c->len))
        whole_pages = 1;
    if (whole_pages && c->len > 0)
        is_async = 1;

    p->ctrl = CARDi_GetRomFlag(CARD_COMMAND_PAGE);
    if (is_async) {
        OSIntrMode bak_psr = OS_DisableInterrupts();
        DC_FlushRange((void *)c->dst, c->len);
        IC_InvalidateRange((void *)c->dst, c->len);
        (void)OS_SetIrqFunction(OS_IE_CARD_DATA, CARDi_OnReadCard);
        (void)OS_ResetRequestIrqMask(OS_IE_CARD_DATA);
        (void)OS_EnableIrqMask(OS_IE_CARD_DATA);
        DC_WaitWriteBufferEmpty();
        (void)OS_RestoreInterrupts(bak_psr);
        CARDi_SetCardDma();
    }
    return is_async;
}

// @symbol CARDi_ReadCard
void CARDi_ReadCard(CARDRomStat *p) {
    CARDiCommon *const c = &cardi_common;
    for (;;) {
        const u32 len = CARD_ROM_PAGE_SIZE;
        u32 src = CARD_ALIGN_HI_BIT(c->src);
        u32 dst;

        if ((src != c->src) || ((c->dst & 3) != 0) || (c->len < len)) {
            dst = (u32)p->cache_buf;
            p->cache_page = (void *)src;
        } else {
            dst = c->dst;
        }

        CARDi_SetRomOpReadPage1(src);
        {
            u32 pos = 0;
            *(vu32 *)REG_CARDCNT = p->ctrl;
            for (;;) {
                const u32 ctrl = *(vu32 *)REG_CARDCNT;
                if ((ctrl & CARD_DATA_READY) != 0) {
                    u32 data = *(vu32 *)REG_CARD_DATA;
                    if (pos < len)
                        ((u32 *)dst)[pos++] = data;
                }
                if (!(ctrl & CARD_START))
                    break;
            }
        }

        if (dst == c->dst) {
            if (!CARDi_OnReadPageDirect(p))
                break;
        } else {
            if (!CARDi_ReadFromCache(p))
                break;
        }
    }
}

// @symbol CARDi_ReadRomSyncCore
static void CARDi_ReadRomSyncCore(CARDiCommon *c) {
    CARDRomStat *const p = &rom_stat;
    (void)c;

    if (CARDi_ReadFromCache(p)) {
        (*p->read_func)(p);
    }
    CARDi_ReadEnd();
}

// @symbol CARDi_ReadRom
void CARDi_ReadRom(u32 dma, const void *src, void *dst, u32 len, MIDmaCallback callback, void *arg,
                   int is_async) {
    CARDRomStat *const p = &rom_stat;
    CARDiCommon *const c = &cardi_common;

    {
        OSIntrMode bak_psr = OS_DisableInterrupts();
        while ((c->flag & CARD_STAT_BUSY) != 0)
            OS_SleepThread(&c->busy_q);
        c->flag |= CARD_STAT_BUSY;
        (void)OS_RestoreInterrupts(bak_psr);
    }

    c->dma = dma;
    c->src = (u32)src;
    c->dst = (u32)dst;
    c->len = (u32)len;
    c->callback = callback;
    c->callback_arg = arg;

    if (dma <= MI_DMA_MAX_NUM)
        MI_StopDma(dma);

    if (CARDi_TryReadCardDma(p)) {
        if (!is_async)
            CARD_WaitRomAsync();
    } else if (is_async) {
        CARDi_SetTask(CARDi_ReadRomSyncCore);
    } else {
        c->cur_th = OSi_ThreadInfo.current;
        CARDi_ReadRomSyncCore(c);
    }
}

// @symbol CARD_WaitRomAsync
void CARD_WaitRomAsync(void) {
    CARDi_WaitAsync();
}

// @symbol CARD_Init
void CARD_Init(void) {
    CARDiCommon *const p = &cardi_common;

    if (!p->flag) {
        p->flag = CARD_STAT_INIT;
        p->src = p->dst = p->len = 0;
        p->dma = (u32)~0;
        p->callback = 0;
        p->callback_arg = 0;

        CARDi_InitCommon();

        rom_stat.read_func = CARDi_ReadCard;

        func_02061138();
    }
}
