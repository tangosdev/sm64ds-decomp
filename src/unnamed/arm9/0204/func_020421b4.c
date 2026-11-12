#include "nitro/os/thread.h"

/* The loader thread's block at data_020a11e4, as func_02042200 creates it:
 * the thread's 0x400-byte stack, then its OSThread. */
typedef struct {
    char stack[0x400];
    OSThread thread;
} LoaderThreadBlock;

extern char data_020a11e4[];

/* Stop the file-share loader thread (see func_02042200): until the thread is
 * terminated (state 2), wake it so that it sees mState cleared and returns,
 * then join it (OS_JoinThread sleeps on its join queue). func_02040aa4 calls
 * this at shutdown. */
void func_020421b4(void)
{
    LoaderThreadBlock *blk = (LoaderThreadBlock *)data_020a11e4;
    if (OS_IsThreadTerminated(&blk->thread))
        return;
    do {
        OS_WakeupThreadDirect(&blk->thread);
        OS_JoinThread(&blk->thread);
    } while (!OS_IsThreadTerminated(&blk->thread));
}
