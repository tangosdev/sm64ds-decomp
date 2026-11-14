#include "types.h"
#include "private/arm9_wbt.h"
#include "nitro/mi/memory.h"
extern int func_02065af0(int *thiz);

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
