//cpp
#include "private/arm9_wbt.h"
#include "nitro/os/system.h"

extern "C" {

extern void func_02065ad0(struct WBTBlockInfoList *list);
extern WBTWork *func_02065b94(void);
extern int func_02065af0(int *thiz);
extern void func_02065be0(void);
extern void func_02065ba0(int v);
extern void func_02065bb0(int v);
extern int data_020a94c0;

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
    if (data_020a94c0 == 0) {
        OS_RestoreInterrupts(saved);
        return;
    }
    data_020a94c0 = 0;
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
    for (i = 0; i < 0x10; i++) {
        work->peer[i].last_cmd_counter = 0;
    }
    cmd.command = WBT_CMD_REQ_NONE;
    func_02065af0((int *)&cmd);
    func_02065be0();
    func_02065ba0(-1);
    func_02065bb0(-1);
    OS_RestoreInterrupts(saved);
}
}
