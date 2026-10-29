#include "types.h"
#include "private/arm9_a9db8_mb_parent.h"
extern void CpuCopy8(const void *src, void *dst, unsigned int size);
extern char *data_020a9db8;

/* NitroSDK MBi_SetRecvBufferFromChild: unpack a child's MP message into its
 * block header. Type 7 (file request) carries a piece number and one piece
 * of the entry request (func_02065de4 assembles them); types 8 and 9 carry
 * a 16-bit request built from two bytes, and 9 also a piece of reserved
 * data. The header follows the SDK's MBCommChildBlockHeader. */
typedef struct MBCommChildBlockHeader {
    u8 type; /* 0x0 */
    u8 pad_1;
    union {
        struct {
            u16 req;        /* 0x2 */
            u8 reserved[1]; /* 0x4 */
        } data;
        struct {
            u8 piece;   /* 0x2 */
            u8 data[1]; /* 0x3 */
        } req_data;
    } u;
} MBCommChildBlockHeader;

u8 *func_02065c2c(u8 *src, u8 *dst) {
    MBCommChildBlockHeader *hd = (MBCommChildBlockHeader *)dst;
    u8 *r4;
    hd->type = src[0];
    switch (hd->type) {
    case 7:
        r4 = src + 2;
        hd->u.req_data.piece = src[1];
        {
            char *g = data_020a9db8;
            if (hd->u.req_data.piece > ((MbParentWork *)g)->mReqPieceCount) {
                return 0;
            }
            CpuCopy8(r4, hd->u.req_data.data, ((MbParentWork *)g)->mReqPieceSize);
            r4 = r4 + ((MbParentWork *)data_020a9db8)->mReqPieceSize;
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
        CpuCopy8(r4, hd->u.data.reserved, ((MbParentWork *)data_020a9db8)->mReqPieceSize);
        r4 = r4 + ((MbParentWork *)data_020a9db8)->mReqPieceSize;
    } break;

    default:
        return 0;
    }

    return r4;
}
