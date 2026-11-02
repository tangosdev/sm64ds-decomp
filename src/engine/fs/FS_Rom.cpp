//cpp
/* The ROM archive (the Nitro SDK's FS_rom.c), arm9 .text
 * 0x0205d94c..0x0205db88.
 *
 * FSi_InitRom sets up the cartridge as the archive "rom" (data_020a8074,
 * pret's fsi_arc_rom). It keeps the DMA channel reads use (data_020a8060,
 * fsi_default_dma_no) and a card lock id (data_020a805c, fsi_card_lock_id),
 * clears the two overlay-table regions (data_020a8064 and data_020a806c,
 * fsi_ovt9 and fsi_ovt7), and loads the archive from the FAT and FNT the ROM
 * header locates. Reads go to the card asynchronously through
 * FSi_ReadRomCallback and end in FSi_OnRomReadDone. Writes fail.
 * FSi_RomArchiveProc locks the card ROM while the archive runs a command
 * and unlocks it when the archive goes idle.
 *
 * These are six of the nine functions in pret's pokediamond FS_rom.c. mwccarm emits them
 * in reverse, so FS_TryLoadTable sits lowest in the ROM and
 * FSi_OnRomReadDone highest. This SDK is older than pret's. FSi_InitRom has
 * no multiboot-child branch, so FSi_ReadDummyCallback and
 * FSi_EmptyArchiveProc are absent. It installs FSi_RomArchiveProc only once
 * the header's tables check out. There is no FS_SetDefaultDMA, and
 * FSi_OnRomReadDone does not ask CARD_IsPulledOut.
 */
#include "types.h"
#include "nitro/card/card_rom.h"
#include "nitro/os/spinLock.h"
#include "private/fs_views.h"

typedef struct FSFile FSFile;

extern "C" {

#define FS_COMMAND_WRITEFILE 1
#define FS_COMMAND_ACTIVATE 9
#define FS_COMMAND_IDLE 10

#define FS_ARCHIVE_PROC_WRITEFILE (1 << FS_COMMAND_WRITEFILE)
#define FS_ARCHIVE_PROC_ACTIVATE (1 << FS_COMMAND_ACTIVATE)
#define FS_ARCHIVE_PROC_IDLE (1 << FS_COMMAND_IDLE)

#define FS_RESULT_SUCCESS 0
#define FS_RESULT_FAILURE 1
#define FS_RESULT_UNSUPPORTED 4
#define FS_RESULT_PROC_ASYNC 6
#define FS_RESULT_PROC_UNKNOWN 8

/* The ROM header's {offset, length} of the FNT and the FAT. */
#define HW_ROM_HEADER_BUF 0x027ffe00

/* NitroSDK CARDRomRegion. */
typedef struct CARDRomRegion {
    u32 offset;
    u32 length;
} CARDRomRegion;

extern u32 data_020a8060;           /* fsi_default_dma_no */
extern s32 data_020a805c;           /* fsi_card_lock_id */
extern CARDRomRegion data_020a8064; /* fsi_ovt9 */
extern CARDRomRegion data_020a806c; /* fsi_ovt7 */
extern FSArchive data_020a8074;     /* fsi_arc_rom */
extern const char data_0209a048[];  /* "rom" */

void CARD_Init(void);

void FS_InitArchive(FSArchive *p_arc);
int FS_RegisterArchiveName(FSArchive *p_arc, const char *name, int name_len);
int FS_LoadArchive(FSArchive *p_arc, u32 base, u32 fat, u32 fat_size, u32 fnt, u32 fnt_size,
                   FS_ARCHIVE_READ_FUNC read_func, FS_ARCHIVE_WRITE_FUNC write_func);
u32 FS_LoadArchiveTables(FSArchive *p_arc, void *p_mem, u32 max_size);
void FS_NotifyArchiveAsyncEnd(FSArchive *p_arc, int ret);
void FS_SetArchiveProc(FSArchive *p_arc, FS_ARCHIVE_PROC_FUNC proc, u32 flags);

void FSi_OnRomReadDone(void *p_arc);
int FSi_ReadRomCallback(FSArchive *p_arc, void *dst, u32 src, u32 len);
int FSi_WriteDummyCallback(FSArchive *p_arc, const void *src, u32 dst, u32 len);
int FSi_RomArchiveProc(FSFile *p_arc, int cmd);
void FSi_InitRom(u32 default_dma_no);
u32 FS_TryLoadTable(void *p_mem, u32 size);

/* pret's CARD_rom.h inlines. */
static inline const CARDRomRegion *CARD_GetRomRegionFNT(void) {
    return (const CARDRomRegion *)(HW_ROM_HEADER_BUF + 0x40);
}

static inline const CARDRomRegion *CARD_GetRomRegionFAT(void) {
    return (const CARDRomRegion *)(HW_ROM_HEADER_BUF + 0x48);
}

static inline void CARD_ReadRomAsync(u32 dma, const void *src, void *dst, u32 len,
                                     MIDmaCallback callback, void *arg) {
    CARDi_ReadRom(dma, src, dst, len, callback, arg, 1);
}

/* The end of a card read: the archive's command succeeded. */
// @symbol FSi_OnRomReadDone
void FSi_OnRomReadDone(void *p_arc) {
    FS_NotifyArchiveAsyncEnd((FSArchive *)p_arc, FS_RESULT_SUCCESS);
}

// @symbol FSi_ReadRomCallback
int FSi_ReadRomCallback(FSArchive *p_arc, void *dst, u32 src, u32 len) {
    CARD_ReadRomAsync(data_020a8060, (const void *)src, dst, len, FSi_OnRomReadDone, p_arc);
    return FS_RESULT_PROC_ASYNC;
}

/* Writes fail. pret's returns FS_RESULT_UNSUPPORTED; this SDK's returns
   FS_RESULT_FAILURE. The name rests on FSi_InitRom passing it to
   FS_LoadArchive as the write callback. */
// @symbol FSi_WriteDummyCallback
int FSi_WriteDummyCallback(FSArchive *p_arc, const void *src, u32 dst, u32 len) {
    return FS_RESULT_FAILURE;
}

// @symbol FSi_RomArchiveProc
int FSi_RomArchiveProc(FSFile *p_arc, int cmd) {
    switch (cmd) {
    case FS_COMMAND_ACTIVATE:
        CARD_LockRom((u16)data_020a805c);
        return FS_RESULT_SUCCESS;
    case FS_COMMAND_IDLE:
        CARD_UnlockRom((u16)data_020a805c);
        return FS_RESULT_SUCCESS;
    case FS_COMMAND_WRITEFILE:
        return FS_RESULT_UNSUPPORTED;
    default:
        return FS_RESULT_PROC_UNKNOWN;
    }
}

/* Set up the "rom" archive. A header whose FNT or FAT offset is 0 or ~0
   has no tables from the ROM maker, and the archive stays unloaded. */
// @symbol FSi_InitRom
void FSi_InitRom(u32 default_dma_no) {
    data_020a8060 = default_dma_no;
    data_020a805c = OS_GetLockID();
    data_020a8064.offset = 0;
    data_020a8064.length = 0;
    data_020a806c.offset = 0;
    data_020a806c.length = 0;

    CARD_Init();

    {
        const CARDRomRegion *const fnt = CARD_GetRomRegionFNT();
        const CARDRomRegion *const fat = CARD_GetRomRegionFAT();

        FS_InitArchive(&data_020a8074);
        FS_RegisterArchiveName(&data_020a8074, data_0209a048, 3);

        if (fnt->offset == 0xffffffff || fnt->offset == 0 || fat->offset == 0xffffffff ||
            fat->offset == 0) {
            return;
        }
        FS_SetArchiveProc(&data_020a8074, FSi_RomArchiveProc,
                          FS_ARCHIVE_PROC_WRITEFILE | FS_ARCHIVE_PROC_ACTIVATE |
                              FS_ARCHIVE_PROC_IDLE);
        FS_LoadArchive(&data_020a8074, 0, fat->offset, fat->length, fnt->offset, fnt->length,
                       FSi_ReadRomCallback, FSi_WriteDummyCallback);
    }
}

// @symbol FS_TryLoadTable
u32 FS_TryLoadTable(void *p_mem, u32 size) {
    return FS_LoadArchiveTables(&data_020a8074, p_mem, size);
}

} /* extern "C" */
