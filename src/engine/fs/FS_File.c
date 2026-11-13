/* The file system's file and directory handles (the Nitro SDK's fs_file.c),
 * arm9 .text 0x0205d368..0x0205d8d8.
 *
 * FS_Init brings the ROM archive up once. An FSFile is opened on an archive
 * by path (FS_OpenFile, through FS_ConvertPathToFileID and FSi_FindPath), by
 * FSFileID (FS_OpenFileFast) or by a raw image range (FS_OpenFileDirect), and
 * every operation is a command queued on that archive with FSi_SendCommand
 * (src/engine/fs/FS_Archive.cpp). A synchronous call marks the handle
 * FS_FILE_STATUS_SYNC and waits in FS_WaitAsync.
 *
 * mwccarm emits the bodies in reverse, so FS_SeekFile sits lowest in the ROM
 * and FS_Init highest. The functions nothing calls (FS_IsAvailable, FS_End,
 * FSi_WriteFileCore, FS_GetPathLength, FS_CancelFile, the write and directory
 * calls) were dead-stripped. This SDK is older than pret's: FS_WaitAsync
 * decides ownership by whether the handle heads its archive's command list,
 * and FS_SeekFile compares the lower bound unsigned.
 */
#include "types.h"
#include "nitro/os/system.h"
#include "nitro/os/thread.h"
#include "private/fs_rom.h"
#include "private/fs_views.h"
#include "nitro/fs/file.h"
#include "nitro/fs/archive.h"

typedef int BOOL;
#define TRUE 1
#define FALSE 0
#define NULL 0

/* The command an idle FSFile holds: FS_InitFile and FS_CloseFile store 12,
   past the last command FSCommandType names (FS_COMMAND_IDLE, 10). */
#define FS_COMMAND_INVALID 12

extern FSDirPos current_dir_pos;

BOOL _Z15FSi_SendCommandP6FSFilei(FSFile *p_file, int command);
BOOL _Z22FSi_ExecuteSyncCommandP6FSFile(FSFile *p_file);

static inline BOOL FS_IsBusy(volatile const FSFile *p_file) {
    return (p_file->stat & FS_FILE_STATUS_BUSY) ? TRUE : FALSE;
}

static inline BOOL FS_IsSucceeded(volatile const FSFile *p_file) {
    return (p_file->error == FS_RESULT_SUCCESS) ? TRUE : FALSE;
}

static inline BOOL FS_IsArchiveLoaded(volatile const FSArchive *p_arc) {
    return (p_arc->flag & FS_ARCHIVE_FLAG_LOADED) ? TRUE : FALSE;
}

static inline BOOL FSi_IsSlash(u32 c) {
    return (c == '/') || (c == '\\');
}

static inline u8 MI_ReadByte(const void *address) {
    return *(u8 *)address;
}

static BOOL is_init = FALSE;

void FS_Init(u32 default_dma_no) {
    if (!is_init) {
        is_init = TRUE;
        _Z11FSi_InitRomj(default_dma_no);
    }
}

void FS_InitFile(FSFile *p_file) {
    p_file->link.next = p_file->link.prev = NULL;
    p_file->queue = 0;
    p_file->arc = NULL;
    p_file->command = FS_COMMAND_INVALID;
    p_file->stat = 0;
}

static BOOL FSi_FindPath(FSFile *p_dir, const char *path, FSFileID *p_file_id,
                         FSDirPos *p_dir_pos) {
    FSDirPos pos;

    if (FSi_IsSlash(MI_ReadByte(path))) {
        pos.own_id = 0;
        pos.arc = current_dir_pos.arc;
        pos.pos = 0;
        pos.index = 0;
        ++path;
    } else {
        int i;

        pos = current_dir_pos;
        for (i = 0; i <= FS_ARCHIVE_NAME_LEN_MAX; ++i) {
            u32 c = MI_ReadByte(path + i);
            if (!c || FSi_IsSlash(c))
                break;
            else if (c == ':') {
                FSArchive *const p_arc = _Z14FS_FindArchivePKci(path, i);
                if (!p_arc) {
                    return FALSE;
                } else if (!FS_IsArchiveLoaded(p_arc)) {
                    return FALSE;
                }

                pos.arc = p_arc;
                pos.pos = 0;
                pos.index = 0;
                pos.own_id = 0;
                path += i + 1;

                if (FSi_IsSlash(MI_ReadByte(path)))
                    ++path;
                break;
            }
        }
    }

    p_dir->arc = pos.arc;
    p_dir->arg.findpath.path = path;
    p_dir->arg.findpath.pos = pos;

    if (p_dir_pos) {
        p_dir->arg.findpath.find_directory = TRUE;
        p_dir->arg.findpath.result.dir = p_dir_pos;
    } else {
        p_dir->arg.findpath.find_directory = FALSE;
        p_dir->arg.findpath.result.file = p_file_id;
    }

    return _Z15FSi_SendCommandP6FSFilei(p_dir, FS_COMMAND_FINDPATH);
}

static s32 FSi_ReadFileCore(FSFile *p_file, void *dst, s32 len, BOOL async) {
    const s32 pos = (s32)p_file->prop.file.pos;
    const s32 rest = (s32)p_file->prop.file.bottom - pos;
    const u32 org = (u32)len;

    if (len > rest)
        len = rest;
    if (len < 0)
        len = 0;

    p_file->arg.readfile.dst = dst;
    p_file->arg.readfile.len_org = org;
    p_file->arg.readfile.len = (u32)(len);

    if (!async)
        p_file->stat |= FS_FILE_STATUS_SYNC;

    (void)_Z15FSi_SendCommandP6FSFilei(p_file, FS_COMMAND_READFILE);

    if (!async) {
        if (FS_WaitAsync(p_file))
            len = (s32)p_file->prop.file.pos - pos;
        else
            len = -1;
    }
    return len;
}

BOOL FS_ConvertPathToFileID(FSFileID *p_file_id, const char *path) {
    FSFile dir;
    FS_InitFile(&dir);

    if (!FSi_FindPath(&dir, path, p_file_id, NULL))
        return FALSE;

    return TRUE;
}

BOOL FS_OpenFileDirect(FSFile *p_file, FSArchive *p_arc, u32 image_top, u32 image_bottom,
                       u32 file_index) {
    p_file->arc = p_arc;
    p_file->arg.openfiledirect.index = file_index;
    p_file->arg.openfiledirect.top = image_top;
    p_file->arg.openfiledirect.bottom = image_bottom;

    if (!_Z15FSi_SendCommandP6FSFilei(p_file, FS_COMMAND_OPENFILEDIRECT))
        return FALSE;

    p_file->stat |= FS_FILE_STATUS_IS_FILE;
    p_file->stat &= ~FS_FILE_STATUS_IS_DIR;

    return TRUE;
}

BOOL FS_OpenFileFast(FSFile *p_file, FSFileID file_id) {
    if (!file_id.arc)
        return FALSE;

    p_file->arc = file_id.arc;
    p_file->arg.openfilefast.id = file_id;

    if (!_Z15FSi_SendCommandP6FSFilei(p_file, FS_COMMAND_OPENFILEFAST))
        return FALSE;

    p_file->stat |= FS_FILE_STATUS_IS_FILE;
    p_file->stat &= ~FS_FILE_STATUS_IS_DIR;

    return TRUE;
}

BOOL FS_OpenFile(FSFile *p_file, const char *path) {
    FSFileID id;
    return (FS_ConvertPathToFileID(&id, path) && FS_OpenFileFast(p_file, id));
}

BOOL FS_CloseFile(FSFile *p_file) {
    if (!_Z15FSi_SendCommandP6FSFilei(p_file, FS_COMMAND_CLOSEFILE))
        return FALSE;

    p_file->arc = NULL;
    p_file->command = FS_COMMAND_INVALID;
    p_file->stat &= ~(FS_FILE_STATUS_IS_FILE | FS_FILE_STATUS_IS_DIR);

    return TRUE;
}

BOOL FS_GetPathName(FSFile *p_file, char *buf, u32 len) {
    if (p_file->command != FS_COMMAND_GETPATH) {
        p_file->arg.getpath.total_len = 0;
        p_file->arg.getpath.dir_id = 0;
    }

    p_file->arg.getpath.buf = (u8 *)buf;
    p_file->arg.getpath.buf_len = len;

    return _Z15FSi_SendCommandP6FSFilei(p_file, FS_COMMAND_GETPATH);
}

BOOL FS_WaitAsync(FSFile *p_file) {
    BOOL is_owner = FALSE;
    OSIntrMode bak_psr = OS_DisableInterrupts();

    if (FS_IsBusy(p_file)) {
        is_owner = (p_file->arc->list.next != p_file) ? TRUE : FALSE;
        if (is_owner)
            p_file->stat |= FS_FILE_STATUS_SYNC;
        do {
            _Z14OS_SleepThreadPt(&p_file->queue);
        } while (FS_IsBusy(p_file));
    }
    (void)OS_RestoreInterrupts(bak_psr);

    if (is_owner) {
        return _Z22FSi_ExecuteSyncCommandP6FSFile(p_file);
    }

    return FS_IsSucceeded(p_file);
}

s32 FS_ReadFileAsync(FSFile *p_file, void *dst, s32 len) {
    return FSi_ReadFileCore(p_file, dst, len, TRUE);
}

s32 FS_ReadFile(FSFile *p_file, void *dst, s32 len) {
    return FSi_ReadFileCore(p_file, dst, len, FALSE);
}

BOOL FS_SeekFile(FSFile *p_file, s32 offset, FSSeekFileMode origin) {
    switch (origin) {
    case FS_SEEK_SET:
        offset += p_file->prop.file.top;
        break;
    case FS_SEEK_CUR:
        offset += p_file->prop.file.pos;
        break;
    case FS_SEEK_END:
        offset += p_file->prop.file.bottom;
        break;
    default:
        return FALSE;
    }

    if (offset < p_file->prop.file.top)
        offset = (s32)p_file->prop.file.top;

    if (offset > (s32)p_file->prop.file.bottom)
        offset = (s32)p_file->prop.file.bottom;

    p_file->prop.file.pos = (u32)offset;

    return TRUE;
}
