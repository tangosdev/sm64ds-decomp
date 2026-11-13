#ifndef ARM9_A9DB8_MB_PARENT_H
#define ARM9_A9DB8_MB_PARENT_H
#include "types.h"
#include "private/mb_views.h"
#include "nitro/mb/mb.h"

/* The wireless-download parent work block that data_020a9db8 points at
 * (arm9 func_02065c2c .. MBi_ParentCallback). Only the fields those functions
 * read are placed; everything else is padding at its measured offset.
 *
 * Message types 3/4 sent and 7/8/9 received, fifteen per-child state words
 * at 0x14e8 and the sixteen-entry file table below are the NitroSDK
 * multiboot (MB) parent layout, so the names follow that role without
 * claiming the SDK's own identifiers. */

/* NitroSDK MbSegmentInfo: one row of a download file's segment table
 * (ARM9 static, ARM7 static, ROM header), as MBi_SetSegmentInfo
 * (MBi_SetSegmentInfo) fills it. */
typedef struct MbSegmentInfo {
    u32 recv_addr;  /* 0x00 -- where the child receives it: load_addr, or a
                       staging buffer for an ARM7 segment */
    u32 load_addr;  /* 0x04 -- where it finally runs */
    u32 size;       /* 0x08 */
    u32 target : 1; /* 0x0c -- set for the ARM7 segment */
    u32 rsv : 31;
} MbSegmentInfo;

/* NitroSDK MbDownloadFileInfoHeader, the start of MBDownloadFileInfo
 * (MBi_MakeDownloadFileInfo builds it). */
typedef struct MbDownloadFileInfoHeader {
    u32 arm9EntryAddr;    /* 0x00 */
    u32 arm7EntryAddr;    /* 0x04 */
    u32 padding;          /* 0x08 */
    MbSegmentInfo seg[3]; /* 0x0c */
} MbDownloadFileInfoHeader;

/* NitroSDK MB_BlockInfoTable, MbParentFile.blockinfo_table: each segment's byte
 * offset and first block number in the stream, and the total block count. */
typedef struct MB_BlockInfoTable {
    u32 seg_head_offset[3]; /* 0x00 */
    u16 block_head[3];      /* 0x0c */
    u16 block_num;          /* 0x12 */
} MB_BlockInfoTable;

/* NitroSDK MB_BlockInfo (mb_fileinfo.h): one block's place, as
 * MBi_get_blockinfo gives it. */
typedef struct MB_BlockInfo {
    u32 child_address; /* 0x0 */
    u32 size;          /* 0x4 */
    u32 offset;        /* 0x8 */
    u8 segment_no;     /* 0xc */
    u8 pad[3];
} MB_BlockInfo;

/* NitroSDK MBDownloadFileInfo: what MBi_CommParentSendDLFileInfo sends a
 * child (0xe4 bytes): the segment table, the authentication code that
 * follows the program image, and the boot parameters. */
typedef struct MBDownloadFileInfo {
    MbDownloadFileInfoHeader header; /* 0x00 */
    u8 auth_code[136];               /* 0x3c */
    u8 reserved[0x20];               /* 0xc4 -- HW_DOWNLOAD_PARAMETER_SIZE */
} MBDownloadFileInfo;

/* One registered file: an element of NitroSDK MB_CommPWork's fileinfo[]
 * (an anonymous struct in the SDK; named here for its size check). Field
 * names are the SDK's. */
typedef struct MbParentFile {
    MBDownloadFileInfo dl_fileinfo; /* 0x000 -- built by MBi_MakeDownloadFileInfo */
    /* 0x0e4 -- built by MBi_MakeGameInfo, then chained into the beacon list */
    MBGameInfo game_info;
    /* 0x59c -- built by MBi_MakeBlockInfoTable, read by MBi_get_blockinfo */
    MB_BlockInfoTable blockinfo_table;
    /* 0x5b0 -- the program the file was registered with: MB_RegisterFile
     * stores it (and refuses it twice) and builds game_info from it */
    MBGameRegistry *game_reg;
    u32 *src_adr; /* 0x5b4 -- the image; a block is copied from src_adr + its offset */
    u16 currentb; /* 0x5b8 -- block number being sent */
    /* 0x5ba -- highest block a child asked for this cycle:
     * MBi_CommParentRecvData zeroes it, MBi_CommParentRecvDataPerChild raises
     * it per request, MBi_calc_sendblock keeps currentb within two of it */
    u16 nextb;
    u16 pollbmp; /* 0x5bc -- children waiting on this file */
    /* 0x5be -- bit 0 (the parent) at registration, plus each child accepted
     * into the file (MBi_CommParentRecvDataPerChild); its population is
     * checked against the player limit */
    u16 gameinfo_child_bmp;
    /* 0x5c0 -- children accepted or dropped since the last update:
     * MBi_CommParentCallback passes it and gameinfo_child_bmp to
     * MB_UpdateGameInfoMember, then clears it */
    u16 gameinfo_changed_bmp;
    u8 active; /* 0x5c2 -- nonzero once the file is registered */
    u8 update; /* 0x5c3 */
} MbParentFile;

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char MbParentFile_size_must_be_0x5c4[sizeof(MbParentFile) == 0x5c4 ? 1 : -1];
#endif

/* NitroSDK MBCommRequestData (nitro/mb.h): the entry request a child sends
 * in pieces. MBi_ReceiveRequestDataPiece assembles it into the child's row
 * of req_data_buf; MBi_CommParentRecvDataPerChild copies it out
 * (MB_COMM_REQ_DATA_SIZE, 0x1d bytes) and files the parts into the
 * per-child tables. */
typedef struct MBCommRequestData {
    u32 ggid;            /* 0x00 -- copied to childggid */
    MBUserInfo userinfo; /* 0x04 -- copied to childUser */
    u16 version;         /* 0x1a -- copied to childversion */
    u8 fileid;           /* 0x1c -- the fileinfo index it asks to enter */
    u8 pad[3];
} MBCommRequestData;

#ifndef SM64DS_PLATFORM_PC
typedef char MBCommRequestData_size_must_be_0x20[sizeof(MBCommRequestData) == 0x20 ? 1 : -1];
#endif

/* NitroSDK MbRequestPieceBuf (mb_block.h): each child's request, assembled
 * piece by piece, and the pieces received so far, by bit. A row is the
 * SDK's 30-byte request buffer rounded up a word, read as an
 * MBCommRequestData once complete. */
typedef struct MbRequestPieceBuf {
    u32 data_buf[15][0x20 / sizeof(u32)]; /* 0x000 */
    u32 data_bmp[15];                     /* 0x1e0 -- cleared on disconnect */
} MbRequestPieceBuf;

/* NitroSDK MB_CommCommonWork (mb_private.h), the head of the work buffer
 * MB_Init is given; mbc points at it. This older SDK keeps the request
 * piece geometry here, where the later one keeps mb_block.c's static
 * req_data_piece: req_data_piece_size and req_data_piece_num are named for
 * its size and num. */
typedef struct MB_CommCommonWork {
    /* 0x0000 -- the MP buffers: MB_Init points the WM work's sendBuf at
     * sendbuf and its recvBuf at recvbuf. Their 32-byte alignment rounds
     * MB_CommPWork up to 0x73e0, the size the parent's WM_End clears. */
    u32 sendbuf[0x400 / sizeof(u32)] __attribute__((aligned(32)));
    u32 recvbuf[0xf00 / sizeof(u32)] __attribute__((aligned(32))); /* 0x0400 */
    /* 0x1300 -- the parent's own MBUserInfo: MB_RegisterFile hands it to
     * MBi_MakeGameInfo, which copies it into each registered file's game
     * record */
    MBUserInfo user;
    /* 0x1316 -- MB_Init refuses while it is set and sets it last, and the
     * WM_End result clears it on either side (MBi_EndCommon) */
    u16 isMbInitialized;
    /* 0x1318 -- bytes of a file carried per block: MB_StartParent sets it to
     * the MP parent size less 6; MBi_MakeBlockInfoTable divides each segment
     * by it and MBi_get_blockinfo turns a block number back into an offset
     * with it */
    int block_size_max;
    /* 0x131c -- bytes of the child's entry request carried per piece:
     * MB_StartParent sets it to the MP child size less 2 (the SDK's
     * MB_COMM_CALC_REQ_DATA_PIECE_SIZE) */
    int req_data_piece_size;
    /* 0x1320 -- pieces a whole request takes, 30 / req_data_piece_size (the
     * SDK's MB_COMM_CALC_REQ_DATA_PIECE_NUM) */
    int req_data_piece_num;
    /* 0x1324 -- MBi_ParentCallback sets it before starting MP on a child's
     * CONNECTED, the START_MP result clears it */
    int start_mp_busy; /* BOOL */
    u8 padding[0x1340 - 0x1328];
} MB_CommCommonWork;

/* NitroSDK MB_CommPWork (mb_private.h), the parent's view of the work
 * (pPwork). */
typedef struct MB_CommPWork {
    MB_CommCommonWork common; /* 0x0000 */
    /* 0x1340 -- per child (AID - 1): the user record from its entry request,
     * with the AID as playerNo (MBi_CommParentRecvDataPerChild);
     * MBi_CommParentCallback hands the table to MB_UpdateGameInfoMember */
    MBUserInfo childUser[15];
    u16 childversion[15]; /* 0x148a -- cleared on disconnect */
    /* 0x14a8 -- the GGID the child asked for, matched against its file's
     * game_reg->ggid */
    u32 childggid[15];
    /* 0x14e4 -- MB_CommSetParentStateCallback stores it,
     * MBi_CommChangeParentStateCallbackOnly calls it with (child, state, arg)
     * when it is set, and MB_StartParent saves it across its clear of the
     * work */
    MBCommPStateCallback parent_callback;
    /* 0x14e8 -- per-child state: MBi_CommChangeParentState sets it,
     * MB_CommGetParentState returns it, and MB_CommIsBootable tests it for 7
     * (send complete) */
    int p_comm_state[15];
    /* 0x1524 -- registered files: MB_RegisterFile refuses a 17th and counts
     * each one; zero means nothing to send */
    u8 file_num;
    u8 cur_fileid;          /* 0x1525 -- fileinfo index, stepped mod 16 per send */
    s8 fileid_of_child[15]; /* 0x1526 -- fileinfo index each child entered, -1 none */
    /* 0x1535 -- children accepted (MBi_CommParentRecvDataPerChild +1,
     * MBi_CommParentCallback -1) */
    u8 child_num;
    u16 child_entry_bmp;            /* 0x1536 -- the same children, by AID bit */
    MbRequestPieceBuf req_data_buf; /* 0x1538 */
    /* 0x1754 -- the application's answer to an entry request
     * (MB_CommResponseRequest); MBi_CommParentRecvDataPerChild acts on 3
     * (accept) and 4 (refuse) for message 7, 2 for message 8 and 5 for
     * message 10, and clears it */
    u16 req2child[15];
    /* 0x1772 -- MB_CommGetChildUser's copy of the user record it returns */
    MBUserInfo childUserBuf;
    MbParentFile fileinfo[16]; /* 0x1788 */
} MB_CommPWork;

/* NitroSDK mb_private.h: the work MB_Init is given, and the parent's view
 * of it. */

#if defined(_MSC_VER) && defined(__cplusplus)
extern "C" {
#endif
extern MB_CommCommonWork *mbc;

#ifdef __cplusplus
extern "C"
#endif
    /* mb_fileinfo.h: MB_FileInfo.c's block lookup, MBi_CommParentSendBlock's
 * source of each block's place. */
    int
    MBi_get_blockinfo(MB_BlockInfo *, MB_BlockInfoTable *, u32, MbDownloadFileInfoHeader *);

#ifdef __cplusplus
extern "C"
#endif
    /* mb_parent.h: the parent's WM callback, which MB_StartParent installs. */
    void
    MBi_CommParentCallback(u16, void *);
#if defined(_MSC_VER) && defined(__cplusplus)
}
#endif

#define pPwork ((MB_CommPWork *)mbc)

#endif
