//cpp
/* The file system's default commands (the Nitro SDK's fs_command_default.c),
 * arm9 .text 0x0205bbdc..0x0205c5e4.
 *
 * FSi_TranslateCommand (src/engine/fs/FS_Command.cpp) falls back to the table
 * fsi_default_command when an archive's proc leaves a command to the
 * default: entry n runs FS_COMMAND n. pret/pokediamond's NitroSDK
 * FS_command_default.c has the same nine commands in the same table order.
 * mwccarm emits the bodies in reverse, so the ROM runs bottom-up from
 * FSi_CloseFileCommand at 0x0205bbdc to FSi_ReadTable at 0x0205c528.
 *
 * This SDK is older than pret's. FSi_ReadTable returns nothing and its
 * callers do not check a result, the read and write commands do not return
 * the archive callback's result, and the three helpers come in the order
 * FSi_ReadTable, FSi_SeekDirDirect, FSi_StrNICmp (pret has FSi_StrNICmp
 * first). Its thread queues are 16-bit masks of thread ids.
 */
#include "types.h"
#include "nitro/fs/file.h"
#include "nitro/fs/archive.h"
#include "nitro/os/thread.h"

extern "C" {

#define FS_INVALID_ID 0x10000

/* FSDirPos as its three words. The SDK's file is C, where an FSDirPos
 * assignment is a block move (ldm/stm); in C++ the same assignment copies
 * member by member, so the copies go through this view. */
typedef struct FSDirPosWords {
    u32 w[3];
} FSDirPosWords;

static inline void FSi_CopyDirPos(FSDirPos *dst, const FSDirPos *src) {
    *(FSDirPosWords *)dst = *(const FSDirPosWords *)src;
}

/* FSFileID as its two words, for the same reason. */
typedef struct FSFileIDWords {
    u32 w[2];
} FSFileIDWords;

static inline void FSi_CopyFileID(FSFileID *dst, const FSFileID *src) {
    *(FSFileIDWords *)dst = *(const FSFileIDWords *)src;
}

typedef struct FSDirEntry {
    union {
        FSFileID file_id;
        FSDirPos dir_id;
    }; /* 0x00 */
    u32 is_directory; /* 0x0c */
    u32 name_len;     /* 0x10 */
    u8 name[128];     /* 0x14 -- FS_FILE_NAME_MAX */
} FSDirEntry;

typedef char FSDirEntry_size_must_be_0x94[sizeof(FSDirEntry) == 0x94 ? 1 : -1];

/* An FAT entry: the file's image range. */
typedef struct FSArchiveFAT {
    u32 top;    /* 0x00 */
    u32 bottom; /* 0x04 */
} FSArchiveFAT;

/* An FNT directory entry. */
typedef struct FSArchiveFNT {
    u32 start;  /* 0x00 -- the first child's offset into the FNT */
    u16 index;  /* 0x04 -- the first child's file id */
    u16 parent; /* 0x06 */
} FSArchiveFNT;

/* The archive and table position FSi_ReadTable reads next. */
typedef struct FSiSyncReadParam {
    FSArchive *arc; /* 0x00 */
    u32 pos;        /* 0x04 */
} FSiSyncReadParam;

int FSi_TranslateCommand(FSFile *p_file, int command);
extern const char data_0209a044[]; /* ":/" */

void FSi_ReadTable(FSiSyncReadParam *p, void *dst, u32 len);
void FSi_SeekDirDirect(FSFile *p_dir, u32 id);
int FSi_StrNICmp(const u8 *str1, const u8 *str2, int len);
}

#include "nitro/os/system.h"
#include "nitro/mi/memory.h"

extern "C" {

/* The archive is still busy with a synchronous table read. The if/return body
 * is measured: `return (flag & FS_ARCHIVE_FLAG_IS_SYNC) != 0;' lets the loop
 * guard get copied. */
static inline int FSi_IsArchiveSync(FSArchive *p_arc) {
    if (p_arc->flag & FS_ARCHIVE_FLAG_IS_SYNC)
        return 1;
    return 0;
}

/* Read len bytes of the FAT or FNT at p->pos through the archive's
 * table_func and advance p->pos. A table_func that answers PROC_ASYNC
 * finishes later; sleep on sync_q until it does. */
void FSi_ReadTable(FSiSyncReadParam *p, void *dst, u32 len) {
    FSArchive *p_arc = p->arc;
    p_arc->flag |= FS_ARCHIVE_FLAG_IS_SYNC;
    switch (p_arc->table_func(p_arc, dst, p->pos, len)) {
    case FS_RESULT_SUCCESS:
    case FS_RESULT_FAILURE:
        p_arc->flag &= ~FS_ARCHIVE_FLAG_IS_SYNC;
        break;
    case FS_RESULT_PROC_ASYNC: {
        OSIntrMode saved = OS_DisableInterrupts();
        while (FSi_IsArchiveSync(p_arc)) {
            OS_SleepThread(&p_arc->sync_q);
        }
        OS_RestoreInterrupts(saved);
        break;
    }
    }
    p->pos += len;
}

/* Point the directory handle at the start of directory id in its own archive
 * and seek there synchronously. */
void FSi_SeekDirDirect(FSFile *p_dir, u32 id) {
    p_dir->stat |= FS_FILE_STATUS_SYNC;
    p_dir->arg.seekdir.pos.arc = p_dir->arc;
    p_dir->arg.seekdir.pos.pos = 0;
    p_dir->arg.seekdir.pos.index = 0;
    p_dir->arg.seekdir.pos.own_id = (u16)id;
    FSi_TranslateCommand(p_dir, FS_COMMAND_SEEKDIR);
}

/* Compare len characters, ignoring the case of A-Z. */
int FSi_StrNICmp(const u8 *str1, const u8 *str2, int len) {
    int i;
    for (i = 0; i < len; i++) {
        u32 c = str1[i] - 'A';
        u32 d = str2[i] - 'A';
        if (c <= 'Z' - 'A')
            c += 'a' - 'A';
        if (d <= 'Z' - 'A')
            d += 'a' - 'A';
        if (c != d)
            return c - d;
    }
    return 0;
}

void FSi_ReadFileCommand(FSFile *p_file) {
    FSArchive *p_arc = p_file->arc;
    u32 len = p_file->arg.readfile.len;
    u32 pos = p_file->prop.file.pos;
    void *dst = p_file->arg.readfile.dst;
    p_file->prop.file.pos += len;
    p_arc->read_func(p_arc, dst, pos, len);
}

void FSi_WriteFileCommand(FSFile *p_file) {
    FSArchive *p_arc = p_file->arc;
    u32 len = p_file->arg.writefile.len;
    u32 pos = p_file->prop.file.pos;
    const void *src = p_file->arg.writefile.src;
    p_file->prop.file.pos += len;
    p_arc->write_func(p_arc, src, pos, len);
}

/* Read the directory's FNT entry, copy arg.seekdir.pos to prop.dir.pos and,
 * when it names the directory's start (index and pos both 0), point it at the
 * first child instead. */
int FSi_SeekDirCommand(FSFile *p_dir) {
    FSArchive *p_arc = p_dir->arc;
    FSDirPos *arg = &p_dir->arg.seekdir.pos;
    FSArchiveFNT fnt_entry;
    FSiSyncReadParam param;

    param.arc = p_arc;
    param.pos = p_arc->fnt + arg->own_id * sizeof(fnt_entry);
    FSi_ReadTable(&param, &fnt_entry, sizeof(fnt_entry));

    FSi_CopyDirPos(&p_dir->prop.dir.pos, arg);
    if (arg->index == 0 && arg->pos == 0) {
        p_dir->prop.dir.pos.index = fnt_entry.index;
        p_dir->prop.dir.pos.pos = p_arc->fnt + fnt_entry.start;
    }
    p_dir->prop.dir.parent = fnt_entry.parent & 0xfff;
    return FS_RESULT_SUCCESS;
}

/* The lead byte (name length in the low seven bits, the is-directory bit
 * above them) and a directory entry's id share one stack slot. */
typedef struct FSiDirEntryHead {
    u8 len;
    u8 pad;
    u16 id;
} FSiDirEntryHead;

/* Read the open directory's next FNT entry into arg.readdir.p_entry. A
 * directory entry is followed by its id, of which the low 12 bits are kept;
 * a file entry takes the next file id. */
int FSi_ReadDirCommand(FSFile *p_dir) {
    FSDirEntry *p_entry = p_dir->arg.readdir.p_entry;
    FSiDirEntryHead head;
    FSiSyncReadParam param;
    u8 len;
    int is_dir, name_len;

    param.arc = p_dir->arc;
    param.pos = p_dir->prop.dir.pos.pos;
    FSi_ReadTable(&param, &head.len, sizeof(head.len));
    len = head.len;
    name_len = len & 0x7f;
    is_dir = (len >> 7) & 1;
    p_entry->name_len = name_len;
    p_entry->is_directory = is_dir;
    if (p_entry->name_len == 0) {
        return FS_RESULT_FAILURE;
    }
    if (p_dir->arg.readdir.skip_string) {
        param.pos = param.pos + p_entry->name_len;
    } else {
        FSi_ReadTable(&param, p_entry->name, p_entry->name_len);
        p_entry->name[p_entry->name_len] = '\0';
    }
    if (p_entry->is_directory) {
        FSi_ReadTable(&param, &head.id, sizeof(head.id));
        p_entry->dir_id.arc = p_dir->arc;
        p_entry->dir_id.own_id = head.id & 0xfff;
        p_entry->dir_id.index = 0;
        p_entry->dir_id.pos = 0;
    } else {
        p_entry->file_id.arc = p_dir->arc;
        p_entry->file_id.file_id = p_dir->prop.dir.pos.index;
        ++p_dir->prop.dir.pos.index;
    }
    p_dir->prop.dir.pos.pos = param.pos;
    return FS_RESULT_SUCCESS;
}

/* A path component runs to a NUL, '/' or '\'. f stays a materialized 0/1:
 * if/return 1 and ?1:0 bodies grow the caller (0x220-0x228). */
static inline int FSi_IsNameChar(int c) {
    int f = 0;
    if (c != '\0' && c != '/' && c != '\\')
        f = 1;
    return f;
}

/* Walk arg.findpath.path one component at a time from the directory at
 * arg.findpath.pos: "." stays, ".." goes to the parent unless at the root,
 * and any other name is looked up entry by entry, matched by kind, length and
 * name (case-insensitive). A directory is entered; the final match is stored
 * through arg.findpath.result, as a file id or, with find_directory, as a
 * directory position. */
int FSi_FindPathCommand(FSFile *p_dir) {
    int find_dir = p_dir->arg.findpath.find_directory;
    /* Read unsigned, as the SDK's MI_ReadByte reads it: the build uses -char signed. */
    const u8 *path = (const u8 *)p_dir->arg.findpath.path;
    u8 c;
    /* First the character that ended the component (nonzero: a separator
     * follows, so a directory is wanted), then the 0/1 the entry must match. */
    int want_dir;
    int len;

    FSDirEntry entry;

    FSi_TranslateCommand(p_dir, FS_COMMAND_SEEKDIR);

    for (c = *path; c != '\0'; path += (*path != '\0') ? 1 : 0, c = *path) {
        len = 0;
        while (FSi_IsNameChar(want_dir = path[len]))
            len = len + 1;

        if (want_dir != 0 || find_dir != 0)
            want_dir = 1;

        if (len == 0)
            return FS_RESULT_FAILURE;

        if (c == '.') {
            if (len == 1) {
                path = path + 1;
                continue;
            }
            if ((len == 2) & (path[1] == '.')) { /* bitwise &: && shrinks it to 0x20c */
                if (p_dir->prop.dir.pos.own_id != 0) {
                    FSi_SeekDirDirect(p_dir, p_dir->prop.dir.parent);
                }
                path = path + 2;
                continue;
            }
        }

        if (len > 127)
            return FS_RESULT_FAILURE;

        p_dir->arg.readdir.p_entry = &entry;
        p_dir->arg.readdir.skip_string = 0;

        for (;;) {
            if (FSi_TranslateCommand(p_dir, FS_COMMAND_READDIR) != FS_RESULT_SUCCESS)
                return FS_RESULT_FAILURE;
            if (want_dir != entry.is_directory)
                continue;
            if (len != entry.name_len)
                continue;
            if (FSi_StrNICmp(path, entry.name, len) != 0)
                continue;
            break;
        }

        if (want_dir != 0) {
            FSi_CopyDirPos(&p_dir->arg.seekdir.pos, &entry.dir_id);
            path = path + len;
            FSi_TranslateCommand(p_dir, FS_COMMAND_SEEKDIR);
            continue;
        }

        if (find_dir != 0)
            return FS_RESULT_FAILURE;
        FSi_CopyFileID(p_dir->arg.findpath.result.file, &entry.file_id);
        return FS_RESULT_SUCCESS;
    }

    if (find_dir == 0)
        return FS_RESULT_FAILURE;
    FSi_CopyDirPos(p_dir->arg.findpath.result.dir, &p_dir->prop.dir.pos);
    return FS_RESULT_SUCCESS;
}

/* Build "arc:/dir/.../name" for an open file or directory into
 * arg.getpath.buf. A file's directory is found by walking every directory's
 * entries until one names the file. The total length and directory id are
 * cached in arg.getpath, so a caller can ask for the length first (buf 0)
 * and then call again with a buffer. */
int FSi_GetPathCommand(FSFile *p_file) {
    FSArchive *p_arc;
    FSDirEntry entry;
    FSFile tmp;
    /* arg.getpath, held through a pointer: the ROM keeps &arg.getpath in a
     * register for the whole function. */
    FSGetPathInfo *m;
    int dir_id;
    int file_id;
    u32 i;
    u32 cnt;
    int is_dir;

    p_arc = p_file->arc;
    m = &p_file->arg.getpath;
    FS_InitFile(&tmp);
    tmp.arc = p_file->arc;

    is_dir = (p_file->stat & FS_FILE_STATUS_IS_DIR) ? 1 : 0;
    if (is_dir != 0) {
        dir_id = p_file->prop.dir.pos.own_id;
        file_id = FS_INVALID_ID;
    } else {
        file_id = p_file->prop.file.own_id;
        if (m->total_len != 0) {
            dir_id = m->dir_id;
        } else {
            dir_id = FS_INVALID_ID;
            i = 0;
            cnt = 0;
            do {
                FSi_SeekDirDirect(&tmp, i);
                if (i == 0)
                    cnt = tmp.prop.dir.parent;
                tmp.arg.readdir.p_entry = &entry;
                tmp.arg.readdir.skip_string = 1;
                if (FSi_TranslateCommand(&tmp, FS_COMMAND_READDIR) == FS_RESULT_SUCCESS) {
                    for (;;) {
                        if (entry.is_directory == 0 && entry.file_id.file_id == file_id) {
                            dir_id = tmp.prop.dir.pos.own_id;
                            break;
                        }
                        if (FSi_TranslateCommand(&tmp, FS_COMMAND_READDIR) != FS_RESULT_SUCCESS)
                            break;
                    }
                }
                if (dir_id != FS_INVALID_ID)
                    break;
                i++;
            } while (i < cnt);
        }
    }

    if (dir_id == FS_INVALID_ID) {
        m->total_len = 0;
        return FS_RESULT_FAILURE;
    }

    if (m->total_len == 0) {
        cnt = 0;
        if (p_arc->name.pack <= 0xff)
            cnt += 1;
        else if (p_arc->name.pack <= 0xff00)
            cnt += 2;
        else
            cnt += 3;
        cnt += 2; /* ":/" */
        if (file_id != FS_INVALID_ID)
            cnt += entry.name_len;

        i = dir_id;
        if (dir_id != 0) {
            FSi_SeekDirDirect(&tmp, dir_id);
            do {
                FSi_SeekDirDirect(&tmp, tmp.prop.dir.parent);
                tmp.arg.readdir.p_entry = &entry;
                tmp.arg.readdir.skip_string = 1;
                if (FSi_TranslateCommand(&tmp, FS_COMMAND_READDIR) == FS_RESULT_SUCCESS) {
                    for (;;) {
                        if (entry.is_directory != 0 && entry.dir_id.own_id == i) {
                            cnt += entry.name_len + 1;
                            break;
                        }
                        if (FSi_TranslateCommand(&tmp, FS_COMMAND_READDIR) != FS_RESULT_SUCCESS)
                            break;
                    }
                }
                i = tmp.prop.dir.pos.own_id;
            } while (i != 0);
        }
        m->total_len = (u16)(cnt + 1);
        m->dir_id = (u16)dir_id;
    }

    {
        u8 *buf = m->buf;
        int count;

        if (buf == 0)
            return FS_RESULT_SUCCESS;
        count = m->total_len;
        if (m->buf_len < (u32)count)
            return FS_RESULT_FAILURE;

        {
            int pos;
            int nb;

            pos = 0;
            if (p_arc->name.pack <= 0xff)
                nb = 1;
            else if (p_arc->name.pack <= 0xff00)
                nb = 2;
            else
                nb = 3;

            MI_CpuCopy8(p_arc->name.ptr, buf, nb);
            pos += nb;
            MI_CpuCopy8(data_0209a044, buf + pos, 2);
        }

        /* The rest is written backwards from the end of the buffer. */
        FSi_SeekDirDirect(&tmp, dir_id);
        if (file_id != FS_INVALID_ID) {
            tmp.arg.readdir.p_entry = &entry;
            tmp.arg.readdir.skip_string = 0;
            if (FSi_TranslateCommand(&tmp, FS_COMMAND_READDIR) == FS_RESULT_SUCCESS) {
                for (;;) {
                    if (entry.is_directory == 0 && entry.file_id.file_id == file_id)
                        break;
                    if (FSi_TranslateCommand(&tmp, FS_COMMAND_READDIR) != FS_RESULT_SUCCESS)
                        break;
                }
            }
            {
                int n = entry.name_len + 1;
                MI_CpuCopy8(entry.name, (buf + count) - n, n);
                count -= n;
            }
        } else {
            /* Measured: buf[count - 1] computes count - 1 first, where the
             * ROM adds buf + count and stores at -1. */
            *(buf + count - 1) = '\0';
            count -= 1;
        }

        if (dir_id != 0) {
            do {
                FSi_SeekDirDirect(&tmp, tmp.prop.dir.parent);
                tmp.arg.readdir.p_entry = &entry;
                tmp.arg.readdir.skip_string = 0;
                *(buf + count - 1) = '/';
                count--;
                if (FSi_TranslateCommand(&tmp, FS_COMMAND_READDIR) == FS_RESULT_SUCCESS) {
                    for (;;) {
                        if (entry.is_directory != 0 && entry.dir_id.own_id == dir_id) {
                            int n = entry.name_len;
                            MI_CpuCopy8(entry.name, (buf + count) - n, n);
                            count -= n;
                            break;
                        }
                        if (FSi_TranslateCommand(&tmp, FS_COMMAND_READDIR) != FS_RESULT_SUCCESS)
                            break;
                    }
                }
                dir_id = tmp.prop.dir.pos.own_id;
            } while (dir_id != 0);
        }
        return FS_RESULT_SUCCESS;
    }
}

/* Look file_id up in the FAT and open its image range directly. */
int FSi_OpenFileFastCommand(FSFile *p_file) {
    FSArchive *p_arc = p_file->arc;
    u32 index = p_file->arg.openfilefast.id.file_id;
    FSArchiveFAT fat;
    FSiSyncReadParam param;
    if (index * sizeof(fat) >= p_arc->fat_size)
        return FS_RESULT_FAILURE;
    param.arc = p_arc;
    param.pos = p_arc->fat + index * sizeof(fat);
    FSi_ReadTable(&param, &fat, sizeof(fat));
    p_file->arg.openfiledirect.top = fat.top;
    p_file->arg.openfiledirect.bottom = fat.bottom;
    p_file->arg.openfiledirect.index = index;
    return FSi_TranslateCommand(p_file, FS_COMMAND_OPENFILEDIRECT);
}

int FSi_OpenFileDirectCommand(FSFile *p_file) {
    p_file->prop.file.top = p_file->arg.openfiledirect.top;
    p_file->prop.file.pos = p_file->arg.openfiledirect.top;
    p_file->prop.file.bottom = p_file->arg.openfiledirect.bottom;
    p_file->prop.file.own_id = p_file->arg.openfiledirect.index;
    return FS_RESULT_SUCCESS;
}

int FSi_CloseFileCommand(FSFile *p_file) {
    return FS_RESULT_SUCCESS;
}
}
