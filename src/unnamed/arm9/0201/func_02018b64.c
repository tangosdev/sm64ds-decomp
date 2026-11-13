#include "nitro/fs/overlay.h"
#include "nitro/fs/file.h"
#include "isdbglib.h"
/* The game's copy of NitroSDK's FS_LoadOverlayImage, in an older SDK's
 * shape: open the overlay's file by its file_id, clear the overlay's RAM
 * image (FS_ClearOverlayImage), read the whole file to its address
 * (myFS_ReadFile), then register the overlay with the IS-NITRO debugger
 * (_ISDbgLib_RegistOverlayInfo) by its processor, address, ROM offset and
 * image size. */

extern int myFS_OpenFileFast(FSFile *p_file, FSFileID id);
extern int myFS_ReadFile(FSFile *p_file, void *dst, int len);

/* NitroSDK nitro/fs/file.h's FS_GetFileImageTop, with its volatile
 * parameter: a plain field read schedules the ROM-offset load one slot
 * later. The header here carries FS_GetLength but not this one yet. */
static inline u32 FS_GetFileImageTop(volatile const FSFile *p_file) {
    return p_file->prop.file.top;
}

int func_02018b64(FSOverlayInfo *p_ovi) {
    FSFile p_file[1];
    FS_InitFile(p_file);

    if (!myFS_OpenFileFast(p_file, FS_GetOverlayFileID(p_ovi))) {
        return 0;
    }

    FS_ClearOverlayImage(p_ovi);
    (void)myFS_ReadFile(p_file, FS_GetOverlayAddress(p_ovi), (int)FS_GetLength(p_file));
    (void)_Z27_ISDbgLib_RegistOverlayInfo11OVERLAYPROCjjj((p_ovi->target == MI_PROCESSOR_ARM9) ? OVERLAYPROC_ARM9
                                                                           : OVERLAYPROC_ARM7,
                                      (u32)FS_GetOverlayAddress(p_ovi), FS_GetFileImageTop(p_file),
                                      FS_GetOverlayImageSize(p_ovi));
    (void)FS_CloseFile(p_file);
    return 1;
}
