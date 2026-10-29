/* NitroSDK FSi_WriteFileCommand (FS_COMMAND_WRITEFILE, entry 1 of the default
 * command table data_02086758): advance prop.file.pos by arg.writefile.len and
 * hand the write at the old position to the archive's write_func, the slot
 * after read_func (func_0205c448). Field names follow the SDK's FSFile and
 * FSArchive, as func_0205d688.c and func_0205c9b8.c spell them; only the
 * fields this function touches are spelled out. */
typedef struct FSArchive FSArchive;

typedef void (*FS_ARCHIVE_WRITE_FUNC)(FSArchive *p_arc, const void *src, unsigned int pos,
                                      unsigned int size);

struct FSArchive {
    char pad_00[0x40];
    FS_ARCHIVE_WRITE_FUNC write_func; /* 0x40 */
};

typedef struct FSFileProp {
    unsigned int own_id; /* 0x00 */
    unsigned int top;    /* 0x04 */
    unsigned int bottom; /* 0x08 */
    unsigned int pos;    /* 0x0c */
} FSFileProp;

typedef struct FSWriteFileArg {
    const void *src;      /* 0x00 */
    unsigned int len_org; /* 0x04 -- the length asked for */
    unsigned int len;     /* 0x08 -- the length after clamping */
} FSWriteFileArg;

typedef struct FSFile {
    char pad_00[8];
    FSArchive *arc; /* 0x08 */
    char pad_0c[0x10];
    union {
        FSFileProp file;
    } prop; /* 0x1c */
    union {
        FSWriteFileArg writefile;
    } arg; /* 0x2c */
} FSFile;

void func_0205c410(char* c)
{
    FSFile *p_file = (FSFile *)c;
    FSArchive *obj = p_file->arc;
    unsigned inc = p_file->arg.writefile.len;
    unsigned b = p_file->prop.file.pos;
    const void *a = p_file->arg.writefile.src;
    p_file->prop.file.pos += inc;
    obj->write_func(obj, a, b, inc);
}
