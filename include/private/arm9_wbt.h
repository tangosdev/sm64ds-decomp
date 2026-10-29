#ifndef ARM9_WBT_H
#define ARM9_WBT_H

/* Views of NitroSDK WBT (wireless block transfer), moved here from the five
 * files that each declared their own copy: the WBT library in the 0x0206
 * cluster (func_02063718, func_02063ea8, func_02064554) and the
 * file-sharing callbacks in the 0x0204 cluster (func_02040f30,
 * func_020412f0). Field names follow the SDK's. Plain C types:
 * func_020412f0 does not include types.h. */

/* The prepare-send arm of a system callback (event 0xd PREPARE_SEND_DATA):
 * WBT asks the application for the bytes of one block packet. */
typedef struct WBTPrepareSendDataCallback {
    unsigned int block_id; /* 0x00 */
    int block_seq_no;      /* 0x04 */
    const void *data_ptr;  /* 0x08 */
    short own_packet_size; /* 0x0c */
} WBTPrepareSendDataCallback;

/* One WBT command. command is 2 REQ_SYNC, 4 REQ_GET_BLOCK,
 * 6 REQ_GET_BLOCKINFO, 10 REQ_USER_DATA or 0xc SYSTEM_CALLBACK; a system
 * callback carries the peer's command type in event (func_02064554). */
typedef struct WBTCommand {
    int command;                    /* 0x00 */
    int event;                      /* 0x04 */
    unsigned short target_bmp;      /* 0x08 */
    unsigned short peer_bmp;        /* 0x0a */
    unsigned char my_cmd_counter;   /* 0x0c */
    unsigned char peer_cmd_counter; /* 0x0d */
    short result;                   /* 0x0e */
    void (*callback)(char *);       /* 0x10 */
    union {
        struct {
            short num_of_list;      /* 0x14 */
            short peer_packet_size; /* 0x16 */
            short my_packet_size;   /* 0x18 */
        } sync;
        /* The layout WBT_GetBlock (func_02065730) builds. */
        struct {
            unsigned int block_id;             /* 0x14 */
            unsigned int recv_data_size;       /* 0x18 */
            unsigned char *recv_buf_table[16]; /* 0x1c -- per aid */
            unsigned int *pkt_bmp_table[16];   /* 0x5c -- per aid */
        } get;
        struct {
            int block_id; /* 0x14 */
        } blockdone;
        struct {
            unsigned char data[9]; /* 0x14 */
            unsigned char size;    /* 0x1d */
        } user_data;
        WBTPrepareSendDataCallback prepare_send_data; /* 0x14 */
    } u;
} WBTCommand;

/* func_020647fc fills it: length, packet count, three cursors, then the
 * bitmap words and the receive buffer. */
typedef struct WBTPacketBitmap {
    int length; /* 0x00 */
    int count;  /* 0x04 */
    int pad_08;
    /* 0x0c -- the packet func_020647a4 marked last (medium: zeroed by
     * func_020647fc, not read back in the WBT files) */
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

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm: a command is 0x9c bytes, a peer record steps
 * 0x68, and peer[] starts at 0x1d4. */
typedef char WBTCommand_size_must_be_0x9c[sizeof(WBTCommand) == 0x9c ? 1 : -1];
typedef char WBTPeerRecord_size_must_be_0x68[sizeof(WBTPeerRecord) == 0x68 ? 1 : -1];
typedef char WBTWork_peer_must_be_at_0x1d4[(int)&((WBTWork *)0)->peer == 0x1d4 ? 1 : -1];
#endif

#endif
