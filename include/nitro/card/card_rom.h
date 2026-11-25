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

/* The DS cartridge header, of which the boot code leaves the first 0x160
 * bytes at HW_ROM_HEADER_BUF. */
typedef struct CARDRomHeader {
    char game_name[12];        /* 0x000 */
    u32 game_code;             /* 0x00c */
    u16 maker_code;            /* 0x010 */
    u8 unit_code;              /* 0x012 */
    u8 encryption_seed_select; /* 0x013 */
    u8 device_capacity;        /* 0x014 */
    u8 reserved_15[7];         /* 0x015 */
    u8 reserved_1c;            /* 0x01c */
    u8 nds_region;             /* 0x01d */
    u8 rom_version;            /* 0x01e */
    u8 autostart;              /* 0x01f */
    u32 main_rom_offset;       /* 0x020 */
    u32 main_entry_address;    /* 0x024 */
    u32 main_ram_address;      /* 0x028 */
    u32 main_size;             /* 0x02c */
    u32 sub_rom_offset;        /* 0x030 */
    u32 sub_entry_address;     /* 0x034 */
    u32 sub_ram_address;       /* 0x038 */
    u32 sub_size;              /* 0x03c */
    CARDRomRegion fnt;         /* 0x040 */
    CARDRomRegion fat;         /* 0x048 */
    CARDRomRegion main_ovt;    /* 0x050 */
    CARDRomRegion sub_ovt;     /* 0x058 */
    u32 normal_command_ctrl;   /* 0x060 */
    u32 key1_command_ctrl;     /* 0x064 */
    u32 banner_offset;         /* 0x068 */
    u16 secure_crc;            /* 0x06c */
    u16 secure_delay;          /* 0x06e */
    u32 main_autoload_done;    /* 0x070 */
    u32 sub_autoload_done;     /* 0x074 */
    u8 secure_area_disable[8]; /* 0x078 */
    u32 rom_size;              /* 0x080 */
    u32 header_size;           /* 0x084 */
    u8 reserved_88[0x38];      /* 0x088 */
    u8 logo_data[0x9c];        /* 0x0c0 */
    u16 logo_crc;              /* 0x15c */
    u16 header_crc;            /* 0x15e */
} CARDRomHeader;

typedef char CARDRomHeader_size_must_be_0x160[sizeof(CARDRomHeader) == 0x160 ? 1 : -1];

/* The ROM header's file name table, file allocation table and overlay
 * tables, from the copy of the header the boot code leaves at
 * HW_ROM_HEADER_BUF. */
#define CARD_ROM_HEADER_OFFSET(field) ((u32)&((CARDRomHeader *)0)->field)

static inline const CARDRomRegion *CARD_GetRomRegionFNT(void) {
    return (const CARDRomRegion *)((const u8 *)HW_ROM_HEADER_BUF + CARD_ROM_HEADER_OFFSET(fnt));
}

static inline const CARDRomRegion *CARD_GetRomRegionFAT(void) {
    return (const CARDRomRegion *)((const u8 *)HW_ROM_HEADER_BUF + CARD_ROM_HEADER_OFFSET(fat));
}

static inline const CARDRomRegion *CARD_GetRomRegionOVT(MIProcessor target) {
    return (target == MI_PROCESSOR_ARM9) ? (const CARDRomRegion *)((const u8 *)HW_ROM_HEADER_BUF +
                                                                   CARD_ROM_HEADER_OFFSET(main_ovt))
                                         : (const CARDRomRegion *)((const u8 *)HW_ROM_HEADER_BUF +
                                                                   CARD_ROM_HEADER_OFFSET(sub_ovt));
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
