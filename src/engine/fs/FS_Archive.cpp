//cpp
/* The file system's archives and their command queues (the Nitro SDK's
 * FS_archive.c), arm9 .text 0x0205c7c4..0x0205d368.
 *
 * An archive is a named view of some medium: base, the FAT and the FNT
 * locate its data, read_func and write_func reach it (the memory callbacks
 * below by default), and table_func reads the tables, from memory once
 * FS_LoadArchiveTables has copied them there. Files queue their commands on
 * the archive's list; FSi_SendCommand appends one, FSi_NextCommand picks the
 * next to run, FSi_ExecuteAsyncCommand and FSi_ExecuteSyncCommand run them
 * through FSi_TranslateCommand (src/engine/fs/FS_Command.cpp), and
 * FS_NotifyArchiveAsyncEnd ends one an archive's procedure ran
 * asynchronously. Registered archives form the list data_020a8048 (pret's
 * arc_list); data_020a804c is the current directory (current_dir_pos).
 *
 * pret's pokediamond FS_archive.c has the same twenty functions. mwccarm
 * emits them in reverse, so FS_SetArchiveProc sits lowest in the ROM and
 * FSi_GetPackedName highest. This SDK is older than pret's, and two
 * functions sit elsewhere in its file: FS_FindArchive comes before
 * FSi_NextCommand rather than after FS_InitArchive, and
 * FS_NotifyArchiveAsyncEnd sits between FSi_SendCommand and FS_InitArchive
 * rather than last. Its thread queues are 16-bit masks of thread ids, its
 * FSi_SendCommand has no suspend test, FS_RegisterArchiveName sets no
 * FS_ARCHIVE_FLAG_REGISTER, FS_LoadArchiveTables checks no read, and
 * FS_UnloadArchive does not resume the archive it suspended.
 */
#include "types.h"
#include "IRQ.h"
#include "nitro/os/thread.h"
#include "private/fs_views.h"

typedef struct FSFile FSFile;

extern "C" {

#define FS_FILE_STATUS_BUSY 0x00000001
#define FS_FILE_STATUS_CANCEL 0x00000002
#define FS_FILE_STATUS_SYNC 0x00000004
#define FS_FILE_STATUS_ASYNC 0x00000008

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

#define FS_ARCHIVE_PROC_SYNC 0x000001fc /* the commands a caller waits for */
#define FS_ARCHIVE_PROC_ACTIVATE 0x00000200
#define FS_ARCHIVE_PROC_IDLE 0x00000400

#define FS_COMMAND_ACTIVATE 9
#define FS_COMMAND_IDLE 10

#define FS_RESULT_SUCCESS 0
#define FS_RESULT_BUSY 2
#define FS_RESULT_CANCELED 3
#define FS_RESULT_PROC_ASYNC 6

#define FS_ARCHIVE_NAME_LEN_MAX 3

#define ALIGN_BYTE(n, a) (((u32)(n) + (a) - 1) & ~((a) - 1))

extern FSArchive *data_020a8048;
extern FSDirPos data_020a804c;

void CpuCopy8(const void *src, void *dst, u32 size);
void func_0205a588(void *dst, u8 value, u32 size);

void FS_InitFile(FSFile *p_file);
int FS_ReadFile(FSFile *p_file, void *dst, int len);
int FS_CloseFile(char *p_file);
int FS_OpenFileDirect(char *p_file, FSArchive *p_arc, u32 image_top, u32 image_bottom,
                      u32 file_index);

int FSi_TranslateCommand(FSFile *p_file, int command);
void FSi_ReleaseCommand(FSFile *p_file, int ret);
void FSi_AppendToList(FSFile *elem, FSFile *list);

u32 FSi_GetPackedName(const char *name, int name_len);
int FSi_ReadMemCallback(FSArchive *p_arc, void *dst, u32 pos, u32 size);
int FSi_WriteMemCallback(FSArchive *p_arc, const void *src, u32 pos, u32 size);
int FSi_ReadMemoryCore(FSArchive *p_arc, void *dst, u32 pos, u32 size);
FSArchive *FS_FindArchive(const char *name, int name_len);
FSFile *FSi_NextCommand(FSArchive *p_arc);
void FSi_ExecuteAsyncCommand(FSFile *p_file);
int FSi_ExecuteSyncCommand(FSFile *p_file);
int FSi_SendCommand(FSFile *p_file, int command);
void FS_NotifyArchiveAsyncEnd(FSArchive *p_arc, int ret);
void FS_InitArchive(FSArchive *p_arc);
int FS_RegisterArchiveName(FSArchive *p_arc, const char *name, int name_len);
void FS_ReleaseArchiveName(FSArchive *p_arc);
int FS_LoadArchive(FSArchive *p_arc, u32 base, u32 fat, u32 fat_size, u32 fnt, u32 fnt_size,
                   FS_ARCHIVE_READ_FUNC read_func, FS_ARCHIVE_WRITE_FUNC write_func);
int FS_UnloadArchive(FSArchive *p_arc);
u32 FS_LoadArchiveTables(FSArchive *p_arc, void *p_mem, u32 max_size);
void *FS_UnloadArchiveTables(FSArchive *p_arc);
int FS_SuspendArchive(FSArchive *p_arc);
int FS_ResumeArchive(FSArchive *p_arc);
void FS_SetArchiveProc(FSArchive *p_arc, FS_ARCHIVE_PROC_FUNC proc, u32 flags);

/* pret's FS_archive.h and FS_file.h predicates: each reads an archive's
   flag or a file's stat through a volatile pointer and comes back as a 0/1
   the caller branches on. */
static inline int FS_IsArchiveLoaded(volatile const FSArchive *p_arc) {
    return (p_arc->flag & FS_ARCHIVE_FLAG_LOADED) ? 1 : 0;
}

static inline int FS_IsArchiveTableLoaded(volatile const FSArchive *p_arc) {
    return (p_arc->flag & FS_ARCHIVE_FLAG_TABLE_LOAD) ? 1 : 0;
}

static inline int FS_IsArchiveSuspended(volatile const FSArchive *p_arc) {
    return (p_arc->flag & FS_ARCHIVE_FLAG_SUSPEND) ? 1 : 0;
}

static inline int FSi_IsArchiveRunning(volatile const FSArchive *p_arc) {
    return (p_arc->flag & FS_ARCHIVE_FLAG_RUNNING) != 0;
}

static inline int FSi_IsArchiveSuspending(volatile const FSArchive *p_arc) {
    return (p_arc->flag & FS_ARCHIVE_FLAG_SUSPENDING) != 0;
}

static inline int FSi_IsArchiveCanceling(volatile const FSArchive *p_arc) {
    return (p_arc->flag & FS_ARCHIVE_FLAG_CANCELING) != 0;
}

static inline int FS_IsCanceling(volatile const FSFile *p_file) {
    return (p_file->stat & FS_FILE_STATUS_CANCEL) ? 1 : 0;
}

static inline int FS_IsFileSyncMode(volatile const FSFile *p_file) {
    return (p_file->stat & FS_FILE_STATUS_SYNC) ? 1 : 0;
}

static inline int FSi_IsArchiveAsync(volatile const FSArchive *p_arc) {
    return (p_arc->flag & FS_ARCHIVE_FLAG_IS_ASYNC) != 0;
}

static inline int FS_IsSucceeded(volatile const FSFile *p_file) {
    return (p_file->error == FS_RESULT_SUCCESS) ? 1 : 0;
}

static inline u32 FS_GetArchiveOffset(const FSArchive *p_arc, u32 pos) {
    return p_arc->base + pos;
}

/* Pack an archive name of at most three characters into a word, 8 bits per
 * character, folding 'A'..'Z' to lower case and stopping at a NUL. The byte
 * is read unsigned, as the SDK's MI_ReadByte reads it: the build uses
 * -char signed. */
u32 FSi_GetPackedName(const char *name, int name_len) {
    u32 ret = 0;
    if (name_len <= FS_ARCHIVE_NAME_LEN_MAX) {
        int i = 0;
        if (name_len > 0) {
            int shift = 0;
            do {
                u32 c = (u8)name[i];
                u32 lc;
                if (c == 0)
                    break;
                lc = c - 'A';
                if (lc <= 'Z' - 'A')
                    lc = lc + 'a';
                else
                    lc = lc + 'A';
                i++;
                ret |= lc << shift;
                shift += 8;
            } while (i < name_len);
        }
    }
    return ret;
}

/* FS_LoadArchive's default read_func: copy from base + pos. */
int FSi_ReadMemCallback(FSArchive *p_arc, void *dst, u32 pos, u32 size) {
    CpuCopy8((const void *)FS_GetArchiveOffset(p_arc, pos), dst, size);
    return FS_RESULT_SUCCESS;
}

/* FS_LoadArchive's default write_func: copy to base + pos. */
int FSi_WriteMemCallback(FSArchive *p_arc, const void *src, u32 pos, u32 size) {
    CpuCopy8(src, (void *)FS_GetArchiveOffset(p_arc, pos), size);
    return FS_RESULT_SUCCESS;
}

/* The table_func of an archive whose tables are loaded: pos is already an
 * address. */
int FSi_ReadMemoryCore(FSArchive *p_arc, void *dst, u32 pos, u32 size) {
    CpuCopy8((const void *)pos, dst, size);
    return FS_RESULT_SUCCESS;
}

/* The registered archive whose packed name matches name, walking the
 * archive list with interrupts off; NULL if there is none. */
FSArchive *FS_FindArchive(const char *name, int name_len) {
    u32 pack = FSi_GetPackedName(name, name_len);
    u32 bak_psr = IRQ::Disable();
    FSArchive *p_arc = data_020a8048;
    while (p_arc != 0 && p_arc->name.pack != pack) {
        p_arc = p_arc->next;
    }
    IRQ::Restore(bak_psr);
    return p_arc;
}

/* With interrupts off: if the archive is canceling, release every queued
 * file marked for cancel as FS_RESULT_CANCELED. Unless the archive is
 * suspended, mark it running (proc gets FS_COMMAND_ACTIVATE when proc_flag
 * asks for it) and return the head of the list, or wake a synchronous one
 * and return NULL. Otherwise the archive goes idle: proc gets
 * FS_COMMAND_IDLE on a scratch FSFile, and a pending suspend takes effect
 * and wakes stat_q. */
FSFile *FSi_NextCommand(FSArchive *p_arc) {
    u32 bak_psr = IRQ::Disable();
    if (FSi_IsArchiveCanceling(p_arc)) {
        FSFile *p, *q;
        p_arc->flag &= ~FS_ARCHIVE_FLAG_CANCELING;
        for (p = p_arc->list.next; p; p = q) {
            q = p->link.next;
            if (FS_IsCanceling(p)) {
                if (p_arc->list.next == p)
                    p_arc->list.next = q;
                FSi_ReleaseCommand(p, FS_RESULT_CANCELED);
                if (!q)
                    q = p_arc->list.next;
            }
        }
    }
    if (!FS_IsArchiveSuspended(p_arc)) {
        FSFile *p_file = p_arc->list.next;
        if (p_file != 0) {
            const int is_start = !FSi_IsArchiveRunning(p_arc);
            if (is_start)
                p_arc->flag |= FS_ARCHIVE_FLAG_RUNNING;
            IRQ::Restore(bak_psr);
            if (is_start) {
                if (p_arc->proc_flag & FS_ARCHIVE_PROC_ACTIVATE)
                    (*p_arc->proc)(p_file, FS_COMMAND_ACTIVATE);
            }
            if (FS_IsFileSyncMode(p_file)) {
                OS_WakeupThread(&p_file->queue);
                return 0;
            }
            return p_file;
        }
    }
    if (FSi_IsArchiveRunning(p_arc)) {
        p_arc->flag &= ~FS_ARCHIVE_FLAG_RUNNING;
        if (p_arc->proc_flag & FS_ARCHIVE_PROC_IDLE) {
            FSFile tmp;
            FS_InitFile(&tmp);
            tmp.arc = p_arc;
            (*p_arc->proc)(&tmp, FS_COMMAND_IDLE);
        }
    }
    if (FSi_IsArchiveSuspending(p_arc)) {
        p_arc->flag &= ~FS_ARCHIVE_FLAG_SUSPENDING;
        p_arc->flag |= FS_ARCHIVE_FLAG_SUSPEND;
        OS_WakeupThread(&p_arc->stat_q);
    }
    IRQ::Restore(bak_psr);
    return 0;
}

/* Run the archive's queued commands one after another. A synchronous file
 * is woken to run itself; otherwise the file is marked asynchronous and its
 * command translated, stopping when the archive answers
 * FS_RESULT_PROC_ASYNC, and FSi_NextCommand gives the next file. Unlike
 * pret's, the synchronous path breaks out without restoring interrupts. */
void FSi_ExecuteAsyncCommand(FSFile *p_file) {
    FSArchive *const p_arc = p_file->arc;
    while (p_file) {
        u32 bak_psr = IRQ::Disable();
        if (FS_IsFileSyncMode(p_file)) {
            OS_WakeupThread(&p_file->queue);
            break;
        }
        p_file->stat |= FS_FILE_STATUS_ASYNC;
        IRQ::Restore(bak_psr);
        if (FSi_TranslateCommand(p_file, p_file->command) == FS_RESULT_PROC_ASYNC)
            break;
        p_file = FSi_NextCommand(p_arc);
    }
}

/* Run the file's command and release it with the result, start the
 * archive's next queued command, and report whether the file's error is
 * FS_RESULT_SUCCESS. */
int FSi_ExecuteSyncCommand(FSFile *p_file) {
    FSFile *p_target;
    int ret = FSi_TranslateCommand(p_file, p_file->command);
    FSi_ReleaseCommand(p_file, ret);
    p_target = FSi_NextCommand(p_file->arc);
    if (p_target != 0)
        FSi_ExecuteAsyncCommand(p_target);
    return FS_IsSucceeded(p_file);
}

/* Record the command, FS_RESULT_BUSY and FS_FILE_STATUS_BUSY. An unloading
 * archive releases the file as FS_RESULT_CANCELED. Otherwise a synchronous
 * command gets FS_FILE_STATUS_SYNC, the file is appended to the archive's
 * command list, and an idle archive is marked running. An asynchronous
 * command starts at once or waits its turn; a synchronous one sleeps on the
 * file's queue until it heads the list, then runs through
 * FSi_ExecuteSyncCommand. */
int FSi_SendCommand(FSFile *p_file, int command) {
    FSArchive *p_arc = p_file->arc;
    const int bit = 1 << command;
    p_file->command = command;
    p_file->error = FS_RESULT_BUSY;
    p_file->stat |= FS_FILE_STATUS_BUSY;
    {
        u32 bak_psr = IRQ::Disable();
        if (p_arc->flag & FS_ARCHIVE_FLAG_UNLOADING) {
            FSi_ReleaseCommand(p_file, FS_RESULT_CANCELED);
            IRQ::Restore(bak_psr);
            return 0;
        }
        if ((bit & FS_ARCHIVE_PROC_SYNC) != 0)
            p_file->stat |= FS_FILE_STATUS_SYNC;
        FSi_AppendToList(p_file, (FSFile *)&p_arc->list);
        if (!FSi_IsArchiveRunning(p_arc)) {
            p_arc->flag |= FS_ARCHIVE_FLAG_RUNNING;
            IRQ::Restore(bak_psr);
            if (p_arc->proc_flag & FS_ARCHIVE_PROC_ACTIVATE)
                (*p_arc->proc)(p_file, FS_COMMAND_ACTIVATE);
            if (!FS_IsFileSyncMode(p_file)) {
                FSi_ExecuteAsyncCommand(p_file);
                return 1;
            }
        } else if (!FS_IsFileSyncMode(p_file)) {
            IRQ::Restore(bak_psr);
            return 1;
        } else {
            do {
                OS_SleepThread(&p_file->queue);
            } while (p_file != p_arc->list.next);
            IRQ::Restore(bak_psr);
        }
    }
    return FSi_ExecuteSyncCommand(p_file);
}

/* The archive's procedure finished the command at the head of its list. An
 * asynchronous one is released with ret and the next command started; a
 * synchronous one gets ret as its error, and the caller waiting on sync_q
 * is woken. */
void FS_NotifyArchiveAsyncEnd(FSArchive *p_arc, int ret) {
    if (FSi_IsArchiveAsync(p_arc)) {
        FSFile *p_file = p_arc->list.next;
        p_arc->flag &= ~FS_ARCHIVE_FLAG_IS_ASYNC;
        FSi_ReleaseCommand(p_file, ret);
        p_file = FSi_NextCommand(p_arc);
        if (p_file)
            FSi_ExecuteAsyncCommand(p_file);
    } else {
        FSFile *p_file = p_arc->list.next;
        u32 bak_psr = IRQ::Disable();
        p_file->error = ret;
        p_arc->flag &= ~FS_ARCHIVE_FLAG_IS_SYNC;
        OS_WakeupThread(&p_arc->sync_q);
        IRQ::Restore(bak_psr);
    }
}

/* Clear the archive, then empty its two thread queues. A thread queue here
 * is a 16-bit mask of thread ids, so emptying one is a store of 0 where
 * pret's calls OS_InitThreadQueue. func_0205a588 is MI_CpuFill8. */
void FS_InitArchive(FSArchive *p_arc) {
    func_0205a588(p_arc, 0, sizeof(FSArchive));
    p_arc->sync_q = 0;
    p_arc->stat_q = 0;
}

/* With interrupts off, unless an archive called name is already registered,
 * append p_arc to the archive list (the first one also becomes the current
 * directory) and store its packed name. TRUE if it was registered. */
int FS_RegisterArchiveName(FSArchive *p_arc, const char *name, int name_len) {
    int ret = 0;
    u32 bak_psr = IRQ::Disable();
    if (!FS_FindArchive(name, name_len)) {
        FSArchive *p_tail = data_020a8048;
        if (!p_tail) {
            data_020a8048 = p_arc;
            data_020a804c.arc = p_arc;
            data_020a804c.pos = 0;
            data_020a804c.index = 0;
            data_020a804c.own_id = 0;
        } else {
            while (p_tail->next)
                p_tail = p_tail->next;
            p_tail->next = p_arc;
            p_arc->prev = p_tail;
        }
        p_arc->name.pack = FSi_GetPackedName(name, name_len);
        ret = 1;
    }
    IRQ::Restore(bak_psr);
    return ret;
}

/* Undo FS_RegisterArchiveName: with interrupts off, unlink a named archive
 * from the archive list and clear its packed name, links and
 * FS_ARCHIVE_FLAG_REGISTER. If it was the current directory's archive, the
 * current directory moves to the root of the list head. */
void FS_ReleaseArchiveName(FSArchive *p_arc) {
    if (p_arc->name.pack) {
        u32 bak_psr = IRQ::Disable();
        if (p_arc->next)
            p_arc->next->prev = p_arc->prev;
        if (p_arc->prev)
            p_arc->prev->next = p_arc->next;
        p_arc->name.pack = 0;
        p_arc->next = p_arc->prev = 0;
        p_arc->flag &= ~FS_ARCHIVE_FLAG_REGISTER;
        if (data_020a804c.arc == p_arc) {
            data_020a804c.arc = data_020a8048;
            data_020a804c.pos = 0;
            data_020a804c.index = 0;
            data_020a804c.own_id = 0;
        }
        IRQ::Restore(bak_psr);
    }
}

/* Point a registered archive at its data: base and the two tables (each
 * also kept in a _bak copy), and the media callbacks, defaulting to the
 * memory ones. Table reads go through read_func until the tables are
 * loaded. Always TRUE. */
int FS_LoadArchive(FSArchive *p_arc, u32 base, u32 fat, u32 fat_size, u32 fnt, u32 fnt_size,
                   FS_ARCHIVE_READ_FUNC read_func, FS_ARCHIVE_WRITE_FUNC write_func) {
    p_arc->base = base;
    p_arc->fat_size = fat_size;
    p_arc->fat = p_arc->fat_bak = fat;
    p_arc->fnt_size = fnt_size;
    p_arc->fnt = p_arc->fnt_bak = fnt;
    p_arc->read_func = read_func ? read_func : FSi_ReadMemCallback;
    p_arc->write_func = write_func ? write_func : FSi_WriteMemCallback;
    p_arc->table_func = p_arc->read_func;
    p_arc->load_mem = 0;
    p_arc->flag |= FS_ARCHIVE_FLAG_LOADED;
    return 1;
}

/* With interrupts off: suspend a loaded archive, mark it unloading, cancel
 * every queued command, then clear its base and tables. */
int FS_UnloadArchive(FSArchive *p_arc) {
    u32 bak_psr = IRQ::Disable();
    if (FS_IsArchiveLoaded(p_arc)) {
        if (FS_IsArchiveTableLoaded(p_arc)) {
            /* pret warns here that the preloaded tables may leak; this
               build compiles the warning out, but not the volatile read. */
        }
        {
            FSFile *p, *q;
            FS_SuspendArchive(p_arc);
            p_arc->flag |= FS_ARCHIVE_FLAG_UNLOADING;
            for (p = p_arc->list.next; p; p = q) {
                q = p->link.next;
                FSi_ReleaseCommand(p, FS_RESULT_CANCELED);
            }
            p_arc->list.next = 0;
        }
        p_arc->base = 0;
        p_arc->fat = 0;
        p_arc->fat_size = 0;
        p_arc->fnt = 0;
        p_arc->fnt_size = 0;
        p_arc->fat_bak = p_arc->fnt_bak = 0;
        p_arc->flag &=
            ~(FS_ARCHIVE_FLAG_CANCELING | FS_ARCHIVE_FLAG_LOADED | FS_ARCHIVE_FLAG_UNLOADING);
    }
    IRQ::Restore(bak_psr);
    return 1;
}

/* If the FAT and FNT fit in max_size (their size rounded up to 32 bytes,
 * plus 32 for alignment), read each into the 32-byte-aligned buffer through
 * a direct file, point fat and fnt at the copies and switch table reads to
 * memory. Returns the size the tables need. */
u32 FS_LoadArchiveTables(FSArchive *p_arc, void *p_mem, u32 max_size) {
    u32 total_size = ALIGN_BYTE(p_arc->fat_size + p_arc->fnt_size + 32, 32);
    if (total_size <= max_size) {
        u8 *p_cache = (u8 *)ALIGN_BYTE(p_mem, 32);
        FSFile tmp;
        FS_InitFile(&tmp);
        FS_OpenFileDirect((char *)&tmp, p_arc, p_arc->fat, p_arc->fat + p_arc->fat_size, (u32)~0);
        FS_ReadFile(&tmp, p_cache, p_arc->fat_size);
        FS_CloseFile((char *)&tmp);
        p_arc->fat = (u32)p_cache;
        p_cache += p_arc->fat_size;
        FS_OpenFileDirect((char *)&tmp, p_arc, p_arc->fnt, p_arc->fnt + p_arc->fnt_size, (u32)~0);
        FS_ReadFile(&tmp, p_cache, p_arc->fnt_size);
        FS_CloseFile((char *)&tmp);
        p_arc->fnt = (u32)p_cache;
        p_arc->load_mem = p_mem;
        p_arc->table_func = FSi_ReadMemoryCore;
        p_arc->flag |= FS_ARCHIVE_FLAG_TABLE_LOAD;
    }
    return total_size;
}

/* Undo FS_LoadArchiveTables on a loaded archive, suspended meanwhile:
 * return the table memory and read the tables through read_func again. */
void *FS_UnloadArchiveTables(FSArchive *p_arc) {
    void *ret = 0;
    if (FS_IsArchiveLoaded(p_arc)) {
        int bak_stat = FS_SuspendArchive(p_arc);
        if (FS_IsArchiveTableLoaded(p_arc)) {
            p_arc->flag &= ~FS_ARCHIVE_FLAG_TABLE_LOAD;
            ret = p_arc->load_mem;
            p_arc->load_mem = 0;
            p_arc->fat = p_arc->fat_bak;
            p_arc->fnt = p_arc->fnt_bak;
            p_arc->table_func = p_arc->read_func;
        }
        if (bak_stat)
            FS_ResumeArchive(p_arc);
    }
    return ret;
}

/* With interrupts off, unless the archive is already suspended: a running
 * archive is asked to suspend and the caller sleeps on stat_q until it has;
 * an idle one is suspended at once. Returns whether it was not suspended
 * before. */
int FS_SuspendArchive(FSArchive *p_arc) {
    u32 bak_psr = IRQ::Disable();
    const int bak_stat = !FS_IsArchiveSuspended(p_arc);
    if (bak_stat) {
        if (FSi_IsArchiveRunning(p_arc)) {
            p_arc->flag |= FS_ARCHIVE_FLAG_SUSPENDING;
            do {
                OS_SleepThread(&p_arc->stat_q);
            } while (FSi_IsArchiveSuspending(p_arc));
        } else {
            p_arc->flag |= FS_ARCHIVE_FLAG_SUSPEND;
        }
    }
    IRQ::Restore(bak_psr);
    return bak_stat;
}

/* If the archive is suspended, clear the flag and pick up its next command
 * with interrupts off; once they are restored, run a command that came
 * back. Returns whether the archive was not suspended. */
int FS_ResumeArchive(FSArchive *p_arc) {
    FSFile *p_target = 0;
    u32 bak_psr = IRQ::Disable();
    const int bak_stat = !FS_IsArchiveSuspended(p_arc);
    if (!bak_stat) {
        p_arc->flag &= ~FS_ARCHIVE_FLAG_SUSPEND;
        p_target = FSi_NextCommand(p_arc);
    }
    IRQ::Restore(bak_psr);
    if (p_target)
        FSi_ExecuteAsyncCommand(p_target);
    return bak_stat;
}

/* Install proc as the archive's user procedure for the commands flags
 * selects. No flags clears proc and no proc clears flags, so the two are
 * set or unset together. */
void FS_SetArchiveProc(FSArchive *p_arc, FS_ARCHIVE_PROC_FUNC proc, u32 flags) {
    if (!flags)
        proc = 0;
    else if (!proc)
        flags = 0;
    p_arc->proc = proc;
    p_arc->proc_flag = flags;
}
}
