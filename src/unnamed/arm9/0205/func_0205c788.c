struct Node;
extern void func_0205d920(struct Node *node);
extern void OS_WakeupThread(unsigned short *self);

/* NitroSDK FSFile, the fields touched here, with the names func_0205d044.cpp
 * gives them. */
typedef struct FSFile {
    char pad_00[0xc];
    unsigned int stat;    /* 0x0c */
    int command;          /* 0x10 */
    int error;            /* 0x14 */
    unsigned short queue; /* 0x18 -- the thread queue a synchronous caller sleeps on */
} FSFile;

/* NitroSDK FSi_ReleaseCommand(p_file, ret): unlink the file from its
 * archive's command list (func_0205d920 = FSi_CutFromList), clear the
 * command status bits (FS_FILE_STATUS_BUSY | CANCEL | SYNC | ASYNC, 0xf),
 * store ret and wake anyone waiting on the file. The parameter keeps its
 * char * spelling: the other declarations are checked against it. */
void func_0205c788(char *self, int arg)
{
    FSFile *p_file = (FSFile *)self;
    func_0205d920((struct Node *)self);
    p_file->stat &= ~0xf;
    p_file->error = arg;
    OS_WakeupThread(&p_file->queue);
}
