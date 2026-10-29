extern int func_0205cdf4(char* self, int n);

/* NitroSDK FS_OpenFileDirect(p_file, p_arc, image_top, image_bottom,
 * file_index): store the archive and arg.openfiledirect, send
 * FS_COMMAND_OPENFILEDIRECT (7) through func_0205cdf4 (FSi_SendCommand), and
 * on success mark the handle FS_FILE_STATUS_IS_FILE (0x10), not
 * FS_FILE_STATUS_IS_DIR (0x20). Field names follow the SDK's FSFile, as
 * func_0205bc0c.c spells it; only the fields this function touches are
 * spelled out. */
typedef struct FSOpenFileDirectArg {
    int top;    /* 0x00 */
    int bottom; /* 0x04 */
    int index;  /* 0x08 */
} FSOpenFileDirectArg;

typedef struct FSFile {
    char pad_00[8];
    int arc;  /* 0x08 -- the FSArchive */
    int stat; /* 0x0c */
    char pad_10[0x1c];
    union {
        FSOpenFileDirectArg openfiledirect;
    } arg; /* 0x2c */
} FSFile;

int func_0205d5e8(char* self, int a1, int a2, int a3, int a4)
{
    FSFile *p_file = (FSFile *)self;
    int* p;
    p_file->arc = a1;
    p_file->arg.openfiledirect.index = a4;
    p_file->arg.openfiledirect.top = a2;
    p_file->arg.openfiledirect.bottom = a3;
    if(func_0205cdf4(self, 7) == 0) return 0;
    p = &p_file->stat;
    *p |= 0x10;
    *p &= ~0x20;
    return 1;
}
