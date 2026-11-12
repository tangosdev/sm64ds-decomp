#ifndef NITRO_FS_FILE_H
#define NITRO_FS_FILE_H

/* The file-handle half of the Nitro SDK's nitro/fs/file.h: the FS_FILE_STATUS_*
 * bits of FSFile.stat and the fs_file.c API (src/engine/fs/FS_File.c). The
 * types are this SDK's layouts in private/fs_views.h. The SDK's BOOL returns
 * are spelled int: no shared header here defines BOOL. */

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

void FS_Init(u32 default_dma_no);
void FS_InitFile(FSFile *p_file);
int FS_ConvertPathToFileID(FSFileID *p_file_id, const char *path);
int FS_OpenFileDirect(FSFile *p_file, FSArchive *p_arc, u32 image_top, u32 image_bottom,
                      u32 file_index);
int FS_OpenFileFast(FSFile *p_file, FSFileID file_id);
int FS_OpenFile(FSFile *p_file, const char *path);
int FS_CloseFile(FSFile *p_file);
int FS_GetPathName(FSFile *p_file, char *buf, u32 len);
int FS_WaitAsync(FSFile *p_file);
s32 FS_ReadFileAsync(FSFile *p_file, void *dst, s32 len);
s32 FS_ReadFile(FSFile *p_file, void *dst, s32 len);
int FS_SeekFile(FSFile *p_file, s32 offset, FSSeekFileMode origin);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_FS_FILE_H */
