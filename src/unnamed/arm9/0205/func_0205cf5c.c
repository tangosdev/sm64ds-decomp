extern int func_0205c5e4(int this, int a);
extern void func_0205c788(int this, int a);
extern int func_0205d044(int a);
extern void func_0205cfa4(int a);

/* NitroSDK FSi_ExecuteSyncCommand(p_file): run the file's command through
 * func_0205c5e4 (FSi_TranslateCommand) and release it with the result
 * (func_0205c788 = FSi_ReleaseCommand), start the archive's next queued
 * command (func_0205d044 = FSi_NextCommand, func_0205cfa4 =
 * FSi_ExecuteAsyncCommand), and report whether the file's error is
 * FS_RESULT_SUCCESS (0). FSFile carries the names func_0205d044.cpp gives
 * those fields. */
typedef struct FSFile {
    char pad_00[8];
    int arc;           /* 0x08 */
    unsigned int stat; /* 0x0c */
    int command;       /* 0x10 */
    int error;         /* 0x14 */
} FSFile;

int func_0205cf5c(int this)
{
    FSFile *p_file = (FSFile *)this;
    int r;
    r = func_0205c5e4(this, p_file->command);
    func_0205c788(this, r);
    r = func_0205d044(p_file->arc);
    if (r != 0) func_0205cfa4(r);
    return p_file->error == 0;
}
