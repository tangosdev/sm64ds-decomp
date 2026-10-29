extern int func_0205c5e4(void *self, int mode);

/* NitroSDK FSi_SeekDirDirect(p_dir, id): mark the directory handle
 * FS_FILE_STATUS_SYNC (4), point arg.seekdir.pos at the start of directory id
 * in the handle's own archive, and run FS_COMMAND_SEEKDIR (2) through
 * func_0205c5e4 (FSi_TranslateCommand). Field names follow the SDK's FSFile
 * and FSDirPos, as func_0205bc88.c and func_0205c048.c spell them; only the
 * fields this function touches are spelled out. */
typedef struct FSArchive FSArchive;

typedef struct FSDirPos {
    FSArchive *arc;        /* 0x00 */
    unsigned short own_id; /* 0x04 */
    unsigned short index;  /* 0x06 */
    unsigned int pos;      /* 0x08 */
} FSDirPos;

typedef struct FSFile {
    char pad_00[8];
    FSArchive *arc;    /* 0x08 */
    unsigned int stat; /* 0x0c */
    char pad_10[0x1c];
    union {
        struct {
            FSDirPos pos; /* 0x2c */
        } seekdir;
    } arg; /* 0x2c */
} FSFile;

void func_0205c4e4(char *self, unsigned short value)
{
    FSFile *p_dir = (FSFile *)self;
    p_dir->stat |= 4;
    p_dir->arg.seekdir.pos.arc = p_dir->arc;
    p_dir->arg.seekdir.pos.pos = 0;
    p_dir->arg.seekdir.pos.index = 0;
    p_dir->arg.seekdir.pos.own_id = value;
    func_0205c5e4(self, 2);
}
