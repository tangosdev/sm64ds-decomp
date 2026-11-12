#include "types.h"
extern void func_0203f6e8(s16 v);
extern int func_0203f818(u16 channel);
extern int OS_GetTickLo(void);
extern int __aeabi_idiv(int a, int b);

extern u16 data_020a0f34;
extern u16 data_020a0fd0[];
extern u8 data_020a0ef4;
extern s16 data_020a0f18;

/* NitroSDK WMMeasureChannelCallback, the record WM hands func_0203f920 when
   a channel measurement completes. */
struct WMMeasureChannelCallback {
    u16 apiid;        /* 0x0 -- 0x1e */
    u16 errcode;      /* 0x2 -- 0 is WM_ERRCODE_SUCCESS */
    u16 wlCmdID;      /* 0x4 */
    u16 wlResult;     /* 0x6 */
    u16 channel;      /* 0x8 */
    u16 ccaBusyRatio; /* 0xa -- percent */
};

/* One channel measured: end with -4 if it failed. Otherwise keep the least
   busy ratio in data_020a0f34 and the channels that share it in
   data_020a0fd0 (data_020a0ef4 of them), then measure the next allowed
   channel. Once none is left, draw one of the kept channels with
   OS_GetTickLo and finish with it; a refused request finishes with -3. */
void func_0203f714(struct WMMeasureChannelCallback *s) {
    u16 a, cur, v;
    int r;
    if (s->errcode != 0) {
        func_0203f6e8(-4);
        return;
    }
    a = s->ccaBusyRatio;
    cur = data_020a0f34;
    v = s->channel;
    if (a < cur) {
        data_020a0f34 = a;
        data_020a0ef4 = 1;
        data_020a0fd0[0] = v;
    } else if (a == cur) {
        data_020a0fd0[data_020a0ef4] = v;
        data_020a0ef4++;
    }
    r = func_0203f818((u16)(v + 1));
    if (r == 1) {
        int n = OS_GetTickLo();
        int idx = n % data_020a0ef4;
        data_020a0f18 = data_020a0fd0[idx];
        func_0203f6e8(data_020a0f18);
    } else if (r == 2) {
        func_0203f6e8(-3);
    }
}
