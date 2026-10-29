#include "private/arm9_a9db8_mb_parent.h"
extern char *data_020a9db8;

/* Step file idx's block cursor for the multiboot parent: a registered file
 * with children waiting sends the next block while mCurrentBlock stays within
 * two of the highest block a child asked for, and otherwise jumps back to
 * that block. The file record is reached the ROM's way, as func_0206655c.c's
 * rec does: the work pointer stepped a whole MbParentFile per index, then
 * mFiles[0] from there, each field folded into one offset. */
void func_02065f08(int idx) {
    unsigned short r1;
    unsigned short r3;
    MbParentWork *b = (MbParentWork *)(data_020a9db8 + idx * sizeof(MbParentFile));
    if (b->mFiles[0].mActive == 0)
        return;
    if (b->mFiles[0].mChildBitmap == 0)
        return;
    r1 = b->mFiles[0].mCurrentBlock;
    r3 = b->mFiles[0].mRequestedBlock;
    if (r3 <= r1 && r1 <= r3 + 2) {
        b->mFiles[0].mCurrentBlock++;
        return;
    }
    b->mFiles[0].mCurrentBlock = r3;
}
