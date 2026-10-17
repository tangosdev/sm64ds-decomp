extern void func_0205c528(void *dst, void *bits, int n);

struct Out { unsigned char b0; unsigned char pad; unsigned short b2; };

/* NitroSDK FSi_ReadDirCommand: read the next FNT entry of the open directory
 * into the caller's FSDirEntry. func_0205c528 is FSi_ReadTable over the
 * {arc, pos} pair in `two`. The entry's lead byte is the name length (low
 * seven bits) and the is-directory bit; a directory entry is followed by its
 * id, of which the low 12 bits are kept. Field names follow the SDK's
 * FSFile/FSDirEntry; only the fields this file reads are spelled out. */
typedef struct FSFileID {
    int arc;              /* 0x00 */
    unsigned int file_id; /* 0x04 */
} FSFileID;

typedef struct FSDirPos {
    int arc;               /* 0x00 */
    unsigned short own_id; /* 0x04 */
    unsigned short index;  /* 0x06 */
    unsigned int pos;      /* 0x08 */
} FSDirPos;

typedef struct FSDirEntry {
    union {
        FSFileID file_id;
        FSDirPos dir_id;
    } id;             /* 0x00 */
    int is_directory; /* 0x0c */
    int name_len;     /* 0x10 */
    char name[1];     /* 0x14 -- FS_FILE_NAME_MAX + 1 in the SDK */
} FSDirEntry;

typedef struct FSFile {
    char pad_00[8];
    int arc; /* 0x08 */
    char pad_0c[0x10];
    FSDirPos dir_pos; /* 0x1c -- prop.dir.pos */
    char pad_28[4];
    FSDirEntry *p_entry; /* 0x2c -- arg.readdir.p_entry */
    int skip_string;     /* 0x30 -- arg.readdir.skip_string */
} FSFile;

int func_0205c264(int *thiz)
{
    FSFile *p_dir = (FSFile *)thiz;
    FSDirEntry *p_entry = p_dir->p_entry;
    struct Out out;                  /* [sp+0] */
    int two[2];                      /* [sp+4]=arc, [sp+8]=pos */
    unsigned char v;
    int hi, lo;

    two[0] = p_dir->arc;
    two[1] = p_dir->dir_pos.pos;
    func_0205c528(&two[0], &out, 1);
    v = out.b0;
    lo = v & 0x7f;
    hi = (v >> 7) & 1;
    p_entry->name_len = lo;
    p_entry->is_directory = hi;
    if (p_entry->name_len == 0) {
        return 1;
    }
    if (p_dir->skip_string != 0) {
        two[1] = two[1] + p_entry->name_len;
    } else {
        func_0205c528(&two[0], p_entry->name, p_entry->name_len);
        p_entry->name[p_entry->name_len] = 0;
    }
    if (p_entry->is_directory != 0) {
        func_0205c528(&two[0], &out.b2, 2);
        p_entry->id.dir_id.arc = p_dir->arc;
        p_entry->id.dir_id.own_id = out.b2 & 0xfff;
        p_entry->id.dir_id.index = 0;
        p_entry->id.dir_id.pos = 0;
    } else {
        p_entry->id.file_id.arc = p_dir->arc;
        p_entry->id.file_id.file_id = p_dir->dir_pos.index;
        ++p_dir->dir_pos.index;
    }
    p_dir->dir_pos.pos = two[1];
    return 0;
}
