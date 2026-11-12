#ifndef NITRO_CARD_ROM_H
#define NITRO_CARD_ROM_H

/*
 * Card ROM reads, under the NitroSDK's own names. The bodies are card_rom.c,
 * in src/engine/card/CARD_Rom.c; the file's own helpers (CARDi_ReadFromCache,
 * CARDi_ReadEnd, CARDi_SetRomOp, CARDi_SetCardDma, CARDi_OnReadCard,
 * CARDi_ReadRomSyncCore) are static there, so they are not declared here.
 *
 * The parameters are unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number.
 */

#include "nitro/card/card_common.h"
#include "nitro/hw/mmap_shared.h"
#include "nitro/mi/exMemory.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CARD_ROM_PAGE_SIZE 0x200

/* The card's normal-mode "read page" command, in the top byte of the first
 * command word (pret NitroSDK card_rom.c's MROMOP_G_READ_PAGE). */
#define MROMOP_G_READ_PAGE 0xB7000000

/* NitroSDK CARDRomRegion: an {offset, length} span of the card ROM, the
 * shape of the ROM header's FNT and FAT entries and of the overlay tables
 * (fsi_ovt9, fsi_ovt7). */
typedef struct CARDRomRegion {
    u32 offset;
    u32 length;
} CARDRomRegion;

/* The ROM header's file name table, file allocation table and overlay
 * tables, from the copy of the header the boot code leaves at
 * HW_ROM_HEADER_BUF. */
static inline const CARDRomRegion *CARD_GetRomRegionFNT(void) {
    return (const CARDRomRegion *)((const u8 *)HW_ROM_HEADER_BUF + 0x40);
}

static inline const CARDRomRegion *CARD_GetRomRegionFAT(void) {
    return (const CARDRomRegion *)((const u8 *)HW_ROM_HEADER_BUF + 0x48);
}

static inline const CARDRomRegion *CARD_GetRomRegionOVT(MIProcessor target) {
    return (target == MI_PROCESSOR_ARM9)
               ? (const CARDRomRegion *)((const u8 *)HW_ROM_HEADER_BUF + 0x50)
               : (const CARDRomRegion *)((const u8 *)HW_ROM_HEADER_BUF + 0x58);
}

/* NitroSDK CARDRomStat (rom_stat, data_020a8780). */
typedef struct CARDRomStat {
    void (*read_func)(struct CARDRomStat *); /* 0x00 -- CARD_Init installs CARDi_ReadCard */
    u32 ctrl;                                /* 0x04 -- the ROMCTRL value a read starts with */
    u8 *cache_page;                          /* 0x08 -- the ROM page cache_buf holds */
    u32 dummy[5];                            /* 0x0c */
    u8 cache_buf[CARD_ROM_PAGE_SIZE];        /* 0x20 */
} CARDRomStat;

extern CARDRomStat rom_stat;

int CARDi_TryReadCardDma(CARDRomStat *);
void CARDi_ReadCard(CARDRomStat *);
void CARDi_ReadRom(u32, const void *, void *, u32, MIDmaCallback, void *, int);
void CARD_WaitRomAsync(void);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_CARD_ROM_H */
