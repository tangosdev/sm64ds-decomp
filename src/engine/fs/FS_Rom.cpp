//cpp
/* The ROM archive (the Nitro SDK's FS_rom.c), arm9 .text
 * 0x0205d94c..0x0205db88.
 *
 * FSi_InitRom sets up the cartridge as the archive "rom" (fsi_arc_rom). It
 * keeps the DMA channel reads use (fsi_default_dma_no) and a card lock id
 * (fsi_card_lock_id), clears the two overlay-table regions (fsi_ovt9 and
 * fsi_ovt7), and loads the archive from the FAT and FNT the ROM header
 * locates. Reads go to the card asynchronously through FSi_ReadRomCallback
 * and end in FSi_OnRomReadDone. Writes fail.
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
#include "private/fs_rom.h"
#include "nitro/fs/archive.h"



extern const char data_0209a048[];
extern "C" {
  /* "rom" */

void CARD_Init(void);
}


void FSi_OnRomReadDone(void *p_arc);
int FSi_ReadRomCallback(FSArchive *p_arc, void *dst, u32 src, u32 len);
int FSi_WriteDummyCallback(FSArchive *p_arc, const void *src, u32 dst, u32 len);
int FSi_RomArchiveProc(FSFile *p_arc, int cmd);
u32 FS_TryLoadTable(void *p_mem, u32 size);

/* pret's CARD_rom.h inline. */
static inline void CARD_ReadRomAsync(u32 dma, const void *src, void *dst, u32 len,
                                     MIDmaCallback callback, void *arg) {
    CARDi_ReadRom(dma, src, dst, len, callback, arg, 1);
}

/* The end of a card read: the archive's command succeeded. */
void FSi_OnRomReadDone(void *p_arc) {
    FS_NotifyArchiveAsyncEnd((FSArchive *)p_arc, FS_RESULT_SUCCESS);
}

int FSi_ReadRomCallback(FSArchive *p_arc, void *dst, u32 src, u32 len) {
    CARD_ReadRomAsync(fsi_default_dma_no, (const void *)src, dst, len, FSi_OnRomReadDone, p_arc);
    return FS_RESULT_PROC_ASYNC;
}

/* Writes fail. pret's returns FS_RESULT_UNSUPPORTED; this SDK's returns
   FS_RESULT_FAILURE. The name rests on FSi_InitRom passing it to
   FS_LoadArchive as the write callback. */
int FSi_WriteDummyCallback(FSArchive *p_arc, const void *src, u32 dst, u32 len) {
    return FS_RESULT_FAILURE;
}

int FSi_RomArchiveProc(FSFile *p_arc, int cmd) {
    switch (cmd) {
    case FS_COMMAND_ACTIVATE:
        CARD_LockRom((u16)fsi_card_lock_id);
        return FS_RESULT_SUCCESS;
    case FS_COMMAND_IDLE:
        CARD_UnlockRom((u16)fsi_card_lock_id);
        return FS_RESULT_SUCCESS;
    case FS_COMMAND_WRITEFILE:
        return FS_RESULT_UNSUPPORTED;
    default:
        return FS_RESULT_PROC_UNKNOWN;
    }
}

/* Set up the "rom" archive. A header whose FNT or FAT offset is 0 or ~0
   has no tables from the ROM maker, and the archive stays unloaded. */
void FSi_InitRom(u32 default_dma_no) {
    fsi_default_dma_no = default_dma_no;
    fsi_card_lock_id = OS_GetLockID();
    fsi_ovt9.offset = 0;
    fsi_ovt9.length = 0;
    fsi_ovt7.offset = 0;
    fsi_ovt7.length = 0;

    CARD_Init();

    {
        const CARDRomRegion *const fnt = CARD_GetRomRegionFNT();
        const CARDRomRegion *const fat = CARD_GetRomRegionFAT();

        FS_InitArchive(&fsi_arc_rom);
        FS_RegisterArchiveName(&fsi_arc_rom, data_0209a048, 3);

        if (fnt->offset == 0xffffffff || fnt->offset == 0 || fat->offset == 0xffffffff ||
            fat->offset == 0) {
            return;
        }
        FS_SetArchiveProc(&fsi_arc_rom, FSi_RomArchiveProc,
                          FS_ARCHIVE_PROC_WRITEFILE | FS_ARCHIVE_PROC_ACTIVATE |
                              FS_ARCHIVE_PROC_IDLE);
        FS_LoadArchive(&fsi_arc_rom, 0, fat->offset, fat->length, fnt->offset, fnt->length,
                       FSi_ReadRomCallback, FSi_WriteDummyCallback);
    }
}

u32 FS_TryLoadTable(void *p_mem, u32 size) {
    return FS_LoadArchiveTables(&fsi_arc_rom, p_mem, size);
}

 /* extern "C" */
