/* NitroSDK FSi_ReadFileCore(p_file, dst, len, async): clamp len to what is
 * left between prop.file.pos and prop.file.bottom (and to 0), store the
 * request in arg.readfile, and send FS_COMMAND_READFILE (0) through
 * func_0205cdf4 (FSi_SendCommand). A synchronous caller (async == 0; FS_ReadFile)
 * marks the file FS_FILE_STATUS_SYNC (4) first, waits in func_0205d3f4
 * (FS_WaitAsync) and returns the bytes the position moved, or -1 on failure;
 * func_0205d3e4 (FS_ReadFileAsync) returns the clamped length at once. Field
 * names follow the SDK's FSFile, as func_0205bc88.c spells them; only the
 * fields this function touches are spelled out. */
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
    char pad_00[0xc];
    unsigned int stat; /* 0x0c */
    char pad_10[0xc];
    union {
        FSFileProp file;
    } prop; /* 0x1c */
    union {
        FSReadFileArg readfile;
    } arg; /* 0x2c */
} FSFile;

extern int func_0205cdf4(void *self, int a);
extern int func_0205d3f4(void *self);
int func_0205d688(char *self, int dst, int len, int async) {
    FSFile *p_file = (FSFile *)self;
    int n;
    int is_async;
    int oldpos;
    oldpos = p_file->prop.file.pos;
    /* The (long long) launders are load-bearing: plain copies change .text. */
    n = (int)(((long long)len));
    is_async = (int)(((long long)async));
    {
        int avail = p_file->prop.file.bottom - oldpos;
        if (n > avail) {
            n = avail;
        }
    }
    p_file->arg.readfile.dst = (void *)dst;
    if (n < 0) {
        n = 0;
    }
    p_file->arg.readfile.len_org = len;
    p_file->arg.readfile.len = n;
    if (is_async == 0) {
        p_file->stat |= 4;
    }
    func_0205cdf4(self, 0);
    if (is_async == 0) {
        if (func_0205d3f4(self)) {
            n = p_file->prop.file.pos - oldpos;
        } else {
            n = -1;
        }
    }
    return n;
}
