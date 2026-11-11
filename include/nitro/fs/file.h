#ifndef NITRO_FS_FILE_H
#define NITRO_FS_FILE_H

/* fs_file.c, under the SDK's names with C linkage (NitroSDK
 * include/nitro/fs/file.h). Parameters unnamed: a named parameter here
 * renumbers every includer's @NNN. FSFile, FSArchive, FSFileID and
 * FSSeekFileMode are this ROM's SDK layouts, from private/fs_views.h. */

#include "types.h"
#include "private/fs_views.h"

#ifdef __cplusplus
extern "C" {
#endif
void FS_InitFile(FSFile *);
int FS_OpenFile(FSFile *, const char *);
int FS_OpenFileFast(FSFile *, FSFileID);
int FS_OpenFileDirect(FSFile *, FSArchive *, u32, u32, u32);
int FS_CloseFile(FSFile *);
int FS_ReadFile(FSFile *, void *, int);
int FS_ReadFileAsync(FSFile *, void *, int);
int FS_SeekFile(FSFile *, int, FSSeekFileMode);
int FS_ConvertPathToFileID(FSFileID *, const char *);
int FS_WaitAsync(FSFile *);
#ifdef __cplusplus
}
#endif

/* An open file's length, the SDK's FS_GetLength, as a macro rather than its
 * static inline: a macro costs an includer no @NNN numbers. */
#define FS_GetLength(p_file) ((u32)((p_file)->prop.file.bottom - (p_file)->prop.file.top))

/* FSFile stat bit 0, FS_FILE_STATUS_BUSY: a command is still in flight. */
#define FS_IsBusy(p_file) (((p_file)->stat & 1) ? 1 : 0)

/* FSFile stat bit 4, FS_FILE_STATUS_IS_FILE: it holds an open file. */
#define FS_IsFile(p_file) (((p_file)->stat & 0x10) ? 1 : 0)

#endif
