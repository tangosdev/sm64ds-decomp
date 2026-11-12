#ifndef PRIVATE_MB_COMMON_H
#define PRIVATE_MB_COMMON_H

/* The multiboot helpers both sides share, as NitroSDK's private
 * libraries/mb/include/mb_common.h declares them: mb_common.c
 * (src/engine/mb/MB_Common.c) defines them, the parent's senders in
 * MB_Parent.c end each block with MBi_BlockHeaderEnd, and the beacon
 * builder in MB_GameInfo.c checksums the game record with MBi_calc_cksum.
 * The cartridge carries only these two of the header's functions. The
 * packet types and the request answers are the header's too.
 *
 * The parameters are unnamed: a named parameter in a declaration costs
 * every including C++ file an @NNN number. */

#include "types.h"

/* The entry request a child sends (MBCommRequestData) is copied out at
 * this size. */
#define MB_COMM_REQ_DATA_SIZE (29)

/* A parent packet's header, in front of what it carries. */
#define MB_COMM_PARENT_HEADER_SIZE (6)

/* A send function's "nothing to send" (WM_ERRCODE_MAX + 1). */
#define MB_SENDFUNC_STATE_ERR (21)

/* The packet types: a parent packet's or a child packet's type byte. */
typedef enum MBCommType {
    MB_COMM_TYPE_DUMMY = 0,
    MB_COMM_TYPE_PARENT_SENDSTART,
    MB_COMM_TYPE_PARENT_KICKREQ,
    MB_COMM_TYPE_PARENT_DL_FILEINFO,
    MB_COMM_TYPE_PARENT_DATA,
    MB_COMM_TYPE_PARENT_BOOTREQ,
    MB_COMM_TYPE_PARENT_MEMBER_FULL,
    MB_COMM_TYPE_CHILD_FILEREQ,
    MB_COMM_TYPE_CHILD_ACCEPT_FILEINFO,
    MB_COMM_TYPE_CHILD_CONTINUE,
    MB_COMM_TYPE_CHILD_STOPREQ,
    MB_COMM_TYPE_CHILD_BOOTREQ_ACCEPTED
} MBCommType;

/* The parent application's answer to a child, held in req2child until the
 * child's next packet acts on it (MB_CommResponseRequest). */
typedef enum MBCommUserReq {
    MB_COMM_USER_REQ_NONE = 0,
    MB_COMM_USER_REQ_DL_START,
    MB_COMM_USER_REQ_SEND_START,
    MB_COMM_USER_REQ_ACCEPT,
    MB_COMM_USER_REQ_KICK,
    MB_COMM_USER_REQ_BOOT
} MBCommUserReq;

#ifdef __cplusplus
extern "C" {
#endif

int MBi_BlockHeaderEnd(int, u16, u32 *);
u16 MBi_calc_cksum(const u16 *, int);

#ifdef __cplusplus
}
#endif

#endif
