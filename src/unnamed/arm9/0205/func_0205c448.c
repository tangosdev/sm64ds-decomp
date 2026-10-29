/* NitroSDK FSi_ReadFileCommand (FS_COMMAND_READFILE, entry 0 of the default
 * command table data_02086758): advance prop.file.pos by arg.readfile.len and
 * hand the read at the old position to the archive's read_func. Field names
 * follow the SDK's FSFile and FSArchive, as func_0205d688.c and
 * func_0205c9b8.c spell them; only the fields this function touches are
 * spelled out. */
typedef struct FSArchive FSArchive;

typedef void (*FS_ARCHIVE_READ_FUNC)(FSArchive *p_arc, void *dst, unsigned int pos,
                                     unsigned int size);

struct FSArchive {
    char pad_00[0x3c];
    FS_ARCHIVE_READ_FUNC read_func; /* 0x3c */
};

typedef struct FSFileProp {
    unsigned int own_id; /* 0x00 */
    unsigned int top;    /* 0x04 */
    unsigned int bottom; /* 0x08 */
    unsigned int pos;    /* 0x0c */
} FSFileProp;

typedef struct FSReadFileArg {
    void *dst;            /* 0x00 */
    unsigned int len_org; /* 0x04 -- the length asked for */
    unsigned int len;     /* 0x08 -- the length after clamping */
} FSReadFileArg;

typedef struct FSFile {
    char pad_00[8];
    FSArchive *arc; /* 0x08 */
    char pad_0c[0x10];
    union {
        FSFileProp file;
    } prop; /* 0x1c */
    union {
        FSReadFileArg readfile;
    } arg; /* 0x2c */
} FSFile;

void func_0205c448(char* c)
{
    FSFile *p_file = (FSFile *)c;
    FSArchive *obj = p_file->arc;
    unsigned inc = p_file->arg.readfile.len;
    unsigned b = p_file->prop.file.pos;
    void *a = p_file->arg.readfile.dst;
    p_file->prop.file.pos += inc;
    obj->read_func(obj, a, b, inc);
}
