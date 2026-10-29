#include "types.h"

/* NitroSDK FSi_GetPathCommand: build "arc:/dir/.../name" for an open file or
 * directory into arg.getpath.buf. A file's directory is found by walking
 * every directory's entries (FSi_SeekDirDirect = func_0205c4e4, then
 * FS_COMMAND_READDIR = 3 through FSi_TranslateCommand = func_0205c5e4) until
 * one names the file's own_id. The total length and directory id are cached
 * in arg.getpath, so a caller can ask for the length first and then call
 * again with a buffer. Field names follow the SDK's FSArchive, FSFile and
 * FSDirEntry, as func_0205c264.c spells them; only the fields read here are
 * spelled out. */
typedef struct FSArchive {
    u32 name; /* 0x00 -- name.pack: up to three characters, zero padded */
} FSArchive;

typedef struct FSFileID {
    FSArchive *arc; /* 0x00 */
    u32 file_id;    /* 0x04 */
} FSFileID;

typedef struct FSDirPos {
    FSArchive *arc; /* 0x00 */
    u16 own_id;     /* 0x04 */
    u16 index;      /* 0x06 */
    u32 pos;        /* 0x08 */
} FSDirPos;

typedef struct FSDirEntry {
    union {
        FSFileID file_id;
        FSDirPos dir_id;
    } id;             /* 0x00 */
    u32 is_directory; /* 0x0c */
    u32 name_len;     /* 0x10 */
    char name[128];   /* 0x14 -- FS_FILE_NAME_MAX */
} FSDirEntry;

typedef struct FSFileProp {
    u32 own_id; /* 0x00 */
    u32 top;    /* 0x04 */
    u32 bottom; /* 0x08 */
    u32 pos;    /* 0x0c */
} FSFileProp;

typedef struct FSDirProp {
    FSDirPos pos; /* 0x00 */
    u32 parent;   /* 0x0c -- read from the root: the directory count */
} FSDirProp;

/* arg.getpath, held through a pointer: the ROM keeps &arg.getpath in a
 * register for the whole function. */
typedef struct FSGetPathArg {
    u8 *buf;       /* 0x00 */
    u32 buf_len;   /* 0x04 */
    u16 total_len; /* 0x08 -- 0 until measured */
    u16 dir_id;    /* 0x0a */
} FSGetPathArg;

typedef struct FSFile {
    char pad_00[8];
    FSArchive *arc; /* 0x08 */
    u32 stat;       /* 0x0c -- FS_FILE_STATUS_IS_DIR = 0x20 */
    char pad_10[0xc];
    union {
        FSFileProp file;
        FSDirProp dir;
    } prop; /* 0x1c */
    union {
        struct {
            FSDirEntry *p_entry; /* 0x2c */
            u32 skip_string;     /* 0x30 */
        } readdir;
        FSGetPathArg getpath;
    } arg; /* 0x2c */
    char pad_38[0xc];
} FSFile;

extern void FS_InitFile(int* s);
extern void func_0205c4e4(void* self, int value);
extern int func_0205c5e4(void* self, int x);
extern void CpuCopy8(const void* src, void* dst, unsigned int size);
extern const unsigned char data_0209a044[];

int func_0205bc88(FSFile *p_file) {
    FSArchive *p_arc;
    FSDirEntry entry;
    FSFile tmp;
    FSGetPathArg *m;
    register int dir_id;
    register int file_id;
    unsigned int i;
    unsigned int cnt;
    int is_dir;

    p_arc = p_file->arc;
    m = &p_file->arg.getpath;
    FS_InitFile((int *)&tmp);
    tmp.arc = p_file->arc;

    is_dir = (p_file->stat & 0x20) ? 1 : 0;
    if (is_dir != 0) {
        dir_id = p_file->prop.dir.pos.own_id;
        file_id = 0x10000;
    } else {
        file_id = p_file->prop.file.own_id;
        if (m->total_len != 0) {
            dir_id = m->dir_id;
        } else {
            dir_id = 0x10000;
            i = 0;
            cnt = 0;
            do {
                func_0205c4e4(&tmp, i);
                if (i == 0)
                    cnt = tmp.prop.dir.parent;
                tmp.arg.readdir.p_entry = &entry;
                tmp.arg.readdir.skip_string = 1;
                if (func_0205c5e4(&tmp, 3) == 0) {
                    for (;;) {
                        if (entry.is_directory == 0 && entry.id.file_id.file_id == file_id) {
                            dir_id = tmp.prop.dir.pos.own_id;
                            break;
                        }
                        if (func_0205c5e4(&tmp, 3) != 0)
                            break;
                    }
                }
                if (dir_id != 0x10000)
                    break;
                i++;
            } while (i < cnt);
        }
    }

    if (dir_id == 0x10000) {
        m->total_len = 0;
        return 1;
    }

    if (m->total_len == 0) {
        cnt = 0;
        if (p_arc->name <= 0xff)
            cnt += 1;
        else if (p_arc->name <= 0xff00)
            cnt += 2;
        else
            cnt += 3;
        cnt += 2;
        if (file_id != 0x10000)
            cnt += entry.name_len;

        i = dir_id;
        if (dir_id != 0) {
            func_0205c4e4(&tmp, dir_id);
            do {
                func_0205c4e4(&tmp, tmp.prop.dir.parent);
                tmp.arg.readdir.p_entry = &entry;
                tmp.arg.readdir.skip_string = 1;
                if (func_0205c5e4(&tmp, 3) == 0) {
                    for (;;) {
                        if (entry.is_directory != 0 && entry.id.dir_id.own_id == i) {
                            cnt += entry.name_len + 1;
                            break;
                        }
                        if (func_0205c5e4(&tmp, 3) != 0)
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
        char *buf = (char *)m->buf;
        int count;

        if (buf == 0)
            return 0;
        count = m->total_len;
        if (m->buf_len < (unsigned)count)
            return 1;

        {
            int pos;
            int nb;

            pos = 0;
            if (p_arc->name <= 0xff)
                nb = 1;
            else if (p_arc->name <= 0xff00)
                nb = 2;
            else
                nb = 3;

            CpuCopy8(p_arc, buf, nb);
            pos += nb;
            CpuCopy8(data_0209a044, buf + pos, 2);
        }

        func_0205c4e4(&tmp, dir_id);
        if (file_id != 0x10000) {
            tmp.arg.readdir.p_entry = &entry;
            tmp.arg.readdir.skip_string = 0;
            if (func_0205c5e4(&tmp, 3) == 0) {
                for (;;) {
                    if (entry.is_directory == 0 && entry.id.file_id.file_id == file_id)
                        break;
                    if (func_0205c5e4(&tmp, 3) != 0)
                        break;
                }
            }
            {
                int k = entry.name_len + 1;
                CpuCopy8(entry.name, (buf + count) - k, k);
                count -= k;
            }
        } else {
            *(char*)(buf + count - 1) = 0;
            count -= 1;
        }

        if (dir_id != 0) {
            do {
                func_0205c4e4(&tmp, tmp.prop.dir.parent);
                tmp.arg.readdir.p_entry = &entry;
                tmp.arg.readdir.skip_string = 0;
                *(char*)(buf + count - 1) = 0x2f;
                count--;
                if (func_0205c5e4(&tmp, 3) == 0) {
                    for (;;) {
                        if (entry.is_directory != 0 && entry.id.dir_id.own_id == dir_id) {
                            int ln = entry.name_len;
                            CpuCopy8(entry.name, (buf + count) - ln, ln);
                            count -= ln;
                            break;
                        }
                        if (func_0205c5e4(&tmp, 3) != 0)
                            break;
                    }
                }
                dir_id = tmp.prop.dir.pos.own_id;
            } while (dir_id != 0);
        }
        return 0;
    }
}
