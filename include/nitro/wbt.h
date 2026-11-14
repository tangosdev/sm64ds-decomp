#ifndef NITRO_WBT_H
#define NITRO_WBT_H
#ifndef __cplusplus
#define WBT_End _Z7WBT_Endv
#define WBT_InitChild _Z13WBT_InitChildPFvPvE
#define WBT_InitParent _Z14WBT_InitParentiiPFvPvE
#define WBT_RegisterBlock _Z17WBT_RegisterBlockP16WBTBlockInfoListjPKvS2_it
#define WBT_UnregisterBlock _Z19WBT_UnregisterBlockj
#endif

/* The Nitro SDK's nitro/wbt.h (wireless block transfer): the WBTCallback a
 * command reports through, the WBTCommandType of a command and of the event
 * a system callback carries, the registered-block list node, the WBTCommand
 * a callback is handed, and the WBT API (the WBT_* files in src/) under
 * the SDK's names with C linkage. The library's own work area (WBTWork and
 * the per-peer records) is in private/arm9_wbt.h.
 *
 * The SDK's BOOL returns are spelled int: no shared header here defines BOOL.
 * Where this revision's definitions take a parameter by another type than
 * the SDK's later headers spell it, the prototype follows the definition.
 * Parameters unnamed: a named parameter here renumbers every includer's
 * @NNN. */

#include "types.h"

#ifdef __cplusplus
#ifdef _MSC_VER
extern "C" {
#endif
#endif
/* A command's completion callback, and the system callback WBT_InitParent and
 * WBT_InitChild install; the argument is the WBTCommand. */
typedef void (*WBTCallback)(void *);

typedef enum WBTCommandType {
    WBT_CMD_REQ_NONE,           /* 0 -- no command: the slot is free */
    WBT_CMD_REQ_WAIT,           /* 1 */
    WBT_CMD_REQ_SYNC,           /* 2 -- WBT_RequestSync */
    WBT_CMD_RES_SYNC,           /* 3 */
    WBT_CMD_REQ_GET_BLOCK,      /* 4 -- WBT_GetBlock */
    WBT_CMD_RES_GET_BLOCK,      /* 5 */
    WBT_CMD_REQ_GET_BLOCKINFO,  /* 6 -- WBT_GetBlockInfo */
    WBT_CMD_RES_GET_BLOCKINFO,  /* 7 */
    WBT_CMD_REQ_GET_BLOCK_DONE, /* 8 */
    WBT_CMD_RES_GET_BLOCK_DONE, /* 9 */
    WBT_CMD_REQ_USER_DATA,      /* 10 -- WBT_PutUserData */
    WBT_CMD_RES_USER_DATA,      /* 11 */
    WBT_CMD_SYSTEM_CALLBACK,    /* 12 -- a peer's request; event names it */
    WBT_CMD_PREPARE_SEND_DATA,  /* 13 -- the application supplies a USER block's packet */
    WBT_CMD_REQ_ERROR,          /* 14 */
    WBT_CMD_RES_ERROR,          /* 15 */
    WBT_CMD_CANCEL              /* 16 */
} WBTCommandType;

#define WBT_USER_ID_LEN 32
#define WBT_SIZE_USER_DATA 9

/* WBTBlockInfoList.block_type */
#define WBT_BLOCK_LIST_TYPE_COMMON 0 /* registered with its data */
#define WBT_BLOCK_LIST_TYPE_USER 1   /* each packet asked for with PREPARE_SEND_DATA */

/* One registered block's description, 0x28 bytes. */
typedef struct WBTBlockInfo {
    u32 id;                      /* 0x00 -- the block id WBT_RegisterBlock is given */
    s32 block_size;              /* 0x04 -- in bytes */
    u8 user_id[WBT_USER_ID_LEN]; /* 0x08 */
} WBTBlockInfo;

/* A node of the registered-block list whose head func_02065ae0 returns
 * (data_020a94d0). WBT_RegisterBlock appends one, WBT_UnregisterBlock unlinks
 * one by id, WBT_NumOfRegisteredBlock counts them, and func_02065a94 searches
 * the list by id. */
typedef struct WBTBlockInfoList {
    WBTBlockInfo data_info;        /* 0x00 */
    struct WBTBlockInfoList *next; /* 0x28 */
    void *data_ptr;                /* 0x2c -- a COMMON block's data */
    u16 permission_bmp;            /* 0x30 -- the AIDs allowed to request it */
    u16 block_type;                /* 0x32 -- WBT_BLOCK_LIST_TYPE_* */
} WBTBlockInfoList;

/* The arms of WBTCommand's union, one per command or event that fills it. */

/* RES_SYNC: the peer's block count and the two packet sizes it settled. */
typedef struct WBTRequestSyncCallback {
    s16 num_of_list;      /* 0x00 */
    s16 peer_packet_size; /* 0x02 */
    s16 my_packet_size;   /* 0x04 */
    u16 pad1;
    u32 padd2[2];
} WBTRequestSyncCallback;

/* REQ_GET_BLOCK_DONE: the block a peer finished receiving. */
typedef struct WBTGetBlockDoneCallback {
    u32 block_id; /* 0x00 */
} WBTGetBlockDoneCallback;

/* PREPARE_SEND_DATA: WBT asks the application for the bytes of one packet
 * of a USER block; the application sets data_ptr (and may move
 * block_seq_no to a packet it has). */
typedef struct WBTPrepareSendDataCallback {
    u32 block_id;        /* 0x00 */
    s32 block_seq_no;    /* 0x04 */
    void *data_ptr;      /* 0x08 */
    s16 own_packet_size; /* 0x0c */
    u16 padd;
} WBTPrepareSendDataCallback;

/* REQ_USER_DATA: the bytes a peer sent with WBT_PutUserData. */
typedef struct WBTRecvUserDataCallback {
    u8 data[WBT_SIZE_USER_DATA]; /* 0x00 */
    u8 size;                     /* 0x09 */
    u8 padd[3];
} WBTRecvUserDataCallback;

/* REQ_GET_BLOCK and REQ_GET_BLOCKINFO: the block and the per-aid receive
 * buffers and packet bitmaps WBT_GetBlock / WBT_GetBlockInfo copied in. */
typedef struct WBTGetBlockCallback {
    u32 block_id;           /* 0x00 */
    u32 recv_data_size;     /* 0x04 */
    u8 *recv_buf_table[16]; /* 0x08 -- per aid */
    u32 *pkt_bmp_table[16]; /* 0x48 -- per aid */
} WBTGetBlockCallback;

/* One WBT command, 0x9c bytes. command is what the command is; a system
 * callback (command SYSTEM_CALLBACK) carries the peer's request in event, and
 * a completion callback the response it completed on. The SDK's union is
 * anonymous; here it is the member u. */
typedef struct WBTCommand {
    WBTCommandType command; /* 0x00 */
    WBTCommandType event;   /* 0x04 */
    u16 target_bmp;         /* 0x08 */
    u16 peer_bmp;           /* 0x0a */
    u8 my_cmd_counter;      /* 0x0c */
    u8 peer_cmd_counter;    /* 0x0d */
    s16 result;             /* 0x0e */
    WBTCallback callback;   /* 0x10 */
    union {
        WBTRequestSyncCallback sync;
        WBTGetBlockDoneCallback blockdone;
        WBTPrepareSendDataCallback prepare_send_data;
        WBTRecvUserDataCallback user_data;
        WBTGetBlockCallback get;
    } u; /* 0x14 */
} WBTCommand;

void WBT_InitParent(int, int, WBTCallback);
void WBT_InitChild(WBTCallback);
void WBT_End(void);
#ifdef __cplusplus
extern "C"
#endif
    int WBT_AidbitmapToAid(u16);
#ifdef __cplusplus
extern "C"
#endif
    int
    WBT_GetOwnAid(void);
#ifdef __cplusplus
extern "C"
#endif
    void
    WBT_SetOwnAid(int);
#ifdef __cplusplus
extern "C"
#endif
    int
    WBT_CalcPacketbitmapSize(int);
#ifdef __cplusplus
extern "C"
#endif
    int
    WBT_NumOfRegisteredBlock(void);
int WBT_RegisterBlock(WBTBlockInfoList *, u32, const void *, const void *, int, u16);
WBTBlockInfoList *WBT_UnregisterBlock(u32);
#ifdef __cplusplus
extern "C"
#endif
    int
    WBT_SetPacketSize(int, int);
#ifdef __cplusplus
extern "C"
#endif
    int
    WBT_MpParentSendHook(void *, int);
#ifdef __cplusplus
extern "C"
#endif
    int
    WBT_MpChildSendHook(void *, int);

#ifdef __cplusplus
extern "C"
#endif
    void WBT_RequestSync(u16, WBTCallback);
#ifdef __cplusplus
extern "C"
#endif
    void
    WBT_GetBlockInfo(u16, int, const void *, WBTCallback);
#ifdef __cplusplus
extern "C"
#endif
    void
    WBT_GetBlock(u16, u32, const void *, u32, const void *, WBTCallback);
#ifdef __cplusplus
extern "C"
#endif
    int
    WBT_PutUserData(u16, const void *, int, WBTCallback);
#ifdef __cplusplus
#ifdef _MSC_VER
}
#endif
#endif

#endif
