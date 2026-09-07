/* Wireless-download parent send step, arm9 0x0206610c (0x1b4 bytes). Steps
 * the current file index mod 16 up to sixteen times looking for a registered
 * file with children waiting on it, fetches the block to send through
 * func_02067530, writes a type-4 record header, copies the block after it,
 * and hands the packet to func_02067f2c.
 *
 * Typed access through private/arm9_a9db8_mb_parent.h is the match under
 * 2004/b56. The raw char* form this replaces pooled the whole 0x1525 address
 * once the byte was read from more than one statement (+12 bytes), and with
 * that fixed by hand a named `g + 0x1000` base still rotated five registers
 * in the three reads before CpuCopy8; a struct field at 0x1525 makes that
 * base a compiler temp and the colouring falls into place. */
#include "types.h"
#include "private/arm9_a9db8_mb_parent.h"
extern MbParentWork *data_020a9db8;

extern void func_02065f08(int idx);
struct Out { int f0; u32 f4; int f8; u8 fc; };
extern int func_02067530(struct Out *o, u8 *in, u32 val, u8 *tbl);

typedef struct Rec {
    u8 type;
    char _pad;
    u16 a;
    u16 b;
} Rec;
extern u8 *func_02065d5c(Rec *src, u8 *dst);
extern void CpuCopy8(void *dst, void *src, unsigned int n);
extern void func_02067f2c(u32 size, u32 arg1, u32 dest);

int func_0206610c(void)
{
    unsigned int loopCount;
    u8 idxByte;
    Rec r;
    struct Out out;

    if (data_020a9db8->unk_1524 == 0) return 0x15;

    loopCount = 0;
    do {
        {
            MbParentWork *g = *(MbParentWork *volatile *)&data_020a9db8;
            g->mCurrentFile = (g->mCurrentFile + 1) % 16;
        }
        idxByte = data_020a9db8->mCurrentFile;

        if (data_020a9db8->mFiles[idxByte].mActive != 0 && data_020a9db8->mFiles[idxByte].mChildBitmap != 0) break;

        loopCount = (loopCount + 1) & 0xff;
    } while (loopCount < 0x10);
    if (loopCount == 0x10) return 0x15;

    func_02065f08(idxByte);

    idxByte = data_020a9db8->mCurrentFile;
    if (func_02067530(&out, data_020a9db8->mFiles[idxByte].mBlockTable, data_020a9db8->mFiles[idxByte].mCurrentBlock, data_020a9db8->mFiles[idxByte].mHeader) == 0) {
        return 0x15;
    }

    {
        MbParentWork *g = data_020a9db8;
        out.f8 = out.f8 + g->mFiles[g->mCurrentFile].mSrcAddr;
        r.type = 4;
        r.a = g->mCurrentFile;
        r.b = g->mFiles[g->mCurrentFile].mCurrentBlock;
    }

    CpuCopy8((void *)out.f8, func_02065d5c(&r, (u8 *)data_020a9db8), out.f4);

    idxByte = data_020a9db8->mCurrentFile;
    func_02067f2c(out.f4 + 6, data_020a9db8->mFiles[idxByte].mChildBitmap, (u32)data_020a9db8);
}
