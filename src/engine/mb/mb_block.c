/* NitroSDK mb_block.c: the packet marshalling helpers used by the download
 * parent's block protocol. This cartridge carries only the three entry points
 * below; the child-side marshalling and piece-buffer setup of later SDK
 * revisions are absent. */
#include "types.h"
#include "nitro/mi/memory.h"
#include "private/arm9_a9db8_mb_parent.h"
#include "private/mb_common.h"
#include "private/mb_comm_child_block_header.h"

typedef struct MBCommParentBlockHeader {
    u8 type;
    u8 pad1[1];
    u16 fid;
    u16 seqno;
} MBCommParentBlockHeader;

u8 *MBi_SetRecvBufferFromChild(u8 *src, u8 *dst);
u8 *MBi_MakeParentSendBuffer(MBCommParentBlockHeader *src, u8 *dst);
int MBi_ReceiveRequestDataPiece(u8 *a0, int idx);

/* File one piece of a child's entry request. The piece number is a0[2] and
 * the piece bytes start at a0[3]; the return value is whether this completes
 * the child's request. */
int MBi_ReceiveRequestDataPiece(u8 *a0, int idx) {
    MB_CommPWork *g = pPwork;
    int piece = a0[2];
    int i = idx - 1;
    int count = g->common.req_data_piece_num;
    char *row = (char *)g->req_data_buf.data_buf;
    {
        int too_big = piece > count;
        row = row + (i << 5);
        if (too_big) {
            return 0;
        }
    }
    {
        int stride = g->common.req_data_piece_size;
        MI_CpuCopy8(a0 + 3, row + (piece * stride), stride);
    }
    (g->req_data_buf.data_bmp)[i] |= 1u << piece;
    {
        int j;
        int found = 0;
        j = found;
        MB_CommPWork *g2 = pPwork;
        int n = g2->common.req_data_piece_num;
        if (n > 0) {
            u32 bits = g->req_data_buf.data_bmp[i];
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

/* Pack a parent packet header. A data packet is followed by its file id and
 * sequence number; the other valid parent packet types are only their type
 * byte. */
u8 *MBi_MakeParentSendBuffer(MBCommParentBlockHeader *src, u8 *dst) {
    u8 *ptr = dst;
    *ptr++ = src->type;
    switch (src->type) {
    case MB_COMM_TYPE_PARENT_DATA:
        *ptr++ = src->fid;
        *ptr++ = (src->fid & 0xff00) >> 8;
        *ptr++ = src->seqno;
        *ptr++ = (src->seqno & 0xff00) >> 8;
        break;
    case MB_COMM_TYPE_PARENT_SENDSTART:
    case MB_COMM_TYPE_PARENT_KICKREQ:
    case MB_COMM_TYPE_PARENT_DL_FILEINFO:
    case MB_COMM_TYPE_PARENT_BOOTREQ:
    case MB_COMM_TYPE_PARENT_MEMBER_FULL:
        break;
    default:
        return 0;
    }
    return ptr;
}

/* Unpack a child packet header. File-request packets carry one piece of the
 * child's entry request; continue packets also carry reserved data. Unlike
 * later SDK revisions, this version leaves filing the request piece to
 * MBi_ReceiveRequestDataPiece's caller. */
u8 *MBi_SetRecvBufferFromChild(u8 *src, u8 *dst) {
    MBCommChildBlockHeader *hd = (MBCommChildBlockHeader *)dst;
    u8 *ptr;
    hd->type = src[0];
    switch (hd->type) {
    case MB_COMM_TYPE_CHILD_FILEREQ:
        ptr = src + 2;
        hd->u.req_data.piece = src[1];
        {
            MB_CommPWork *g = pPwork;
            if (hd->u.req_data.piece > g->common.req_data_piece_num) {
                return 0;
            }
            MI_CpuCopy8(ptr, hd->u.req_data.data, g->common.req_data_piece_size);
            ptr = ptr + pPwork->common.req_data_piece_size;
        }
        break;

    case MB_COMM_TYPE_CHILD_ACCEPT_FILEINFO: {
        int lo = src[1];
        u16 *p = (u16 *)((int)&hd->u.data.req);
        ptr = src + 3;
        hd->u.data.req = lo & 0xff;
        *((u16 *)((int)&hd->u.data.req)) = (*p) | ((src[2] << 8) & 0xff00);
    } break;

    case MB_COMM_TYPE_CHILD_CONTINUE: {
        int lo = src[1];
        u16 *p = &hd->u.data.req;
        hd->u.data.req = lo & 0xff;
        {
            int hi = src[2];
            ptr = src + 3;
            *p = (*p) | ((hi << 8) & 0xff00);
        }
        MI_CpuCopy8(ptr, hd->u.data.reserved, pPwork->common.req_data_piece_size);
        ptr = ptr + pPwork->common.req_data_piece_size;
    } break;

    default:
        return 0;
    }

    return ptr;
}
