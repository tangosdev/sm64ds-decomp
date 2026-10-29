extern int func_0205cdf4(void *self, int arg);

/* NitroSDK FS_CloseFile: send FS_COMMAND_CLOSEFILE (8), then clear arc, set
 * command to FS_COMMAND_INVALID (0xc) and drop FS_FILE_STATUS_IS_FILE |
 * FS_FILE_STATUS_IS_DIR (0x30). Field names follow the SDK's FSFile, as
 * func_0205d044.cpp spells them; only the fields this function touches. */
typedef struct FSFile {
    struct FSFile *prev;   /* 0x00 -- link.prev */
    struct FSFile *next;   /* 0x04 -- link.next */
    struct FSArchive *arc; /* 0x08 */
    unsigned int stat;     /* 0x0c */
    int command;           /* 0x10 */
} FSFile;
typedef char FSFile_size_must_be_0x14[sizeof(FSFile) == 0x14 ? 1 : -1];

int FS_CloseFile(char *self)
{
    FSFile *p_file = (FSFile *)self;

    if (func_0205cdf4(self, 8) == 0)
        return 0;

    p_file->arc = 0;
    p_file->command = 0xc;
    p_file->stat &= ~0x30;
    return 1;
}
