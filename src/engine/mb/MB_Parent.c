/* The wireless-download parent's protocol (the Nitro SDK's mb_parent.c),
 * arm9 .text 0x02065eb8..0x020672d0.
 *
 * pPwork holds a state word per child (AID 1..15) and a sixteen-entry file
 * table. MBi_CommParentCallback is the WM parent callback: it moves children
 * through their states on connect and disconnect, hands each MP receive to
 * MBi_CommParentRecvData, and on each send turn MBi_CommParentSendData picks
 * what goes out: a state message to the children waiting on one, a file's
 * download info, or the next block of a file. The application sees each
 * state change through the callback it stores with
 * MB_CommSetParentStateCallback, and answers entry requests with
 * MB_CommResponseRequest.
 *
 * Written as ntrtwl/NitroSDK libraries/mb/src/mb_parent.c, in this older
 * SDK's shape. It has no any_recv_bitmap: MBi_calc_sendblock steps every
 * active file with children waiting, not only those a child asked a block
 * of in this receive. It has no block cache either: MBi_CommParentSendBlock
 * copies straight from the file's source address, and MBi_ReloadCache is
 * absent. A finished WM_End clears the work without calling the state
 * callback again, and MB_SendGameInfoBeacon takes no arguments.
 *
 * Names: pret's pmd-sky keeps this file as assembly in overlay_00, and every
 * function here except MB_CommGetChildrenNumber is matched to one of its
 * functions by body. MB_CommGetChildrenNumber is not in pmd-sky. It takes
 * the SDK's public mb.h name for what its body returns, the count of
 * accepted children. mwccarm emits the bodies in reverse, so
 * MB_CommSetParentStateCallback, the first in source, is the highest in the
 * ROM.
 */
#include "types.h"
#include "nitro/mi/memory.h"
#include "nitro/os/system.h"
#include "private/arm9_a9db8_mb_parent.h"
#include "private/mb_comm_child_block_header.h"
#include "private/mb_common.h"
#include "private/wm_views.h"
#include "nitro/wm/wm_api.h"

typedef int BOOL;
#define TRUE 1
#define FALSE 0
#define NULL 0

#define WM_NUM_MAX_CHILD 15
#define MB_MAX_FILE 16
#define MB_GAMEINFO_CHILD_FLAG(__child_aid__) (1 << (__child_aid__))

#define MB_SEND_THRESHOLD 2

/* mb_private.h: the WM callback codes MB_WmBase.cpp passes on. */
#define MB_CALLBACK_CHILD_CONNECTED 0
#define MB_CALLBACK_CHILD_DISCONNECTED 1
#define MB_CALLBACK_MP_PARENT_RECV 3
#define MB_CALLBACK_END_COMPLETE 0x11
#define MB_CALLBACK_INIT_COMPLETE 0x15
#define MB_CALLBACK_MP_SEND_ENABLE 0x19
#define MB_CALLBACK_BEACON_SENT 0x1c
#define MB_CALLBACK_API_ERROR 0xff
#define MB_CALLBACK_ERROR 0x100

/* mb_block.h: the header MBi_MakeParentSendBuffer packs in front of a
 * parent packet: the type byte, then for a block (type 4) its file and
 * block number. */
typedef struct {
    u8 type;
    u8 pad1[1];
    u16 fid;
    u16 seqno;
} MBCommParentBlockHeader;

extern u8 *MBi_SetRecvBufferFromChild(u8 *src, u8 *dst);
extern u8 *MBi_MakeParentSendBuffer(MBCommParentBlockHeader *hdr, u8 *buf);
extern int MBi_ReceiveRequestDataPiece(u8 *hdr, int aid);
extern void MB_SendGameInfoBeacon(void);
extern void MB_UpdateGameInfoMember(char *self, void *src, u32 flags, int s);
extern int _Z13func_0206881cv(void);

static void MBi_CommChangeParentState(u16 child, int state, void *arg);
static void MBi_CommChangeParentStateCallbackOnly(u16 child, int state, void *arg);
static void MBi_CommParentRecvDataPerChild(void *arg, u16 child);
static void MBi_CommParentRecvData(void *arg);
static int MBi_CommParentSendMsg(u8 type, u16 pollbmp);
static int MBi_CommParentSendDLFileInfo(void);
static int MBi_CommParentSendBlock(void);
static int MBi_CommParentSendData(void);
static void MBi_calc_sendblock(u8 file_id);
static u16 MBi_calc_nextsendblock(u16 next_block, u16 next_block_req);

static inline u16 max(u16 a, u16 b);
static BOOL IsChildAidValid(u16 child_aid);
static void MBi_CommCallParentError(u16 aid, u16 errcode);

void MB_CommSetParentStateCallback(MBCommPStateCallback callback) {
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();
    pPwork->parent_callback = callback;

    OS_RestoreInterrupts(enabled);
}

int MB_CommGetParentState(u16 child_aid) {
    if (pPwork && IsChildAidValid(child_aid)) {
        return pPwork->p_comm_state[child_aid - 1];
    }

    return 0;
}

const MBUserInfo *MB_CommGetChildUser(u16 child_aid) {
    OSIntrMode enabled = OS_DisableInterrupts();

    if (pPwork && IsChildAidValid(child_aid)) {
        MI_CpuCopy8(&pPwork->childUser[child_aid - 1], &pPwork->childUserBuf, sizeof(MBUserInfo));
        OS_RestoreInterrupts(enabled);
        return &pPwork->childUserBuf;
    }

    OS_RestoreInterrupts(enabled);
    return NULL;
}

u8 MB_CommGetChildrenNumber(void) {
    if (pPwork) {
        return pPwork->child_num;
    }

    return 0;
}

BOOL MB_CommIsBootable(u16 child_aid) {
    if (pPwork && IsChildAidValid(child_aid)) {
        if (pPwork->p_comm_state[child_aid - 1] == MB_COMM_PSTATE_SEND_COMPLETE) {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL MB_CommResponseRequest(u16 child_aid, MBCommResponseRequestType ack) {
    u16 req;
    int state;
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();

    switch (ack) {
    case MB_COMM_RESPONSE_REQUEST_KICK:
        state = MB_COMM_PSTATE_REQUESTED;
        req = MB_COMM_USER_REQ_KICK;
        break;
    case MB_COMM_RESPONSE_REQUEST_ACCEPT:
        state = MB_COMM_PSTATE_REQUESTED;
        req = MB_COMM_USER_REQ_ACCEPT;
        break;
    case MB_COMM_RESPONSE_REQUEST_DOWNLOAD:
        state = MB_COMM_PSTATE_WAIT_TO_SEND;
        req = MB_COMM_USER_REQ_SEND_START;
        break;
    case MB_COMM_RESPONSE_REQUEST_BOOT:
        state = MB_COMM_PSTATE_SEND_COMPLETE;
        req = MB_COMM_USER_REQ_BOOT;
        break;
    default:
        OS_RestoreInterrupts(enabled);
        return FALSE;
    }

    if (pPwork && IsChildAidValid(child_aid)) {
        if (pPwork->p_comm_state[child_aid - 1] == state) {
            pPwork->req2child[child_aid - 1] = req;
            OS_RestoreInterrupts(enabled);
            return TRUE;
        }
    }

    OS_RestoreInterrupts(enabled);
    return FALSE;
}

static void MBi_CommChangeParentState(u16 child, int state, void *arg) {
    if (IsChildAidValid(child)) {
        pPwork->p_comm_state[child - 1] = state;
    }

    MBi_CommChangeParentStateCallbackOnly(child, state, arg);
}

static void MBi_CommChangeParentStateCallbackOnly(u16 child, int state, void *arg) {
    if (pPwork->parent_callback) {
        (*pPwork->parent_callback)(child, state, arg);
    }
}

void MBi_CommParentCallback(u16 type, void *arg) {
    switch (type) {
    case MB_CALLBACK_INIT_COMPLETE:
        MBi_CommChangeParentState(0, MB_COMM_PSTATE_INIT_COMPLETE, arg);
        break;
    case MB_CALLBACK_END_COMPLETE:
        MBi_CommChangeParentState(0, MB_COMM_PSTATE_END, NULL);
        break;
    case MB_CALLBACK_CHILD_CONNECTED: {
        WmStartParentCallback *sparg = arg;

        if (sparg->aid == 0)
            break;

        if (sparg->aid >= 16) {
            break;
        }

        MBi_CommChangeParentState(sparg->aid, MB_COMM_PSTATE_CONNECTED, arg);
    } break;
    case MB_CALLBACK_CHILD_DISCONNECTED: {
        WmStartParentCallback *sparg = arg;

        if (sparg->aid == 0)
            break;

        if (sparg->aid >= 16) {
            break;
        }

        pPwork->childversion[sparg->aid - 1] = 0;
        MI_CpuClear8(&pPwork->childggid[sparg->aid - 1], sizeof(u32));
        MI_CpuClear8(&pPwork->childUser[sparg->aid - 1], sizeof(MBUserInfo));

        /* MBi_ClearParentPieceBuffer */
        MI_CpuClear8(pPwork->req_data_buf.data_buf[sparg->aid - 1],
                     (_Z13func_0206881cv() - 2) * (MB_COMM_REQ_DATA_SIZE + 1) /
                         (_Z13func_0206881cv() - 2));
        pPwork->req_data_buf.data_bmp[sparg->aid - 1] = 0;

        if (pPwork->fileid_of_child[sparg->aid - 1] != -1) {
            u8 fileID = pPwork->fileid_of_child[sparg->aid - 1];
            u16 child = sparg->aid;

            pPwork->fileinfo[fileID].gameinfo_child_bmp &= ~(MB_GAMEINFO_CHILD_FLAG(child));
            pPwork->fileinfo[fileID].gameinfo_changed_bmp |= MB_GAMEINFO_CHILD_FLAG(child);
            pPwork->fileid_of_child[child - 1] = -1;
            pPwork->fileinfo[fileID].pollbmp &= ~(0x0001 << (child));
        }

        if (pPwork->child_entry_bmp & (0x0001 << (sparg->aid))) {
            pPwork->child_num--;
            pPwork->child_entry_bmp &= ~(0x0001 << (sparg->aid));
        }

        if (pPwork->p_comm_state[sparg->aid - 1] == MB_COMM_PSTATE_BOOT_REQUEST) {
            MBi_CommChangeParentState(sparg->aid, MB_COMM_PSTATE_BOOT_STARTABLE, NULL);
        }

        MBi_CommChangeParentState(sparg->aid, MB_COMM_PSTATE_DISCONNECTED, arg);
        pPwork->p_comm_state[sparg->aid - 1] = MB_COMM_PSTATE_NONE;
    } break;
    case MB_CALLBACK_MP_PARENT_RECV:
        MBi_CommParentRecvData(arg);
        break;
    case MB_CALLBACK_MP_SEND_ENABLE:
        MBi_CommParentSendData();
        break;
    case MB_CALLBACK_BEACON_SENT: {
        u8 i;

        for (i = 0; i < MB_MAX_FILE; i++) {
            if (pPwork->fileinfo[i].active && pPwork->fileinfo[i].gameinfo_changed_bmp) {
                MB_UpdateGameInfoMember((char *)&pPwork->fileinfo[i].game_info,
                                        &pPwork->childUser[0],
                                        pPwork->fileinfo[i].gameinfo_child_bmp,
                                        pPwork->fileinfo[i].gameinfo_changed_bmp);

                pPwork->fileinfo[i].gameinfo_changed_bmp = 0;
            }
        }
    }

        MB_SendGameInfoBeacon();
        break;

    case MB_CALLBACK_API_ERROR: {
        u16 apiid, errcode;

        apiid = ((u16 *)arg)[0];
        errcode = ((u16 *)arg)[1];

        switch (errcode) {
        case WM_ERRCODE_INVALID_PARAM:
        case WM_ERRCODE_FAILED:
        case WM_ERRCODE_WM_DISABLE:
        case WM_ERRCODE_NO_DATASET:
        case WM_ERRCODE_FIFO_ERROR:
        case WM_ERRCODE_TIMEOUT:
            MBi_CommCallParentError(0, MB_ERRCODE_FATAL);
            break;
        case WM_ERRCODE_OPERATING:
        case WM_ERRCODE_ILLEGAL_STATE:
        case WM_ERRCODE_NO_CHILD:
        case WM_ERRCODE_OVER_MAX_ENTRY:
        case WM_ERRCODE_NO_ENTRY:
        case WM_ERRCODE_INVALID_POLLBITMAP:
        case WM_ERRCODE_NO_DATA:
        case WM_ERRCODE_SEND_QUEUE_FULL:
        case WM_ERRCODE_SEND_FAILED:
        default:
            MBi_CommCallParentError(0, MB_ERRCODE_WM_FAILURE);
            break;
        }
    } break;
    case MB_CALLBACK_ERROR: {
        WMCallback *pWmcb = arg;
        switch (pWmcb->apiid) {
        case WM_APIID_INITIALIZE:
        case WM_APIID_SET_LIFETIME:
        case WM_APIID_SET_P_PARAM:
        case WM_APIID_SET_BEACON_IND:
        case WM_APIID_START_PARENT:
        case WM_APIID_START_MP:
        case WM_APIID_SET_MP_DATA:
        case WM_APIID_START_DCF:
        case WM_APIID_SET_DCF_DATA:
        case WM_APIID_DISCONNECT:
        case WM_APIID_START_KS:
            MBi_CommCallParentError(0, MB_ERRCODE_FATAL);
            break;
        case WM_APIID_RESET:
        case WM_APIID_END:
        default:
            MBi_CommCallParentError(0, MB_ERRCODE_WM_FAILURE);
            break;
        }
    } break;
    }

    if (type == MB_CALLBACK_END_COMPLETE) {
        /* MI_CpuClearFast(pPwork, sizeof(MB_CommPWork)): the cartridge
         * stores the fill value to the stack and reloads it, which only a
         * volatile reproduces (no inline spelling of the SDK's does). */
        volatile u32 fill = 0;
        MI_CpuFillFast(pPwork, fill, sizeof(MB_CommPWork));
        pPwork = NULL;
    }
}

/* One child's packet from an MP receive, by its type: an entry request
 * (assembled piece by piece, then answered as the application decides), the
 * child's acceptance of the download info, its next block request, or its
 * stop request. */
static void MBi_CommParentRecvDataPerChild(void *arg, u16 child) {
    MBCommChildBlockHeader hd;
    int state;

    MBi_SetRecvBufferFromChild((u8 *)&((WMMpRecvData *)arg)->cdata[0], (u8 *)&hd);

    if (child == 0 || child > WM_NUM_MAX_CHILD) {
        return;
    }

    state = pPwork->p_comm_state[child - 1];

    switch (hd.type) {
    case MB_COMM_TYPE_CHILD_FILEREQ: {
        if (state == MB_COMM_PSTATE_CONNECTED) {
            MBCommRequestData req_data;

            if (MBi_ReceiveRequestDataPiece((u8 *)&hd, child) == TRUE) {
                MI_CpuCopy8(pPwork->req_data_buf.data_buf[child - 1], &req_data,
                            MB_COMM_REQ_DATA_SIZE);

                pPwork->childggid[child - 1] = req_data.ggid;
                pPwork->childversion[child - 1] = req_data.version;

                MI_CpuCopy8(&req_data.userinfo, &pPwork->childUser[child - 1], sizeof(MBUserInfo));

                pPwork->childUser[child - 1].playerNo = child;
                MBi_CommChangeParentState(child, MB_COMM_PSTATE_REQUESTED, &req_data.userinfo);
            }
        }

        if (state == MB_COMM_PSTATE_REQUESTED) {
            u8 i, entry_num = 0;
            u8 fileid = ((MBCommRequestData *)pPwork->req_data_buf.data_buf[child - 1])->fileid;

            if (fileid >= MB_MAX_FILE || pPwork->fileinfo[fileid].active == 0 ||
                pPwork->childggid[child - 1] != pPwork->fileinfo[fileid].game_reg->ggid) {
                pPwork->req2child[child - 1] = MB_COMM_USER_REQ_KICK;
            } else {
                for (i = 0; i < WM_NUM_MAX_CHILD + 1; i++) {
                    if (pPwork->fileinfo[fileid].gameinfo_child_bmp & (0x0001 << i)) {
                        entry_num++;
                    }
                }

                if (entry_num >= pPwork->fileinfo[fileid].game_reg->maxPlayerNum) {
                    pPwork->req2child[child - 1] = MB_COMM_USER_REQ_NONE;
                    MBi_CommChangeParentState(child, MB_COMM_PSTATE_MEMBER_FULL, NULL);
                    break;
                }
            }

            switch (pPwork->req2child[child - 1]) {
            case MB_COMM_USER_REQ_ACCEPT: {
                if (0 == (pPwork->child_entry_bmp & (0x0001 << (child)))) {
                    pPwork->child_num++;
                    pPwork->child_entry_bmp |= (0x0001 << (child));
                    pPwork->fileid_of_child[child - 1] = fileid;

                    pPwork->fileinfo[fileid].gameinfo_child_bmp |= MB_GAMEINFO_CHILD_FLAG(child);
                    pPwork->fileinfo[fileid].gameinfo_changed_bmp |= MB_GAMEINFO_CHILD_FLAG(child);

                    pPwork->req2child[child - 1] = MB_COMM_USER_REQ_NONE;
                    MBi_CommChangeParentState(child, MB_COMM_PSTATE_REQ_ACCEPTED, NULL);
                }
            } break;
            case MB_COMM_USER_REQ_KICK:
                pPwork->req2child[child - 1] = MB_COMM_USER_REQ_NONE;
                MBi_CommChangeParentState(child, MB_COMM_PSTATE_KICKED, NULL);
                break;
            }
        }
    } break;
    case MB_COMM_TYPE_CHILD_ACCEPT_FILEINFO:
        if (state == MB_COMM_PSTATE_REQ_ACCEPTED) {
            MBi_CommChangeParentState(child, MB_COMM_PSTATE_WAIT_TO_SEND, NULL);
        } else if (state == MB_COMM_PSTATE_WAIT_TO_SEND) {
            if (pPwork->req2child[child - 1] == MB_COMM_USER_REQ_SEND_START) {
                u8 fid = pPwork->fileid_of_child[child - 1];
                pPwork->fileinfo[fid].pollbmp |= (0x0001 << (child));
                pPwork->fileinfo[fid].currentb = 0;
                pPwork->req2child[child - 1] = MB_COMM_USER_REQ_NONE;
                MBi_CommChangeParentState(child, MB_COMM_PSTATE_SEND_PROCEED, NULL);
            }
        }
        break;
    case MB_COMM_TYPE_CHILD_CONTINUE:
        if (state == MB_COMM_PSTATE_SEND_PROCEED) {
            u8 fileid = pPwork->fileid_of_child[child - 1];

            if (fileid == (u8)-1)
                break;

            pPwork->fileinfo[fileid].nextb =
                MBi_calc_nextsendblock(pPwork->fileinfo[fileid].nextb, hd.u.data.req);
        }
        break;
    case MB_COMM_TYPE_CHILD_STOPREQ:
        if (state == MB_COMM_PSTATE_SEND_PROCEED) {
            u8 fileid = pPwork->fileid_of_child[child - 1];

            if (fileid == (u8)-1)
                break;

            pPwork->fileinfo[fileid].pollbmp &= ~(0x0001 << (child));

            MBi_CommChangeParentState(child, MB_COMM_PSTATE_SEND_COMPLETE, NULL);
        } else if (state == MB_COMM_PSTATE_SEND_COMPLETE) {
            if (pPwork->req2child[child - 1] == MB_COMM_USER_REQ_BOOT) {
                pPwork->req2child[child - 1] = MB_COMM_USER_REQ_NONE;
                MBi_CommChangeParentState(child, MB_COMM_PSTATE_BOOT_REQUEST, NULL);
                break;
            }
        }
        break;
    case MB_COMM_TYPE_CHILD_BOOTREQ_ACCEPTED:
        if (state == MB_COMM_PSTATE_BOOT_REQUEST) {
            break;
        }
        break;
    default:
        break;
    }
}

/* An MP receive: clear each registered file's requested block, then pass
 * every child's nonempty packet on. */
static void MBi_CommParentRecvData(void *arg) {
    WMMpRecvHeader *mpHeader = arg;
    u16 i;
    WMMpRecvData *datap;

    for (i = 0; i < MB_MAX_FILE; i++) {
        if (pPwork->fileinfo[i].active)
            pPwork->fileinfo[i].nextb = 0;
    }

    for (i = 1; i <= WM_NUM_MAX_CHILD; ++i) {
        datap = WM_ReadMPData(mpHeader, i);

        if (datap != NULL) {
            if (datap->length == 0xffff) {
            } else if (datap->length != 0) {
                MBi_CommParentRecvDataPerChild(datap, i);
            }
        }
    }
}

static int MBi_CommParentSendMsg(u8 type, u16 pollbmp) {
    MBCommParentBlockHeader hd;

    hd.type = type;

    MBi_MakeParentSendBuffer(&hd, (u8 *)pPwork->common.sendbuf);
    return MBi_BlockHeaderEnd(MB_COMM_PARENT_HEADER_SIZE, pollbmp, pPwork->common.sendbuf);
}

/* Send the download info of the next file (mod 16) that a child in
 * REQ_ACCEPTED has entered, to every such child of that file. */
/* The SDK's function-local `static s8 prev_fid = -1` of
 * MBi_CommParentSendDLFileInfo: the file whose download info went out last.
 * Its byte (0xff) sits at 0x0209a078, followed by three bytes of linker
 * alignment before MB_FileInfo.c's .data. The object's 1-byte .data ends
 * short of that word, and the TU tooling cannot yet license a claim like that
 * inside arm9's shared .data, so the static keeps its configured address
 * name. */
extern s8 data_0209a078;

static int MBi_CommParentSendDLFileInfo(void) {
    MBCommParentBlockHeader hd;
    u8 *databuf = ((u8 *)pPwork->common.sendbuf) + MB_COMM_PARENT_HEADER_SIZE;
    u16 child;
    u8 i, fid;
    s8 send_candidate_fid = -1;
    u8 file_req_num[MB_MAX_FILE];
    u16 pollbmp = 0;

    MI_CpuClear8(&file_req_num[0], sizeof(u8) * MB_MAX_FILE);

    for (child = 1; child <= WM_NUM_MAX_CHILD; child++) {
        if (pPwork->p_comm_state[child - 1] == MB_COMM_PSTATE_REQ_ACCEPTED) {
            ++(file_req_num[pPwork->fileid_of_child[child - 1]]);
        }
    }

    fid = data_0209a078;

    for (i = 0; i < MB_MAX_FILE; i++) {
        fid = ((fid + 1) % MB_MAX_FILE);

        if (pPwork->fileinfo[fid].active && file_req_num[fid] > 0) {
            send_candidate_fid = fid;
            break;
        }
    }

    if (send_candidate_fid == -1)
        return MB_SENDFUNC_STATE_ERR;

    data_0209a078 = send_candidate_fid;

    for (child = 1; child <= WM_NUM_MAX_CHILD; child++) {
        if (pPwork->p_comm_state[child - 1] == MB_COMM_PSTATE_REQ_ACCEPTED &&
            pPwork->fileid_of_child[child - 1] == send_candidate_fid) {
            pollbmp |= (1 << child);
        }
    }

    hd.type = MB_COMM_TYPE_PARENT_DL_FILEINFO;
    hd.fid = send_candidate_fid;

    databuf = MBi_MakeParentSendBuffer(&hd, (u8 *)pPwork->common.sendbuf);
    if (databuf) {
        MI_CpuCopy8(&pPwork->fileinfo[send_candidate_fid].dl_fileinfo, databuf,
                    sizeof(MBDownloadFileInfo));
    }

    return MBi_BlockHeaderEnd(sizeof(MBDownloadFileInfo) + MB_COMM_PARENT_HEADER_SIZE, pollbmp,
                              pPwork->common.sendbuf);
}

/* Send the next block of the next file (mod 16) with children waiting on
 * it, copied straight from the file's image. */
static int MBi_CommParentSendBlock(void) {
    MBCommParentBlockHeader hd;
    u8 *databuf;
    u8 i;
    MB_BlockInfo bi;

    if (pPwork->file_num == 0)
        return MB_SENDFUNC_STATE_ERR;

    for (i = 0; i < MB_MAX_FILE; i++) {
        pPwork->cur_fileid = ((pPwork->cur_fileid + 1) % MB_MAX_FILE);
        if (pPwork->fileinfo[pPwork->cur_fileid].active &&
            pPwork->fileinfo[pPwork->cur_fileid].pollbmp) {
            break;
        }
    }

    if (i == MB_MAX_FILE) {
        return MB_SENDFUNC_STATE_ERR;
    }

    MBi_calc_sendblock(pPwork->cur_fileid);

    if (!MBi_get_blockinfo(&bi, &pPwork->fileinfo[pPwork->cur_fileid].blockinfo_table,
                           pPwork->fileinfo[pPwork->cur_fileid].currentb,
                           &pPwork->fileinfo[pPwork->cur_fileid].dl_fileinfo.header)) {
        return MB_SENDFUNC_STATE_ERR;
    }

    bi.offset += (u32)pPwork->fileinfo[pPwork->cur_fileid].src_adr;

    hd.type = MB_COMM_TYPE_PARENT_DATA;
    hd.fid = pPwork->cur_fileid;
    hd.seqno = pPwork->fileinfo[pPwork->cur_fileid].currentb;

    databuf = MBi_MakeParentSendBuffer(&hd, (u8 *)pPwork->common.sendbuf);
    MI_CpuCopy8((void *)bi.offset, databuf, bi.size);

    return MBi_BlockHeaderEnd((bi.size + MB_COMM_PARENT_HEADER_SIZE),
                              pPwork->fileinfo[pPwork->cur_fileid].pollbmp, pPwork->common.sendbuf);
}

/* A send turn: sort the children by state into AID bitmaps and send one
 * thing, the first of: a boot request, a send start, a member-full notice, a
 * kick, the download info, or a block. With nothing to send, a dummy goes
 * to everyone. */
static int MBi_CommParentSendData(void) {
    struct bitmap {
        u16 connected;
        u16 req;
        u16 kick;
        u16 boot;
        u16 mem_full;
    };
    struct bitmap bmp;
    u16 child;
    int errcode;

    {
        /* MI_CpuClear16(&bmp, sizeof(struct bitmap)), through a volatile
         * fill value as in MBi_CommParentCallback. */
        volatile u16 fill = 0;
        MI_CpuFill16(&bmp, fill, sizeof(struct bitmap));
    }

    for (child = 1; child <= WM_NUM_MAX_CHILD; child++) {
        switch (pPwork->p_comm_state[child - 1]) {
        case MB_COMM_PSTATE_CONNECTED:
            bmp.connected |= (1 << child);
            break;
        case MB_COMM_PSTATE_REQ_ACCEPTED:
            bmp.req |= (1 << child);
            break;
        case MB_COMM_PSTATE_KICKED:
            bmp.kick |= (1 << child);
            break;
        case MB_COMM_PSTATE_SEND_PROCEED:
            break;
        case MB_COMM_PSTATE_BOOT_REQUEST:
            bmp.boot |= (1 << child);
            break;
        case MB_COMM_PSTATE_MEMBER_FULL:
            bmp.mem_full |= (1 << child);
            break;
        default:
            break;
        }
    }

    if (bmp.boot) {
        errcode = MBi_CommParentSendMsg(MB_COMM_TYPE_PARENT_BOOTREQ, bmp.boot);
    } else if (bmp.connected) {
        errcode = MBi_CommParentSendMsg(MB_COMM_TYPE_PARENT_SENDSTART, bmp.connected);
    } else if (bmp.mem_full) {
        errcode = MBi_CommParentSendMsg(MB_COMM_TYPE_PARENT_MEMBER_FULL, bmp.mem_full);
    } else if (bmp.kick) {
        errcode = MBi_CommParentSendMsg(MB_COMM_TYPE_PARENT_KICKREQ, bmp.kick);
    } else if (bmp.req) {
        errcode = MBi_CommParentSendDLFileInfo();
    } else {
        errcode = MBi_CommParentSendBlock();
    }

    if (MB_SENDFUNC_STATE_ERR == errcode) {
        errcode = MBi_CommParentSendMsg(MB_COMM_TYPE_DUMMY, 0xffff);
    }

    return errcode;
}

/* Step file file_id's block cursor: while currentb stays within
 * MB_SEND_THRESHOLD of the highest block a child asked for, send the next
 * block, and otherwise jump back to that block. */
static void MBi_calc_sendblock(u8 file_id) {
    if (pPwork->fileinfo[file_id].active && pPwork->fileinfo[file_id].pollbmp) {
        if (pPwork->fileinfo[file_id].nextb <= pPwork->fileinfo[file_id].currentb &&
            pPwork->fileinfo[file_id].currentb <=
                pPwork->fileinfo[file_id].nextb + MB_SEND_THRESHOLD) {
            pPwork->fileinfo[file_id].currentb++;
        } else {
            pPwork->fileinfo[file_id].currentb = pPwork->fileinfo[file_id].nextb;
        }
    }
}

static u16 MBi_calc_nextsendblock(u16 next_block, u16 next_block_req) {
    return max(next_block_req, next_block);
}

static inline u16 max(u16 a, u16 b) {
    return (a > b) ? a : b;
}

static BOOL IsChildAidValid(u16 child_aid) {
    return (child_aid >= 1 && child_aid <= WM_NUM_MAX_CHILD) ? TRUE : FALSE;
}

static void MBi_CommCallParentError(u16 aid, u16 errcode) {
    MBErrorStatus e_stat;
    e_stat.errcode = errcode;

    MBi_CommChangeParentStateCallbackOnly(aid, MB_COMM_PSTATE_ERROR, &e_stat);
}
