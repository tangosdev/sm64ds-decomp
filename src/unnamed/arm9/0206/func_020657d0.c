#include "types.h"
#include "private/arm9_wbt.h"
extern int func_02065af0(int *thiz);

/* NitroSDK WBT_RequestSync: queue a REQ_SYNC command (2) to the peers in
 * id (target_bmp). */
void func_020657d0(u16 id, void *obj)
{
    WBTCommand cmd;
    cmd.command = 2;
    cmd.target_bmp = id;
    cmd.callback = (void (*)(char *))obj;
    func_02065af0((int *)&cmd);
}
