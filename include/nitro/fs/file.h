#ifndef NITRO_FS_FILE_H
#define NITRO_FS_FILE_H

/* The file-handle half of the Nitro SDK's nitro/fs/file.h: the FS_FILE_STATUS_*
 * bits of FSFile.stat and the fs_file.c API (src/engine/fs/FS_File.c), under
 * the SDK's names with C linkage. The types are this SDK's layouts in
 * private/fs_views.h. The SDK's BOOL returns are spelled int: no shared header
 * here defines BOOL. Parameters unnamed: a named parameter here renumbers
 * every includer's @NNN. */

#include "types.h"
#include "private/fs_views.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FS_FILE_STATUS_BUSY 0x00000001
#define FS_FILE_STATUS_CANCEL 0x00000002
#define FS_FILE_STATUS_SYNC 0x00000004
#define FS_FILE_STATUS_ASYNC 0x00000008
#define FS_FILE_STATUS_IS_FILE 0x00000010
#define FS_FILE_STATUS_IS_DIR 0x00000020

/* FS_Init's DMA channel: none, the card is read by the CPU. */
#define FS_DMA_NOT_USE ((u32)~0)

void FS_Init(u32);
void FS_InitFile(FSFile *);
int FS_ConvertPathToFileID(FSFileID *, const char *);
int FS_OpenFileDirect(FSFile *, FSArchive *, u32, u32, u32);
int FS_OpenFileFast(FSFile *, FSFileID);
int FS_OpenFile(FSFile *, const char *);
int FS_CloseFile(FSFile *);
int FS_GetPathName(FSFile *, char *, u32);
int FS_WaitAsync(FSFile *);
s32 FS_ReadFileAsync(FSFile *, void *, s32);
s32 FS_ReadFile(FSFile *, void *, s32);
int FS_SeekFile(FSFile *, s32, FSSeekFileMode);

#ifdef __cplusplus
}
#endif

/* The SDK's accessors as macros rather than its static inlines: a macro costs
 * an includer no @NNN numbers. FS_IsBusy is not here: fs_file.c keeps it as
 * its own static inline, and a macro of that name would rewrite it. */
#define FS_GetLength(p_file) ((u32)((p_file)->prop.file.bottom - (p_file)->prop.file.top))
#define FS_IsFile(p_file) (((p_file)->stat & FS_FILE_STATUS_IS_FILE) ? 1 : 0)

#endif /* NITRO_FS_FILE_H */
