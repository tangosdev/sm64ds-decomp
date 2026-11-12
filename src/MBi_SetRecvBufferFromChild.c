#include "types.h"
#include "private/arm9_a9db8_mb_parent.h"
#include "private/mb_comm_child_block_header.h"
#include "nitro/mi/memory.h"

/* NitroSDK MBi_SetRecvBufferFromChild: unpack a child's MP message into its
 * block header. Type 7 (file request) carries a piece number and one piece
 * of the entry request (MBi_ReceiveRequestDataPiece assembles them); types 8 and 9 carry
 * a 16-bit request built from two bytes, and 9 also a piece of reserved
 * data. The header follows the SDK's MBCommChildBlockHeader.
 *
 * This build's SDK revision takes no child id: ntrtwl's later
 * MBi_SetRecvBufferFromChild(recvbuf, hdr, child_id) files the request
 * piece itself, where this one leaves that to the caller's MBi_ReceiveRequestDataPiece. */
u8 *MBi_SetRecvBufferFromChild(u8 *src, u8 *dst) {
    MBCommChildBlockHeader *hd = (MBCommChildBlockHeader *)dst;
    u8 *r4;
    hd->type = src[0];
    switch (hd->type) {
    case 7:
        r4 = src + 2;
        hd->u.req_data.piece = src[1];
        {
            MB_CommPWork *g = pPwork;
            if (hd->u.req_data.piece > g->common.req_data_piece_num) {
                return 0;
            }
            MI_CpuCopy8(r4, hd->u.req_data.data, g->common.req_data_piece_size);
            r4 = r4 + pPwork->common.req_data_piece_size;
        }
        break;

    case 8: {
        int lo = src[1];
        /* the (int) round trips on p and on the second store are
         * load-bearing: plain &hd->u.data.req and *p swap r2 and r3 */
        u16 *p = (u16 *)((int)&hd->u.data.req);
        r4 = src + 3;
        hd->u.data.req = lo & 0xff;
        *((u16 *)((int)&hd->u.data.req)) = (*p) | ((src[2] << 8) & 0xff00);
    } break;

    case 9: {
        int lo = src[1];
        u16 *p = &hd->u.data.req;
        hd->u.data.req = lo & 0xff;
        {
            int hi = src[2];
            r4 = src + 3;
            *p = (*p) | ((hi << 8) & 0xff00);
        }
        MI_CpuCopy8(r4, hd->u.data.reserved, pPwork->common.req_data_piece_size);
        r4 = r4 + pPwork->common.req_data_piece_size;
    } break;

    default:
        return 0;
    }

    return r4;
}
