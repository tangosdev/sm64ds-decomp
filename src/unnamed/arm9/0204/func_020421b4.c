#include "nitro/os/thread.h"

/* The file-share parent's loader thread block; func_02042200 describes it. */
typedef struct FileShareLoaderThread {
    u8 stack[0x400]; /* 0x000 */
    OSThread thread; /* 0x400 */
    u16 unk494;      /* 0x494 */
} FileShareLoaderThread;

extern FileShareLoaderThread data_020a11e4;

/* Stop the file-share loader thread (see func_02042200): until it is
 * terminated, wake it so that it sees mState cleared and returns, then join
 * it (OS_JoinThread sleeps on its join queue). func_02040aa4 calls this at
 * shutdown. The (int) launder is needed as in func_02042200: with a plain
 * `&data_020a11e4` the function comes out 0x50 bytes, not the ROM's 0x4c. */
void func_020421b4(void) {
    FileShareLoaderThread *work = (FileShareLoaderThread *)(int)&data_020a11e4;
    if (OS_IsThreadTerminated(&work->thread))
        return;
    do {
        OS_WakeupThreadDirect(&work->thread);
        OS_JoinThread(&work->thread);
    } while (!OS_IsThreadTerminated(&work->thread));
}
