#ifndef NITRO_FS_ARCHIVE_H
#define NITRO_FS_ARCHIVE_H
#ifndef __cplusplus
#define FS_SetArchiveProc _Z17FS_SetArchiveProcP9FSArchivePFiP6FSFileiEj
#define FS_ResumeArchive _Z16FS_ResumeArchiveP9FSArchive
#define FS_SuspendArchive _Z17FS_SuspendArchiveP9FSArchive
#define FS_UnloadArchiveTables _Z22FS_UnloadArchiveTablesP9FSArchive
#define FS_LoadArchiveTables _Z20FS_LoadArchiveTablesP9FSArchivePvj
#define FS_UnloadArchive _Z16FS_UnloadArchiveP9FSArchive
#define FS_LoadArchive _Z14FS_LoadArchiveP9FSArchivejjjjjPFiS0_PvjjEPFiS0_PKvjjE
#define FS_ReleaseArchiveName _Z21FS_ReleaseArchiveNameP9FSArchive
#define FS_RegisterArchiveName _Z22FS_RegisterArchiveNameP9FSArchivePKci
#define FS_InitArchive _Z14FS_InitArchiveP9FSArchive
#define FS_NotifyArchiveAsyncEnd _Z24FS_NotifyArchiveAsyncEndP9FSArchivei
#define FS_FindArchive _Z14FS_FindArchivePKci
#endif

/* The archive half of the Nitro SDK's nitro/fs/archive.h: the FSResult codes
 * an archive's procedure and callbacks return, the FSCommandType a procedure
 * is handed, the FS_ARCHIVE_FLAG_* bits of FSArchive.flag, the
 * FS_ARCHIVE_PROC_* bits of FSArchive.proc_flag, and the fs_archive.c API
 * (src/engine/fs/FS_Archive.cpp) under the SDK's names with C linkage. The
 * types are this SDK's layouts in private/fs_views.h.
 *
 * The SDK's BOOL returns are spelled int: no shared header here defines BOOL.
 * Its FSResult and FSCommandType parameters are spelled int as well, the way
 * FS_Archive.cpp defines them and FS_ARCHIVE_PROC_FUNC (private/fs_views.h)
 * types a procedure. Parameters unnamed: a named parameter here renumbers
 * every includer's @NNN. */

#include "types.h"
#include "private/fs_views.h"

#ifdef __cplusplus
#ifdef _MSC_VER
extern "C" {
#endif
#endif

#define FS_ARCHIVE_NAME_LEN_MAX 3

typedef enum FSResult {
    FS_RESULT_SUCCESS,      /* 0 */
    FS_RESULT_FAILURE,      /* 1 */
    FS_RESULT_BUSY,         /* 2 */
    FS_RESULT_CANCELED,     /* 3 */
    FS_RESULT_UNSUPPORTED,  /* 4 */
    FS_RESULT_ERROR,        /* 5 */
    FS_RESULT_PROC_ASYNC,   /* 6 -- the procedure finishes it later (FS_NotifyArchiveAsyncEnd) */
    FS_RESULT_PROC_DEFAULT, /* 7 -- the procedure leaves it to the default command */
    FS_RESULT_PROC_UNKNOWN  /* 8 -- the procedure does not know the command */
} FSResult;

typedef enum FSCommandType {
    FS_COMMAND_READFILE,       /* 0 */
    FS_COMMAND_WRITEFILE,      /* 1 */
    FS_COMMAND_SEEKDIR,        /* 2 */
    FS_COMMAND_READDIR,        /* 3 */
    FS_COMMAND_FINDPATH,       /* 4 */
    FS_COMMAND_GETPATH,        /* 5 */
    FS_COMMAND_OPENFILEFAST,   /* 6 */
    FS_COMMAND_OPENFILEDIRECT, /* 7 */
    FS_COMMAND_CLOSEFILE,      /* 8 */
    FS_COMMAND_ACTIVATE,       /* 9 */
    FS_COMMAND_IDLE            /* 10 */
} FSCommandType;

#define FS_ARCHIVE_FLAG_REGISTER 0x00000001
#define FS_ARCHIVE_FLAG_LOADED 0x00000002
#define FS_ARCHIVE_FLAG_TABLE_LOAD 0x00000004
#define FS_ARCHIVE_FLAG_SUSPEND 0x00000008
#define FS_ARCHIVE_FLAG_RUNNING 0x00000010
#define FS_ARCHIVE_FLAG_CANCELING 0x00000020
#define FS_ARCHIVE_FLAG_SUSPENDING 0x00000040
#define FS_ARCHIVE_FLAG_UNLOADING 0x00000080
#define FS_ARCHIVE_FLAG_IS_ASYNC 0x00000100
#define FS_ARCHIVE_FLAG_IS_SYNC 0x00000200

/* Which commands FS_SetArchiveProc hands the procedure. */
#define FS_ARCHIVE_PROC_READFILE (1 << FS_COMMAND_READFILE)
#define FS_ARCHIVE_PROC_WRITEFILE (1 << FS_COMMAND_WRITEFILE)
#define FS_ARCHIVE_PROC_SEEKDIR (1 << FS_COMMAND_SEEKDIR)
#define FS_ARCHIVE_PROC_READDIR (1 << FS_COMMAND_READDIR)
#define FS_ARCHIVE_PROC_FINDPATH (1 << FS_COMMAND_FINDPATH)
#define FS_ARCHIVE_PROC_GETPATH (1 << FS_COMMAND_GETPATH)
#define FS_ARCHIVE_PROC_OPENFILEFAST (1 << FS_COMMAND_OPENFILEFAST)
#define FS_ARCHIVE_PROC_OPENFILEDIRECT (1 << FS_COMMAND_OPENFILEDIRECT)
#define FS_ARCHIVE_PROC_CLOSEFILE (1 << FS_COMMAND_CLOSEFILE)
#define FS_ARCHIVE_PROC_ACTIVATE (1 << FS_COMMAND_ACTIVATE)
#define FS_ARCHIVE_PROC_IDLE (1 << FS_COMMAND_IDLE)
#define FS_ARCHIVE_PROC_ALL (~0)
/* The commands a caller waits for: SEEKDIR through CLOSEFILE (0x1fc). */
#define FS_ARCHIVE_PROC_SYNC                                                                     \
    (FS_ARCHIVE_PROC_SEEKDIR | FS_ARCHIVE_PROC_READDIR | FS_ARCHIVE_PROC_FINDPATH |              \
     FS_ARCHIVE_PROC_GETPATH | FS_ARCHIVE_PROC_OPENFILEFAST | FS_ARCHIVE_PROC_OPENFILEDIRECT |   \
     FS_ARCHIVE_PROC_CLOSEFILE)

FSArchive *FS_FindArchive(const char *, int);
void FS_NotifyArchiveAsyncEnd(FSArchive *, int);
void FS_InitArchive(FSArchive *);
int FS_RegisterArchiveName(FSArchive *, const char *, int);
void FS_ReleaseArchiveName(FSArchive *);
int FS_LoadArchive(FSArchive *, u32, u32, u32, u32, u32, FS_ARCHIVE_READ_FUNC,
                   FS_ARCHIVE_WRITE_FUNC);
int FS_UnloadArchive(FSArchive *);
u32 FS_LoadArchiveTables(FSArchive *, void *, u32);
void *FS_UnloadArchiveTables(FSArchive *);
int FS_SuspendArchive(FSArchive *);
int FS_ResumeArchive(FSArchive *);
void FS_SetArchiveProc(FSArchive *, FS_ARCHIVE_PROC_FUNC, u32);

#ifdef __cplusplus
#ifdef _MSC_VER
}
#endif
#endif

#endif /* NITRO_FS_ARCHIVE_H */
