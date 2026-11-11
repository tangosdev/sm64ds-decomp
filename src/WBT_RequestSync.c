#include "types.h"
#include "private/arm9_wbt.h"
extern int func_02065af0(int *thiz);

/* NitroSDK WBT_RequestSync: queue a REQ_SYNC command (2) to the peers in
 * target_bmp. */
void WBT_RequestSync(u16 target_bmp, void (*callback)(void *)) {
    WBTCommand cmd;
    cmd.command = 2;
    cmd.target_bmp = target_bmp;
    cmd.callback = (void (*)(char *))callback;
    func_02065af0((int *)&cmd);
}
