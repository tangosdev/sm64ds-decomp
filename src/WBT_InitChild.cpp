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

/* NitroSDK WBT_InitChild(callback): as WBT_InitParent (WBT_InitParent), but
 * stores -1 through WBT_SetOwnAid and sets both packet sizes to 0. The
 * callback becomes the system command's callback; func_02040c34
 * passes WFSi_OnChildSystemCallback, the file-share child's. */
void WBT_InitChild(WBTCallback callback) {
    OSIntrMode saved = OS_DisableInterrupts();
    int i;
    WBTWork *work;
    WBTCommand cmd;

    if (data_020a94c0 == 1) {
        OS_RestoreInterrupts(saved);
        return;
    }
    data_020a94c0 = 1;
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
    for (i = 0; i < 0x10; i++) {
        work->peer[i].last_cmd_counter = 0;
    }
    cmd.command = WBT_CMD_REQ_NONE;
    func_02065af0((int *)&cmd);
    func_02065be0();
    func_02065ba0(0);
    func_02065bb0(0);
    OS_RestoreInterrupts(saved);
}
