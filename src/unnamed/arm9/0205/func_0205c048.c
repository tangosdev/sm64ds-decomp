extern int func_0205c5e4(void *self, int mode);
extern void func_0205c4e4(void *self, unsigned int value);
extern int func_0205c480(unsigned char *a, unsigned char *b, int n);

/* NitroSDK FSi_FindPathCommand(p_dir): walk arg.findpath.path one component
 * at a time from the directory at arg.findpath.pos ("." skips, ".." seeks to
 * prop.dir.parent unless at the root), reading entries (FS_COMMAND_READDIR =
 * 3 through FSi_TranslateCommand = func_0205c5e4) until one matches by kind,
 * length and name (func_0205c480 = FSi_StrNICmp); a directory is entered
 * (FS_COMMAND_SEEKDIR = 2), and the final match is stored through
 * arg.findpath.result as a file id or, with find_directory, a directory
 * position. Returns 0 (FS_RESULT_SUCCESS) or 1 (FS_RESULT_FAILURE). Field
 * names follow the SDK's FSFile and FSDirEntry, as func_0205bc88.c spells
 * them; only the fields this function touches are spelled out. */
typedef struct FSFileID {
    void *arc;            /* 0x00 */
    unsigned int file_id; /* 0x04 */
} FSFileID;

typedef struct FSDirPos {
    void *arc;             /* 0x00 */
    unsigned short own_id; /* 0x04 */
    unsigned short index;  /* 0x06 */
    unsigned int pos;      /* 0x08 */
} FSDirPos;

typedef struct FSDirEntry {
    union {
        FSFileID file_id;
        FSDirPos dir_id;
    } id;                    /* 0x00 */
    int is_directory;        /* 0x0c */
    int name_len;            /* 0x10 */
    unsigned char name[128]; /* 0x14 -- FS_FILE_NAME_MAX */
} FSDirEntry;

typedef struct FSFile {
    char pad_00[0x1c];
    union {
        struct {
            FSDirPos pos;        /* 0x1c */
            unsigned int parent; /* 0x28 */
        } dir;
    } prop; /* 0x1c */
    union {
        struct {
            FSDirPos pos; /* 0x2c */
        } seekdir;
        struct {
            FSDirEntry *p_entry; /* 0x2c */
            int skip_string;     /* 0x30 */
        } readdir;
        struct {
            FSDirPos pos;        /* 0x2c -- where the search starts */
            unsigned char *path; /* 0x38 */
            int find_directory;  /* 0x3c */
            union {
                FSFileID *file;
                FSDirPos *dir;
            } result; /* 0x40 */
        } findpath;
    } arg; /* 0x2c */
} FSFile;

typedef char FSDirEntry_size_must_be_0x94[sizeof(FSDirEntry) == 0x94 ? 1 : -1];
typedef char FSFile_size_must_be_0x44[sizeof(FSFile) == 0x44 ? 1 : -1];

/* f stays a materialized 0/1: if/return 1 and ?1:0 bodies grow the caller (0x220-0x228) */
static inline int IsNameChar(int c) {
    int f = 0;
    if (c != 0 && c != 0x2f && c != 0x5c)
        f = 1;
    return f;
}

int func_0205c048(unsigned char *self) {
    FSFile *p_dir = (FSFile *)self;
    int find_dir = p_dir->arg.findpath.find_directory;
    unsigned char *path = p_dir->arg.findpath.path;
    unsigned char c;
    /* First the character that ended the component (nonzero: a separator
     * follows, so a directory is wanted), then the 0/1 the entry must match. */
    int want_dir;
    int len;

    FSDirEntry entry;

    func_0205c5e4(self, 2);

    for (c = *path; c != 0; path += (*path != 0) ? 1 : 0, c = *path) {
        len = 0;
        while (IsNameChar(want_dir = path[len]))
            len = len + 1;

        if (want_dir != 0 || find_dir != 0)
            want_dir = 1;

        if (len == 0)
            return 1;

        if (c == 0x2e) {
            if (len == 1) {
                path = path + 1;
                continue;
            }
            if ((len == 2) & (path[1] == 0x2e)) { /* bitwise &: && shrinks it to 0x20c */
                if (p_dir->prop.dir.pos.own_id != 0) {
                    func_0205c4e4(self, p_dir->prop.dir.parent);
                }
                path = path + 2;
                continue;
            }
        }

        if (len > 0x7f)
            return 1;

        p_dir->arg.readdir.p_entry = &entry;
        p_dir->arg.readdir.skip_string = 0;

        for (;;) {
            if (func_0205c5e4(self, 3) != 0)
                return 1;
            if (want_dir != entry.is_directory)
                continue;
            if (len != entry.name_len)
                continue;
            if (func_0205c480(path, entry.name, len) != 0)
                continue;
            break;
        }

        if (want_dir != 0) {
            p_dir->arg.seekdir.pos = entry.id.dir_id;
            path = path + len;
            func_0205c5e4(self, 2);
            continue;
        }

        if (find_dir != 0)
            return 1;
        {
            *p_dir->arg.findpath.result.file = entry.id.file_id;
        }
        return 0;
    }

    if (find_dir == 0)
        return 1;
    *p_dir->arg.findpath.result.dir = p_dir->prop.dir.pos;
    return 0;
}
