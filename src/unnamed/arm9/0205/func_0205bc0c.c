/* NitroSDK FSi_OpenFileFastCommand (entry 6, FS_COMMAND_OPENFILEFAST, of the
 * default command table data_02086758): look up arg.openfilefast.id.file_id
 * in the archive's FAT. An index at or past the FAT's entry count
 * (fat_size / 8) fails (1, FS_RESULT_FAILURE);
 * otherwise func_0205c528 (FSi_ReadTable) reads the 8-byte FAT entry at
 * fat + index * 8, its top and bottom go to arg.openfiledirect with the index,
 * and the file is handed on as FS_COMMAND_OPENFILEDIRECT (7) through
 * func_0205c5e4 (FSi_TranslateCommand). Field names follow the SDK's FSFile
 * and FSArchive, as func_0205c9b8.c and func_0205bc88.c spell them; only the
 * fields this function touches are spelled out. */
typedef struct FSArchive {
    char pad_00[0x20];
    char *fat;             /* 0x20 */
    unsigned int fat_size; /* 0x24 */
} FSArchive;

typedef struct FSFileID {
    FSArchive *arc;       /* 0x00 */
    unsigned int file_id; /* 0x04 */
} FSFileID;

typedef struct FSOpenFileDirectArg {
    void *top;          /* 0x00 */
    void *bottom;       /* 0x04 */
    unsigned int index; /* 0x08 */
} FSOpenFileDirectArg;

typedef struct FSFile {
    char pad_00[8];
    FSArchive *arc; /* 0x08 */
    char pad_0c[0x20];
    union {
        struct {
            FSFileID id;
        } openfilefast;
        FSOpenFileDirectArg openfiledirect;
    } arg; /* 0x2c */
} FSFile;

/* FSi_ReadTable's {arc, pos} read parameter (FSiSyncReadParam), and the FAT
 * entry it fills (FSArchiveFAT). */
struct Pair { void *a; void *b; };
extern void func_0205c528(struct Pair *in, struct Pair *out, int n);
extern int func_0205c5e4(void *self, int x);

int func_0205bc0c(char *self)
{
    FSFile *p_file = (FSFile *)self;
    FSArchive *p_arc = p_file->arc;
    int n = p_file->arg.openfilefast.id.file_id;
    struct Pair out;
    struct Pair in;
    if ((unsigned)(n << 3) >= p_arc->fat_size)
        return 1;
    in.a = p_arc;
    in.b = p_arc->fat + (n << 3);
    func_0205c528(&in, &out, 8);
    p_file->arg.openfiledirect.top = out.a;
    p_file->arg.openfiledirect.bottom = out.b;
    p_file->arg.openfiledirect.index = n;
    return func_0205c5e4(self, 7);
}
