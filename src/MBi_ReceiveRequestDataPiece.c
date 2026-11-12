#include "types.h"
#include "private/arm9_a9db8_mb_parent.h"
#include "nitro/mi/memory.h"

/* File one piece of child idx's entry request into the multiboot parent
 * work (pPwork): a0[2] is the piece number and the data starts at
 * a0 + 3. A piece number past req_data_piece_num is refused; otherwise the
 * piece is copied to its place in the child's req_data_buf.data_buf row, its bit
 * is set in req_data_buf.data_bmp, and the result is whether every piece is
 * now in. MBi_CommParentRecvDataPerChild calls it for each request message.
 * ntrtwl's NitroSDK (libraries/mb/src/mb_block.c) and pret's pmd-sky have
 * MBi_ReceiveRequestDataPiece filing pieces the same way; theirs returns
 * the completed request rather than a 0/1. This is an older SDK's version
 * of that function, and an SDK function keeps its name across versions. */
int MBi_ReceiveRequestDataPiece(u8 *a0, int idx) {
    int new_var;
    int new_var2;
    MB_CommPWork *g = pPwork;
    int r5 = (u8)a0[2];
    int i = idx - 1;
    char *base;
    int cnt = g->common.req_data_piece_num;
    char *row = (char *)g->req_data_buf.data_buf;
    {
        int too_big = r5 > cnt;
        row = row + (i << 5);
        if (too_big) {
            return 0;
        }
    }
    new_var = (int)g;
    {
        int stride = g->common.req_data_piece_size;
        MI_CpuCopy8(a0 + 3, row + (r5 * stride), stride);
    }
    {
        /* Indexed through the decayed pointer: ->req_data_buf.data_bmp[i] |= ...
         * builds a different sequence. */
        ((u32 *)g->req_data_buf.data_bmp)[i] |= 1u << r5;
    }
    {
        int j;
        int found = new_var2 = 0;
        j = found;
        MB_CommPWork *g2 = pPwork;
        int n = g2->common.req_data_piece_num;
        if (n > 0) {
            u32 bits = ((MB_CommPWork *)new_var)->req_data_buf.data_bmp[i];
            do {
                if ((1u << j) & bits) {
                    found++;
                    found &= 0xff;
                }
                j = (j + 1) & 0xff;
            } while (j < ((volatile MB_CommPWork *)g2)->common.req_data_piece_num);
        }
        return found == n;
    }
}
