#include "types.h"
#include "private/arm9_a9db8_mb_parent.h"
extern char *data_020a9db8;
extern void CpuCopy8(const void *src, void *dst, unsigned int size);

/* File one piece of child idx's entry request into the multiboot parent
 * work (data_020a9db8): a0[2] is the piece number and the data starts at
 * a0 + 3. A piece number past mReqPieceCount is refused; otherwise the
 * piece is copied to its place in the child's mChildRequest row, its bit
 * is set in mChildRequestPieces, and the result is whether every piece is
 * now in. func_0206655c calls it for each request message. */
int func_02065de4(u8 *a0, int idx) {
    int new_var;
    int new_var2;
    char *g = data_020a9db8;
    int r5 = (u8)a0[2];
    int i = idx - 1;
    char *base;
    int cnt = ((MbParentWork *)g)->mReqPieceCount;
    char *row = (char *)((MbParentWork *)g)->mChildRequest;
    {
        int too_big = r5 > cnt;
        row = row + (i << 5);
        if (too_big) {
            return 0;
        }
    }
    new_var = (int)g;
    {
        int stride = ((MbParentWork *)g)->mReqPieceSize;
        CpuCopy8(a0 + 3, row + (r5 * stride), stride);
    }
    {
        /* Indexed through the decayed pointer: ->mChildRequestPieces[i] |= ...
         * builds a different sequence. */
        ((u32 *)((MbParentWork *)g)->mChildRequestPieces)[i] |= 1u << r5;
    }
    {
        int j;
        int found = new_var2 = 0;
        j = found;
        char *g2 = data_020a9db8;
        int n = ((MbParentWork *)g2)->mReqPieceCount;
        if (n > 0) {
            u32 bits = ((MbParentWork *)new_var)->mChildRequestPieces[i];
            do {
                if ((1u << j) & bits) {
                    found++;
                    found &= 0xff;
                }
                j = (j + 1) & 0xff;
            } while (j < ((volatile MbParentWork *)g2)->mReqPieceCount);
        }
        return found == n;
    }
}
