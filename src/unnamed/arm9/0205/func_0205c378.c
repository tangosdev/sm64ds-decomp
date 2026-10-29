extern void func_0205c528(void *a, void *b, int n);

/* NitroSDK FSi_SeekDirCommand (FS_COMMAND_SEEKDIR): read directory own_id's
 * 8-byte FNT entry at fnt + own_id * 8 through func_0205c528 (FSi_ReadTable),
 * copy arg.seekdir.pos to prop.dir.pos and, when it names the directory's
 * start (index and pos both 0), point it at the entry's first child instead.
 * The parent id keeps its low 12 bits. Field names follow the SDK's FSFile,
 * FSArchive and FSDirPos, as func_0205c048.c and func_0205c9b8.c spell them;
 * only the fields this function touches are spelled out. */
typedef struct FSArchive {
    char pad_00[0x28];
    int fnt; /* 0x28 */
} FSArchive;

typedef struct FSDirPos {
    FSArchive *arc;        /* 0x00 */
    unsigned short own_id; /* 0x04 */
    unsigned short index;  /* 0x06 */
    unsigned int pos;      /* 0x08 */
} FSDirPos;

typedef struct FSFile {
    char pad_00[8];
    FSArchive *arc; /* 0x08 */
    char pad_0c[0x10];
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
    } arg; /* 0x2c */
} FSFile;

/* FSi_ReadTable's {arc, pos} read parameter (FSiSyncReadParam), and the FNT
 * directory entry it fills (FSArchiveFNT). */
typedef struct FSiSyncReadParam {
    FSArchive *arc; /* 0x00 */
    int pos;        /* 0x04 */
} FSiSyncReadParam;

typedef struct FSArchiveFNT {
    int start;             /* 0x00 -- the first child's offset into fnt */
    unsigned short index;  /* 0x04 -- the first child's file id */
    unsigned short parent; /* 0x06 */
} FSArchiveFNT;

int func_0205c378(int *thiz)
{
    FSFile *p_dir = (FSFile *)thiz;
    FSArchive *p_arc = p_dir->arc;
    FSDirPos *p_pos = &p_dir->arg.seekdir.pos;
    FSArchiveFNT fnt;
    FSiSyncReadParam param;

    param.arc = p_arc;
    param.pos = p_arc->fnt + ((int)p_pos->own_id << 3);
    func_0205c528(&param, &fnt, 8);

    p_dir->prop.dir.pos = *p_pos;

    if (p_pos->index == 0 && p_pos->pos == 0) {
        p_dir->prop.dir.pos.index = fnt.index;
        p_dir->prop.dir.pos.pos = p_arc->fnt + fnt.start;
    }
    p_dir->prop.dir.parent = fnt.parent & 0xfff;
    return 0;
}
