#include "types.h"
#include "private/arm9_wbt.h"
extern void CpuCopy8(const void *src, void *dst, u32 size);  /* memcpy-like */
extern int func_02065af0(int *thiz);

/* NitroSDK WBT_PutUserData: queue a REQ_USER_DATA command (10) that sends
 * the size bytes at data, at most 9, to the peers in target_bmp. */
int func_020656d4(u16 arg0, const void *arg1, s32 arg2, u32 arg3)
{
    WBTCommand cmd;
    if (arg2 < 0 || arg2 > 9)
        return 0;
    cmd.target_bmp = arg0;
    cmd.command = 10;
    cmd.callback = (void (*)(char *))arg3;
    CpuCopy8(arg1, cmd.u.user_data.data, arg2);
    cmd.u.user_data.size = (u8)arg2;
    return func_02065af0((int *)&cmd);
}
