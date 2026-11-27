/* NitroSDK wbt_api.c: the public wireless-block-transfer API and the
 * library-instance state behind it. The protocol machine and packet writers
 * are in wbt_context.c. */
#include "types.h"
#include "nitro/os/system.h"
#include "nitro/mi/memory.h"
#include "nitro/wbt.h"
#include "private/arm9_wbt.h"

/* data_020a94c0 is the terminal ARM9 .bss blob below; these labels are
 * interior views of it which the linker script also exposes to other TUs. */
extern u8 data_020a94c0[];
extern u8 data_020a94c4;
extern s16 data_020a94c8;
extern s16 data_020a94cc;
extern WBTBlockInfoList *data_020a94d0;
extern WBTWork data_020a94d4;
extern WBTCommand data_020a9570;

void func_02063ea8(WBTWork *work, int a, int b, int c);
int func_02064fe8(void *dst, unsigned short b, char c);

WBTBlockInfoList *func_02065a94(int key);
void func_02065ad0(WBTBlockInfoList *list);
WBTBlockInfoList *func_02065ae0(void);
int func_02065af0(int *thiz);
WBTCommand *func_02065b88(void);
WBTWork *func_02065b94(void);
void func_02065ba0(int v);
void func_02065bb0(int v);
int func_02065bc0(void);
int func_02065bd0(void);
void func_02065be0(void);
void func_02065bf4(void);
int func_02065c1c(void);

int func_02065c1c(void) {
    return data_020a94c4;
}

void func_02065bf4(void) {
    data_020a94c4++;
    if (data_020a94c4 == 0) {
        data_020a94c4++;
    }
}

void func_02065be0(void) {
    data_020a94c4 = 0;
}

int func_02065bd0(void) {
    return data_020a94c8;
}

int func_02065bc0(void) {
    return data_020a94cc;
}

void func_02065bb0(int v) {
    data_020a94c8 = v;
}

void func_02065ba0(int v) {
    data_020a94cc = v;
}

WBTWork *func_02065b94(void) {
    return &data_020a94d4;
}

WBTCommand *func_02065b88(void) {
    return &data_020a9570;
}

int func_02065af0(int *thiz) {
    OSIntrMode saved = OS_DisableInterrupts();
    WBTCommand *r;
    if (*(int *)data_020a94c0 != 1) {
        OS_RestoreInterrupts(saved);
        return 0;
    }
    r = func_02065b88();
    if (r == 0 || r->command != WBT_CMD_REQ_NONE) {
        OS_RestoreInterrupts(saved);
        return 0;
    }
    MI_CpuCopy32(thiz, &data_020a9570, 0x9c);
    OS_RestoreInterrupts(saved);
    return 1;
}

WBTBlockInfoList *func_02065ae0(void) {
    return data_020a94d0;
}

void func_02065ad0(WBTBlockInfoList *list) {
    data_020a94d0 = list;
}

WBTBlockInfoList *func_02065a94(int key) {
    WBTBlockInfoList *p = func_02065ae0();
    while (p != 0) {
        if (p->data_info.id == key)
            return p;
        p = p->next;
    }
    return 0;
}

int data_0209a074 = -1;

/* NitroSDK WBT_SetOwnAid and WBT_GetOwnAid: this console's aid, 0 on the
 * parent (WBT_InitParent), -1 before a child has one (WBT_InitChild,
 * WBT_End). */
void WBT_SetOwnAid(int aid) {
    data_0209a074 = aid;
}

int WBT_GetOwnAid(void) {
    return data_0209a074;
}

int WBT_AidbitmapToAid(u16 aid_bmp) {
    int i;
    for (i = 0; i < 16; i++)
        if ((aid_bmp >> i) & 1)
            return i;
    return -1;
}

int WBT_CalcPacketbitmapSize(int a) {
    int d = func_02065bd0();
    int n = (a % d != 0 ? 1 : 0) + a / d;
    int r = (n & 0x1f ? 1 : 0) + (n >> 5);
    return r << 2;
}

int WBT_SetPacketSize(int a, int b) {
    int x = func_02065b94()->cmd.command;
    if (x == WBT_CMD_REQ_GET_BLOCK || x == WBT_CMD_REQ_GET_BLOCKINFO)
        return 0;
    func_02065ba0(a - 0xe);
    func_02065bb0(b - 0xe);
    return 1;
}

int WBT_NumOfRegisteredBlock(void) {
    int n = 0;
    WBTBlockInfoList *p = func_02065ae0();
    if (p != 0) {
        do {
            p = p->next;
            n++;
        } while (p != 0);
    }
    return n;
}

int WBT_RegisterBlock(WBTBlockInfoList *block_info_list, u32 block_id, const void *user_id,
                      const void *data_ptr, int data_size, u16 permission_bmp) {
    OSIntrMode irq = OS_DisableInterrupts();
    if (func_02065ae0() == 0) {
        func_02065ad0(block_info_list);
    } else {
        WBTBlockInfoList *p = func_02065ae0();
        WBTBlockInfoList *nx = p->next;
        if (nx != 0) {
            do {
                p = nx;
                nx = nx->next;
            } while (nx != 0);
        }
        p->next = block_info_list;
    }
    if (data_ptr == 0) {
        block_info_list->block_type = WBT_BLOCK_LIST_TYPE_USER;
    } else {
        block_info_list->data_ptr = (void *)data_ptr;
        block_info_list->block_type = WBT_BLOCK_LIST_TYPE_COMMON;
    }
    block_info_list->next = 0;
    block_info_list->data_info.id = block_id;
    block_info_list->data_info.block_size = data_size;
    block_info_list->permission_bmp = permission_bmp;
    MI_CpuCopy8(user_id, block_info_list->data_info.user_id, WBT_USER_ID_LEN);
    OS_RestoreInterrupts(irq);
    return 1;
}

WBTBlockInfoList *WBT_UnregisterBlock(u32 block_id) {
    OSIntrMode saved = (0, OS_DisableInterrupts());
    WBTBlockInfoList *head = func_02065ae0();
    WBTBlockInfoList *cur;
    WBTBlockInfoList *prev;
    if (head == 0) {
        OS_RestoreInterrupts(saved);
        return 0;
    }
    if (head->data_info.id == block_id) {
        func_02065ad0(head->next);
        OS_RestoreInterrupts(saved);
        return head;
    }
    prev = head;
    cur = prev->next;
    while (cur != 0) {
        if (cur->data_info.id == block_id) {
            prev->next = cur->next;
            OS_RestoreInterrupts(saved);
            return cur;
        }
        prev = cur;
        cur = cur->next;
    }

    OS_RestoreInterrupts(saved);
    return 0;
}

/* NitroSDK WBT_RequestSync: queue a REQ_SYNC command to the peers in
 * target_bmp. */
void WBT_RequestSync(u16 target_bmp, WBTCallback callback) {
    WBTCommand cmd;
    cmd.command = WBT_CMD_REQ_SYNC;
    cmd.target_bmp = target_bmp;
    cmd.callback = callback;
    func_02065af0((int *)&cmd);
}

/* NitroSDK WBT_GetBlockInfo: WBT_GetBlock's REQ_GET_BLOCK built as
 * REQ_GET_BLOCKINFO. Entry index of the peers' block list is fetched
 * into the per-aid buffers in recv_buf_table, each one WBTBlockInfo
 * (recv_data_size); only the receive-buffer table is filled. */
void WBT_GetBlockInfo(u16 target_bmp, int index, const void *recv_buf_table, WBTCallback callback) {
    WBTCommand cmd;
    cmd.command = WBT_CMD_REQ_GET_BLOCKINFO;
    cmd.target_bmp = target_bmp;
    cmd.callback = callback;
    cmd.u.get.block_id = index;
    cmd.u.get.recv_data_size = sizeof(WBTBlockInfo);
    MI_CpuCopy32(recv_buf_table, cmd.u.get.recv_buf_table, sizeof(cmd.u.get.recv_buf_table));
    func_02065af0((int *)&cmd);
}

/* NitroSDK WBT_GetBlock: queue a REQ_GET_BLOCK command asking the peers
 * in target_bmp for block block_id, recv_size bytes, into the per-aid
 * receive buffers and packet bitmaps of the two 16-entry tables. */
void WBT_GetBlock(u16 target_bmp, u32 block_id, const void *recv_buf_table, u32 recv_size,
                  const void *pkt_bmp_table, WBTCallback callback) {
    WBTCommand cmd;
    cmd.command = WBT_CMD_REQ_GET_BLOCK;
    cmd.target_bmp = target_bmp;
    cmd.callback = callback;
    cmd.u.get.block_id = block_id;
    cmd.u.get.recv_data_size = recv_size;
    MI_CpuCopy32(recv_buf_table, cmd.u.get.recv_buf_table, sizeof(cmd.u.get.recv_buf_table));
    MI_CpuCopy32(pkt_bmp_table, cmd.u.get.pkt_bmp_table, sizeof(cmd.u.get.pkt_bmp_table));
    func_02065af0((int *)&cmd);
}

/* NitroSDK WBT_PutUserData: queue a REQ_USER_DATA command that sends the
 * size bytes at data, at most WBT_SIZE_USER_DATA, to the peers in
 * target_bmp. */
int WBT_PutUserData(u16 target_bmp, const void *data, s32 size, WBTCallback callback) {
    WBTCommand cmd;
    if (size < 0 || size > WBT_SIZE_USER_DATA)
        return 0;
    cmd.target_bmp = target_bmp;
    cmd.command = WBT_CMD_REQ_USER_DATA;
    cmd.callback = callback;
    MI_CpuCopy8(data, cmd.u.user_data.data, size);
    cmd.u.user_data.size = (u8)size;
    return func_02065af0((int *)&cmd);
}

int data_0209a070 = 1;

int func_02065538(void *buf, int size) {
    int r;
    int i;
    WBTWork *work = func_02065b94();

    for (i = 1; i < 16; i++) {
        r = func_02063d3c(work, i, buf, size);
        if (r)
            return r;
    }

    for (i = 1; i < 16; i++) {
        r = func_02063e08(work, i, buf, size);
        if (r)
            return r;
    }

    for (i = 1; i < 16; i++) {
        r = func_02063ae4(work, i, buf, size);
        if (r)
            return r;
    }

    for (i = 1; i < 16; i++) {
        r = func_02063da0(work, i, buf, size);
        if (r)
            return r;
    }

    r = func_02063980(work, buf, size);
    if (r)
        return r;

    r = func_02063718(work, buf, size);
    if (r)
        return r;

    i = data_0209a070;
    do {
        i = i + 1;
        if (i > 0xf) {
            i = 1;
        }
        r = func_02063bc4(work, i, buf, size);
        if (r) {
            data_0209a070 = i;
            return r;
        }
    } while (i != data_0209a070);

    return func_02064fe8(buf, 0xfffe, 0);
}

void func_02065500(int a, int b, int c) {
    if (c <= 0)
        return;
    func_02063ea8(func_02065b94(), a, b, c);
}

void func_020654d0(int a, int b) {
    func_02063ea8(func_02065b94(), 0, a, b);
}

int func_020653cc(void *buf, int size) {
    int r;
    WBTWork *work = func_02065b94();
    r = func_02063d3c(work, 0, buf, size);
    if (r)
        return r;
    r = func_02063e08(work, 0, buf, size);
    if (r)
        return r;
    r = func_02063ae4(work, 0, buf, size);
    if (r)
        return r;
    r = func_02063da0(work, 0, buf, size);
    if (r)
        return r;
    r = func_02063bc4(work, 0, buf, size);
    if (r)
        return r;
    r = func_02063980(work, buf, size);
    if (r)
        return r;
    r = func_02063718(work, buf, size);
    if (r)
        return r;
    return func_02064fe8(buf, 0xffff, 0);
}

/* NitroSDK WBT_InitParent(send_packet_size, recv_packet_size, callback): once
 * (data_020a94c0), store 0 through WBT_SetOwnAid (a child stores -1), reset
 * the running and system commands of the WBTWork that func_02065b94 returns,
 * make callback the system command's callback, forget each peer's last
 * command counter, queue an empty command (func_02065af0 copies its 0x9c
 * bytes into queued_cmd), and set the two packet sizes less the 0xe-byte
 * header (func_02065ba0, func_02065bb0). func_02040c34 passes the file-share
 * packet sizes and WFSi_OnParentSystemCallback, the file-share parent's callback. */
void WBT_InitParent(int send_packet_size, int recv_packet_size, WBTCallback callback) {
    OSIntrMode saved = OS_DisableInterrupts();
    int i;
    WBTWork *work;
    WBTCommand cmd;

    if (*(int *)data_020a94c0 == 1) {
        OS_RestoreInterrupts(saved);
        return;
    }
    *(int *)data_020a94c0 = 1;
    WBT_SetOwnAid(0);
    func_02065ad0(0);
    work = func_02065b94();
    work->cmd.command = WBT_CMD_REQ_NONE;
    work->cmd.target_bmp = 0;
    work->cmd.peer_bmp = 0;
    work->cmd.callback = 0;
    work->system_cmd.command = WBT_CMD_REQ_NONE;
    work->system_cmd.target_bmp = 0;
    work->system_cmd.peer_bmp = 0;
    work->system_cmd.callback = callback;
    i = 0;
    do {
        work->peer[i].last_cmd_counter = 0;
        i++;
    } while (i < 16);
    cmd.command = WBT_CMD_REQ_NONE;
    func_02065af0((int *)&cmd);
    func_02065be0();
    func_02065ba0(send_packet_size - 0xe);
    func_02065bb0(recv_packet_size - 0xe);
    OS_RestoreInterrupts(saved);
}

/* NitroSDK WBT_InitChild(callback): as WBT_InitParent (WBT_InitParent), but
 * stores -1 through WBT_SetOwnAid and sets both packet sizes to 0. The
 * callback becomes the system command's callback; func_02040c34
 * passes WFSi_OnChildSystemCallback, the file-share child's. */
void WBT_InitChild(WBTCallback callback) {
    OSIntrMode saved = OS_DisableInterrupts();
    int i;
    WBTWork *work;
    WBTCommand cmd;

    if (*(int *)data_020a94c0 == 1) {
        OS_RestoreInterrupts(saved);
        return;
    }
    *(int *)data_020a94c0 = 1;
    WBT_SetOwnAid(-1);
    func_02065ad0(0);
    work = func_02065b94();
    work->cmd.command = WBT_CMD_REQ_NONE;
    work->cmd.target_bmp = 0;
    work->cmd.peer_bmp = 0;
    work->cmd.callback = 0;
    work->system_cmd.command = WBT_CMD_REQ_NONE;
    work->system_cmd.target_bmp = 0;
    work->system_cmd.peer_bmp = 0;
    work->system_cmd.callback = callback;
    for (i = 0; i < 16; i++) {
        work->peer[i].last_cmd_counter = 0;
    }
    cmd.command = WBT_CMD_REQ_NONE;
    func_02065af0((int *)&cmd);
    func_02065be0();
    func_02065ba0(0);
    func_02065bb0(0);
    OS_RestoreInterrupts(saved);
}

/* NitroSDK WBT_End: the teardown twin of WBT_InitParent (WBT_InitParent) and
 * WBT_InitChild (WBT_InitChild). Once initialized (data_020a94c0), clear the
 * flag, store -1 through WBT_SetOwnAid (the parent stores 0 there, a child -1),
 * reset the running and system commands of the WBTWork that func_02065b94
 * returns, forget each peer's last command counter, queue an empty command
 * (func_02065af0 copies its 0x9c bytes into queued_cmd) and set both packet
 * sizes (func_02065ba0, func_02065bb0) to -1. */
void WBT_End(void) {
    OSIntrMode saved = OS_DisableInterrupts();
    WBTWork *work;
    int i;
    WBTCommand cmd;
    if (*(int *)data_020a94c0 == 0) {
        OS_RestoreInterrupts(saved);
        return;
    }
    *(int *)data_020a94c0 = 0;
    WBT_SetOwnAid(-1);
    func_02065ad0(0);
    work = func_02065b94();
    work->cmd.command = WBT_CMD_REQ_NONE;
    work->cmd.target_bmp = 0;
    work->cmd.peer_bmp = 0;
    work->cmd.callback = 0;
    work->system_cmd.command = WBT_CMD_REQ_NONE;
    work->system_cmd.target_bmp = 0;
    work->system_cmd.peer_bmp = 0;
    work->system_cmd.callback = 0;
    for (i = 0; i < 16; i++) {
        work->peer[i].last_cmd_counter = 0;
    }
    cmd.command = WBT_CMD_REQ_NONE;
    func_02065af0((int *)&cmd);
    func_02065be0();
    func_02065ba0(-1);
    func_02065bb0(-1);
    OS_RestoreInterrupts(saved);
}

/* The tail of the ARM9 .bss: the WBT child-thread state at the base, then a
   WM block and newlib's __cs/__atexit/signal state. One object so the
   configured interior labels keep their addresses. */
u8 data_020a94c0[0xf60];

int WBT_MpParentSendHook(void *sendbuf, int send_size) {
    unsigned short r = 0;
    if (send_size > 0 && sendbuf != 0) {
        r = func_02065538(sendbuf, send_size);
    }
    return r;
}

void WBT_MpParentRecvHook(int a, int b, int c) {
    if (a == 0)
        return;
    if (b == 0)
        return;
    if (b == 0xffff)
        return;
    func_02065500(c, a, b);
}

/* No return statement when there is no room or no buffer: the ROM returns
 * from that path with r0 untouched, so a send_size <= 0 call gets the buffer
 * address back as its "length" (and a null buffer 0). Spelling that as
 * `return (int)sendbuf` would need a cast the SDK's void * buffer never
 * otherwise takes; WBT_MpParentSendHook returns 0 on the same path. */
int WBT_MpChildSendHook(void *sendbuf, int send_size) {
    if (send_size > 0 && sendbuf != 0)
        return func_020653cc(sendbuf, send_size);
}

void WBT_MpChildRecvHook(int a, int b) {
    if (a == 0)
        return;
    if (b == 0)
        return;
    if (b == 0xffff)
        return;
    func_020654d0(a, b);
}
