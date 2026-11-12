#include "nitro/wm/wm_api.h"
#include "private/arm9_a0f90_channel_measure.h"

extern void func_0203f650(int x);
extern void func_0203f6a4(int v);
extern int data_02099e20;
extern void func_0203f920(void *arg);

/* Pick a quiet wireless channel. sysBuf is the WM system buffer
   (func_0203e20c allocates 0xb000 bytes), callback the one that receives
   the {result, channel} pair (func_0203e978). Refuse with -10 unless idle
   (state 3); then initialize WM (WM_Initialize, command 0, DMA 1) with
   func_0203f920 as the callback: -1 if that is refused, else state 0.
   Every allowed channel is then measured and one of the least busy drawn
   at random (func_0203f714), the shape of NitroSDK's wireless sample
   library's channel measurement. */
void func_0203f9d4(void *sysBuf, ChannelMeasureCallback callback) {
    data_020a0f90 = callback;
    if (data_02099e20 != 3) {
        func_0203f650(-10);
        return;
    }
    if (WM_Initialize(sysBuf, func_0203f920, 1) != WM_ERRCODE_OPERATING) {
        func_0203f650(-1);
        return;
    }
    func_0203f6a4(0);
}
