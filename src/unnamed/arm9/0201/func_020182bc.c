/* NitroSDK's FSFileID: the archive and the file id inside it. */
struct FSFileID {
    int arc, file_id;
};

extern void FS_InitFile(int *s);
extern int myFS_OpenFileFast(int buf, struct FSFileID ab);
extern void Crash(void);
extern unsigned int func_0201834c(int *buf, int x);
extern int myFS_ConvertPathToFileID(void *out_file_id, const char *path);

/* Load the file an FSFileID names (see func_0201834c); flush nonzero
   flushes the result from the data cache. */
int func_020182ec(struct FSFileID fileId, int flush) {
    int file[0x11];
    int result = 0;
    FS_InitFile(file);
    if (myFS_OpenFileFast((int)file, fileId) == 0)
        Crash();
    else
        result = func_0201834c(file, flush);
    return result;
}

/* Load the ROM file at path into a new heap block, decompressed if it is
   LZ77-tagged, and flush it from the data cache (Ov004_LoadFileByPath). */
void *func_020182bc(const char *path) {
    struct FSFileID fileId;
    myFS_ConvertPathToFileID(&fileId, path);
    return (void *)func_020182ec(fileId, 1);
}
