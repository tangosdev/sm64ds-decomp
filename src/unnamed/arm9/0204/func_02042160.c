#include "private/arm9_a1fc0_file_share.h"

extern char data_020a1fc0[];
extern void func_02041e14(void);

/* The parent's reply-send callback: once the file-sharing work is up
 * (mState) and running as the parent, clear mReplySending, which
 * func_02041e14 set after its send, and let it send the next reply. */
void func_02042160(void)
{
    FileShareWork *g = FILE_SHARE_WORK;
    if (g->mState == 0)
        return;
    if (g->mIsParent == 0)
        return;
    {
        FileShareParentArea *s = (FileShareParentArea *)g->mArea;
        s->mReplySending = 0;
        func_02041e14();
    }
}
