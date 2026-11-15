#ifndef ARM9_A1FC0_FILE_SHARE_H
#define ARM9_A1FC0_FILE_SHARE_H

#include "private/fs_views.h"

/* The WFS (wireless file system) work block at data_020a1fc0, used by the
 * link unit's file-share layer (WFS_EnableSync .. WFSi_ReadRomCallback). The
 * types and fields take the WFS spellings of the public Sonic Rush Adventure
 * decomp (RushRE, its wfs.h); this copy is older, and keeps its parent or
 * child context inside the block instead of behind a pointer. The transfer
 * runs over NitroSDK WBT (wireless block transfer): the child's tables below
 * are the ones WBT_GetBlockInfo/WBT_GetBlock take, and each parent node
 * embeds the WBTBlockInfoList (nitro/wbt.h) its region is registered with. */

#include "nitro/wm/wm.h"
#include "nitro/wbt.h"

/* Parent area at +0x440: a pool of three 0xcc0-byte nodes, each holding one
 * registered region. A node moves free -> busy (state 1, loading; WFSi_FromFreeToBusy)
 * -> ready (state 2; WFSi_FromBusyToAlive) -> busy (state 3, releasing; WFSi_FromAliveToBusy)
 * -> free (state 0; WFSi_FromBusyToFree). */
enum { FILE_SHARE_NODE_SIZE = 0xcc0 }; /* node slot stride, sizeof(WFSiFileList) */

typedef struct WFSiFileList {
    struct WFSiFileList *next; /* 0x00 -- list link (WFSi_MoveList moves nodes) */
    /* 0x04 -- the block list node WBT_RegisterBlock registers the region
     * with (block id, size, 0x20-byte name, data); WFSi_FindAliveForID finds a
     * node by its id */
    WBTBlockInfoList info;
    /* 0x38 -- FS_InitFile here; WFSi_TaskThread opens, reads and closes the
     * region's file through it, WFSi_OnParentSystemCallback refills a page through it.
     * WFSi_FindAlive matches a request against its open extent and
     * WFSi_SendOpenAck sizes the registration by it */
    FSFile file;
    int stat; /* 0x7c -- 0 free, 1 loading, 2 ready, 3 releasing */
    /* 0x80 -- WFSi_OnParentSystemCallback: +1 per child reading the region, -1 when it is
     * done; at 0 the node is released */
    int ref;
    /* 0x84 -- WFSi_OnParentSystemCallback: children asking for a region that is still
     * loading, by AID bit */
    int req_bitmap;
    /* 0x88 -- WFSi_TaskThread opens the file by it; WFSi_OnParentSystemCallback hands it to
     * the requesting child. Seeded per node in func_02040c34 */
    int own_id;
    /* 0x8c / 0x90 -- the region's extent in the ROM archive: a child's request
     * (offset, length) is stored here when no ready node holds it
     * (WFSi_FindBusy, which also matches loading nodes by the pair), and
     * WFSi_TaskThread opens [rom_src, rom_src + rom_len) with
     * FS_OpenFileDirect. WFSi_FindAlive matches a ready node by the same pair
     * through file's top and bottom - top */
    int rom_src;
    int rom_len;
    /* 0x94 -- the block sequence number the previous WBT prepare-send asked
     * for: WFSi_OnParentSystemCallback serves that one when its bytes are cached, then
     * stores the new request's number. WFSi_TaskThread zeroes it on load */
    int ack_seq;
    /* 0x98 -- page being refilled by an asynchronous read, 3 for none
     * (WFSi_TaskThread loads with 3; WFSi_OnParentSystemCallback sets it to the page it
     * refills and back to 3 once file is no longer busy); the cache search
     * skips it */
    int busy_page;
    /* 0x9c -- the last page refilled (WFSi_TaskThread loads with 2):
     * WFSi_OnParentSystemCallback picks the next one round-robin from it */
    int last_page;
    /* 0xa0 -- region file offset each page holds: 0, 0x400, 0x800 on load
     * (WFSi_TaskThread), a 0x200-aligned offset on refill (WFSi_OnParentSystemCallback) */
    int page[3];
    unsigned char pad_0ac[0x14];
    unsigned char cache[3][0x400]; /* 0xc0 -- the cached file pages */
} WFSiFileList;

/* One file-share message: 9 bytes on the air (WBT_PutUserData), 0xc as a slot
 * of WFSParentContext::recv_msg. type is 0 for a child's open request,
 * 1 for its reply, 2 for a child's release request and 3 for its reply. The
 * parent packet size rides split across pck_h (bits 8-11) and
 * pck_l. In kinds 1-3 arg is the region's registered id
 * (WFSiFileList::own_id, the child's block_id), and flag is 1, except
 * on the packet-size notice WFSi_SendAck sends as kind 1 with flag 0. The
 * open request (kind 0) carries the region's length in those 24 bits and its
 * offset in the second word instead; WFSi_ReadRequest builds it under those
 * names. */
typedef struct WFSiMessage {
    unsigned int type : 4;
    unsigned int pck_h : 4;
    unsigned int flag : 24;
    unsigned int arg;    /* 0x04 */
    unsigned char pck_l; /* 0x08 */
} WFSiMessage;

typedef struct WFSParentContext {
    /* 0x0000 -- one reply per AID, each a WFSiMessage (above):
     * WFSi_OnParentSystemCallback fills it, WFSi_SendOpenAck sets its file id, WFSi_SendAck
     * sends it */
    unsigned char recv_msg[0x10][0xc];
    WFSiFileList list[3];     /* 0x00c0 */
    WFSiFileList *free_list;  /* 0x2700 */
    WFSiFileList *alive_list; /* 0x2704 -- WFSi_FindAliveForID searches it */
    WFSiFileList *busy_list;  /* 0x2708 -- WFSi_TaskThread loads its state-1 nodes */
    /* 0x270c -- children owed a reply, by AID bit (WFSi_OnParentSystemCallback,
     * WFSi_SendOpenAck set; WFSi_SendAck sends and clears) */
    int ack_bitmap;
    /* 0x2710 -- children answered as one group, by AID bit: WFSi_SendAck
     * sends to exactly this set once all of it is in ack_bitmap, leaves
     * it out of any other send, and clears it when a member's reply no
     * longer matches; func_02040c34 zeroes it */
    int sync_bitmap;
    /* 0x2714 -- WFSi_SendAck sets it after a send and waits; the send
     * callback WFSi_OnSendMessageDone clears it */
    int msg_busy;
    /* 0x2718 -- by AID; bit 0 is the parent itself. WFSi_PortCallback sets a
     * child's bit on connect and clears it on disconnect */
    int all_bitmap;
    /* 0x271c -- children with a region open, by AID bit: WFSi_OnParentSystemCallback sets
     * the bit on each open request (message kind 0); WFSi_SendAck clears it
     * when it answers that child's release (kind 2) and masks it by
     * all_bitmap; func_02040c34 zeroes it */
    int busy_bitmap;
    /* 0x2720 -- WFSi_OnParentSystemCallback: a request named another packet size;
     * WFSi_SendAck applies new_packet_size once busy_count is 0 */
    int is_changing;
    /* 0x2724 -- nodes off the free list (WFSi_FromFreeToBusy +1, WFSi_FromBusyToFree -1) */
    int busy_count;
    /* 0x2728 -- children to tell the new size, by AID bit (WFSi_OnParentSystemCallback
     * sets, WFSi_SendAck sends) */
    int deny_bitmap;
    int new_packet_size; /* 0x272c -- WFSi_SendAck copies it to parent_packet_size */
} WFSParentContext;

/* Child area at +0x440. */
typedef struct WFSChildContext {
    /* 0x000 -- a WBTBlockInfoTable: func_02040c34 points entry i at the 0x28-byte
     * WBTBlockInfo at 0xc0 + i * 0x28; WFSi_OnChildSystemCallback passes the table to
     * WBT_GetBlockInfo (WBT_GetBlockInfo) */
    void *block_info_table[0x10];
    /* 0x040 -- a WBTRecvBufTable: WFSi_OnChildSystemCallback sets the peer's entry to the
     * receive buffer before WBT_GetBlock (WBT_GetBlock) */
    void *recv_buf_table[0x10];
    /* 0x080 -- a WBTPacketBitmapTable, WBT_GetBlock's last table: WFSi_ReallocBitmap
     * points entry 0 at recv_pkt_bmp_buf */
    void *recv_buf_packet_bmp_table[0x10];
    /* 0x0c0 -- the sixteen 0x28-byte WBTBlockInfo block_info_table points at
     * (func_02040c34) */
    unsigned char block_info[0x10][0x28];
    /* 0x340 -- WFS_Start's WM_ReadStatus copy of the WM status block;
     * it reads the aid */
    WMStatus status;
    /* 0xc70 -- WFSi_ReallocBitmap allocates it (about four bytes per packet of
     * max_file_size); WFS_End frees it */
    void *recv_pkt_bmp_buf;
    int max_file_size; /* 0xc74 -- WFSi_ReallocBitmap grows it to the largest read */
    /* 0xc78 -- WFSi_OnChildSystemCallback: offset of the region being read, as the parent
     * registered it (the tables are 0x20000) */
    int block_id;
} WFSChildContext;

/* The wireless file-sharing work block at data_020a1fc0. func_02040c34 stores
 * its five arguments in the fields whose comments name them. */
typedef struct WFSWork {
    int is_parent;           /* 0x00 -- isParent; the parent builds the node pool at +0x440,
                              the child the receive tables; WFSi_SetMPData picks the
                              parent or child send from it */
    int send_idle;           /* 0x04 -- set by WFS_Start/WFSi_OnSetMPDataDone before
                              WFSi_SetMPData, which sends only while it is set */
    int is_running;          /* 0x08 -- WFSi_SetMPData: the port send returned 2 */
    int state;               /* 0x0c -- 1 here, 2 once ready (WFS_Start,
                              WFSi_OnChildSystemCallback), 0 at shutdown (WFS_End) */
    void (*state_func)(int); /* 0x10 -- stateCallback; called with 0 when state becomes 2 */
    void *(*alloc_func)(void *arg, unsigned int size,
                        void *ptr); /* 0x14 -- allocFunc; (arg, size, 0) allocates, (arg, 0, ptr)
                              frees (WFSi_ReallocBitmap, WFS_End) */
    void *alloc_arg;                /* 0x18 -- allocArg; first argument to alloc_func */
    struct FSFile *target_file;     /* 0x1c -- child read in flight, an FSFile: WFSi_ReadRequest
                              sets it, WFSi_OnChildSystemCallback completes and clears it */
    void *table;                    /* 0x20 -- WFSi_LoadTables: the card header's FNT, FAT and
                              overlay tables, read into one allocation */
    unsigned int table_size;        /* 0x24 -- that allocation's size */
    int parent_packet_size; /* 0x28 -- parent MP send size; WFSi_OnChildSystemCallback sets it
                              from the WBT sync's peer packet size + 0xe */
    int child_packet_size;  /* 0x2c -- child MP send size, likewise */
    unsigned short port;    /* 0x30 -- port; WM_SetPortCallback registers the port
                              callback on it, WFSi_SetMPData sends on it */
    unsigned char padding1[0xe];
    /* 0x40 -- the MP send buffer, 32-byte aligned (0x020a2000): WFSi_SetMPData
     * builds the next packet in it (WBT's parent or child send hook) and hands
     * it to WM_SetMPDataToPort. Its extent is the gap up to context. */
    unsigned char send_buf[0x400];
    union {
        WFSParentContext parent;
        WFSChildContext child;
    } context; /* 0x440 -- parent or child, by is_parent */
} WFSWork;

typedef char WFSiFileList_size_must_be_0xcc0[sizeof(WFSiFileList) == FILE_SHARE_NODE_SIZE ? 1 : -1];
typedef char WFSParentContext_size_must_be_0x2730[sizeof(WFSParentContext) == 0x2730 ? 1 : -1];
typedef char WFSChildContext_size_must_be_0xc7c[sizeof(WFSChildContext) == 0xc7c ? 1 : -1];
typedef char WFSiMessage_size_must_be_0xc[sizeof(WFSiMessage) == 0xc ? 1 : -1];
typedef char WFSWork_size_must_be_0x2b70[sizeof(WFSWork) == 0x2b70 ? 1 : -1];

/* The work block itself, in .bss: it ends at 0x020a4b30, below the next
 * symbol (0x020a4b40). data_020a2400 (context) and data_020a3fc0 (block +
 * 0x2000, the literal WFS_GetCurrentBitmap and WFS_EnableSync load) are
 * labels inside it: dsd needs a symbol at every relocation target. */
extern WFSWork data_020a1fc0;

/* The block through a pointer cast. Read as data_020a1fc0.context, context's
 * address folds into a literal of its own (data_020a1fc0+0x440);
 * WFSi_PortCallback and WFSi_SendAck add 0x440 to the block's address instead,
 * which this spelling keeps. */
#define FILE_SHARE_WORK ((WFSWork *)&data_020a1fc0)

#endif
