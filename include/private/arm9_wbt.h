#ifndef ARM9_WBT_H
#define ARM9_WBT_H

/* The WBT (wireless block transfer) library's own work area: the running,
 * queued and system commands and one parse record per peer, as the library
 * in the 0x0206 cluster (func_02063718 .. func_02065b94) lays them out in
 * this SDK revision. The SDK's public types and API (WBTCommand,
 * WBTCommandType, WBTCallback, WBT_*) are in nitro/wbt.h. */

#include "nitro/wbt.h"

/* func_020647fc fills it: length, packet count, three cursors, then the
 * bitmap words and the receive buffer. */
typedef struct WBTPacketBitmap {
    int length; /* 0x00 */
    int count;  /* 0x04 */
    int next;   /* 0x08 -- the missing packet func_0206470c found last */
    /* 0x0c -- the packet func_020647a4 marked last; func_0206470c
     * searches on from it */
    int current;
    /* 0x10 -- packets marked so far: func_020647a4 counts each one */
    int recv_count;
    unsigned int *bitmap;  /* 0x14 */
    unsigned char *buffer; /* 0x18 */
} WBTPacketBitmap;

/* A peer's receive state for the running transfer. */
typedef struct WBTPeerRecv {
    WBTPacketBitmap pkt_bmp;         /* 0x00 */
    unsigned int blockinfo_bits[10]; /* 0x1c -- GET_BLOCKINFO's bitmap words */
} WBTPeerRecv;

/* What func_02063ea8 parsed out of one peer's packet. cmd_counter is the
 * peer's command counter; last_cmd_counter is the one func_02064554 already
 * reported, so a repeat is dropped. */
typedef struct WBTPeerRecord {
    unsigned char state;            /* 0x00 -- 1 parsing, 2 complete */
    unsigned char command;          /* 0x01 */
    unsigned char cmd_counter;      /* 0x02 */
    unsigned char last_cmd_counter; /* 0x03 */
    short my_packet_size;           /* 0x04 */
    short peer_packet_size;         /* 0x06 */
    unsigned short target_bmp;      /* 0x08 */
    short num_of_list;              /* 0x0a */
    unsigned char pad_0c;
    unsigned char user_data_size; /* 0x0d */
    unsigned char pad_0e[2];
    int block_id; /* 0x10 */
    int seq_no;   /* 0x14 */
    unsigned char pad_18[8];
    int user_data;    /* 0x20 -- the payload's address in the packet */
    WBTPeerRecv recv; /* 0x24 */
} WBTPeerRecord;

/* The WBT work area the library functions take as their first argument. */
typedef struct WBTWork {
    WBTCommand cmd;         /* 0x000 -- the command running */
    WBTCommand queued_cmd;  /* 0x09c -- the next one; command 0 = none */
    WBTCommand system_cmd;  /* 0x138 -- the block system callbacks are raised with */
    WBTPeerRecord peer[16]; /* 0x1d4 -- indexed by aid */
} WBTWork;

/* The send-packet builders. WBT_MpParentSendHook runs func_02065538 and
 * WBT_MpChildSendHook runs func_020653cc. Each offers the send buffer in
 * turn to the responders to a peer's completed request (USER_DATA
 * func_02063d3c, SYNC func_02063e08, GET_BLOCKINFO func_02063ae4,
 * GET_BLOCK_DONE func_02063da0, GET_BLOCK func_02063bc4), to the running
 * command (func_02063980) and to the queued one (func_02063718), returns the
 * length of the first packet one of them writes, and with none written has
 * func_02064fe8 write a WAIT packet. Every builder takes the buffer and its
 * size, as the SDK's later WBTi_TryCreateResponse and WBTi_CheckRequest do
 * (wbt_context.c); this revision's builders do not read the size, but their
 * callers pass it. Parameters unnamed. */
#ifdef __cplusplus
extern "C"
#endif
#if defined(_MSC_VER) && defined(__cplusplus)
    extern "C" {
#endif
int func_02065538(void *, int);
#ifdef __cplusplus
extern "C"
#endif
    int
    func_020653cc(void *, int);
#ifdef __cplusplus
extern "C"
#endif
    int
    func_02063d3c(WBTWork *, int, void *, int);
#ifdef __cplusplus
extern "C"
#endif
    int
    func_02063e08(WBTWork *, int, void *, int);
#ifdef __cplusplus
extern "C"
#endif
    int
    func_02063ae4(WBTWork *, int, void *, int);
#ifdef __cplusplus
extern "C"
#endif
    int
    func_02063da0(WBTWork *, int, void *, int);
#ifdef __cplusplus
extern "C"
#endif
    int
    func_02063bc4(WBTWork *, int, void *, int);
#ifdef __cplusplus
extern "C"
#endif
    int
    func_02063980(WBTWork *, void *, int);
#ifdef __cplusplus
extern "C"
#endif
    int
    func_02063718(WBTWork *, void *, int);
#if defined(_MSC_VER) && defined(__cplusplus)
}
#endif

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm: a command is 0x9c bytes, a block list node
 * 0x34, a peer record steps 0x68, and peer[] starts at 0x1d4. */
typedef char WBTCommand_size_must_be_0x9c[sizeof(WBTCommand) == 0x9c ? 1 : -1];
typedef char WBTBlockInfoList_size_must_be_0x34[sizeof(WBTBlockInfoList) == 0x34 ? 1 : -1];
typedef char WBTPeerRecord_size_must_be_0x68[sizeof(WBTPeerRecord) == 0x68 ? 1 : -1];
typedef char WBTWork_peer_must_be_at_0x1d4[(int)&((WBTWork *)0)->peer == 0x1d4 ? 1 : -1];
#endif

#endif
