#ifndef PRIVATE_MB_COMM_CHILD_BLOCK_HEADER_H
#define PRIVATE_MB_COMM_CHILD_BLOCK_HEADER_H

#include "types.h"

/* NitroSDK MBCommChildBlockHeader, a child packet unpacked by
 * MBi_SetRecvBufferFromChild: the type byte, then either the block number a
 * child asks for (types 8 and 9, the latter with reserved data) or one piece of
 * its entry request (type 7, which MBi_ReceiveRequestDataPiece files into req_data_buf.data_buf). */
typedef struct MBCommChildBlockHeader {
    u8 type; /* 0x00 */
    u8 pad1[1];
    union {
        struct {
            u16 req;           /* 0x02 */
            u8 reserved[0x10]; /* 0x04 */
        } data;
        struct {
            u8 piece;      /* 0x02 */
            u8 data[0x10]; /* 0x03 */
            u8 pad2[1];
        } req_data;
    } u;
} MBCommChildBlockHeader;

#ifndef SM64DS_PLATFORM_PC
/* The SDK packs it to 0x14: 2 + the 0x12-byte request piece. */
typedef char
    MBCommChildBlockHeader_size_must_be_0x14[sizeof(MBCommChildBlockHeader) == 0x14 ? 1 : -1];
#endif

#endif
