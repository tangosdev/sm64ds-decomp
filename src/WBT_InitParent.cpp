//cpp
#include "types.h"
#include "private/arm9_wbt.h"
#include "nitro/os/system.h"



extern int data_020a94c0;
extern "C" {

extern void func_02065ad0(struct WBTBlockInfoList *list);
extern WBTWork *func_02065b94(void);
}

extern int func_02065af0(int *thiz);
extern "C" {

extern void func_02065be0(void);
extern void func_02065ba0(int v);
extern void func_02065bb0(int v);
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

    if (data_020a94c0 == 1) {
        OS_RestoreInterrupts(saved);
        return;
    }
    data_020a94c0 = 1;
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
    } while (i < 0x10);
    cmd.command = WBT_CMD_REQ_NONE;
    func_02065af0((int *)&cmd);
    func_02065be0();
    func_02065ba0(send_packet_size - 0xe);
    func_02065bb0(recv_packet_size - 0xe);
    OS_RestoreInterrupts(saved);
}
