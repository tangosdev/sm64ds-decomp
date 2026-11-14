#include "types.h"
#include "private/arm9_wbt.h"
extern int func_02065af0(WBTCommand *thiz);

/* NitroSDK WBT_RequestSync: queue a REQ_SYNC command to the peers in
 * target_bmp. */
void WBT_RequestSync(u16 target_bmp, WBTCallback callback) {
    WBTCommand cmd;
    cmd.command = WBT_CMD_REQ_SYNC;
    cmd.target_bmp = target_bmp;
    cmd.callback = callback;
    func_02065af0(&cmd);
}
