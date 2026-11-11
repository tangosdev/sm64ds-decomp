#include "types.h"
#include "private/arm9_wbt.h"
extern void MI_CpuCopy8(const void *src, void *dst, u32 size); /* memcpy-like */
extern int func_02065af0(int *thiz);

/* NitroSDK WBT_PutUserData: queue a REQ_USER_DATA command (10) that sends
 * the size bytes at data, at most 9, to the peers in target_bmp. */
int WBT_PutUserData(u16 target_bmp, const void *data, s32 size, void (*callback)(void *)) {
    WBTCommand cmd;
    if (size < 0 || size > 9)
        return 0;
    cmd.target_bmp = target_bmp;
    cmd.command = 10;
    cmd.callback = (void (*)(char *))callback;
    MI_CpuCopy8(data, cmd.u.user_data.data, size);
    cmd.u.user_data.size = (u8)size;
    return func_02065af0((int *)&cmd);
}
