#include "types.h"
// NitroSDK MBi_MakeParentSendBuffer: packs a parent packet's header into the
// send buffer: the type byte, then for a block (type 4) its file id and
// sequence number little-endian. Returns the advanced buffer pointer, or 0
// for an unknown type. The header layout is MB_Parent.c's.
typedef struct MBCommParentBlockHeader {
    u8 type;
    u8 pad1[1];
    u16 fid;
    u16 seqno;
} MBCommParentBlockHeader;

u8 *MBi_MakeParentSendBuffer(MBCommParentBlockHeader *src, u8 *dst) {
    u8 *d = dst;
    *d++ = src->type;
    switch (src->type) {
    case 4:
        *d++ = src->fid;
        *d++ = (src->fid & 0xff00) >> 8;
        *d++ = src->seqno;
        *d++ = (src->seqno & 0xff00) >> 8;
        break;
    case 1:
    case 2:
    case 3:
    case 5:
    case 6:
        break;
    default:
        return 0;
    }
    return d;
}
